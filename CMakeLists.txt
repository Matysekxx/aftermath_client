cmake_minimum_required(VERSION 3.31)
project(aftermath_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(USE_ZLIB OFF CACHE BOOL "" FORCE)
set(USE_TLS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG        v11.4.4
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(ixwebsocket json)

add_executable(
        aftermath_client
        client/main.cpp
        client/network/WebSocketHandler.cpp
        client/event/GameEvent.h
        client/event/GameEvent.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(aftermath_client
        PRIVATE
        ixwebsocket::ixwebsocket
        nlohmann_json::nlohmann_json
        Threads::Threads
)