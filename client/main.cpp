#include <iostream>

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
        std::string url;
        if (argc > 1) {
            url = argv[1];
        } else {
            std::cout << "Usage: " << argv[0] << " <server_url>" << std::endl;
            std::cout << "Example: " << argv[0] << " ws://localhost:8080/game" << std::endl;
            std::cout << "Please provide the server URL as an argument." << std::endl;
            return 1;
        }
        const Application app(url);
        app.execute();
    }

    ix::uninitNetSystem();
    return 0;
}
