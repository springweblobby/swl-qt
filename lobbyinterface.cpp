#include "lobbyinterface.h"
#include <QSysInfo>

LobbyInterface::LobbyInterface(QObject *parent) :
    QObject(parent) {
}

void LobbyInterface::init() {
    #ifdef Q_OS_LINUX
        setOs("Linux");
    #elif Q_OS_MSDOS
        setOs("Windows");
    #elif Q_OS_MAC
        setOS("Mac");
    #endif
}

void LobbyInterface::setOs(std::string os) {
    this->os = os;
    /* TODO: convert following code into C++
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
    f.mkdirs();*/
}
