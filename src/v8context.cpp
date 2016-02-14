#include "app.h"
#include "lobbyinterface.h"
#include "marshal.h"
#include <cef_version.h>
#include <wrapper/cef_helpers.h>

#define START_DEFINE_API() if (false) {
#define REGISTER_FUNCTION(f) } else if (name == #f) { retval = Marshal::callNative(&LobbyInterface::f, &api, args);
#define END_DEFINE_API() }

class Handler : public CefV8Handler {
public:
    Handler(CefRefPtr<CefV8Context> context) : api(context) {}
    bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
            const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval,
            CefString& exception) override {
        try {
            START_DEFINE_API()
            REGISTER_FUNCTION(init)
            REGISTER_FUNCTION(getApiVersion)
            REGISTER_FUNCTION(getSpringHome)
            REGISTER_FUNCTION(readSpringHomeSetting)
            REGISTER_FUNCTION(writeSpringHomeSetting)
            END_DEFINE_API()
            return true;
        } catch (Marshal::NotEnoughArgumentsException) {
            exception = "Not enough arguments for API function " + name.ToString();
        } catch (Marshal::TooManyArgumentsException) {
            exception = "Too many arguments for API function " + name.ToString();
        } catch (Marshal::TypeMismatchException) {
            exception = "Type mismatch in API function " + name.ToString();
        }
        // For some reason just setting the exception string and returning
        // false as the docs instruct us doesn't actually throw an exception,
        // so we have to improvise.
        CefRefPtr<CefV8Value> val;
        CefRefPtr<CefV8Exception> e;
        CefV8Context::GetEnteredContext()->Eval("throw new TypeError('" + exception.ToString() +
            "');", val, e);
        return false;
    }
private:
    LobbyInterface api;
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
    CefRefPtr<Handler> handler(new Handler(context));
    apiObj->SetValue("cefVersion", CefV8Value::CreateString(CEF_VERSION),
        V8_PROPERTY_ATTRIBUTE_READONLY);
    for (auto i : {
            "init",
            "getApiVersion",
            "getSpringHome",
            "readSpringHomeSetting",
            "writeSpringHomeSetting" }) {

        apiObj->SetValue(i, CefV8Value::CreateFunction(i, handler), V8_PROPERTY_ATTRIBUTE_NONE);
    }
    global->SetValue("SwlAPI", apiObj, V8_PROPERTY_ATTRIBUTE_NONE);

    context->Exit();
}
