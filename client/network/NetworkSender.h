//
// Created by chalo on 30.01.2026.
//

#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"
#include "WebSocketHandler.h"
#include <thread>
#include <atomic>

class NetworkSender {
private:
    BlockingQueue<GameEvent>* outputQueue;
    WebSocketHandler* wsHandler;
    std::thread senderThread;
    std::atomic<bool> running{false};

public:
    NetworkSender(BlockingQueue<GameEvent>* queue, WebSocketHandler* handler);
    ~NetworkSender();

    void start();
    void stop();
};

#endif //NETWORKSENDER_H
