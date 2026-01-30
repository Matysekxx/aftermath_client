//
// Created by chalo on 30.01.2026.
//

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"

class InputHandler {
private:
    BlockingQueue<GameEvent>* outputQueue;
    std::map<char, std::function<void()>> keyBindings;
    void setupBindings();
public:
    explicit InputHandler(BlockingQueue<GameEvent>* outQueue);

    void processInput();
};

#endif //INPUTHANDLER_H
