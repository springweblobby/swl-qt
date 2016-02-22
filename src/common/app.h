#ifndef _APP_H
#define _APP_H

#include <cef_app.h>
#include <cef_client.h>
#include <cef_scheme.h>
#include "platform.h"

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
        registrar->AddCustomScheme("cef", true, false, false);
    }
private:
    CefRefPtr<CefBrowser> mainBrowser; // only available in render process
    IMPLEMENT_REFCOUNTING(App)
};

class Client :
    public CefClient,
    public CefLoadHandler,
    public CefLifeSpanHandler,
    public CefDisplayHandler,
    public CefKeyboardHandler {
public:
    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override {
        return this;
    }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override {
        return this;
    }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
        return this;
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser>, CefProcessId,
        CefRefPtr<CefProcessMessage>) override;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        if (!mainBrowser)
            mainBrowser = browser;
        else
            devTools = browser;
    }
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        if (browser->IsSame(mainBrowser)) {
            mainBrowser = NULL;
            CefQuitMessageLoop();
        } else {
            devTools = NULL;
        }
    }
    bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& evt,
            CefEventHandle, bool*) override {
        if (evt.windows_key_code == 0x74 && evt.type == KEYEVENT_RAWKEYDOWN) { // F5
            browser->ReloadIgnoreCache();
            return true;
        } else if (evt.windows_key_code == 0x7B && evt.type == KEYEVENT_RAWKEYDOWN) { // F12
            if (!devTools) {
                CefWindowInfo info;
#ifdef OS_WIN
                info.SetAsPopup(NULL, "devtools");
#endif
                browser->GetHost()->ShowDevTools(info, this,
                    CefBrowserSettings(), CefPoint());
            }
            return true;
        }
        return false;
    }
    void OnLoadError(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, ErrorCode errCode,
            const CefString& errText, const CefString& url) override {
        if (frame->IsMain() && errCode != ERR_NONE && errCode != ERR_ABORTED) {
            std::string str = "<h1>Error</h1><h2>Failed loading " + url.ToString() + "</h2><h2>" +
                errText.ToString() + "</h2><h3>Error code " + std::to_string(errCode) + "</h3>";
            frame->LoadString(str, url);
        }
    }
    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override {
        Platform::setWindowTitle(browser->GetHost()->GetWindowHandle(), title.ToString());
    }
    // TODO CefResourceHander::OnBeforeBrowse (but OnBeforeNavigation probs better)
private:
    CefRefPtr<CefBrowser> mainBrowser;
    CefRefPtr<CefBrowser> devTools;
    IMPLEMENT_REFCOUNTING(Client)
};

class AppSchemeFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
        const CefString&, CefRefPtr<CefRequest>) override;
private:
    IMPLEMENT_REFCOUNTING(AppSchemeFactory)
};

#endif // _APP_H
