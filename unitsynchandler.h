#ifndef _UNITSYNC_HANDLER_H
#define _UNITSYNC_HANDLER_H

#include "logger.h"
#include <string>
#include <exception>
#include <QObject>

class UnitsyncHandler : public QObject {
    Q_OBJECT
public:
    UnitsyncHandler(QObject* parent, Logger&, std::string dllpath);
    ~UnitsyncHandler();
    bool isReady() {
        return ready;
    }

    UnitsyncHandler(const UnitsyncHandler&) = delete;
    UnitsyncHandler& operator=(const UnitsyncHandler&) = delete;
    UnitsyncHandler(UnitsyncHandler&&);

    struct bad_fptr : public std::exception {
        bad_fptr(std::string symbol) {
            m_what = "Symbol " + symbol + " not found in the unitsync library.";
        }
        const char* what() { return m_what.c_str(); }
        std::string m_what;
    };

public slots:
    // Unitsync functions.
    // TODO: should be auto-generated.
    int init(bool, int);
    QString getSpringVersion();
    QString getSpringVersionPatchset();
    bool isSpringReleaseVersion();

private:
    Logger& logger;
    bool ready;
    void* handle;

    // Unisync function pointers.
    // TODO: should be auto-generated.
    typedef int (*fptr_type_Init)(bool, int);
    fptr_type_Init fptr_Init;
    typedef const char* (*fptr_type_GetSpringVersion)();
    fptr_type_GetSpringVersion fptr_GetSpringVersion;
    typedef const char* (*fptr_type_GetSpringVersionPatchset)();
    fptr_type_GetSpringVersionPatchset fptr_GetSpringVersionPatchset;
    typedef bool (*fptr_type_IsSpringReleaseVersion)();
    fptr_type_IsSpringReleaseVersion fptr_IsSpringReleaseVersion;
};

#endif // _UNITSYNC_HANDLER_H
