#include <QApplication>
#include <curl/curl.h>
#ifdef Q_OS_LINUX
    #include <mpg123.h>
#endif
#include "weblobbywindow.h"

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    #ifdef Q_OS_LINUX
        mpg123_init();
    #endif
    QApplication app(argc, argv);

    WebLobbyWindow webLobbyWindow;
    #if defined Q_OS_WINDOWS
        webLobbyWindow.setWindowIcon(app.windowIcon());
    #else
        webLobbyWindow.setWindowIcon(QIcon("icon.png"));
    #endif
    webLobbyWindow.showMaximized();

    auto exitCode = app.exec();
    #ifdef Q_OS_LINUX
        mpg123_exit();
    #endif
    curl_global_cleanup();
    return exitCode;
}
