//
// Created by chalo on 30.01.2026.
//

#include "WebSocketHandler.h"

WebSocketHandler::WebSocketHandler(const std::string &url) {
    webSocket.setUrl(url);
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr &message) {
        //TODO: pridat logiku  reseni zprav z spring boot serveru
    });
}

void WebSocketHandler::start() {
    webSocket.start();
}

void WebSocketHandler::stop() {
    webSocket.stop();
}

WebSocketHandler::~WebSocketHandler() {
    webSocket.close();
}


