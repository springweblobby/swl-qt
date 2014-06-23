#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QtWebKitWidgets>

class QWebView;
class LobbyInterface;

class MyPage : public QWebPage {
public:
    MyPage(QObject* parent, LobbyInterface** iface) : QWebPage(parent), lobbyInterface(iface) {}
protected:
    void javaScriptConsoleMessage(const QString& message, int lineNumber,
                                  const QString& sourceID);
    LobbyInterface** lobbyInterface; // double pointers all the way across the sky
};

class WebLobbyWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit WebLobbyWindow(QWidget *parent = 0);

protected slots:

    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

    void addJSObject();
private:


    QWebView *view;
    LobbyInterface *lobbyInterface;
    int progress;
};
