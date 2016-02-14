#include <curl/curl.h>
#include "common/app.h"

#ifdef OS_LINUX
#include <X11/Xlib.h>
#endif

int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);

#ifdef OS_LINUX
    auto errHandler = [](Display* display, XErrorEvent* evt) -> int {
        return 0;
    };
    auto ioErrHandler = [](Display* display) -> int {
        return 0;
    };
    XSetErrorHandler(errHandler);
    XSetIOErrorHandler(ioErrHandler);
#endif

    CefRefPtr<App> app(new App);
    CefMainArgs args(argc, argv);

    int exitCode = CefExecuteProcess(args, app, NULL);
    if (exitCode > 0)
        return exitCode;

    CefSettings settings;
    settings.no_sandbox = 1;
    CefString(&settings.product_version).FromASCII("weblobby");

    CefInitialize(args, settings, app, NULL);
    CefRunMessageLoop();
    CefShutdown();
    curl_global_cleanup();

    return 0;

    /*WebLobbyWindow webLobbyWindow;
    #if defined Q_OS_WINDOWS
        webLobbyWindow.setWindowIcon(app.windowIcon());
    #else
        webLobbyWindow.setWindowIcon(QIcon("icon.png"));
    #endif
    webLobbyWindow.showMaximized();

    auto exitCode = app.exec();
    return exitCode;*/
}
