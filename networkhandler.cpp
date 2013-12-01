#include "lobbyinterface.h"
#include <QCoreApplication>

namespace asio = boost::asio;
namespace ip = asio::ip;

void NetworkHandler::connect(std::string host, unsigned int port) {
    try {
        auto it = resolver.resolve({ host, std::to_string(port) });
        socket.connect({ it->endpoint().address(), it->endpoint().port() });
    } catch(boost::system::system_error e) {
        // TODO
    }
    asio::async_read_until(socket, readBuf, '\n', boost::bind(&NetworkHandler::onRead, this, _1, _2));
}

// Called in the network thread.
void NetworkHandler::onRead(const boost::system::error_code& ec, std::size_t bytes) {
    if(!ec) {
        std::istream is(&readBuf);
        std::string msg;
        std::getline(is, msg);
        QCoreApplication::postEvent(eventReceiver, new ReadEvent(msg));
        asio::async_read_until(socket, readBuf, '\n', boost::bind(&NetworkHandler::onRead, this, _1, _2));
    } else {
        // TODO
    }
}

void NetworkHandler::send(std::string msg) {
    try {
        asio::write(socket, boost::asio::buffer(msg));
    } catch(boost::system::system_error e) {
        // TODO
    }
}

void NetworkHandler::disconnect() {
    socket.close();
}

void NetworkHandler::runService() {
    service.run();
}

NetworkHandler::NetworkHandler(QObject* eventReceiver) : resolver(service), socket(service), eventReceiver(eventReceiver) {
    work = new asio::io_service::work(service);
    thread = boost::thread(boost::bind(&NetworkHandler::runService, this));
}

NetworkHandler::~NetworkHandler() {
    delete work;
    thread.join();
}
