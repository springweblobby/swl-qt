#include "app.h"
#include <cef_version.h>
#include <wrapper/cef_helpers.h>
#include <iostream>

class Handler : public CefV8Handler {
public:
    typedef std::function<CefRefPtr<CefV8Value>(const CefV8ValueList&)> HandlerFunc;
    Handler(HandlerFunc func) : m_func(func) {}
    bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
            const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval,
            CefString& exception) override {
        std::cout << "Called native function " << name.ToString() << std::endl;
        retval = m_func(args);
        return true;
    }
private:
    HandlerFunc m_func;
    IMPLEMENT_REFCOUNTING(Handler)
};

void App::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) {

    CEF_REQUIRE_RENDERER_THREAD()
    if (!browser->IsSame(mainBrowser) || !frame->IsMain())
        return;

    context->Enter();

    auto global = context->GetGlobal();
    auto apiObj = CefV8Value::CreateObject(NULL);
    apiObj->SetValue("cefVersion", CefV8Value::CreateString(CEF_VERSION),
        V8_PROPERTY_ATTRIBUTE_READONLY);
    apiObj->SetValue("test", CefV8Value::CreateFunction("test", new Handler(
        [](const CefV8ValueList& args){
            std::cout << "Called test()\n";
            return CefV8Value::CreateInt(42);
        })), V8_PROPERTY_ATTRIBUTE_NONE);
    global->SetValue("SwlAPI", apiObj, V8_PROPERTY_ATTRIBUTE_NONE);

    context->Exit();
}
