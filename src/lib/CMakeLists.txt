set(lib_path ${PROJECT_SOURCE_DIR}/src/lib)

set(lib_sources
    ${lib_path}/cef_wrapper.h
    ${lib_path}/cef_wrapper_internal.h
    ${lib_path}/cef_wrapper.cpp
    ${lib_path}/v8context.cpp
    ${lib_path}/ipc.cpp
    ${lib_path}/app_schema.cpp
)

add_library(
    cef_wrapper SHARED
    ${common_sources}
    ${lib_sources}
)
target_link_libraries(cef_wrapper ${common_libs})

add_executable(
    render WIN32
    ${common_sources}
    ${lib_sources}
    ${lib_path}/main_render.cpp
)
target_link_libraries(render ${common_libs})

add_executable(
    cef_wrapper_test
    ${lib_path}/test.cpp
)
target_link_libraries(cef_wrapper_test cef_wrapper)
