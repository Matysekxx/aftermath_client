#include "Application.h"
#include <thread>
#include <chrono>

#include "network/NetworkSender.h"

Application::Application(const std::string &url) {
    this->fromServerToClient = std::make_unique<BlockingQueue<GameEvent> >();
    this->fromClientToServer = std::make_unique<BlockingQueue<GameEvent> >();
    this->networkHandler = std::make_unique<NetworkHandler>(url, fromServerToClient.get());
    this->gameController = std::make_unique<GameController>(fromServerToClient.get(), fromClientToServer.get());
    this->inputHandler = std::make_unique<InputHandler>(fromClientToServer.get());
    this->inputHandler->setGameController(gameController.get());
    this->networkSender = std::make_unique<NetworkSender>(fromClientToServer.get(), networkHandler.get());
}

void Application::execute() const {
    networkHandler->start();
    networkSender->start();

    while (gameController->isRunning()) {
        gameController->handleInput(*inputHandler);
        gameController->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
