#ifndef LOBBYINTERFACE_H
#define LOBBYINTERFACE_H

#include "logger.h"
#include "unitsynchandler.h"
#include <QObject>
#include <QEvent>
#include <QStringList>
#if defined Q_OS_WIN32 || defined Q_OS_MAC
    #include <QMediaPlayer>
#elif defined Q_OS_LINUX
    #include <unistd.h>
    #include <libgen.h>
#endif
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <functional>
#include <map>

class QWebFrame;

class ProcessRunner {
public:
    ProcessRunner(QObject* eventReceiver, const std::string& cmd, const std::vector<std::wstring>& args);
    ProcessRunner(ProcessRunner&&);
    ~ProcessRunner();

    // Throws boost::system::system_error on failure.
    void run();
    // Throws boost::system::system_error on failure.
    void terminate();

    // This event is posted to eventReceiver when the underlying process
    // writes a line into stdout.
    struct ReadEvent : QEvent {
        ReadEvent(std::string cmd, std::string msg) : QEvent(QEvent::Type(TypeId)), cmd(cmd), msg(msg) {}
        std::string cmd;
        std::string msg;
        static const int TypeId = QEvent::User + 3; // more magic numbers
    };
    // This event is posted when the process terminates.
    struct TerminateEvent : QEvent {
        TerminateEvent(std::string cmd) : QEvent(QEvent::Type(TypeId)), cmd(cmd) {}
        std::string cmd;
        static const int TypeId = QEvent::User + 4; // even more magic numbers
    };
private:
    void runService();
    QObject* eventReceiver;
    std::string cmd;
    std::vector<std::wstring> args;
    std::function<void()> terminate_func;
    boost::asio::io_service service;
    boost::thread runServiceThread, waitForExitThread;
};

class NetworkHandler {
public:
    void connect(std::string host, unsigned int port);
    void disconnect();
    void send(std::string msg);

    NetworkHandler(QObject* eventReceiver, Logger& logger);
    ~NetworkHandler();

    // This event is posted to eventReceiver when some data arrives in the socket.
    struct ReadEvent : QEvent {
        ReadEvent(std::string msg) : QEvent(QEvent::Type(TypeId)), msg(msg) {}
        std::string msg;
        static const int TypeId = QEvent::User + 1;
    };
private:
    void runService();
    void onRead(const boost::system::error_code&, std::size_t);
    boost::asio::io_service service;
    boost::asio::io_service::work* work;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf readBuf;
    boost::thread thread;
    QObject* eventReceiver;
    Logger& logger;
};

class LobbyInterface : public QObject {
    Q_OBJECT
public:
    explicit LobbyInterface(QObject *parent, QWebFrame *frame);
    ~LobbyInterface() {
        network.disconnect();
    }
    bool event(QEvent* evt);
signals:
public slots:
    //add public functions here

    void init();
    void jsMessage(std::string source, int line, std::string msg);

    QString listDirs(QString path);
    QString listFiles(QString path);
    QString readFileLess(QString path, unsigned int lines);
    void writeToFile(QString path, QString line);
    void createScript(QString path, QString script);
    void createUiKeys(QString path);
    void deleteSpringSettings(QString path);

    QObject* getUnitsync(QString path);

    void killCommand(QString cmdName);
    void runCommand(QString cmdName, QStringList args);

    void connect(QString host, unsigned int port);
    void disconnect();
    void send(QString msg);
    bool downloadFile(QString url, QString target);
    unsigned int getUserID();
    int sendSomePacket(QString host, unsigned int port, QString msg);

    void playSound(QString url);

    QString getSpringHome();
    void setSpringHome(QString path);
    int getApiVersion() { return 1; } // TODO: hook to have revision in here?
private:
    QString listFilesPriv(QString path, bool dirs);
    void evalJs(const std::string&);
    std::string escapeJs(const std::string&);

    std::string os;
    boost::filesystem::path springHome;
    Logger logger;
    bool debugNetwork, debugCommands;
    NetworkHandler network;
    #if defined Q_OS_WIN32 || defined Q_OS_MAC
        QMediaPlayer mediaPlayer;
    #elif defined Q_OS_LINUX
        QString gstreamerPlayPath;
    #endif

    QWebFrame* frame;
    std::map<boost::filesystem::path, UnitsyncHandler> unitsyncs;
    std::map<std::string, ProcessRunner> processes;
};

#endif // LOBBYINTERFACE_H
