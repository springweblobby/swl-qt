#include <curl/curl.h>
#include "common/app.h"
#include <cef_parser.h>

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
#elif OS_WIN
    CefEnableHighDPISupport();
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

void App::OnContextInitialized() {
    CefWindowInfo info;
    Platform::setWindowInfo(info);

    CefRegisterSchemeHandlerFactory("app", "", new AppSchemeFactory);
    
    CefBrowserSettings settings;
    CefString(&settings.default_encoding).FromASCII("UTF-8");
    settings.javascript_access_clipboard = STATE_ENABLED;
    settings.javascript_close_windows = STATE_ENABLED;
    settings.universal_access_from_file_urls = STATE_ENABLED;
    CefParseCSSColor("black", false, settings.background_color);

    auto url = CefCommandLine::GetGlobalCommandLine()->GetSwitchValue("url");
    if (url.empty())
        url = "app://";
    auto browser = CefBrowserHost::CreateBrowserSync(info, new Client, url, settings, NULL);
    Platform::setWindowTitle(browser->GetHost()->GetWindowHandle(), "Loading...");
    Platform::showWindow(browser->GetHost()->GetWindowHandle(), !CefCommandLine::GetGlobalCommandLine()->
        HasSwitch("no-fullscreen"));
}
