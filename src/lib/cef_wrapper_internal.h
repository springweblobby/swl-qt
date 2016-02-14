#ifndef _CEF_WRAPPER_INTERNAL_H
#define _CEF_WRAPPER_INTERNAL_H

#include <cef_frame.h>
#include <cef_v8.h>
#include <map>

namespace Internal {

    // Only available in the browser process.
    extern std::map<std::string,
        std::function<std::string(const std::string&)>>
        registeredApiFunctions;
    extern std::function<int(const std::string&, char**)> appSchemaHandler;
    extern CefRefPtr<CefFrame> mainFrame;

    // Only available in the render process.
    extern std::map<int, CefRefPtr<CefV8Value>> apiCallbacks;
    extern int callbackId;
}

#endif // _CEF_WRAPPER_INTERNAL_H
