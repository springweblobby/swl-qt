#include "lobbyinterface.h"
#include "logger.h"
#include <QCoreApplication>
#include <QSysInfo>
#include <QWebFrame>
#include <QWebPage>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <fstream>
#include <deque>
namespace fs = boost::filesystem;

LobbyInterface::LobbyInterface(QObject *parent, QWebFrame *frame) :
        QObject(parent), springHome(""), network(this, logger), frame(frame) {
    logger.setEventReceiver(this);
    auto args = QCoreApplication::arguments();
    logger.setDebug(args.contains("-debug"));
}

void LobbyInterface::init() {
    #ifdef Q_OS_LINUX
        os = "Linux";
    #elif Q_OS_WIN32 // Defined on 64-bit Windows too.
        os = "Windows";
    #elif Q_OS_MAC
        os = "Mac";
    #else
        #error "Unknown target OS."
    #endif

    if (springHome != "");
    else if (os == "Windows") {
        // This calls SHGetFolderPath(... CSIDL_PERSONAL ...), same as what Spring uses.
        springHome = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString() +
            "\\My Games\\Spring";
    } else {
        // ~/.spring
        springHome = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString() +
            "/.spring";
    }
    logger.info("springHome is ", springHome);

    try {
        const char slash = (os == "Windows" ? '\\' : '/');
        const std::string weblobbyDir = springHome + slash + "weblobby" + slash;
        fs::create_directories({ weblobbyDir + "engine" });
        fs::create_directories({ weblobbyDir + "pr-downloader" });
        fs::create_directories({ weblobbyDir + "logs" });
        frame->page()->settings()->setLocalStoragePath(QString::fromStdString(weblobbyDir + "storage"));
        logger.setLogFile(weblobbyDir + slash + "weblobby.log");
    } catch(fs::filesystem_error e) {
        logger.error("Creating folders failed with: ", e.what());
    }
}

QString LobbyInterface::getSpringHome() {
    return QString::fromStdString(springHome);
}

// Should be called prior to init() to take effect.
void LobbyInterface::setSpringHome(QString path) {
    springHome = path.toStdString();
}

void LobbyInterface::connect(QString host, unsigned int port) {
    network.connect(host.toStdString(), port);
}

void LobbyInterface::disconnect() {
    network.disconnect();
}

void LobbyInterface::send(QString msg) {
    network.send(msg.toStdString());
}

bool LobbyInterface::event(QEvent* evt) {
    if (evt->type() == NetworkHandler::ReadEvent::TypeId) {
        auto readEvt = dynamic_cast<NetworkHandler::ReadEvent&>(*evt);
        evalJs("on_socket_get('" + escapeJs(readEvt.msg) + "')");
        return true;
    } else if (evt->type() == Logger::LogEvent::TypeId) {
        auto logEvt = dynamic_cast<Logger::LogEvent&>(*evt);
        if(logEvt.lev == Logger::level::error)
            evalJs("alert2('" + escapeJs(logEvt.msg) + "')");
        return true;
    } else {
        return QObject::event(evt);
    }
}

void LobbyInterface::jsMessage(std::string source, int lineNumber, std::string message) {
    if (message.find("<TASSERVER>") == std::string::npos &&
        message.find("<LOCAL>") == std::string::npos) {
        logger.warning(source, ":", lineNumber, " ", message);
    } else {
        logger.debug(message);
    }
}

QString LobbyInterface::listDirs(QString path) {
    return listFilesPriv(path, true);
}

QString LobbyInterface::listFiles(QString path) {
    return listFilesPriv(path, false);
}

QString LobbyInterface::listFilesPriv(QString qpath, bool dirs) {
    std::list<std::string> files;
    std::string path = qpath.toStdString();
    fs::path folder(path);

    if (fs::exists(path) && fs::is_directory(folder)) {
        fs::directory_iterator end_itr;
        for (fs::directory_iterator it(folder); it != end_itr; ++it) {
            if (dirs && fs::is_directory(it->path())) {
                std::string f = it->path().filename().string();
                files.push_back(f);
            } else if (!dirs && !fs::is_directory(it->path())) {
                std::string f = it->path().filename().string();
                files.push_back(f);
            }
        }
    }
    std::string out = "";
    if (!files.empty()) {
        out += files.front();
        for (std::list<std::string>::iterator it = ++(files.begin()); it != files.end(); it++) {
            out += "||" + *it;
        }
    }

    return QString::fromStdString(out);
}

