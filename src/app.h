#ifndef _APP_H
#define _APP_H

#include <cef_app.h>

class App :
    public CefApp,
    public CefBrowserProcessHandler,
    public CefRenderProcessHandler {
public:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnContextInitialized() override;
    // TODO Render::OnUncaughtException
    // TODO App::OnRegisterCustomSchemes
    // TODO Render::OnBeforeNavigation
private:
    CefRefPtr<CefBrowser> mainBrowser;
    IMPLEMENT_REFCOUNTING(App)
};

#endif // _APP_H
