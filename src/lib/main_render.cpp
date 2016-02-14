// Entry point for the renderer process executable.

#include "common/app.h"

int main(int argc, char** argv) {
    return CefExecuteProcess(CefMainArgs(argc, argv), new App, NULL);
}
