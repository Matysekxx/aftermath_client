#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../network/BlockingQueue.h"
#include "../event/GameEvent.h"
#include "GameState.h"
#include "../ui/TuiRenderer.h"
#include "../audio/AudioService.h"
#include <nlohmann/json.hpp>

class InputHandler;

class GameController {
public:
    GameController(BlockingQueue<GameEvent> *inputQueue, BlockingQueue<GameEvent> *outputQueue, AudioService* audioService);

    void update();

    void handleInput(InputHandler &inputHandler);

    bool isRunning() const;

    void stop();

private:
    BlockingQueue<GameEvent> *inputQueue;
    BlockingQueue<GameEvent> *outputQueue;
    GameState gameState;
    TuiRenderer renderer;
    AudioService* audioService;
    bool running;

    using json = nlohmann::json;

    void dispatchEvent(const EventType& type, const json& payload);

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
