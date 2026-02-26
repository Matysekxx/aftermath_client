#include "InputHandler.h"
#include "../game/GameController.h"
#include <conio.h>
#include <iostream>

using json = nlohmann::json;

namespace KeyCodes {
    constexpr int Up = 72;
    constexpr int Down = 80;
    constexpr int Left = 75;
    constexpr int Right = 77;
    constexpr int Enter = 13;
    constexpr int Backspace = 8;
    constexpr int ExtendedOffset = 1000;
    constexpr int Escape = 27;
}

InputHandler::InputHandler(BlockingQueue<GameEvent> *outQueue) : outputQueue(outQueue), gameController(nullptr) {
    setupBindings();
}

void InputHandler::setGameController(GameController* controller) {
    this->gameController = controller;
}

void InputHandler::setupBindings() {
    using namespace KeyCodes;

    keyBindings[Up + ExtendedOffset] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "UP"}})"));
    };
    keyBindings[Down + ExtendedOffset] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "DOWN"}})"));
    };
    keyBindings[Left + ExtendedOffset] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "LEFT"}})"));
    };
    keyBindings[Right + ExtendedOffset] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "RIGHT"}})"));
    };

    keyBindings[' '] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "ATTACK", "payload": {}})"));
    };
    keyBindings['e'] = [this]() { outputQueue->enqueue(parseEvent(R"({"type": "INTERACT", "payload": {}})")); };
    keyBindings['u'] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "USE", "payload": {"slotIndex": 0}})"));
    };
}

GameEvent InputHandler::parseEvent(const std::string& jsonStr) {
    auto j = json::parse(jsonStr);
    const std::string typeStr = j.value("type", "UNKNOWN");
    auto type = EventType::UNKNOWN;

    if (typeStr == "MOVE") type = EventType::PLAYER_MOVED;
    else if (typeStr == "ATTACK") type = EventType::UNKNOWN;
    else if (typeStr == "INTERACT") type = EventType::UNKNOWN;

    return {type, j};
}

void InputHandler::processGameInput(GameState &state) {
    if (_kbhit()) {
        int key = _getch();
        bool isExtended = false;
        if (key == 0 || key == 224) {
            key = _getch();
            isExtended = true;
        }

        if (state.isAnnouncementOpen) {
            handleAnnouncementInput(state, key);
            return;
        }

        if (state.isDialogOpen) {
            handleDialogInput(state, key);
            return;
        }

        if (state.showHelp) {
            handleHelpInput(state, key, isExtended);
            return;
        }

        if (state.isMenuOpen) {
            handleMenuInput(state, key, isExtended);
            return;
        }

        if (state.isPayDebtOpen) {
            handlePayDebtInput(state, key);
            return;
        }

        if (state.isMetroUiOpen) {
            handleMetroInput(state, key, isExtended);
            return;
        }

        if (state.isTradeUiOpen) {
            handleTradeInput(state, key, isExtended);
            return;
        }

        if (state.isInventoryOpen) {
            handleInventoryInput(state, key, isExtended);
            return;
        }

        handleStandardGameInput(state, key, isExtended);
    }
}

void InputHandler::handleAnnouncementInput(GameState &state, int key) {
    if (key == KeyCodes::Enter || key == KeyCodes::Escape) {
        state.closeAnnouncement();
    }
}

void InputHandler::handleDialogInput(GameState &state, int key) {
    if (key == KeyCodes::Enter || key == KeyCodes::Escape) {
        state.closeDialog();
    }
}

void InputHandler::handleHelpInput(GameState &state, int key, bool isExtended) {
    if (key == KeyCodes::Escape || (!isExtended && (key == 'h' || key == 'H'))) {
        state.toggleHelp();
    }
}

