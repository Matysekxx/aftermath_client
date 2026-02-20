#include "NetworkSender.h"
#include <iostream>
#include <nlohmann/json.hpp>

NetworkSender::NetworkSender(BlockingQueue<GameEvent> *outputQueue, NetworkHandler *networkHandler) {
    this->outputQueue = outputQueue;
    this->ws = networkHandler->getWebSocket();
    this->running = false;
}

NetworkSender::~NetworkSender() {
    stop();
}

void NetworkSender::start() {
    running = true;
    senderThread = std::thread(&NetworkSender::run, this);
}

void NetworkSender::stop() {
    running = false;
    if (senderThread.joinable()) {
        senderThread.join();
    }
}

void NetworkSender::run() {
    while (running) {
        if (GameEvent event(EventType::UNKNOWN, nullptr);
            outputQueue->tryPop(event)) {
            std::string msg = event.getPayload().dump();
            ws->send(msg);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void NetworkSender::sendPayDebt(int amount) {
    nlohmann::json payload;
    payload["amount"] = amount;
    nlohmann::json root;
    root["type"] = "PAY_DEBT";
    root["payload"] = payload;
    outputQueue->enqueue(GameEvent(EventType::PAY_DEBT, root));
}
