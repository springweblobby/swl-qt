greaterThan(QT_MAJOR_VERSION, 4):QT += widgets webkitwidgets
CONFIG += c++11
#CONFIG += debug

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

LIBS += -lboost_filesystem -lboost_system -lboost_thread -lboost_iostreams -lboost_chrono
# Windows build
#LIBS += -Ld:/mingw32/lib -lboost_filesystem-mgw48-mt-1_55 -lboost_system-mgw48-mt-1_55 -lboost_thread-mgw48-mt-1_55 -lboost_iostreams-mgw48-mt-1_55 -lboost_chrono-mgw48-mt-1_55
#LIBS += -lws2_32 -lwsock32
#LIBS += -Wl,-subsystem,console -mconsole

# Add more folders to ship with the application, here

# Define TOUCH_OPTIMIZED_NAVIGATION for touch optimization and flicking
#DEFINES += TOUCH_OPTIMIZED_NAVIGATION

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Please do not modify the following two lines. Required for deployment.
#include(html5applicationviewer/html5applicationviewer.pri)
#qtcAddDeployment()
