/*
 * This provides a C interface to open a CEF window and register native API javascript functions.
 * Typical usage:
 *
 * initialize("render.exe", argc, argv);
 * registerApiFunction("doThing", ...);
 * registerApiFunction("doAnotherThing", ...);
 * startMessageLoop(); // will block until the CEF window closes
 * deinitialize();
 */

#ifndef _CEF_WRAPPER_H
#define _CEF_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT void initialize(const char* renderProcessExecutablePath, int argc, char** argv);
EXPORT void deinitialize();
EXPORT void setWindowProperties();
EXPORT void startMessageLoop();
EXPORT void registerAppSchemaHandler(int (*getResource)(const char* url, char** data));
EXPORT void registerApiFunction(const char* name, const char* (*handler)(const char* jsonArgs));
EXPORT void executeJavascript(const char* code);

#ifdef __cplusplus
}
#endif

#endif // _CEF_WRAPPER_H
