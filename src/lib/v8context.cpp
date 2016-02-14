#include "common/app.h"
#include "cef_wrapper_internal.h"
#include <cef_version.h>
#include <wrapper/cef_helpers.h>

class Handler : public CefV8Handler {
public:
    bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
            const CefV8ValueList& args_, CefRefPtr<CefV8Value>& retval,
            CefString& exception) override {

        CefRefPtr<CefV8Context> context = CefV8Context::GetEnteredContext();
        CefV8ValueList args(args_);
        CefRefPtr<CefV8Value> callback;
        if (args.size() > 0 && args.back()->IsFunction()) {
            callback = args.back();
            args.pop_back();
        }

        // It would of course be more proper to use per-context callback maps
        // instead of a global var, but we only ever have one active context.
        int id = Internal::callbackId++;
        Internal::apiCallbacks[id] = callback;

        auto msg = CefProcessMessage::Create("call_api_func");
        msg->GetArgumentList()->SetInt(0, id);
        msg->GetArgumentList()->SetString(1, name);
        msg->GetArgumentList()->SetString(2, context->GetGlobal()->
            GetValue("__argsToJSON")->ExecuteFunction(NULL, args)->GetStringValue());
        context->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
        return true;
    }
private:
    IMPLEMENT_REFCOUNTING(Handler)
};

void App::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) {

    CEF_REQUIRE_RENDERER_THREAD()
    if (!browser->IsSame(mainBrowser) || !frame->IsMain())
        return;

    CefRefPtr<CefV8Value> val;
    CefRefPtr<CefV8Exception> e;
    context->Eval(
        "__argsToJSON = function(){"
        "  return JSON.stringify(Array.from(arguments));"
        "}",
        val, e);
    CefRefPtr<Handler> handler(new Handler);

    context->Enter();

    auto global = context->GetGlobal();
    auto apiObj = CefV8Value::CreateObject(NULL);
    apiObj->SetValue("cefVersion", CefV8Value::CreateString(CEF_VERSION),
        V8_PROPERTY_ATTRIBUTE_READONLY);
    // Dummy function to indirectly store the handler in the context.
    apiObj->SetValue("__dummy", CefV8Value::CreateFunction("__dummy", handler),
        V8_PROPERTY_ATTRIBUTE_NONE);
    browser->SendProcessMessage(PID_BROWSER, CefProcessMessage::Create("get_api_func_list"));
    global->SetValue("CefWrapperAPI", apiObj, V8_PROPERTY_ATTRIBUTE_NONE);

    context->Exit();
    Internal::mainFrame = frame;
}
