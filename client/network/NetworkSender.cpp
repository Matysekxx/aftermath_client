//
// Created by chalo on 30.01.2026.
//

#include "NetworkSender.h"
#include <iostream>
#include <nlohmann/json.hpp>

NetworkSender::NetworkSender(BlockingQueue<GameEvent>* queue, WebSocketHandler* handler) {
    this->outputQueue = queue;
    this->wsHandler = handler;
}

NetworkSender::~NetworkSender() {
    stop();
}

void NetworkSender::start() {
    if (running) return;
    running = true;

    senderThread = std::thread([this]() {
        while (running) {
            const GameEvent event = outputQueue->take();
            const nlohmann::json jsonPayload = event.getPayload();
            const std::string message = jsonPayload.dump();
            if (wsHandler) {
                wsHandler->send(message);
            }
        }
    });
}

void NetworkSender::stop() {
    running = false;
    outputQueue->enqueue(GameEvent(EventType::UNKNOWN, {}));
    if (senderThread.joinable()) {
        senderThread.join();
    }
}
