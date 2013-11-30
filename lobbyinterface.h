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
    QString listDirs(QString path);
    QString listFiles(QString path);
    QString readFileLess(QString path, unsigned int lines);

    bool downloadFile(QString source, QString target);

    QString getSpringHome();
    void setSpringHome(QString path);
private:
    void setOs(std::string os);
    QString listFilesPriv(QString path, bool dirs);

    std::string os;
    std::string springHome;
};

#endif // LOBBYINTERFACE_H
