#include "platform.h"
#include <X11/Xlib.h>

#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2

static Atom atom(const char* name) {
    return XInternAtom(cef_get_xdisplay(), name, false);
}

static void send_NET_WM_event(Window window, int32 action, int32 first, int32 second) {
    Display* display = cef_get_xdisplay();
    XEvent evt;
    memset(&evt, 0, sizeof(evt));
    evt.type = ClientMessage;
    evt.xclient.window = window;
    evt.xclient.message_type = atom("_NET_WM_STATE");
    evt.xclient.format = 32;
    evt.xclient.data.l[0] = action;
    evt.xclient.data.l[1] = first;
    evt.xclient.data.l[2] = second;
    XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &evt);
}

void Platform::setWindowTitle(CefWindowHandle handle, std::string title) {
    Display* display = cef_get_xdisplay();
    Window window = handle;
    XChangeProperty(display, window, atom("_NET_WM_NAME"), atom("UTF8_STRING"), 8,
        PropModeReplace, (const unsigned char*)title.c_str(), title.size());
}

void Platform::maximizeWindow(CefWindowHandle handle, bool maximize) {
    send_NET_WM_event(handle, maximize ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE,
        atom("_NET_WM_STATE_MAXIMIZED_HORZ"), atom("_NET_WM_STATE_MAXIMIZED_VERT"));
}

void Platform::makeWindowFullscreen(CefWindowHandle handle, bool fullscreen) {
    send_NET_WM_event(handle, fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE,
        atom("_NET_WM_STATE_FULLSCREEN"), 0);
}
