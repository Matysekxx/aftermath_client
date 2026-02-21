cmake_minimum_required(VERSION 3.31)
project(aftermath_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(Threads REQUIRED)

set(USE_ZLIB OFF CACHE BOOL "" FORCE)
set(USE_TLS OFF CACHE BOOL "" FORCE)
set(USE_DEFLATE OFF CACHE BOOL "" FORCE)

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

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG        v5.0.0
)

FetchContent_MakeAvailable(ixwebsocket json ftxui)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "client/*.cpp"
        "client/*.h"
        "client/*.hpp"
)

add_executable(aftermath_client ${CLIENT_SOURCES})

target_include_directories(aftermath_client PRIVATE client)

target_link_libraries(aftermath_client
        PRIVATE
        ixwebsocket
        nlohmann_json::nlohmann_json
        Threads::Threads
        ftxui::screen
        ftxui::dom
        ftxui::component
        $<$<PLATFORM_ID:Windows>:ws2_32 iphlpapi userenv psapi winmm>
)

if(MINGW)
    target_link_options(aftermath_client PRIVATE -static -static-libgcc -static-libstdc++)
endif()

add_custom_command(TARGET aftermath_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:aftermath_client>/res
)
