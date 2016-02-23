#include "cef_wrapper.h"
#include "cef_wrapper_internal.h"
#include "common/app.h"
#include <cef_parser.h>

#ifdef OS_LINUX
#include <X11/Xlib.h>
static int errHandler(Display* display, XErrorEvent* evt) {
    return 0;
}
static int ioErrHandler(Display* display) {
    return 0;
}
#endif

// Instaniate global vars.
std::map<std::string,
    std::function<std::string(const std::string&)>>
    Internal::registeredApiFunctions;
std::function<int(const char*, char*, char**)> Internal::appSchemaHandler;
CefRefPtr<CefFrame> Internal::mainFrame;
std::map<int, CefRefPtr<CefV8Value>> Internal::apiCallbacks;
int Internal::callbackId = 0;

void initialize(const char* renderExe, int argc, char** argv) {

#ifdef OS_LINUX
    XSetErrorHandler(&errHandler);
    XSetIOErrorHandler(&ioErrHandler);
#elif OS_WIN
    CefEnableHighDPISupport();
#endif

    CefRefPtr<App> app(new App);
#ifdef OS_WIN
    CefMainArgs args(GetModuleHandle(NULL));
#else
    CefMainArgs args(argc, argv);
#endif

    CefSettings settings;
    settings.no_sandbox = 1;
    CefString(&settings.browser_subprocess_path).FromString(renderExe);
    CefString(&settings.product_version).FromASCII("cef-wrapper");

    CefInitialize(args, settings, app, NULL);
}

void deinitialize() {
    CefShutdown();
    Internal::registeredApiFunctions.clear();
    Internal::appSchemaHandler = std::function<int(const char*, char*, char**)>();
    Internal::mainFrame = NULL;
}

void startMessageLoop(const char* urlStr, const char* bgColor, int fullscreen) {
    CefWindowInfo info;
    Platform::setWindowInfo(info);

    CefBrowserSettings settings;
    CefString(&settings.default_encoding).FromASCII("UTF-8");
    settings.javascript_access_clipboard = STATE_ENABLED;
    settings.javascript_close_windows = STATE_ENABLED;
    settings.universal_access_from_file_urls = STATE_ENABLED;
    CefParseCSSColor(bgColor, false, settings.background_color);

    auto url = CefCommandLine::GetGlobalCommandLine()->GetSwitchValue("url");
    if (url.empty())
        url = urlStr;
    auto browser = CefBrowserHost::CreateBrowserSync(info, new Client, url, settings, NULL);
    Platform::setWindowTitle(browser->GetHost()->GetWindowHandle(), "Loading...");
    Platform::showWindow(browser->GetHost()->GetWindowHandle(), fullscreen != 0);

    CefRunMessageLoop();
}

void registerAppSchemaHandler(int (*handler)(const char* url, char* type, char** data)) {
    Internal::appSchemaHandler = handler;
}

void registerApiFunction(const char* name, const char* (*handler)(const char* jsonArgs)) {
    Internal::registeredApiFunctions[name] = [=](const std::string& jsonArgs) -> std::string {
        return handler(jsonArgs.c_str());
    };
}

void executeJavascript(const char* code) {
    if (Internal::mainFrame && Internal::mainFrame->IsValid())
        Internal::mainFrame->ExecuteJavaScript(code, Internal::mainFrame->GetURL(), 0);
}

void App::OnContextInitialized() {
    CefRegisterSchemeHandlerFactory("cef", "", new AppSchemeFactory);
}