void InputHandler::handleMenuInput(GameState &state, int key, bool isExtended) {
    if (key == KeyCodes::Escape) {
        state.toggleMenu();
        return;
    }
    if (isExtended && key == KeyCodes::Up) {
        state.scrollMenu(-1);
    }
    else if (isExtended && key == KeyCodes::Down) {
        state.scrollMenu(1);
    }
    else if (key == KeyCodes::Enter) {
        switch (state.menuSelectionIndex) {
            case 0: state.toggleMenu(); break;
            case 1: state.toggleHelp(); break;
            case 2: state.exitRequested = true; break;
            default: ;
        }
    }
}

void InputHandler::handlePayDebtInput(GameState &state, int key) {
    if (key == KeyCodes::Escape) {
        state.togglePayDebt();
        return;
    }
    if (key == KeyCodes::Enter) {
        if (!state.debtInput.empty()) {
            try {
                int amount = std::stoi(state.debtInput);
                json payload;
                payload["amount"] = amount;
                json req;
                req["type"] = "PAY_DEBT";
                req["payload"] = payload;
                outputQueue->enqueue(parseEvent(req.dump()));
                state.togglePayDebt();
            } catch (...) {
                state.setError("Invalid amount");
            }
        }
    } else if (key == KeyCodes::Backspace) {
        if (!state.debtInput.empty()) state.debtInput.pop_back();
    } else if (isdigit(key)) {
        state.debtInput += static_cast<char>(key);
    }
}

void InputHandler::handleMetroInput(GameState &state, int key, bool isExtended) {
    if (isExtended && key == KeyCodes::Up) {
        state.scrollMetro(-1);
    }
    else if (isExtended && key == KeyCodes::Down) {
        state.scrollMetro(1);
    }
    else if (key == KeyCodes::Enter) {
        auto[id, name] = state.getSelectedMetroStation();
        if (!id.empty()) {
            json payload;
            payload["mapId"] = id;
            payload["lineId"] = state.metroUi.lineId;
            json req;
            req["type"] = "TRAVEL";
            req["payload"] = payload;
            outputQueue->enqueue(parseEvent(req.dump()));
            state.closeMetroUi();
        }
    } else if (key == KeyCodes::Escape) {
        state.closeMetroUi();
    }
}

void InputHandler::handleTradeInput(GameState &state, int key, bool isExtended) {
    if (isExtended && key == KeyCodes::Up) {
        state.scrollTrade(-1);
    }
    else if (isExtended && key == KeyCodes::Down) {
        state.scrollTrade(1);
    }
    else if (key == KeyCodes::Enter) {
        if (state.tradeMode == TradeMode::BUY) {
            auto item = state.getSelectedTradeItem();
            if (!item.id.empty()) {
                json payload;
                payload["npcId"] = state.tradeUi.npcId;
                payload["itemIndex"] = state.tradeSelectionIndex;
                json req;
                req["type"] = "BUY";
                req["payload"] = payload;
                outputQueue->enqueue(parseEvent(req.dump()));
            }
        } else {
            int slot = state.getSelectedInventorySlot();
            if (slot != -1) {
                json payload;
                payload["npcId"] = state.tradeUi.npcId;
                payload["slotIndex"] = slot;
                json req;
                req["type"] = "SELL";
                req["payload"] = payload;
                outputQueue->enqueue(parseEvent(req.dump()));
            }
        }
    } else if (!isExtended && (key == 's' || key == 'S')) {
        state.toggleTradeMode();
    } else if (key == KeyCodes::Escape) {
        state.closeTradeUi();
    }
}

