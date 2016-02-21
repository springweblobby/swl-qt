#include "common/app.h"

CefRefPtr<CefResourceHandler> AppSchemeFactory::Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, const CefString& schema, CefRefPtr<CefRequest> request) {
    return NULL; // TODO
}
