#include "GameController.h"
#include "../input/InputHandler.h"
#include "../dto/GameEventTypes.h"
#include <iostream>
#include <fstream>

namespace {
    using json = nlohmann::json;
    std::string safeString(const json &j, const std::string &key, const std::string &def) {
        if (j.contains(key) && !j[key].is_null()) {
            return j.value(key, def);
        }
        return def;
    }

    int safeInt(const json &j, const std::string &key, int def) {
        if (j.contains(key) && !j[key].is_null() && j[key].is_number()) {
            return j[key].get<int>();
        }
        return def;
    }

    long safeLong(const json &j, const std::string &key, long def) {
        if (j.contains(key) && !j[key].is_null() && j[key].is_number()) {
            return j[key].get<long>();
        }
        return def;
    }

    dto::ItemDto parseItem(const json &j) {
        if (j.is_null()) return {};
        dto::ItemDto item;
        item.id = safeString(j, "id", "");
        item.name = safeString(j, "name", "Unknown");
        item.type = safeString(j, "type", "MISC");
        item.description = safeString(j, "description", "");
        item.rarity = safeString(j, "rarity", "COMMON");
        item.quantity = safeInt(j, "quantity", 1);
        item.price = safeInt(j, "price", 0);
        return item;
    }

    dto::MapDataResponse parseMap(const json &j) {
        dto::MapDataResponse m;
        if (j.is_null()) return m;

        m.mapName = safeString(j, "mapName", "Unknown Map");
        m.centerX = safeInt(j, "centerX", 0);
        m.centerY = safeInt(j, "centerY", 0);
        m.centerZ = safeInt(j, "centerZ", 0);
        m.rangeX = safeInt(j, "rangeX", 0);
        m.rangeY = safeInt(j, "rangeY", 0);
        if (j.contains("layers") && j["layers"].is_object()) {
            for (auto &[key, val]: j["layers"].items()) {
                if (val.is_array()) {
                    m.layers[key] = val.get<std::vector<std::string> >();
                }
            }
        }
        return m;
    }