void InputHandler::handleInventoryInput(GameState &state, int key, bool isExtended) {
    if ((!isExtended && (key == 'i' || key == 'I')) || key == KeyCodes::Escape) {
        state.toggleInventory();
        return;
    }
    if (isExtended && key == KeyCodes::Up) {
        state.scrollInventory(-1);
    }
    else if (isExtended && key == KeyCodes::Down) {
        state.scrollInventory(1);
    }
    else if (!isExtended && (key == 'e' || key == 'E')) {
        if (const int slot = state.getSelectedInventorySlot();
            slot != -1) {
            outputQueue->enqueue(parseEvent(R"({"type": "EQUIP", "payload": {"slotIndex": )" + std::to_string(slot) + "}}"));
        }
    } else if (!isExtended && (key == 'u' || key == 'U')) {
        if (const int slot = state.getSelectedInventorySlot();
            slot != -1) {
            outputQueue->enqueue(parseEvent(R"({"type": "USE", "payload": {"slotIndex": )" + std::to_string(slot) + "}}"));
        }
    } else if (!isExtended && (key == 'd' || key == 'D')) {
        if (const int slot = state.getSelectedInventorySlot();
            slot != -1) {
            outputQueue->enqueue(parseEvent(R"({"type": "DROP", "payload": {"slotIndex": )" + std::to_string(slot) + R"(, "amount": 1}})"));
        }
    }
}

void InputHandler::handleStandardGameInput(GameState &state, int key, bool isExtended) {
    const int bindingKey = isExtended ? key + KeyCodes::ExtendedOffset : key;

    if (key == KeyCodes::Escape) {
        state.toggleMenu();
        return;
    }

    if (!isExtended && (key == 'i' || key == 'I')) {
        state.toggleInventory();
        return;
    }

    if (!isExtended && (key == 'l' || key == 'L')) {
        state.toggleLogs();
        return;
    }

    if (!isExtended && (key == 'h' || key == 'H')) {
        state.toggleHelp();
        return;
    }

    if (!isExtended && (key == 'p' || key == 'P')) {
        state.togglePayDebt();
        return;
    }

    if (keyBindings.count(bindingKey)) {
        keyBindings[bindingKey]();
    } else if (!isExtended && keyBindings.count(tolower(key))) {
        keyBindings[tolower(key)]();
    }
}

void InputHandler::processLoginInput(GameState &state) {
    if (_kbhit()) {
        int key = _getch();
        bool isExtended = false;

        if (key == 0 || key == 224) {
            key = _getch();
            isExtended = true;
        }

        if (state.loginStep == 1) {
            if (isExtended && key == KeyCodes::Up && state.selectedClassIndex > 0) {
                state.selectedClassIndex--;
            }
            if (isExtended && key == KeyCodes::Down && state.selectedClassIndex < state.loginOptions.classes.size() - 1) {
                state.selectedClassIndex++;
            }
        } else if (state.loginStep == 2) {
            if (isExtended && key == KeyCodes::Up && state.selectedMapIndex > 0) {
                state.selectedMapIndex--;
            }
            if (isExtended && key == KeyCodes::Down && state.selectedMapIndex < state.loginOptions.maps.size() - 1) {
                state.selectedMapIndex++;
            }
        }

        if (key == KeyCodes::Backspace) {
            if (state.loginStep == 0 && !state.inputUsername.empty()) {
                state.inputUsername.pop_back();
            }
            return;
        }

        if (key == KeyCodes::Enter) {
            if (state.loginStep == 0 && !state.inputUsername.empty()) state.loginStep++;
            else if (state.loginStep == 1 && !state.loginOptions.classes.empty()) state.loginStep++;
            else if (state.loginStep == 2 && !state.loginOptions.maps.empty()) {
                json payload;
                payload["username"] = state.inputUsername;
                payload["playerClass"] = state.loginOptions.classes[state.selectedClassIndex];
                payload["startingMapId"] = state.loginOptions.maps[state.selectedMapIndex].mapId;

                json req;
                req["type"] = "LOGIN";
                req["payload"] = payload;

                outputQueue->enqueue(parseEvent(req.dump()));
                state.loginStep = 3;
            }
            return;
        }
        if (state.loginStep == 0) {
            if (!isExtended && (isalnum(key) || key == '_' || key == '-')) {
                state.inputUsername += static_cast<char>(key);
            }
        }
    }
}
