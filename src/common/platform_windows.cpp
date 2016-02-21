#include "platform.h"
#include <windows.h>
#include <cef_browser.h>


void Platform::setWindowInfo(CefWindowInfo& info) {
    info.SetAsPopup(NULL, "cefwindow");
    info.style &= ~WS_VISIBLE;
}

void Platform::setWindowTitle(CefWindowHandle handle, std::string title) {
    SetWindowText(handle, title.c_str());
}

void Platform::showWindow(CefWindowHandle handle, bool fullscreen) {
    if (fullscreen) {
        SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
        MONITORINFO info;
        info.cbSize = sizeof(info);
        GetMonitorInfo(MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST), &info);
        SetWindowPos(handle, HWND_TOP, info.rcMonitor.left, info.rcMonitor.top,
            (info.rcMonitor.right - info.rcMonitor.left), (info.rcMonitor.bottom - info.rcMonitor.top),
            SWP_FRAMECHANGED);
        ShowWindow(handle, SW_SHOW);
    } else {
        ShowWindow(handle, SW_SHOWMAXIMIZED);
    }
}
