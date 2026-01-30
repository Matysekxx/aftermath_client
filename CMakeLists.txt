cmake_minimum_required(VERSION 3.31)
project(aftermath_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(WITH_SSL OFF CACHE BOOL "" FORCE)
set(LIBUS_NO_SSL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        libuv
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG        v1.48.0
)

FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG        v1.3.1
)

FetchContent_Declare(
        usockets
        GIT_REPOSITORY https://github.com/uNetworking/uSockets.git
        GIT_TAG        v0.8.8
)

FetchContent_Declare(
        uwebsockets
        GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
        GIT_TAG        v20.70.0
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(libuv zlib usockets uwebsockets json)

file(GLOB USOCKETS_SOURCES
    "${usockets_SOURCE_DIR}/src/*.c"
    "${usockets_SOURCE_DIR}/src/eventing/*.c"
    "${usockets_SOURCE_DIR}/src/crypto/*.c"
)

add_executable(
        aftermath_client
        client/main.cpp
        client/Application.h
        client/Application.cpp
        client/network/WebSocketHandler.cpp
        client/network/NetworkSender.h
        client/network/NetworkSender.cpp
        client/event/GameEvent.h
        client/event/GameEvent.cpp
        client/controller/GameController.h
        client/controller/GameController.cpp
        client/ui/Renderer.h
        client/ui/Renderer.cpp
        client/model/GameState.h
        client/model/GameState.cpp
        client/input/InputHandler.h
        client/input/InputHandler.cpp
        ${USOCKETS_SOURCES}
)

find_package(Threads REQUIRED)

target_include_directories(aftermath_client PRIVATE
        ${usockets_SOURCE_DIR}/src
        ${uwebsockets_SOURCE_DIR}/src
        ${libuv_SOURCE_DIR}/include
        ${zlib_SOURCE_DIR}
        ${zlib_BINARY_DIR}
)

target_compile_definitions(aftermath_client PRIVATE
    LIBUS_NO_SSL
    LIBUS_USE_LIBUV=1
)

target_link_libraries(aftermath_client
        PRIVATE
        uv_a
        zlibstatic
        nlohmann_json::nlohmann_json
        Threads::Threads
        ws2_32
        iphlpapi
        userenv
        psapi
)
