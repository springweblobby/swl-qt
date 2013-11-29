#ifndef LOBBYINTERFACE_H
#define LOBBYINTERFACE_H

#include <QObject>

class LobbyInterface : public QObject {
    Q_OBJECT
public:
    explicit LobbyInterface(QObject *parent = 0);
signals:
public slots:
    //add public functions here

    void init();
    std::string listDirs(std::string path);
    std::string listFiles(std::string path);
    std::string listFilesPriv(std::string path, bool dirs);

    std::string getSpringHome();
    void setSpringHome(std::string path);
private:
    void setOs(std::string os);

    std::string os;
    std::string springHome;
};

#endif // LOBBYINTERFACE_H
