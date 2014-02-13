greaterThan(QT_MAJOR_VERSION, 4):QT += widgets webkitwidgets
win32:QT += multimedia
CONFIG += c++11
unix:CONFIG += debug

SOURCES += main.cpp \
    weblobbywindow.cpp \
    lobbyinterface.cpp \
    networkhandler.cpp \
    unitsynchandler.cpp \
    processrunner.cpp

HEADERS += \
    weblobbywindow.h \
    lobbyinterface.h \
    logger.h \
    unitsynchandler.h

INCLUDEPATH += Boost.Process-0.5

unix {
    LIBS += -lboost_filesystem -lboost_system -lboost_thread -lboost_iostreams -lboost_chrono -lcurl
}
win32 {
    LIBS += -Ld:/mingw32/lib -lboost_filesystem-mgw48-mt-1_55 -lboost_system-mgw48-mt-1_55 -lboost_thread-mgw48-mt-1_55 -lboost_iostreams-mgw48-mt-1_55 -lboost_chrono-mgw48-mt-1_55
    LIBS += -lws2_32 -lwsock32
    LIBS += -Wl,-subsystem,console -mconsole
}
