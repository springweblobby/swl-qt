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

void initialize(const char* renderProcessExecutablePath, int argc, char** argv);
void deinitialize();
void setWindowProperties();
void startMessageLoop();
void registerAppSchemaHandler(int (*getResource)(const char* url, char** data));
void registerApiFunction(const char* name, const char* (*handler)(const char* jsonArgs));
void executeJavascript(const char* code);

#ifdef __cplusplus
}
#endif

#endif // _CEF_WRAPPER_H
