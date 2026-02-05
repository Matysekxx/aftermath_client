#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H
#include "NetworkHandler.h"
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include <thread>
#include <atomic>


class NetworkSender {
private:
    BlockingQueue<GameEvent> *outputQueue;
    ix::WebSocket *ws;
    std::thread senderThread;
    std::atomic<bool> running;

    void run();

public:
    NetworkSender(BlockingQueue<GameEvent> *outputQueue, NetworkHandler *networkHandler);

    ~NetworkSender();

    void start();

    void stop();
};


#endif //NETWORKSENDER_H
