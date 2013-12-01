#include <QtNetwork>
#include <iostream>

#include "weblobbywindow.h"
#include "lobbyinterface.h"

void MyPage::javaScriptConsoleMessage(const QString& message, int lineNumber,
                                      const QString& sourceID) {
    qDebug() << sourceID << ":" << lineNumber << " " << message;
}

WebLobbyWindow::WebLobbyWindow(QWidget *parent) : QMainWindow(parent) {
    resize(1280, 800);
    progress = 0;
    lobbyInterface = NULL;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view->setPage(new MyPage());
    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    view->load(QUrl("http://weblobby.springrts.com/desktop/index.html"));
    //view->load(QUrl("file:///home/user/dev/swl-qtcpp/site/index.html"));
    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJSObject()));


    /*viewer.setOrientation(Html5ApplicationViewer::ScreenOrientationAuto);
    viewer.showExpanded();
    viewer.loadUrl(QUrl(QLatin1String("http://weblobby.springrts.com/desktop/index.html")));*/
}

void WebLobbyWindow::viewSource() {
    QNetworkAccessManager* accessManager = view->page()->networkAccessManager();
    QNetworkRequest request(view->url());
    QNetworkReply* reply = accessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(slotSourceDownloaded()));
}

void WebLobbyWindow::adjustTitle() {
    if (progress <= 0 || progress >= 100) {
        setWindowTitle(view->title());
    } else {
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
    }
}

void WebLobbyWindow::setProgress(int p) {
    progress = p;
    adjustTitle();
}

void WebLobbyWindow::finishLoading(bool) {
    progress = 100;
    adjustTitle();
    //view->page()->mainFrame()->evaluateJavaScript(jQuery);

    //rotateImages(rotateAction->isChecked());
}

void WebLobbyWindow::addJSObject() {
    // Create a new LobbyInterface every time a page loads. This is done so
    // that reloading the page doesn't drag over the old state.
    if(lobbyInterface)
        delete lobbyInterface;
    lobbyInterface = new LobbyInterface(this, view->page()->mainFrame());
    view->page()->mainFrame()->addToJavaScriptWindowObject("QWeblobbyApplet", lobbyInterface);
}
