#include "NetworkSender.h"
#include <iostream>

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
        GameEvent event(EventType::UNKNOWN, nullptr);
        if (outputQueue->tryPop(event)) {
            std::string msg = event.getPayload().dump();
            ws->send(msg);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}
