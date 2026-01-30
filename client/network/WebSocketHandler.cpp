//
// Created by chalo on 30.01.2026.
//

#include "WebSocketHandler.h"

WebSocketHandler::WebSocketHandler(const std::string &url) {
    webSocket.setUrl(url);
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr &message) {

    });
}

void WebSocketHandler::start() {
    webSocket.start();
}

void WebSocketHandler::stop() {
    webSocket.stop();
}

