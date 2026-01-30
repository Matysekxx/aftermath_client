//
// Created by chalo on 30.01.2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H
#include "controller/GameController.h"
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include "input/InputHandler.h"
#include "network/NetworkSender.h"
#include "network/WebSocketHandler.h"


class Application {
    private:
    std::unique_ptr<BlockingQueue<GameEvent>> fromServerToClient;
    std::unique_ptr<BlockingQueue<GameEvent>> fromClientToServer;
    std::unique_ptr<WebSocketHandler> wsHandler;
    std::unique_ptr<GameController> gameController;
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<NetworkSender> networkSender;

    public:
    explicit Application(const std::string& url);
    void execute() const;
};



#endif //APPLICATION_H
