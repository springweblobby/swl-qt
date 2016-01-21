#include "weblobbywindow.h"
#include <iostream>
#include <QtNetwork>
#include <QDesktopServices>
#include <QThread>


MyPage::MyPage(QObject* parent) : QWebPage(parent), lobbyInterface(parent, mainFrame()) {
    connect(mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        mainFrame()->addToJavaScriptWindowObject("QWeblobbyApplet", &lobbyInterface);
    });

    QWebSettings* settings = this->settings();
    settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
}

void MyPage::javaScriptConsoleMessage(const QString& message, int lineNumber,
                                      const QString& sourceID) {
    lobbyInterface.jsMessage(sourceID.toStdString(), lineNumber, message.toStdString());
}

WebLobbyWindow::WebLobbyWindow(QWidget *parent) : QMainWindow(parent) {
    progress = 0;
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view->setPage(new MyPage(view));
    view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    view->installEventFilter(new F5Filter(this));
    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    auto args = QCoreApplication::arguments();
    if (args.contains("-url") && args.indexOf("-url") + 1 < args.length())
        view->load(QUrl(args[args.indexOf("-url") + 1]));
    else
        view->load(QUrl("http://weblobby.springrts.com/qt/index.html"));

    connect(view, &QWebView::linkClicked, [](const QUrl& url){
        QDesktopServices::openUrl(url);
    });
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
}

bool F5Filter::eventFilter(QObject* obj, QEvent* evt) {
    if (evt->type() == QEvent::KeyPress) {
        auto keyEvt = static_cast<QKeyEvent&>(*evt);
        if (keyEvt.key() == Qt::Key_F5) {
            auto view = dynamic_cast<QWebView*>(obj);
            QUrl url = view->page()->mainFrame()->url();
            view->setPage(new MyPage(view));
            // Give WebKit threads some time to catch up, prevents random hangs.
            QThread::msleep(500);
            view->load(url);
            return true;
		} else {
            return QObject::eventFilter(obj, evt);
        }
    } else {
        return QObject::eventFilter(obj, evt);
    }
}