QString LobbyInterface::readFileLess(QString path, unsigned int lines) {
    // TODO: deque? Just read the file in the reverse order, geez.
    std::ifstream in(path.toStdString().c_str());
    if (!in) {
        logger.warning("readFileLess(): Could not open ", path.toStdString());
        return "";
    }
    std::deque<std::string> q;
    while (in.good()) {
        if (q.size() > lines)
            q.pop_front();
        std::string s;
        std::getline(in, s);
        q.push_back(std::move(s));
    }
    QString res;
    for (auto l : q)
        res += (l + "\n").c_str();
    return res;
}

bool LobbyInterface::downloadFile(QString source, QString target) {
    return true;
    // TODO
    /*try {
        System.out.println("Copy file: " + source + " > " + target );
        URLConnection dl = new URL( source ).openConnection();
        dl.setUseCaches(false);

        File f = new File(target);
        dl.setIfModifiedSince( f.lastModified() );

        if (dl.getContentLength() <= 0) {
            System.out.println("File not modified, using cache");
            return true;
        }

        ReadableByteChannel rbc = Channels.newChannel(dl.getInputStream());

        FileOutputStream fos = new FileOutputStream(target);
        fos.getChannel().transferFrom(rbc, 0, 1 << 24);
        //System.out.println(fos.getChannel().size());
        fos.close();
        rbc.close();

        if (target.endsWith("pr-downloader")) {
            CLibrary libc = (CLibrary) Native.loadLibrary("c", CLibrary.class); //breaks applet on windows
            //Path targetFile = Paths.get(target); // fails on Linux
            //Set<PosixFilePermission> perms = PosixFilePermissions.fromString("rwxr-x---");
            //Files.setPosixFilePermissions(targetFile, perms);

            libc.chmod(target, 0750);
        }

    } catch (Exception e)	{
        e.printStackTrace();
    }
    return true;*/
}

QObject* LobbyInterface::getUnitsync(QString qpath) {
    std::string path = qpath.toStdString();
    if (!unitsyncs.count(path)) {
        unitsyncs.insert(std::make_pair(path, UnitsyncHandler(this, logger, path)));
    }

    if (unitsyncs.find(path)->second.isReady())
        return &(unitsyncs.find(path)->second);
    else {
        logger.error("Unitsync not loaded at ", path);
        return NULL;
    }
}

