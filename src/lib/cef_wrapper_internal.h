#ifndef _CEF_WRAPPER_INTERNAL_H
#define _CEF_WRAPPER_INTERNAL_H

#include <cef_frame.h>
#include <map>

namespace Internal {
    extern std::map<std::string,
        std::function<std::string(const std::string&)>>
        registeredApiFunctions;
    extern std::map<std::string,
        std::function<int(const std::string&, char**)>>
        registeredSchemaHandlers;
    extern CefRefPtr<CefFrame> mainFrame;
}

#endif // _CEF_WRAPPER_INTERNAL_H
