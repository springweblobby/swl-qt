#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QtWebKitWidgets>

//!!
//FIXME: We are currently not using the HTML5ApplicationViewer,
//but the project includes it anyway. Either make use of it or discard it.
//#include "html5applicationviewer.h"

class QWebView;
class LobbyInterface;

class MyPage : public QWebPage {
protected:
    void javaScriptConsoleMessage(const QString& message, int lineNumber,
                                  const QString& sourceID);
};

class WebLobbyWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit WebLobbyWindow(QWidget *parent = 0);

protected slots:

    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

    void viewSource();

    void addJSObject();
private:


    QWebView *view;
    LobbyInterface *lobbyInterface;
    int progress;
};
