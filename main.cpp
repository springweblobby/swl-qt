#include <QApplication>
#include <curl/curl.h>
#include "weblobbywindow.h"

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    QApplication app(argc, argv);

    WebLobbyWindow webLobbyWindow;
    webLobbyWindow.showMaximized();

    auto exitCode = app.exec();
    curl_global_cleanup();
    return exitCode;
}
