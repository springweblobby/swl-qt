cmake_minimum_required(VERSION 2.6)
project(swl-browser)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")
endif ()

include_directories(${PROJECT_SOURCE_DIR}/src)

set(common_path ${PROJECT_SOURCE_DIR}/src/common)

if (APPLE)
    set(platform_sources ${common_path}/platform_mac.cpp)
elseif (WIN32)
    set(platform_sources ${common_path}/platform_windows.cpp)
elseif (UNIX)
    set(platform_sources ${common_path}/platform_linux.cpp)
else ()
    message(FATAL_ERROR "Unsupported platform.")
endif()

set(common_sources
    ${common_path}/app.h
    ${common_path}/app.cpp
    ${common_path}/marshal.h
    ${common_path}/platform.h
    ${platform_sources}
)

if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    include_directories(${X11_X11_INCLUDE_PATH})
endif()

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
set(cef_path ${PROJECT_SOURCE_DIR}/${cef_fullname})
if (EXISTS ${cef_path})
    message(STATUS "Found CEF ${cef_version}")
    include_directories(${cef_path})
    include_directories(${cef_path}/include)
    add_subdirectory(${cef_path}/libcef_dll)
    if (APPLE)
        message(FATAL_ERROR "Implement this for Mac")
    elseif (WIN32)
        set(libcef_dll ${cef_path}/Release/libcef.lib)
    elseif (UNIX)
        set(libcef_dll ${cef_path}/Release/libcef.so)
    endif ()
    MESSAGE(STATUS "Copying resource files")
    file(GLOB cef_resources ${cef_path}/Resources/* ${cef_path}/Release/*.bin)
    file(COPY ${cef_resources} DESTINATION .)
else ()
    message(FATAL_ERROR "Please download CEF version ${cef_version} for ${cef_arch} from http://cefbuilds.com and extract into the source directory.")
endif ()

set(common_libs ${X11_X11_LIB} libcef_dll_wrapper ${libcef_dll})
add_subdirectory(${PROJECT_SOURCE_DIR}/src/lobby .)
