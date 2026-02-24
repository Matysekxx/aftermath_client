#include "GameController.h"
#include "../input/InputHandler.h"
#include "../dto/GameEventTypes.h"
#include "../utils/JsonParser.h"
#include <iostream>
#include <fstream>

using namespace utils;

GameController::GameController(BlockingQueue<GameEvent> *inputQueue, BlockingQueue<GameEvent> *outputQueue) {
    this->inputQueue = inputQueue;
    this->outputQueue = outputQueue;
    this->running = true;
}

void GameController::update() {
    if (gameState.exitRequested) {
        stop();
        return;
    }

    GameEvent event(EventType::UNKNOWN, nullptr);
    static std::ofstream logger("client_debug.log", std::ios::app);

    while (inputQueue->tryPop(event)) {
        const auto &type = event.getType();
        const auto &root = event.getPayload();

        if (type == EventType::CONNECTION_ESTABLISHED) {
            handleConnectionEstablished();
            continue;
        }

        if (root.is_null()) {
            if (logger.is_open()) logger << "[ERROR] Event payload is NULL!" << std::endl;
            continue;
        }

        if (logger.is_open()) {
            std::string typeStr = JsonParser::safeString(root, "type", "UNKNOWN_TYPE");
            logger << "[EVENT] Type: " << typeStr << " | Enum: " << static_cast<int>(type) << std::endl;
            std::lock_guard lock(gameState.stateMutex);
            gameState.addNetworkLog("IN", typeStr, root.value("payload", json({})).dump());
        }

        if (type == EventType::UNKNOWN) {
            std::string rawType = JsonParser::safeString(root, "type", "???");
            std::lock_guard lock(gameState.stateMutex);
            gameState.connectionStatus = "Unknown Event: " + rawType;
        }

        if (!root.contains("payload")) {
            std::string rawType = JsonParser::safeString(root, "type", "???");
            std::lock_guard lock(gameState.stateMutex);
            gameState.connectionStatus = "Missing payload for: " + rawType;
            continue;
        }

        const auto &data = root["payload"];
        if (data.is_null()) {
            continue;
        }

        dispatchEvent(type, data);
    }

    renderer.render(gameState);
}

void GameController::dispatchEvent(const EventType& type, const json& data) {
    std::lock_guard lock(gameState.stateMutex);

    switch (type) {
        case EventType::SEND_STATS: handleSendStats(data); break;
        case EventType::SEND_INVENTORY: handleSendInventory(data); break;
        case EventType::SEND_PLAYER_POSITION: handleSendPlayerPosition(data); break;
        case EventType::SEND_MAP_DATA: handleSendMapData(data); break;
        case EventType::SEND_LOGIN_OPTIONS: handleSendLoginOptions(data); break;
        case EventType::SEND_NPCS: handleSendNpcs(data); break;
        case EventType::SEND_MAP_OBJECTS: handleSendMapObjects(data); break;
        case EventType::BROADCAST_CHAT_MSG: handleBroadcastChatMsg(data); break;
        case EventType::OPEN_METRO_UI: handleOpenMetroUi(data); break;
        case EventType::OPEN_TRADE_UI: handleOpenTradeUi(data); break;
        case EventType::SEND_GAME_OVER: handleSendGameOver(); break;
        case EventType::SEND_MESSAGE: handleSendMessage(data, false); break;
        case EventType::SEND_ERROR: handleSendMessage(data, true); break;
        case EventType::GLOBAL_ANNOUNCEMENT: handleGlobalAnnouncement(data); break;
        case EventType::BROADCAST_PLAYERS: handleBroadcastPlayers(data); break;
        case EventType::DIALOG: handleDialog(data); break;
        default: break;
    }
}

void GameController::handleConnectionEstablished() {
    std::lock_guard lock(gameState.stateMutex);
    gameState.connectionStatus = "Connected. Sending INIT...";
    this->outputQueue->enqueue(parseEvent(R"({"type": "INIT", "payload": null})"));
}

void GameController::handleSendStats(const json& data) {
    gameState.player.hp = JsonParser::safeInt(data, "hp", gameState.player.hp);
    gameState.player.maxHp = JsonParser::safeInt(data, "maxHp", gameState.player.maxHp);
    gameState.player.rads = JsonParser::safeInt(data, "rads", gameState.player.rads);
    gameState.player.credits = JsonParser::safeInt(data, "credits", gameState.player.credits);
    gameState.player.debt = JsonParser::safeInt(data, "debt", gameState.player.debt);
    gameState.player.globalDebt = JsonParser::safeLong(data, "globalDebt", gameState.player.globalDebt);

    if (data.contains("equippedWeaponSlot")) {
        if (data["equippedWeaponSlot"].is_null()) gameState.player.equippedWeaponSlot = "";
        else if (data["equippedWeaponSlot"].is_number()) gameState.player.equippedWeaponSlot = std::to_string(data["equippedWeaponSlot"].get<int>());
        else if (data["equippedWeaponSlot"].is_string()) gameState.player.equippedWeaponSlot = data["equippedWeaponSlot"].get<std::string>();
    }

    if (data.contains("equippedMaskSlot")) {
        if (data["equippedMaskSlot"].is_null()) gameState.player.equippedMaskSlot = "";
        else if (data["equippedMaskSlot"].is_number()) gameState.player.equippedMaskSlot = std::to_string(data["equippedMaskSlot"].get<int>());
        else if (data["equippedMaskSlot"].is_string()) gameState.player.equippedMaskSlot = data["equippedMaskSlot"].get<std::string>();
    }

    gameState.clientState = ClientState::PLAYING;
    gameState.clearError();
}

