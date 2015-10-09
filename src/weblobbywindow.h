#pragma once
// On mingw lobbyinterface.h must be included first or you'll get compilation errors.
#include "lobbyinterface.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QtWebKitWidgets>
#if defined Q_OS_MAC
    #include <QtWebKit>
    #include <QWebPage>
#endif

class QWebView;

class MyPage : public QWebPage {
public:
    MyPage(QObject* parent);
protected:
    void javaScriptConsoleMessage(const QString& message, int lineNumber,
                                  const QString& sourceID);
    LobbyInterface lobbyInterface;
};

class WebLobbyWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit WebLobbyWindow(QWidget *parent = 0);
    QWebView* getView() { return view; }

protected slots:

    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);
private:


    QWebView *view;
    int progress;
};

class F5Filter : public QObject {
    Q_OBJECT
public:
    F5Filter(QObject* parent) : QObject(parent) {}
    bool eventFilter(QObject* obj, QEvent* evt);
};
