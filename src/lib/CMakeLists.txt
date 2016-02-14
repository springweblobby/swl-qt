set(lib_path ${PROJECT_SOURCE_DIR}/src/lib)

set(lib_sources
    ${lib_path}/cef_wrapper.h
    ${lib_path}/cef_wrapper_internal.h
    ${lib_path}/cef_wrapper.cpp
    ${lib_path}/v8context.cpp
)

add_library(
    cef_wrapper SHARED
    ${common_sources}
    ${lib_sources}
)

add_executable(
    render
    ${common_sources}
    ${lib_sources}
    ${lib_path}/main_render.cpp
)

target_link_libraries(render ${common_libs})