/*
public void createScript(String scriptFile, String script)
{
    this.createScriptFile(scriptFile, script);
}

private int byteToInt(byte b)
{
    return (int) b & 0xff;
}
private int byteToShort(byte b)
{
    return (short) ((short) b & 0xff);
}

public int[] jsReadFileVFS(String unitsyncPath, int fd, int size)
{
    byte[] bytes = new byte[size];
    ByteBuffer buff = ByteBuffer.wrap(bytes);
    int bytesRead;
    bytesRead = this.getUnitsync(unitsyncPath).readFileVFS(fd, buff, size );
    //this.echoJs( buff.toString() );
    System.out.println("Bytes read: " + bytesRead );
    byte[] byteArray = buff.array();
    int[] ints = new int[ byteArray.length ];

    for (int i = 0; i < byteArray.length; ++i)
    {
        //ints[i] = (int)byteArray[i];
        //ints[i] = byteToInt( byteArray[i] );
        ints[i] = byteArray[i];

    }
    return ints;
}



public boolean killCommand( final String cmdName )
{
    Process p = processes.get(cmdName);
    if (p != null) {
        p.destroy();
    }
    return true;
}


private void setOs(String os) {
    this.os = os;
    File f;
    this.slash = os.equals("Windows") ? "\\" : "/";
    if	(springHome != "") {

    } else if( os.equals("Windows")) {
        springHome = System.getProperty("user.home") + "\\Documents\\My Games\\Spring";
    } else if( os.equals("Mac") || os.equals("Linux")) {
        springHome = System.getProperty("user.home") + "/.spring";
    } else {
        return;
    }
    f = new File( springHome );
    f.mkdirs();

    if (!f.isDirectory()) {
        System.err.println("alert('Cannot access spring home folder " + jsFix(springHome) + "\nApparently, automatic detection has failed. Please set the correct one in settings.');");
    }

    String weblobbyHome = springHome + this.slash + "weblobby";

    f = new File( weblobbyHome + this.slash + "engine" );
    f.mkdirs();

    f = new File( weblobbyHome + this.slash + "pr-downloader" );
    f.mkdirs();

    f = new File( weblobbyHome + this.slash + "logs" );
    f.mkdirs();
}


public void createDir(final String path) {
    File f = new File( path );
    f.mkdir();
}

public void runCommand(final String cmdName, final String[] cmd) {
    //
    // Chromium Bug:
    // cmd is coming in as a string(?) instead of as an array of strings.
    // Appears fine in javascript but not in this function
    // Chromium only, not Windows Chrome
    //
    //
    echoJs( "begin runCommand1: " );
    echoJs( "begin runCommand2: " + cmd );
    echoJs( "begin runCommand3: " + cmd[0] );
    //

    new Thread(new Runnable() {
        public void run() {
            runCommandThread(cmdName, cmd);
        }
    }).start(); //new Thread(new Runnable() {
}

private void createScriptFile(final String scriptFile, final String script) {
    System.out.println("Creating script: " + scriptFile );
    try {
        PrintWriter out = new PrintWriter( scriptFile );
        System.out.println( "Writing to script file: " +  scriptFile );
        out.print(script);
        out.close();
    } catch (Exception e) {
        WriteToLogFile( e );
    }
}

public void createUiKeys(final String path) {
    System.out.println( "Creating empty uikeys: " + path );
    try {
        PrintWriter out = new PrintWriter( path );
        out.print("");
        out.close();
    } catch(Exception e) {
        e.printStackTrace();
    }
}
public void deleteSpringSettings(final String path) {
    if (!path.endsWith("springsettings.cfg")) {
        System.out.println( "Delete SpringSettings error: " + path );
        return;
    }
    echoJs( "Delete SpringSettings: " + path );
    try	{
        File f = new File( path );
        f.delete();
    } catch(Exception e) {
        WriteToLogFile( e );
    }
}

public String jsFix(String str) {
    str = str.replace("\\", "\\\\");
    str = str.replace("'", "\\'");
    str = str.replace("\n", "\\n");
    str = str.replace("\r", "");
    return str;
}

private void setupEnvironment(ProcessBuilder pb) {
    pb.environment().put( "OMP_WAIT_POLICY", "ACTIVE" );
}

private void runCommandThread(final String cmdName, final String[] cmd) {
    if(cmd[0].contains( "pr-downloader")) {
        //String newCmd = this.springHome + this.slash + "pr-downloader" + this.slash + "pr-downloader";
        //cmd[0] = cmd[0].replace( "pr-downloader", newCmd );
    } else if(cmd[0].toLowerCase().contains("spring")) {
        System.out.println("Starting Spring shortly... " +  cmd[0] );
    } else if(cmd[1].toLowerCase().contains( "spring")) {
        System.out.println("Starting Spring shortly... " +  cmd[0] + " " + cmd[1] );
    } else {
        System.err.println("Bad command.");
        return;
    }

    if (processes.get(cmdName) != null)	{
        return;
    } try {
        //echoJs( "running command... " + cmd[0] );

        //Runtime runtime = Runtime.getRuntime();
        //Process pr = runtime.exec( cmd2 );


        ProcessBuilder builder = new ProcessBuilder(cmd);
        builder.redirectErrorStream(true);
        setupEnvironment( builder );
        Process pr = builder.start();

        processes.put(cmdName, pr);

        BufferedReader buf = new BufferedReader(new InputStreamReader(pr.getInputStream()));

        String line = "";

        while ((line=buf.readLine())!=null) {
            doJs("commandStream('"+ jsFix(cmdName) +"', '"+jsFix(line)+"')");
        }
        processes.remove(cmdName);
        doJs("commandStream('exit', '"+jsFix(cmdName)+"')");
    } catch (Exception e) {
        WriteToLogFile(e);
        e.printStackTrace();
    }
}


private void WriteToLogFile(Exception e) {
    String logFile = this.springHome + this.slash + "WebLobbyLog.txt" ;
    try	{
        PrintWriter out = new PrintWriter( logFile );
        echoJs( "Error. Writing to log file: " +  logFile );
        out.println( "Begin log file.\n" );

        e.printStackTrace( out );

        out.close();
    } catch(Exception e2)
    {
        e.printStackTrace();
    }

}*/

