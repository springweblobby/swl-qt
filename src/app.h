#ifndef _APP_H
#define _APP_H

#include <cef_app.h>

// Remember that BrowserProcessHandler and CefRenderProcessHandler methods run in
// separate processes, so data has to be shared explicitly.

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
    void OnContextCreated(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, CefRefPtr<CefV8Context>) override;
    // TODO Render::OnUncaughtException
    // TODO App::OnRegisterCustomSchemes
    // TODO Render::OnBeforeNavigation

    void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override {
        if (!mainBrowser)
            mainBrowser = browser;
    }
    void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override {
        if (browser->IsSame(mainBrowser))
            mainBrowser = NULL;
    }
private:
    CefRefPtr<CefBrowser> mainBrowser; // only available in render process
    IMPLEMENT_REFCOUNTING(App)
};

#endif // _APP_H
