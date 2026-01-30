//
// Created by chalo on 30.01.2026.
//

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../event/GameEvent.h"
#include "../event/BlockingQueue.h"
#include <memory>

class GameController {
private:
    BlockingQueue<GameEvent>* inputQueue;
    BlockingQueue<GameEvent>* outputQueue;
    bool running;

public:
    GameController(BlockingQueue<GameEvent>* inQueue, BlockingQueue<GameEvent>* outQueue);

    void update();
    void handleEvent(const GameEvent& event);
    void stop();
};

#endif //GAMECONTROLLER_H
