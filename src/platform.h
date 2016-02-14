#ifndef _PLATFORM_H
#define _PLATFORM_H

// All platform-specific functions in one place.

#include <cef_base.h>

namespace Platform {

    void setWindowTitle(CefWindowHandle handle, std::string title);
    void maximizeWindow(CefWindowHandle handle, bool maximize = true);
    void makeWindowFullscreen(CefWindowHandle handle, bool fullscreen = true);

}

#endif // _PLATFORM_H
