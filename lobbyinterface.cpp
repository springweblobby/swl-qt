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
        QObject(parent), springHome(""), debugNetwork(false), debugCommands(false),
        network(this, logger), frame(frame) {
    logger.setEventReceiver(this);
    auto args = QCoreApplication::arguments();
    if (args.contains("-debug-all")) {
        debugNetwork = debugCommands = true;
        logger.setDebug(true);
    }
    if (args.contains("-debug-net")) {
        debugNetwork = true;
        logger.setDebug(true);
    }
    if (args.contains("-debug-cmd")) {
        debugCommands = true;
        logger.setDebug(true);
    }
    if (args.contains("-debug"))
        logger.setDebug(true);
}

void LobbyInterface::init() {
    #if defined Q_OS_LINUX
        os = "Linux";
    #elif defined Q_OS_WIN32 // Defined on 64-bit Windows too.
        os = "Windows";
    #elif defined Q_OS_MAC
        os = "Mac";
    #else
        #error "Unknown target OS."
    #endif

    if (springHome != "");
    else if (os == "Windows") {
        // This calls SHGetFolderPath(... CSIDL_PERSONAL ...), same as what Spring uses.
        springHome = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString() +
            "/My Games/Spring";
    } else {
        // ~/.spring
        springHome = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString() +
            "/.spring";
    }
    logger.info("springHome is ", springHome);

    try {
        //const char slash = (os == "Windows" ? '\\' : '/');
        const char slash = '/';
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
    } else if (evt->type() == ProcessRunner::ReadEvent::TypeId) {
        auto readEvt = dynamic_cast<ProcessRunner::ReadEvent&>(*evt);
        evalJs("commandStream('" + escapeJs(readEvt.cmd) + "', '" + escapeJs(readEvt.msg) + "')");
        return true;
    } else if (evt->type() == ProcessRunner::TerminateEvent::TypeId) {
        auto termEvt = dynamic_cast<ProcessRunner::TerminateEvent&>(*evt);
        evalJs("commandStream('exit', '" + escapeJs(termEvt.cmd) + "')");
        logger.info("Command finished: ", termEvt.cmd);
        if(processes.count(termEvt.cmd)) {
            processes.find(termEvt.cmd)->second.terminate();
            processes.erase(processes.find(termEvt.cmd));
        }
        return true;
    } else {
        return QObject::event(evt);
    }
}

void LobbyInterface::jsMessage(std::string source, int lineNumber, std::string message) {
    if (message.find("<TASSERVER>") != std::string::npos ||
            message.find("<LOCAL>") != std::string::npos) {
        if (debugNetwork)
            logger.debug(message);
    } else if (message.find("<CMD>") != std::string::npos) {
        if (debugCommands)
            logger.debug(message);
    } else {
        logger.warning(source, ":", lineNumber, " ", message);
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

void LobbyInterface::createScript(QString path, QString script) {
    std::ofstream out(path.toStdString());
    if (out.is_open() && out.good())
        out << script.toStdString();
    else
        logger.error("Could not create script at ", path.toStdString());
}

void LobbyInterface::killCommand(QString qcmdName)
{
    auto cmdName = qcmdName.toStdString();
    logger.info("Killing command: ", cmdName);
    if(processes.count(cmdName)) {
        processes.find(cmdName)->second.terminate();
        processes.erase(processes.find(cmdName));
    }
}

void LobbyInterface::runCommand(QString qcmdName, QStringList cmd) {
    auto cmdName = qcmdName.toStdString();
    if(!processes.count(cmdName)) {
        std::vector<std::string> args;
        for(auto i : cmd)
            args.push_back(i.toStdString());
        logger.info("Running command (", cmdName, "):\n", cmd.join(" ").toStdString());
        try {
            processes.insert(std::make_pair(cmdName, ProcessRunner(this, cmdName, args))).
                first->second.run();
        } catch(boost::system::system_error e) {
            logger.error("Cannot start command: ", cmdName, ": ", e.what());
        }
    }
}

void LobbyInterface::createUiKeys(QString qpath) {
    boost::system::error_code ec;
    std::string path = qpath.toStdString();
    if (!fs::exists({ path }, ec) && ec == 0) {
        logger.info("Creating empty uikeys: ", path);
        std::ofstream out(path);
    }
}

void LobbyInterface::deleteSpringSettings(QString qpath) {
    std::string path = qpath.toStdString();
    if (!qpath.endsWith("springsettings.cfg")) {
        logger.error("deleteSpringSettings(): not a spring settings file: ", path);
        return;
    }
    boost::system::error_code ec;
    fs::remove({ path.c_str() }, ec);
    if (ec == 0)
        logger.info("Deleted spring settings: ", path);
    else
        logger.warning("Couldn't delete spring settings: ", path);
}

void LobbyInterface::writeToFile(QString path, QString line) {
    std::ofstream out(path.toStdString(), std::ios_base::app);
    out << line.toStdString() << std::endl;
}


/* TODO: replace with a function to get replay info because that's how
 * ReadFileMore() was used anyway.
public String ReadFileMore(final String logFile, final int numLines) {
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

int LobbyInterface::sendSomePacket(QString host, unsigned int port, QString msg) {
    return -1;
    /* TODO: This is called from JS but the result is never used. The purpose of
     * learning the local port of a UDP socket is unclear.
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
    */
}

long LobbyInterface::getUserID() {
    for(auto i : QNetworkInterface::allInterfaces()) {
        if((i.flags() & QNetworkInterface::IsUp) &&
                (i.flags() & QNetworkInterface::IsRunning) &&
                !(i.flags() & QNetworkInterface::IsLoopBack) &&
                !i.hardwareAddress().startsWith("00:00:00:00")) { // hax
            logger.debug("Using ", i.hardwareAddress().toStdString(), " mac address in getUserID()");
            std::string str = i.hardwareAddress().toStdString() + "lobby.springrts.com";
            boost::crc_32_type crc;
            // Do we include '\0'? Does it matter?
            crc.process_bytes(str.c_str(), str.size());
            return crc.checksum();
        }
    }
    return 0;
}
