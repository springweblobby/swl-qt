#ifndef LOBBYINTERFACE_H
#define LOBBYINTERFACE_H

#include "logger.h"
#include "ufstream.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <functional>
#include <map>
#include <cef_v8.h>

// XXX
class QWebFrame;

class ProcessRunner {
public:
    ProcessRunner(QObject* eventReceiver, Logger& logger, const std::string& cmd, const std::vector<std::wstring>& args);
    ProcessRunner(ProcessRunner&&);
    ProcessRunner(const ProcessRunner&) = delete;
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
        TerminateEvent(std::string cmd, int retCode) : QEvent(QEvent::Type(TypeId)), cmd(cmd), returnCode(retCode) {}
        std::string cmd;
        int returnCode;
        static const int TypeId = QEvent::User + 4; // QEvent::registerEventType() is evil black magic!
    };
private:
    void runService();
    QObject* eventReceiver;
    Logger& logger;
    std::string cmd;
    std::vector<std::wstring> args;
    std::function<void()> terminate_func;
    int returnCode;
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
        static const int TypeId = QEvent::User + 1; // watch ma, no hands! magic! (hides saw)
    };
    // This event represents a socket error. The socket is always closed before this event is posted.
    struct ErrorEvent : QEvent {
        ErrorEvent(std::string reason) : QEvent(QEvent::Type(TypeId)), reason(reason) {}
        std::string reason;
        static const int TypeId = QEvent::User + 7; // lucky magic number
    };
private:
    void runService();
    void onRead(const boost::system::error_code&, std::size_t);
    boost::asio::io_service service;
    boost::asio::io_service::work* work;
    boost::asio::ip::udp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf readBuf;
    boost::thread thread;
    QObject* eventReceiver;
    Logger& logger;
};

class LobbyInterface {
public:
    LobbyInterface(CefRefPtr<CefV8Context> context);
    ~LobbyInterface();
    bool event(QEvent* evt);

    // This is posted for asynchronous HTTP downloads.
    struct DownloadEvent : QEvent {
        DownloadEvent(std::string name, std::string msg) : QEvent(QEvent::Type(TypeId)), name(name), msg(msg) {}
        std::string name, msg;
        static const int TypeId = QEvent::User + 6; // grep for 'magic' to check for conflicts
    };

    void init();
    std::string listDirs(std::string path);
    std::string listFiles(std::string path);
    std::string readFileLess(std::string path, unsigned int lines);
    void writeToFile(std::string path, std::string line);
    void createScript(std::string path, std::string script);
    void createUiKeys(std::string path);
    void deleteSpringSettings(std::string path);

    //QObject* getUnitsync(std::string path);
    //QObject* getUnitsyncAsync(std::string path);

    void killCommand(std::string cmdName);
    //bool runCommand(std::string cmdName, std::stringList args);

    void connect(std::string host, unsigned int port);
    void disconnect();
    void send(std::string msg);
    bool downloadFile(std::string url, std::string target);
    void startDownload(std::string name, std::string url, std::string file, bool checkIfModified);
    unsigned int getUserID();
    int sendSomePacket(std::string host, unsigned int port, std::string msg);

    std::string getSpringHome();
    std::string readSpringHomeSetting();
    void writeSpringHomeSetting(std::string path);
    // The version number is major * 100 + minor.
    // major is incremented with every breaking change in the API.
    int getApiVersion() { return 105; }
private:
    /*QString listFilesPriv(QString path, bool dirs);
    void evalJs(const std::string&);
    std::string escapeJs(const std::string&);
    void move(const boost::filesystem::path& from, const boost::filesystem::path& to);
    bool downloadFile(QString name, QString qurl, QString qtarget, bool checkIfModified, QObject* eventReceiver);*/

    CefRefPtr<CefV8Context> context;
    std::string os;
    boost::filesystem::path springHome;
    boost::filesystem::path springHomeSetting;
    boost::filesystem::path executablePath;
    Logger logger;
    NetworkHandler network;

    //std::map<boost::filesystem::path, UnitsyncHandler> unitsyncs;
    //std::map<boost::filesystem::path, UnitsyncHandlerAsync> unitsyncs_async;
    std::map<std::string, ProcessRunner> processes;
    // This doesn't ever get cleared for simplicity on the presumption that
    // there are never enough downloads for that to matter.
    std::vector<boost::thread> downloadThreads;
};

// utf-8 string to utf-16 (on windows).
inline std::wstring toStdWString(const std::string& str) {
    return CefString(str).ToWString();
}
// the opposite
inline std::string toStdString(const std::wstring& str) {
    return CefString(str).ToString();
}

#endif // LOBBYINTERFACE_H
