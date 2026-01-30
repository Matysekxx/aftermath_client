//
// Created by chalo on 30.01.2026.
//

#ifndef APP_H
#define APP_H
#include "controller/GameController.h"
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include "network/WebSocketHandler.h"


class Application {
    private:
    std::unique_ptr<BlockingQueue<GameEvent>> fromServerToClient;
    std::unique_ptr<BlockingQueue<GameEvent>> fromClientToServer;
    std::unique_ptr<WebSocketHandler> wsHandler;
    std::unique_ptr<GameController> gameController;
    public:
    explicit Application(const std::string& url);
    void execute();
};



#endif //APP_H
