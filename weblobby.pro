greaterThan(QT_MAJOR_VERSION, 4):QT += widgets webkitwidgets
win32:QT += multimedia
CONFIG += c++11
unix:CONFIG += debug

SOURCES += src/main.cpp \
    src/weblobbywindow.cpp \
    src/lobbyinterface.cpp \
    src/networkhandler.cpp \
    src/unitsynchandler.cpp \
    src/unitsynchandler_t.cpp \
    src/processrunner.cpp

HEADERS += \
    src/weblobbywindow.h \
    src/lobbyinterface.h \
    src/logger.h \
    src/ufstream.h\
    src/unitsynchandler.h\
    src/unitsynchandler_t.h

INCLUDEPATH += Boost.Process-0.5

unix:!macx {
    LIBS += -ldl -lboost_filesystem -lboost_system -lboost_thread -lboost_iostreams -lboost_chrono -lcurl -lmpg123 -lasound
}
win32 {
    RC_FILE = icon.rc
    LIBS += -Ld:/mingw32/lib -lboost_filesystem-mgw48-mt-1_55 -lboost_system-mgw48-mt-1_55 -lboost_thread-mgw48-mt-1_55 -lboost_iostreams-mgw48-mt-1_55 -lboost_chrono-mgw48-mt-1_55
    LIBS += -lws2_32 -lwsock32 -lcurl
    LIBS += -Wl,-subsystem,console -mconsole
}
macx {
    INCLUDEPATH += /opt/local/include
    LIBS += -L /opt/local/lib
    LIBS += -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_iostreams-mt -lboost_chrono-mt -lcurl -ldl
}
