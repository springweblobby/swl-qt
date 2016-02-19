#include "cef_wrapper.h"
#include "cef_wrapper_internal.h"
#include "common/app.h"

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
std::function<int(const std::string&, char**)> Internal::appSchemaHandler;
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
    Internal::appSchemaHandler = std::function<int(const std::string&, char**)>();
    Internal::mainFrame = NULL;
}

void setWindowProperties() {
    // TODO
}

void startMessageLoop() {
    CefRunMessageLoop();
}

void registerAppSchemaHandler(int (*handler)(const char* url, char** data)) {
    Internal::appSchemaHandler = [=](const std::string& url, char** data) -> int {
        return handler(url.c_str(), data);
    };
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
