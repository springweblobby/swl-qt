#ifndef _PLATFORM_H
#define _PLATFORM_H

// All platform-specific functions in one place.

#include <cef_base.h>

namespace Platform {

    void setWindowInfo(CefWindowInfo& info);
    void setWindowTitle(CefWindowHandle handle, std::string title);
    void showWindow(CefWindowHandle handle, bool fullscreen);

}

#endif // _PLATFORM_H
