//
// Created by chalo on 30.01.2026.
//

#include "Application.h"
#include <thread>
#include <chrono>

Application::Application(const std::string& url) {
    this->fromServerToClient = std::make_unique<BlockingQueue<GameEvent>>();
    this->fromClientToServer = std::make_unique<BlockingQueue<GameEvent>>();
    this->wsHandler = std::make_unique<WebSocketHandler>(url, fromServerToClient.get());
    this->gameController = std::make_unique<GameController>(fromServerToClient.get(), fromClientToServer.get());
    this->inputHandler = std::make_unique<InputHandler>(fromClientToServer.get());
    this->networkSender = std::make_unique<NetworkSender>(fromClientToServer.get(), wsHandler.get());
}

void Application::execute() const {
    wsHandler->start();
    networkSender->start();

    while (gameController->isRunning()) {
        inputHandler->processInput();
        gameController->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
