#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../event/GameEvent.h"
#include "GameState.h"
#include "../ui/TuiRenderer.h"
#include <nlohmann/json.hpp>

#include "event/BlockingQueue.h"

class InputHandler;

/**
 * @brief Manages the core game logic and state updates.
 *
 * The GameController processes incoming events from the server, updates the
 * GameState accordingly, and triggers the rendering of the UI. It also
 * handles user input by delegating to the InputHandler.
 */
class GameController {
public:
    /**
     * @brief Constructs the GameController.
     * @param inputQueue Queue for events received from the server.
     * @param outputQueue Queue for events to be sent to the server.
     */
    GameController(BlockingQueue<GameEvent> *inputQueue, BlockingQueue<GameEvent> *outputQueue);

    /**
     * @brief Processes pending events from the input queue and updates the game state.
     *
     * This method should be called in the main loop. It pops events from the queue,
     * dispatches them to appropriate handlers, and then triggers a re-render.
     */
    void update();

    /**
     * @brief Delegates input processing to the InputHandler.
     * @param inputHandler The handler responsible for capturing and processing keyboard input.
     */
    void handleInput(InputHandler &inputHandler);

    /**
     * @brief Checks if the game loop should continue running.
     * @return True if the game is running, false if an exit has been requested.
     */
    bool isRunning() const;

    /**
     * @brief Signals the game loop to stop.
     */
    void stop();

private:
    BlockingQueue<GameEvent> *inputQueue;
    BlockingQueue<GameEvent> *outputQueue;
    GameState gameState;
    TuiRenderer renderer;
    bool running;

    using json = nlohmann::json;

    /**
     * @brief Dispatches a game event to the specific handler method based on its type.
     * @param type The type of the event.
     * @param payload The JSON payload containing event data.
     */
    void dispatchEvent(const EventType& type, const json& payload);

    /**
     * @brief Logs the incoming event to a file and the in-game network log.
     * @param type The type of the event.
     * @param payload The JSON payload of the event.
     */
    void logEvent(const EventType& type, const json& payload);

    void handleConnectionEstablished();
    void handleSendStats(const json& data);
    void handleSendInventory(const json& data);
    void handleSendPlayerPosition(const json& data);
    void handleSendMapData(const json& data);
    void handleSendLoginOptions(const json& data);
    void handleSendNpcs(const json& data);
    void handleSendMapObjects(const json& data);
    void handleBroadcastChatMsg(const json& data);
    void handleOpenMetroUi(const json& data);
    void handleOpenTradeUi(const json& data);
    void handleSendGameOver();
    void handleSendMessage(const json& data, bool isError);
    void handleGlobalAnnouncement(const json& data);
    void handleBroadcastPlayers(const json& data);
    void handleDialog(const json& data);
};

#endif //GAMECONTROLLER_H
