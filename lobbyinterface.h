#ifndef LOBBYINTERFACE_H
#define LOBBYINTERFACE_H

#include <QObject>
#include <QEvent>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class QWebFrame;

class NetworkHandler {
public:
    void connect(std::string host, unsigned int port);
    void disconnect();
    void send(std::string msg);

    NetworkHandler(QObject* eventReceiver);
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
    
    QString listDirs(QString path);
    QString listFiles(QString path);
    QString readFileLess(QString path, unsigned int lines);
    void writeToFile(QString path, QString line);

    void connect(QString host, unsigned int port);
    void disconnect();
    void send(QString msg);
    bool downloadFile(QString source, QString target);
    long getUserID();

    QString getSpringHome();
    void setSpringHome(QString path);
private:
    void setOs(std::string os);
    QString listFilesPriv(QString path, bool dirs);
    void evalJs(const std::string&);
    std::string escapeJs(const std::string&);

    std::string os;
    std::string springHome;
    NetworkHandler network;
    QWebFrame* frame;
};

#endif // LOBBYINTERFACE_H