    dto::LoginOptionsResponse parseLoginOptions(const json &j) {
        dto::LoginOptionsResponse r;
        if (j.is_null()) return r;

        if (j.contains("classes") && !j["classes"].is_null()) {
            r.classes = j["classes"].get<std::vector<std::string> >();
        }
        if (j.contains("maps") && !j["maps"].is_null()) {
            for (const auto &m: j["maps"]) {
                if (!m.is_null()) {
                    r.maps.push_back({safeString(m, "mapId", ""), safeString(m, "mapName", "")});
                }
            }
        }
        return r;
    }
}

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
            {
                std::lock_guard lock(gameState.stateMutex);
                gameState.connectionStatus = "Connected. Sending INIT...";
            }
            this->outputQueue->enqueue(parseEvent(R"({"type": "INIT", "payload": null})"));
            continue;
        }

        if (root.is_null()) {
            if (logger.is_open()) logger << "[ERROR] Event payload is NULL!" << std::endl;
            continue;
        }

        if (logger.is_open()) {
            std::string typeStr = safeString(root, "type", "UNKNOWN_TYPE");
            logger << "[EVENT] Type: " << typeStr << " | Enum: " << (int)type << std::endl;
            std::lock_guard lock(gameState.stateMutex);
            gameState.addNetworkLog("IN", typeStr, root.value("payload", json({})).dump());
        }

        if (type == EventType::UNKNOWN) {
            std::string rawType = safeString(root, "type", "???");
            std::lock_guard lock(gameState.stateMutex);
            gameState.connectionStatus = "Unknown Event: " + rawType;

        }

        if (!root.contains("payload")) {
            std::string rawType = safeString(root, "type", "???");
            std::lock_guard lock(gameState.stateMutex);
            gameState.connectionStatus = "Missing payload for: " + rawType;
            continue;
        }

        const auto &data = root["payload"];
        if (data.is_null()) {
            continue;
        }

        std::lock_guard lock(gameState.stateMutex);

        if (type == EventType::SEND_STATS) {
            gameState.player.hp = safeInt(data, "hp", gameState.player.hp);
            gameState.player.maxHp = safeInt(data, "maxHp", gameState.player.maxHp);
            gameState.player.rads = safeInt(data, "rads", gameState.player.rads);
            gameState.player.credits = safeInt(data, "credits", gameState.player.credits);
            gameState.player.debt = safeInt(data, "debt", gameState.player.debt);
            gameState.player.globalDebt = safeLong(data, "globalDebt", gameState.player.globalDebt);

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
        } else if (type == EventType::SEND_INVENTORY) {
            gameState.player.inventory.slots.clear();
            if (data.is_object()) {
                for (auto &[key, val]: data.items()) {
                    if (!val.is_null()) {
                        gameState.player.inventory.slots[std::stoi(key)] = parseItem(val);
                    }
                }
            }
            gameState.clientState = ClientState::PLAYING;
        } else if (type == EventType::SEND_PLAYER_POSITION) {
            gameState.player.x = safeInt(data, "x", gameState.player.x);
            gameState.player.y = safeInt(data, "y", gameState.player.y);
            gameState.player.layerIndex = safeInt(data, "z", gameState.player.layerIndex);
            gameState.clientState = ClientState::PLAYING;
        } else if (type == EventType::SEND_MAP_DATA) {
            gameState.updateMap(parseMap(data));
            if (logger.is_open()) {
                logger << "[MAP] Updated map: " << gameState.map.mapName
                       << " Range: " << gameState.map.rangeX << "x" << gameState.map.rangeY << std::endl;
            }
            gameState.addGameLog("Mapa načtena: " + gameState.map.mapName);
        } else if (type == EventType::SEND_LOGIN_OPTIONS) {
            gameState.loginOptions = parseLoginOptions(data);
            gameState.clientState = ClientState::LOGIN_SCREEN;
            gameState.loginStep = 0;
            gameState.addGameLog("Login options received.");
        } else if (type == EventType::SEND_NPCS) {
            std::vector<dto::NpcDto> npcs;
            if (data.is_array()) {
                for (const auto &npcJson: data) {
                    if (!npcJson.is_null()) {
                        npcs.push_back({
                            safeString(npcJson, "id", ""),
                            safeString(npcJson, "name", ""),
                            safeString(npcJson, "type", ""),
                            safeInt(npcJson, "x", 0),
                            safeInt(npcJson, "y", 0),
                            safeInt(npcJson, "hp", 0),
                            safeInt(npcJson, "maxHp", 0),
                            npcJson.value("aggressive", false)
                        });
                    }
                }
            }
            gameState.updateNpcs(npcs);
        } else if (type == EventType::SEND_MAP_OBJECTS) {
            std::vector<dto::MapObjectDto> objects;
            if (data.is_array()) {
                for (const auto &objJson : data) {
                    if (!objJson.is_null()) {
                        dto::MapObjectDto obj;
                        obj.id = safeString(objJson, "id", "");
                        obj.type = safeString(objJson, "type", "");
                        obj.action = safeString(objJson, "action", "");
                        obj.description = safeString(objJson, "description", "");
                        obj.x = safeInt(objJson, "x", 0);
                        obj.y = safeInt(objJson, "y", 0);
                        obj.z = safeInt(objJson, "z", 0);
                        objects.push_back(obj);
                    }
                }
            }
            gameState.updateObjects(objects);
        } else if (type == EventType::BROADCAST_CHAT_MSG) {
            gameState.addChatMessage({safeString(data, "message", "")});
            gameState.addGameLog("CHAT: " + safeString(data, "message", ""));
        } else if (type == EventType::OPEN_METRO_UI) {
            dto::MetroUiResponse metro;
            metro.lineId = safeString(data, "lineId", "");
            if (data.contains("stations") && data["stations"].is_array()) {
                for (const auto &st: data["stations"]) {
                    if (!st.is_null()) {
                        metro.stations.push_back({safeString(st, "id", ""), safeString(st, "name", "")});
                    }
                }
            }
            gameState.setMetroUi(metro);
            gameState.addGameLog("Metro UI opened.");
        } else if (type == EventType::OPEN_TRADE_UI) {
            dto::TradeUiLoadResponse trade;
            trade.npcId = safeString(data, "npcId", "");
            trade.npcName = safeString(data, "npcName", "");
            if (data.contains("items") && data["items"].is_array()) {
                for (const auto &item: data["items"]) {
                    if (!item.is_null()) {
                        trade.items.push_back(parseItem(item));
                    }
                }
            }
            gameState.setTradeUi(trade);
            gameState.addGameLog("Trade UI opened.");
        } else if (type == EventType::SEND_GAME_OVER) {
            gameState.addGameLog("GAME OVER!");
            gameState.setError("YOU DIED! Restart client to respawn.");
        } else if (type == EventType::SEND_MESSAGE || type == EventType::SEND_ERROR) {
            if (data.is_string()) {
                std::string msg = data.get<std::string>();
                gameState.addChatMessage({msg});
                if (type == EventType::SEND_ERROR) {
                    gameState.setError(msg);
                }
            }
        } else if (type == EventType::GLOBAL_ANNOUNCEMENT) {
            if (data.is_string()) {
                std::string msg = data.get<std::string>();
                gameState.addChatMessage({"[GLOBAL] " + msg});
                gameState.addGameLog("[GLOBAL] " + msg);
            }
        } else if (type == EventType::BROADCAST_PLAYERS) {
            std::vector<dto::OtherPlayerDto> players;
            if (data.is_array()) {
                for (const auto &pJson : data) {
                    if (!pJson.is_null()) {
                        std::string pid = safeString(pJson, "id", "");
                        if (pid != gameState.player.id) {
                            players.push_back({
                                pid,
                                safeString(pJson, "name", "Unknown"),
                                safeInt(pJson, "x", 0),
                                safeInt(pJson, "y", 0),
                                safeInt(pJson, "z", 0)
                            });
                        }
                    }
                }
            }
            gameState.updateOtherPlayers(players);
        }
    }

    renderer.render(gameState);
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