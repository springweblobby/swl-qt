cmake_minimum_required(VERSION 2.6)
project(swl-qt)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")
endif ()

include_directories("${swl-qt_SOURCE_DIR}/Boost.Process-0.5")

add_executable(
    weblobby

    src/main.cpp
    src/app.h
    src/app.cpp
    src/v8context.cpp
    src/lobbyinterface.cpp
    src/networkhandler.cpp
    src/processrunner.cpp
    src/lobbyinterface.h
    src/logger.h
    src/ufstream.h
)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.52 REQUIRED COMPONENTS system filesystem thread iostreams chrono)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(weblobby ${Boost_LIBRARIES})

find_package(CURL 7 REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(weblobby ${CURL_LIBRARIES})

if (UNIX)
    find_package(X11 REQUIRED)
    include_directories(${X11_X11_INCLUDE_PATH})
    target_link_libraries(weblobby ${X11_X11_LIB})
endif ()

set(cef_version 3.2454.1344.g2782fb8)

if (APPLE)
    set(cef_arch macosx64)
elseif (WIN32)
    set(cef_arch windows32)
elseif (UNIX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(cef_arch linux64)
elseif (UNIX AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(cef_arch linux32)
else ()
    message(FATAL_ERROR "Cannot determine target CEF architecture!")
endif ()
set(cef_fullname cef_binary_${cef_version}_${cef_arch})
set(cef_path ${swl-qt_SOURCE_DIR}/${cef_fullname})
if (EXISTS ${cef_path})
    message(STATUS "Found CEF ${cef_version}")
    include_directories(${cef_path})
    include_directories(${cef_path}/include)
    add_subdirectory(${cef_path}/libcef_dll)
    target_link_libraries(weblobby libcef_dll_wrapper)
    if (APPLE)
        message(FATAL_ERROR "Implement this for Mac")
    elseif (WIN32)
        target_link_libraries(weblobby ${cef_path}/Release/libcef.lib)
    elseif (UNIX)
        target_link_libraries(weblobby ${cef_path}/Release/libcef.so)
    endif ()
    MESSAGE(STATUS "Copying resource files")
    file(GLOB cef_resources ${cef_path}/Resources/* ${cef_path}/Release/*.bin)
    file(COPY ${cef_resources} DESTINATION .)
else ()
    message(SEND_ERROR "Please download CEF version ${cef_version} for ${cef_arch} from http://cefbuilds.com and extract into the source directory.")
endif ()
