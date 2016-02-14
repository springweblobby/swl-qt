#include "common/app.h"
#include "cef_wrapper_internal.h"
#include <cef_version.h>
#include <wrapper/cef_helpers.h>

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
    // TODO populate from registered api functions
    global->SetValue("CefWrapperAPI", apiObj, V8_PROPERTY_ATTRIBUTE_NONE);

    context->Exit();
    Internal::mainFrame = frame; // TODO release on OnContextReleased
}
