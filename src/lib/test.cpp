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
    "<body>"
    "<h1>It works!</h1>"
    "<script>"
    "document.write('<h3>CEF ' + CefWrapperAPI.cefVersion + '</h3>');"
    "</script>"
    "</body></html>";
int appSchemaHandler(const char* url, char* type, char** data) {
    char mimeType[] = "text/html";
    std::copy(mimeType, mimeType + sizeof(mimeType) / sizeof(char), type);
    *data = page;
    return sizeof(page);
}

int main(int argc, char** argv) {
    // "./render" works fine on both Linux and Windows.
    initialize("./render", argc, argv);
    registerAppSchemaHandler(&appSchemaHandler);
    registerApiFunction("testNumber", &testNumber);
    registerApiFunction("testString", &testString);
    registerApiFunction("testException", &testException);
    startMessageLoop("white", false);
    deinitialize();
}
