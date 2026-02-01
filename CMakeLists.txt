cmake_minimum_required(VERSION 3.31)
project(aftermath_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Musíme najít vlákna, aby mohl fungovat target Threads::Threads
find_package(Threads REQUIRED)

# 2. Vypnutí závislostí PŘED stažením IXWebSocket
set(USE_ZLIB OFF CACHE BOOL "" FORCE)
set(USE_TLS OFF CACHE BOOL "" FORCE)
set(USE_DEFLATE OFF CACHE BOOL "" FORCE)

# 3. Stažení knihoven
FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG        v11.0.8
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(ixwebsocket json)

# 4. Načtení tvých souborů
file(GLOB_RECURSE CLIENT_SOURCES
        "client/*.cpp"
        "client/*.h"
        "client/*.hpp"
)

add_executable(aftermath_client ${CLIENT_SOURCES})

target_include_directories(aftermath_client PRIVATE client)

# 5. Linkování (používáme přímo název ixwebsocket, protože alias někdy zlobí)
target_link_libraries(aftermath_client
        PRIVATE
        ixwebsocket
        nlohmann_json::nlohmann_json
        Threads::Threads
        $<$<PLATFORM_ID:Windows>:ws2_32 iphlpapi userenv psapi>
)