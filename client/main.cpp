#include "Application.h"
#include <string>
#include <ixwebsocket/IXNetSystem.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void EnableVTMode() {
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

int main(const int argc, char* argv[]) {
    ix::initNetSystem();
    #ifdef _WIN32
    EnableVTMode();
    #endif

    {
        const std::string url = (argc > 1) ? argv[1] : "ws://127.0.0.1:8080/game";
        const Application app(url);
        app.execute();
    }

    ix::uninitNetSystem();
    return 0;
}
