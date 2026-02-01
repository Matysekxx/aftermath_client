//
// Created by chalo on 30.01.2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include "input/InputHandler.h"


class Application {
    private:
    std::unique_ptr<BlockingQueue<GameEvent>> fromServerToClient;
    std::unique_ptr<BlockingQueue<GameEvent>> fromClientToServer;
    std::unique_ptr<InputHandler> inputHandler;

    public:
    explicit Application(const std::string& url);
    void execute() const;
};



#endif //APPLICATION_H
