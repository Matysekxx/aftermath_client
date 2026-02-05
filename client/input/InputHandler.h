#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"
#include "../game/GameState.h"

class InputHandler {
private:
    BlockingQueue<GameEvent> *outputQueue;
    std::map<int, std::function<void()> > keyBindings;

    void setupBindings();

public:
    explicit InputHandler(BlockingQueue<GameEvent> *outQueue);

    void processGameInput(GameState &state);

    void processLoginInput(GameState &state);
};

#endif //INPUTHANDLER_H
