cmake_minimum_required(VERSION 3.31)
project(aftermath_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG        asio-1-30-2
)

FetchContent_Declare(
        websocketpp
        GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
        GIT_TAG        0.8.2
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(asio websocketpp json)

add_executable(aftermath_client
        client/main.cpp
        client/Application.cpp
        client/network/WebSocketHandler.cpp
        client/event/GameEvent.cpp
)

target_include_directories(aftermath_client PRIVATE
        ${asio_SOURCE_DIR}/asio/include
        ${websocketpp_SOURCE_DIR}
        ${json_SOURCE_DIR}/include
)

target_compile_definitions(aftermath_client PRIVATE
        ASIO_STANDALONE
        _WEBSOCKETPP_CPP11_STL_
        _WEBSOCKETPP_CPP11_THREAD_
        _WEBSOCKETPP_CPP11_CHRONO_
        _WEBSOCKETPP_CPP11_RANDOM_
        _WEBSOCKETPP_CPP11_MEMORY_
        _WEBSOCKETPP_CPP11_TYPE_TRAITS_
        _WEBSOCKETPP_CPP11_FUNCTIONAL_
        _WEBSOCKETPP_NO_CPP11_FUNCTIONAL_
)

find_package(Threads REQUIRED)

if(WIN32)
    set(EXTRA_LIBS ws2_32 mswsock)
endif()

find_package(Threads REQUIRED)

if(WIN32)
    set(EXTRA_LIBS ws2_32 mswsock)
endif()

target_link_libraries(aftermath_client
        PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
        ${EXTRA_LIBS}
)