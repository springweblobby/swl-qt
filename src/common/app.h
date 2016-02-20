#ifndef _APP_H
#define _APP_H

#include <cef_app.h>
#include <cef_client.h>
#include <cef_scheme.h>

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
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser>, CefProcessId,
        CefRefPtr<CefProcessMessage>) override;
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
    void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
        registrar->AddCustomScheme("app", false, true, false);
    }
private:
    CefRefPtr<CefBrowser> mainBrowser; // only available in render process
    IMPLEMENT_REFCOUNTING(App)
};

class ClientBase : public CefClient {
public:
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser>, CefProcessId,
        CefRefPtr<CefProcessMessage>) override;
};

class AppSchemeFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
        const CefString&, CefRefPtr<CefRequest>) override;
private:
    IMPLEMENT_REFCOUNTING(AppSchemeFactory)
};

#endif // _APP_H
