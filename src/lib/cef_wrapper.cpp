#include "cef_wrapper.h"
#include "cef_wrapper_internal.h"
#include "common/app.h"

#ifdef OS_LINUX
#include <X11/Xlib.h>
#endif

// Instaniate global vars.
std::map<std::string,
    std::function<std::string(const std::string&)>>
    Internal::registeredApiFunctions;
std::map<std::string,
    std::function<int(const std::string&, char**)>>
    Internal::registeredSchemaHandlers;
CefRefPtr<CefFrame> Internal::mainFrame;

void initialize(const char* renderExe, int argc, char** argv) {

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

    CefSettings settings;
    settings.no_sandbox = 1;
    CefString(&settings.product_version).FromASCII("cef-wrapper");

    CefInitialize(args, settings, app, NULL);
}

void deinitialize() {
    CefShutdown();
    Internal::registeredApiFunctions.clear();
    Internal::registeredSchemaHandlers.clear();
    Internal::mainFrame = NULL;
}

void setWindowProperties() {
    // TODO
}

void startMessageLoop() {
    CefRunMessageLoop();
}

void registerCustomSchemaHandler(const char* schema, int (*handler)(const char* url, char** data)) {
    Internal::registeredSchemaHandlers[schema] = [=](const std::string& url, char** data) -> int {
        return handler(url.c_str(), data);
    };
}

void registerApiFunction(const char* name, const char* (*handler)(const char* jsonArgs)) {
    Internal::registeredApiFunctions[name] = [=](const std::string& jsonArgs) -> std::string {
        return handler(jsonArgs.c_str());
    };
}

void executeJavascript(const char* code) {
    if (Internal::mainFrame)
        Internal::mainFrame->ExecuteJavaScript(code, Internal::mainFrame->GetURL(), 0);
}