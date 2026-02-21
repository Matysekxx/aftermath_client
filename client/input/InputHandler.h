#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"
#include "../game/GameState.h"
#include <nlohmann/json.hpp>

class InputHandler {
private:
    BlockingQueue<GameEvent> *outputQueue;
    std::map<int, std::function<void()> > keyBindings;

    using json = nlohmann::json;

    void setupBindings();
    GameEvent parseEvent(const std::string& jsonStr);

    void handleAnnouncementInput(GameState &state, int key);
    void handleDialogInput(GameState &state, int key);
    void handleHelpInput(GameState &state, int key, bool isExtended);
    void handleMenuInput(GameState &state, int key, bool isExtended);
    void handlePayDebtInput(GameState &state, int key);
    void handleMetroInput(GameState &state, int key, bool isExtended);
    void handleTradeInput(GameState &state, int key, bool isExtended);
    void handleInventoryInput(GameState &state, int key, bool isExtended);
    void handleStandardGameInput(GameState &state, int key, bool isExtended);

public:
    explicit InputHandler(BlockingQueue<GameEvent> *outQueue);

    void processGameInput(GameState &state);

    void processLoginInput(GameState &state);
};

#endif //INPUTHANDLER_H
