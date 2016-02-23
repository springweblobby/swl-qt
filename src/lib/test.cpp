#include "cef_wrapper.h"
#include <iostream>

const char* testNumber(const char* args) {
    std::cout << "testNumber() called with " << args << std::endl;
    return "42";
}

const char* testString(const char* args) {
    std::cout << "testString() called with " << args << std::endl;
    return "\"this is a string\"";
}

const char* testException(const char* argc) {
    executeJavascript("window.foo = 2; throw new Error('Test error');");
    return "null";
}

char page[] =
    "<html><head><title>CEF wrapper test</title></head>"
    "<link href=\"page.css\" rel=\"stylesheet\" type=\"text/css\">"
    "<body>"
    "<h1>It works!</h1>"
    "<script>"
    "document.write('<h3>CEF ' + CefWrapperAPI.cefVersion + '</h3>');"
    "</script>"
    "</body></html>";
char css[] =
    "h1::after {"
    "  content: ' OK';"
    "  color: green;"
    "  font-size: 0.6em;"
    "}";
int appSchemaHandler(const char* url_, char* type, char** data) {
    std::string url = url_;
    if (url == "cef://app/") {
        char mimeType[] = "text/html";
        *data = page;
        std::copy(mimeType, mimeType + sizeof(mimeType) / sizeof(char), type);
        return sizeof(page);
    } else if (url == "cef://app/page.css") {
        char mimeType[] = "text/css";
        *data = css;
        std::copy(mimeType, mimeType + sizeof(mimeType) / sizeof(char), type);
        return sizeof(css);
    } else {
        return -1;
    }
}

int main(int argc, char** argv) {
    // "./render" works fine on both Linux and Windows.
    initialize("./render", argc, argv);
    registerAppSchemaHandler(&appSchemaHandler);
    registerApiFunction("testNumber", &testNumber);
    registerApiFunction("testString", &testString);
    registerApiFunction("testException", &testException);
    startMessageLoop("cef://app/", "white", false);
    deinitialize();
}
