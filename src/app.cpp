#include <cef_client.h>
#include "app.h"
#include "platform.h"

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
                browser->GetHost()->ShowDevTools(CefWindowInfo(), this,
                    CefBrowserSettings(), CefPoint());
            }
            return true;
        }
        return false;
    }
    void OnLoadError(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, ErrorCode errCode,
            const CefString& errText, const CefString& url) override {
        if (frame->IsMain()) {
            std::string str = "<h1>Error</h1><h2>Failed loading " + url.ToString() + "</h2><h2>" +
                errText.ToString() + "</h2><h3>Error code " + std::to_string(errCode) + "</h3>";
            frame->LoadString(str, "about:error");
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

void App::OnContextInitialized() {
    CefWindowInfo info;
#ifdef OS_WIN
    info.SetAsPopup(NULL, "weblobby");
#endif
    
    CefBrowserSettings settings;
    CefString(&settings.default_encoding).FromASCII("UTF-8");
    settings.javascript_access_clipboard = STATE_ENABLED;
    settings.javascript_close_windows = STATE_ENABLED;
    settings.universal_access_from_file_urls = STATE_ENABLED;

    auto url = CefCommandLine::GetGlobalCommandLine()->GetSwitchValue("url");
    if (url.empty())
        url = "weblobby://";
    auto browser = CefBrowserHost::CreateBrowserSync(info, new Client, url, settings, NULL);
    Platform::setWindowTitle(browser->GetHost()->GetWindowHandle(), "Loading...");
    Platform::maximizeWindow(browser->GetHost()->GetWindowHandle());
}