void LobbyInterface::writeToFile(QString path, QString line) {
    std::ofstream out(path.toStdString(), std::ios_base::app);
    out << line.toStdString() << std::endl;
}


/*public String ReadFileMore(final String logFile, final int numLines) {
    try {
        File f = new File(logFile);
        if (!f.exists()) {
            return "";
        }
        String out = "";
        String curLine;
        int curLineNum = 1;

        BufferedReader br = null;
        br = new BufferedReader(new FileReader(logFile));

        while ((curLine = br.readLine()) != null && curLineNum <= numLines ) {
            out += curLine;
            curLineNum++;
        }

        return out;
    }
    catch(Exception e) {
        e.printStackTrace();
        //echoJs( "Problem reading from log file ("+logFile+"): " + e.toString() );
        return "";
    }
}*/

void LobbyInterface::evalJs(const std::string& code) {
    frame->evaluateJavaScript(QString::fromStdString("__java_js_wrapper(function(){" + code + "}, this);"));
}

std::string LobbyInterface::escapeJs(const std::string& str) {
    std::string res = "";
    for(char c : str) {
        if(c == '\'') res += "\\'";
        else if(c == '"') res += "\\\"";
        else if(c == '\n') res += "\\n";
        else if(c == '\r') res += "\\r";
        else res += c;
    }
    return res;
}
/*

private DatagramSocket dsocket;
public int sendSomePacket(final String host, final int port, final String messageString ) {
    try {
        //int port = 90;

        byte[] message = messageString.getBytes();

        // Get the internet address of the specified host
        InetAddress address = InetAddress.getByName(host);

        // Initialize a datagram packet with data and address
        DatagramPacket packet = new DatagramPacket(message, message.length, address, port);

        // Create a datagram socket, send the packet through it, close it.
        //DatagramSocket dsocket = new DatagramSocket();
        dsocket.send(packet);
        System.out.println(""+dsocket.getLocalPort());
        return dsocket.getLocalPort();
        //dsocket.close();
    } catch (Exception e) {
        System.err.println(e);
        for(int i=0; i<e.getStackTrace().length; i++)
        {
            echoJs( e.getStackTrace()[i]+"" );
        }
    }
    return -1;

}


public String getMacAddress() {
    try {
        InetAddress ip = InetAddress.getLocalHost();
        //echoJs("IP = " + ip);

        //NetworkInterface network = NetworkInterface.getByName("wlan0");

        NetworkInterface network = NetworkInterface.getByInetAddress(ip);
        if (network == null) {
            Enumeration<NetworkInterface> networks = NetworkInterface.getNetworkInterfaces();
            for (NetworkInterface netint : Collections.list(networks)) {
                if (!netint.isLoopback() && !netint.isVirtual()) {
                    network = netint;
                }
            }
        }

        byte[] mac = network.getHardwareAddress();
        if (mac != null && mac.length > 0)	{
            StringBuilder sb = new StringBuilder();
            //sb.append( mac.length + "" );
            for (int i = 0; i < mac.length; i++) {
                //sb.append(String.format("%02X%s", mac[i], (i < mac.length - 1) ? ":" : ""));
                sb.append(String.format( byteToInt( mac[i] ) +"%s", (i < mac.length - 1) ? ":" : ""));
                //sb.append(String.format( mac[i] +"%s", (i < mac.length - 1) ? ":" : ""));
            }

            return sb.toString();
        }
    } catch (Exception e) {
        e.printStackTrace();
    }

    return "";

}*/

long LobbyInterface::getUserID() {
    for(auto i : QNetworkInterface::allInterfaces()) {
        if((i.flags() & QNetworkInterface::IsUp) &&
                (i.flags() & QNetworkInterface::IsRunning) &&
                !(i.flags() & QNetworkInterface::IsLoopBack) &&
                !i.hardwareAddress().startsWith("00:00:00:00")) { // hax
            std::string str = i.hardwareAddress().toStdString() + "lobby.springrts.com";
            boost::crc_32_type crc;
            // Do we include '\0'? Does it matter?
            crc.process_bytes(str.c_str(), str.size());
            return crc.checksum();
        }
    }
    return 0;
    /*String mac = getMacAddress() + "lobby.springrts.com";

    CRC32 crc32 = new CRC32();
    crc32.update( mac.getBytes() );
    return crc32.getValue();*/
}
