include_directories("${PROJECT_SOURCE_DIR}/src/lobby/Boost.Process-0.5")

set(lobby_path ${PROJECT_SOURCE_DIR}/src/lobby)

set(lobby_sources
    ${lobby_path}/lobbyinterface.cpp
    ${lobby_path}/lobbyinterface.h
    ${lobby_path}/logger.h
    ${lobby_path}/main.cpp
    ${lobby_path}/networkhandler.cpp
    ${lobby_path}/processrunner.cpp
    ${lobby_path}/ufstream.h
    ${lobby_path}/v8context.cpp
    ${lobby_path}/ipc.cpp
    ${lobby_path}/app_schema.cpp
)

add_executable(
    weblobby WIN32
    ${common_sources}
    ${lobby_sources}
)

target_link_libraries(weblobby ${common_libs})

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.52 REQUIRED COMPONENTS system filesystem thread iostreams chrono)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(weblobby ${Boost_LIBRARIES})

find_package(CURL 7 REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(weblobby ${CURL_LIBRARIES})
