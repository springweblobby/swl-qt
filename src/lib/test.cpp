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

int main(int argc, char** argv) {
    initialize("render", argc, argv);
    registerApiFunction("testNumber", &testNumber);
    registerApiFunction("testString", &testString);
    startMessageLoop();
    deinitialize();
}
