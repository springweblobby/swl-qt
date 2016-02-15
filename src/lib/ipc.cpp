#include "common/app.h"
#include "cef_wrapper_internal.h"

using namespace Internal;

// Render process received IPC message.
bool App::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId pid, CefRefPtr<CefProcessMessage> msg) {

    auto args = msg->GetArgumentList();
    if (msg->GetName() == "api_func_result") {
        if (apiCallbacks.count(args->GetInt(0)) == 0)
            return false;
        auto context = browser->GetMainFrame()->GetV8Context();
        auto callback = apiCallbacks[args->GetInt(0)];
        if (callback) {
            context->Enter();
            callback->ExecuteFunction(NULL, {
                context->GetGlobal()->GetValue("JSON")->GetValue("parse")->
                ExecuteFunction(NULL, { CefV8Value::CreateString(args->GetString(1)) })
            });
            context->Exit();
        }
        apiCallbacks.erase(args->GetInt(0));
        return true;
    } else if (msg->GetName() == "api_func_list") {
        auto context = browser->GetMainFrame()->GetV8Context();
        context->Enter();
        auto apiObj = context->GetGlobal()->GetValue("CefWrapperAPI");
        auto handler = apiObj->GetValue("__dummy")->GetFunctionHandler();
        for(unsigned int i = 0; i < args->GetSize(); i++) {
            apiObj->SetValue(args->GetString(i), CefV8Value::CreateFunction(
                args->GetString(i), handler), V8_PROPERTY_ATTRIBUTE_NONE);
        }
        context->Exit();
        return true;
    }
    return false;
}

// Browser process received IPC message.
bool ClientBase::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId pid, CefRefPtr<CefProcessMessage> msg) {

    auto args = msg->GetArgumentList();
    if (msg->GetName() == "call_api_func") {
        if (registeredApiFunctions.count(args->GetString(1).ToString()) == 0)
            return false;
        auto result = registeredApiFunctions[args->GetString(1).ToString()](
            args->GetString(2).ToString());
        auto resMsg = CefProcessMessage::Create("api_func_result");
        resMsg->GetArgumentList()->SetInt(0, args->GetInt(0));
        resMsg->GetArgumentList()->SetString(1, result);
        browser->SendProcessMessage(PID_RENDERER, resMsg);
        return true;
    } else if (msg->GetName() == "get_api_func_list") {
        auto resMsg = CefProcessMessage::Create("api_func_list");
        int n = 0;
        for (auto i : registeredApiFunctions)
            resMsg->GetArgumentList()->SetString(n++, i.first);
        browser->SendProcessMessage(PID_RENDERER, resMsg);
        return true;
    }
    return false;
}
