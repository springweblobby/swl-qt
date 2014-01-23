#include "unitsynchandler.h"
#if defined Q_OS_LINUX || defined Q_OS_MAC
    #include <dlfcn.h>
#elif defined Q_OS_WIN32
    #error "Not implemented yet."
#else
    #error "Unknown target OS."
#endif

UnitsyncHandler::UnitsyncHandler(QObject* parent, Logger& logger, std::string path) :
        QObject(parent), logger(logger), ready(false), handle(NULL) {
    logger.info("Loading unitsync at ", path);
    #if defined Q_OS_LINUX || defined Q_OS_MAC
        handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if(handle == NULL) {
            logger.error("Could not load unitsync at ", path, ": ", dlerror());
            return;
        }

        // TODO: should be auto-generated.
        fptr_Init = (fptr_type_Init)dlsym(handle, "Init");
        fptr_GetSpringVersion = (fptr_type_GetSpringVersion)dlsym(handle, "GetSpringVersion");
        fptr_GetSpringVersionPatchset = (fptr_type_GetSpringVersionPatchset)dlsym(handle, "GetSpringVersionPatchset");
        fptr_IsSpringReleaseVersion = (fptr_type_IsSpringReleaseVersion)dlsym(handle, "IsSpringReleaseVersion");

        ready = true;
    #elif defined Q_OS_WIN32
        #error "Not implemented yet."
    #else
        #error "Unknown target OS."
    #endif
}

UnitsyncHandler::~UnitsyncHandler() {
    #if defined Q_OS_LINUX || defined Q_OS_MAC
        if (handle)
            dlclose(handle);
    #elif defined Q_OS_WIN32
        #error "Not implemented yet."
    #else
        #error "Unknown target OS."
    #endif
}

UnitsyncHandler::UnitsyncHandler(UnitsyncHandler&& h) : QObject(h.parent()),
        logger(h.logger), ready(h.ready), handle(h.handle) {

    h.handle = NULL; // Quite an important line, if you ask me.

    // TODO: should be auto-generated.
    fptr_Init = h.fptr_Init;
    fptr_GetSpringVersion = h.fptr_GetSpringVersion;
    fptr_GetSpringVersionPatchset = h.fptr_GetSpringVersionPatchset;
    fptr_IsSpringReleaseVersion = h.fptr_IsSpringReleaseVersion;
}

// TODO: should be auto-generated.
int UnitsyncHandler::init(bool isServer, int id) {
    if(fptr_Init == NULL)
        throw bad_fptr("Init");
    logger.debug("call Init(", isServer, ", ", id, ")");
    return fptr_Init(isServer, id);
}
QString UnitsyncHandler::getSpringVersion() {
    if(fptr_GetSpringVersion == NULL)
        throw bad_fptr("GetSpringVersion");
    logger.debug("call GetSpringVersion()");
    return QString(fptr_GetSpringVersion());
}
QString UnitsyncHandler::getSpringVersionPatchset() {
    if(fptr_GetSpringVersionPatchset == NULL)
        throw bad_fptr("GetSpringVersionPatchset");
    logger.debug("call GetSpringVersionPatchset()");
    return QString(fptr_GetSpringVersionPatchset());
}
bool UnitsyncHandler::isSpringReleaseVersion() {
    if(fptr_IsSpringReleaseVersion == NULL)
        throw bad_fptr("IsSpringReleaseVersion");
    logger.debug("call IsSpringReleaseVersion()");
    return fptr_IsSpringReleaseVersion();
}
