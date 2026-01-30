//
// Created by chalo on 30.01.2026.
//

#include "Application.h"

Application::Application(const std::string& url) {
    this->fromServerToClient = std::make_unique<BlockingQueue<GameEvent>>();
    this->fromClientToServer = std::make_unique<BlockingQueue<GameEvent>>();
    this->wsHandler = std::make_unique<WebSocketHandler>(url, fromServerToClient.get());
}

void Application::execute() {

}
