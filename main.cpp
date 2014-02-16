#include <QApplication>
#include <curl/curl.h>
#include "weblobbywindow.h"

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    QApplication app(argc, argv);

    WebLobbyWindow webLobbyWindow;
    #if defined Q_OS_WINDOWS
        webLobbyWindow.setWindowIcon(app.windowIcon());
    #elif defined Q_OS_LINUX
        webLobbyWindow.setWindowIcon(QIcon("icon.png"));
    #endif
    webLobbyWindow.showMaximized();

    auto exitCode = app.exec();
    curl_global_cleanup();
    return exitCode;
}
