#include <curl/curl.h>
#include <cef_app.h>

class App : public CefApp
{
public:
    App() {};
private:
    IMPLEMENT_REFCOUNTING(App)
};

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    CefMainArgs args(argc, argv);
    CefExecuteProcess(args, new App, NULL);
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
