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

private:
    void setOs(std::string os);

    std::string os;
};

#endif // LOBBYINTERFACE_H
