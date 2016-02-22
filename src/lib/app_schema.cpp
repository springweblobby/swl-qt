#include "common/app.h"
#include "cef_wrapper_internal.h"

#include <cef_parser.h>
#include <wrapper/cef_stream_resource_handler.h>


CefRefPtr<CefResourceHandler> AppSchemeFactory::Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, const CefString& schema, CefRefPtr<CefRequest> request) {
    if (!Internal::appSchemaHandler)
        return NULL;
    char* data;
    char typeBuf[256];
    int size = Internal::appSchemaHandler(request->GetURL().ToString().c_str(), typeBuf, &data);
    if (size >= 0) {
        CefString type = typeBuf;
        if (type == "") {
            std::string urlStr = request->GetURL().ToString();
            type = CefGetMimeType(urlStr.substr(urlStr.find_last_of('.') + 1));
        }
        CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(data, size);
        return new CefStreamResourceHandler(type, stream);
    } else {
        return NULL;
    }
}
