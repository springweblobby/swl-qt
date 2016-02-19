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

int main(int argc, char** argv) {
    // "./render" works fine on both Linux and Windows.
    initialize("./render", argc, argv);
    registerApiFunction("testNumber", &testNumber);
    registerApiFunction("testString", &testString);
    registerApiFunction("testException", &testException);
    startMessageLoop();
    deinitialize();
}
