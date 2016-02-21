#include "common/app.h"
#include "cef_wrapper_internal.h"

#include <wrapper/cef_stream_resource_handler.h>


CefRefPtr<CefResourceHandler> AppSchemeFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
        const CefString& schema, CefRefPtr<CefRequest> request) {
    if (!Internal::appSchemaHandler)
        return NULL;
    char* data;
    char type[256];
    size_t size = Internal::appSchemaHandler(request->GetURL().ToString().c_str(), type, &data);
    if (size >= 0) {
        CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(data, size);
        return new CefStreamResourceHandler(type, stream);
    } else {
        return new CefStreamResourceHandler(404, "", "", CefResponse::HeaderMap(), NULL);
    }
}
