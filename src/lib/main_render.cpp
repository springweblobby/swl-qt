// Entry point for the renderer process executable.

#include "common/app.h"

#ifdef OS_WIN

#include <windows.h>

int __stdcall WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int show) {
    CefMainArgs args(instance);
    CefEnableHighDPISupport();

#else

int main(int argc, char** argv) {
    CefMainArgs args(argc, argv);

#endif
    return CefExecuteProcess(args, new App, NULL);
}
