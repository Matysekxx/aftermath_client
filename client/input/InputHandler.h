#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"
#include "../game/GameState.h"
#include <nlohmann/json.hpp>

class GameController;

/**
 * @brief Handles user input from the keyboard.
 *
 * This class captures keyboard events, interprets them based on the current
 * game state (e.g., menu, inventory, game world), and sends corresponding
 * GameEvents to the output queue.
 */
class InputHandler {
private:
    BlockingQueue<GameEvent> *outputQueue;
    std::map<int, std::function<void()> > keyBindings;
    GameController* gameController;

    using json = nlohmann::json;

    /**
     * @brief Initializes default key bindings for standard game actions.
     */
    void setupBindings();

    /**
     * @brief Helper to create a GameEvent from a JSON string.
     * @param jsonStr The JSON payload string.
     * @return A constructed GameEvent.
     */
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
    /**
     * @brief Constructs the InputHandler.
     * @param outQueue The queue where generated GameEvents will be pushed.
     */
    explicit InputHandler(BlockingQueue<GameEvent> *outQueue);

    /**
     * @brief Sets the reference to the GameController.
     * @param controller Pointer to the GameController instance.
     */
    void setGameController(GameController* controller);

    /**
     * @brief Processes input when the client is in the PLAYING state.
     * @param state The current game state.
     */
    void processGameInput(GameState &state);

    /**
     * @brief Processes input when the client is in the LOGIN_SCREEN state.
     * @param state The current game state.
     */
    void processLoginInput(GameState &state);
};

#endif //INPUTHANDLER_H
