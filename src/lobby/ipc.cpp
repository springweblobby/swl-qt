#include "common/app.h"

// Render process received IPC message.
bool App::OnProcessMessageReceived(CefRefPtr<CefBrowser>,
        CefProcessId, CefRefPtr<CefProcessMessage>) {
    return false;
}

// Browser process received IPC message.
bool ClientBase::OnProcessMessageReceived(CefRefPtr<CefBrowser>,
        CefProcessId, CefRefPtr<CefProcessMessage>) {
    return false;
}
