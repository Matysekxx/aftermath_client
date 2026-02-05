#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H
#include "ixwebsocket/IXWebSocket.h"
#include <string>

#include "event/BlockingQueue.h"
#include "event/GameEvent.h"


class NetworkHandler {
private:
    BlockingQueue<GameEvent> *inputQueue;
    std::unique_ptr<ix::WebSocket> webSocket;

    void init() const;

public:
    NetworkHandler(const std::string &url, BlockingQueue<GameEvent> *inputQueue);

    [[nodiscard]] ix::WebSocket *getWebSocket() const;

    void start() const;
};


#endif //NETWORKHANDLER_H
