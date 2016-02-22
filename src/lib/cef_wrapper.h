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

// Initialize CEF, taking the path for the render subprocess executable and the program arguments.
EXPORT void initialize(const char* renderProcessExecutablePath, int argc, char** argv);

// Deinitialize CEF.
EXPORT void deinitialize();

// Open cef://app/ in a window and start CEF message loop. This function will block until the window
// is closed. bgColor is a string describing the default background color of the browser window in CSS
// format, like "black" or "rgb(20, 50, 100)". If fullscreen is non-zero the window will be shown in
// borderless fullscreen mode.
EXPORT void startMessageLoop(const char* bgColor, int fullscreen);

// Register a handler for an API function. The function will be accessible to Javascript in the main frame
// in the global CefWrapperAPI object. JS code can call the function with a function(result){ ... } callback
// as the last argument to retrieve the return value. The callback can be omitted. The C side will receive
// the arguments in a JSON-encoded array and must return a valid JSON string to be given to the callback.
// You can return "null" for functions not having a return value.
// The handler will be called on the thread that called startMessageLoop().
EXPORT void registerApiFunction(const char* name, const char* (*handler)(const char* jsonArgs));

// Register the handler to be used with cef:// URLs. The handler must allocate the data and return the
// size in bytes or return -1 to indicate an error. The data is safe to deallocate when the handler
// gets called again or after startMessageLoop() returns.
// The handler should set type to the appropriate mime type for the request. The buffer for the type string
// is allocated by cef_wrapper with a length of 256 bytes. If the returned mime type is empty, the library
// will try to guess one based on the extension.
// The handler will be called on a separate IO thread, not the one that called startMessageLoop().
EXPORT void registerAppSchemaHandler(int (*getResource)(const char* url, char* type, char** data));

// Execute arbitrary Javascript code in the main frame. Can be called from any thread.
EXPORT void executeJavascript(const char* code);

#ifdef __cplusplus
}
#endif

#endif // _CEF_WRAPPER_H
