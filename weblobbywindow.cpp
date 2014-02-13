// On mingw lobbyinterface.h must be included first or you'll get compilation errors.
#include "lobbyinterface.h"

#include "weblobbywindow.h"
#include <iostream>
#include <QtNetwork>
#include <QDesktopServices>


void MyPage::javaScriptConsoleMessage(const QString& message, int lineNumber,
                                      const QString& sourceID) {
    if (*lobbyInterface)
        (*lobbyInterface)->jsMessage(sourceID.toStdString(), lineNumber, message.toStdString());
}

WebLobbyWindow::WebLobbyWindow(QWidget *parent) : QMainWindow(parent) {
    progress = 0;
    lobbyInterface = NULL;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view->setPage(new MyPage(view, &lobbyInterface)); // dat hax
    view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    QWebSettings* settings = view->page()->settings();
    settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);

    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    auto args = QCoreApplication::arguments();
    if (args.contains("-url") && args.indexOf("-url") + 1 < args.length())
        view->load(QUrl(args[args.indexOf("-url") + 1]));
    else
        view->load(QUrl("http://weblobby.springrts.com/test-qtcpp/index.html"));

    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJSObject()));
    connect(view, &QWebView::linkClicked, [](const QUrl& url){
        QDesktopServices::openUrl(url);
    });
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
    if (lobbyInterface)
        delete lobbyInterface;
    lobbyInterface = new LobbyInterface(this, view->page()->mainFrame());
    view->page()->mainFrame()->addToJavaScriptWindowObject("QWeblobbyApplet", lobbyInterface);
}
