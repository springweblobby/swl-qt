// Entry point for the renderer process executable.

#include "common/app.h"

int main(int argc, char** argv) {
#ifdef OS_WIN
    CefMainArgs args(GetModuleHandle(NULL));
#else
    CefMainArgs args(argc, argv);
#endif
    return CefExecuteProcess(args, new App, NULL);
}
