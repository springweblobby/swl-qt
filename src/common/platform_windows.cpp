#include "platform.h"
#include <windows.h>
#include <cef_base.h>


void Platform::setWindowTitle(CefWindowHandle handle, std::string title) {
    SetWindowText(handle, title.c_str());
}

void Platform::maximizeWindow(CefWindowHandle handle, bool maximize) {
    ShowWindow(handle, SW_MAXIMIZE);
}

void Platform::makeWindowFullscreen(CefWindowHandle handle, bool fullscreen) {
}