void GameController::handleSendInventory(const json& data) {
    gameState.player.inventory.slots.clear();
    if (data.is_object()) {
        for (auto &[key, val]: data.items()) {
            if (!val.is_null()) {
                gameState.player.inventory.slots[std::stoi(key)] = JsonParser::parseItem(val);
            }
        }
    }
    gameState.clientState = ClientState::PLAYING;
}

void GameController::handleSendPlayerPosition(const json& data) {
    gameState.player.x = JsonParser::safeInt(data, "x", gameState.player.x);
    gameState.player.y = JsonParser::safeInt(data, "y", gameState.player.y);
    gameState.player.layerIndex = JsonParser::safeInt(data, "z", gameState.player.layerIndex);
    gameState.clientState = ClientState::PLAYING;
}

void GameController::handleSendMapData(const json& data) {
    gameState.updateMap(JsonParser::parseMap(data));
    static std::ofstream logger("client_debug.log", std::ios::app);
    if (logger.is_open()) {
        logger << "[MAP] Updated map: " << gameState.map.mapName
               << " Range: " << gameState.map.rangeX << "x" << gameState.map.rangeY << std::endl;
    }
    gameState.addGameLog("Mapa načtena: " + gameState.map.mapName);
}

void GameController::handleSendLoginOptions(const json& data) {
    gameState.loginOptions = JsonParser::parseLoginOptions(data);
    gameState.clientState = ClientState::LOGIN_SCREEN;
    gameState.loginStep = 0;
    gameState.addGameLog("Login options received.");
}

void GameController::handleSendNpcs(const json& data) {
    gameState.updateNpcs(JsonParser::parseNpcs(data));
}

void GameController::handleSendMapObjects(const json& data) {
    gameState.updateObjects(JsonParser::parseMapObjects(data));
}

void GameController::handleBroadcastChatMsg(const json& data) {
    gameState.addChatMessage({JsonParser::safeString(data, "message", "")});
    gameState.addGameLog("CHAT: " + JsonParser::safeString(data, "message", ""));
}

void GameController::handleOpenMetroUi(const json& data) {
    dto::MetroUiResponse metro;
    metro.lineId = JsonParser::safeString(data, "lineId", "");
    if (data.contains("stations") && data["stations"].is_array()) {
        for (const auto &st: data["stations"]) {
            if (!st.is_null()) {
                metro.stations.push_back({JsonParser::safeString(st, "id", ""), JsonParser::safeString(st, "name", "")});
            }
        }
    }
    gameState.setMetroUi(metro);
    gameState.addGameLog("Metro UI opened.");
}

void GameController::handleOpenTradeUi(const json& data) {
    dto::TradeUiLoadResponse trade;
    trade.npcId = JsonParser::safeString(data, "npcId", "");
    trade.npcName = JsonParser::safeString(data, "npcName", "");
    if (data.contains("items") && data["items"].is_array()) {
        for (const auto &item: data["items"]) {
            if (!item.is_null()) {
                trade.items.push_back(JsonParser::parseItem(item));
            }
        }
    }
    gameState.setTradeUi(trade);
    gameState.addGameLog("Trade UI opened.");
}

void GameController::handleSendGameOver() {
    gameState.addGameLog("GAME OVER!");
    gameState.setError("YOU DIED! Restart client to respawn.");
}

void GameController::handleSendMessage(const json& data, bool isError) {
    if (data.is_string()) {
        const std::string msg = data.get<std::string>();
        gameState.addChatMessage({msg});
        if (isError) {
            gameState.setError(msg);
        }
    }
}

void GameController::handleGlobalAnnouncement(const json& data) {
    if (data.is_string()) {
        std::string msg = data.get<std::string>();
        gameState.addChatMessage({"[GLOBAL] " + msg});
        gameState.addGameLog("[GLOBAL] " + msg);
        gameState.showAnnouncement(msg);
    }
}

void GameController::handleBroadcastPlayers(const json& data) {
    gameState.updateOtherPlayers(JsonParser::parseOtherPlayers(data, gameState.player.id));
}

void GameController::handleDialog(const json& data) {
    dto::DialogResponse dialog;
    dialog.npcName = JsonParser::safeString(data, "npcName", "Unknown");
    dialog.text = JsonParser::safeString(data, "text", "");
    gameState.openDialog(dialog);
    gameState.addGameLog("Dialog started with " + dialog.npcName);
}

void GameController::handleInput(InputHandler &inputHandler) {
    std::lock_guard lock(gameState.stateMutex);

    if (gameState.clientState == ClientState::LOGIN_SCREEN) {
        inputHandler.processLoginInput(gameState);
    } else if (gameState.clientState == ClientState::PLAYING) {
        inputHandler.processGameInput(gameState);
    }
}

bool GameController::isRunning() const {
    return running;
}

void GameController::stop() {
    running = false;
}
