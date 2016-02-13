#include <curl/curl.h>
#include "app.h"

int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);

    CefRefPtr<App> app(new App);
    CefMainArgs args(argc, argv);

    int exitCode = CefExecuteProcess(args, app.get(), NULL);
    if (exitCode > 0)
        return exitCode;

    CefSettings settings;
    settings.no_sandbox = 1;
    CefString(&settings.product_version).FromASCII("weblobby");

    CefInitialize(args, settings, app.get(), NULL);
    CefRunMessageLoop();
    CefShutdown();

    return 0;

    /*WebLobbyWindow webLobbyWindow;
    #if defined Q_OS_WINDOWS
        webLobbyWindow.setWindowIcon(app.windowIcon());
    #else
        webLobbyWindow.setWindowIcon(QIcon("icon.png"));
    #endif
    webLobbyWindow.showMaximized();

    auto exitCode = app.exec();
    curl_global_cleanup();
    return exitCode;*/
}
