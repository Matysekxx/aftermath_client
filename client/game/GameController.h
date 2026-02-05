#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include "GameState.h"
#include "../ui/TuiRenderer.h"
#include <atomic>

class InputHandler;

class GameController {
    BlockingQueue<GameEvent> *outputQueue;
    BlockingQueue<GameEvent> *inputQueue;
    GameState gameState;
    TuiRenderer renderer;
    std::atomic<bool> running;

public:
    GameController(BlockingQueue<GameEvent> *inputQueue, BlockingQueue<GameEvent> *outputQueue);

    void update();

    void handleInput(InputHandler &inputHandler);

    [[nodiscard]] bool isRunning() const;

    void stop();
};


#endif //GAMECONTROLLER_H
