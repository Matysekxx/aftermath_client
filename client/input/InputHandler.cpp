#include "InputHandler.h"
#include <conio.h>
#include <iostream>

using json = nlohmann::json;

#define KEY_UP_CODE 72
#define KEY_DOWN_CODE 80
#define KEY_LEFT_CODE 75
#define KEY_RIGHT_CODE 77
#define KEY_ENTER_CODE 13
#define KEY_BACKSPACE_CODE 8
#define EXTENDED_OFFSET 1000

InputHandler::InputHandler(BlockingQueue<GameEvent> *outQueue) : outputQueue(outQueue) {
    setupBindings();
}

void InputHandler::setupBindings() {
    keyBindings[KEY_UP_CODE + EXTENDED_OFFSET] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "UP"}})"));
    };
    keyBindings[KEY_DOWN_CODE + EXTENDED_OFFSET] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "DOWN"}})"));
    };
    keyBindings[KEY_LEFT_CODE + EXTENDED_OFFSET] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "LEFT"}})"));
    };
    keyBindings[KEY_RIGHT_CODE + EXTENDED_OFFSET] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "MOVE", "payload": {"direction": "RIGHT"}})"));
    };

    keyBindings[' '] = [this]() { outputQueue->enqueue(parseEvent(R"({"type": "ATTACK", "payload": {}})")); };
    keyBindings['e'] = [this]() { outputQueue->enqueue(parseEvent(R"({"type": "INTERACT", "payload": {}})")); };
    keyBindings['u'] = [this]() {
        outputQueue->enqueue(parseEvent(R"({"type": "USE", "payload": {"slotIndex": 0}})"));
    };
}

void InputHandler::processGameInput(GameState &state) {
    if (_kbhit()) {
        int key = _getch();
        bool isExtended = false;
        if (key == 0 || key == 224) {
            key = _getch();
            isExtended = true;
        }

        const int bindingKey = isExtended ? key + EXTENDED_OFFSET : key;

        if (state.showHelp) {
            if (key == 27 || (!isExtended && (key == 'h' || key == 'H'))) {
                state.toggleHelp();
            }
            return;
        }

        if (state.isMenuOpen) {
            if (key == 27) {
                state.toggleMenu();
                return;
            }
            if (isExtended && key == KEY_UP_CODE) state.scrollMenu(-1);
            else if (isExtended && key == KEY_DOWN_CODE) state.scrollMenu(1);
            else if (key == KEY_ENTER_CODE) {
                switch (state.menuSelectionIndex) {
                    case 0: state.toggleMenu(); break;
                    case 1: state.toggleHelp(); break;
                    case 2: state.exitRequested = true; break;
                    default: ;
                }
            }
            return;
        }

        if (state.isMetroUiOpen) {
            if (isExtended && key == KEY_UP_CODE) state.scrollMetro(-1);
            else if (isExtended && key == KEY_DOWN_CODE) state.scrollMetro(1);
            else if (key == KEY_ENTER_CODE) {
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
            } else if (key == 27) {
                state.closeMetroUi();
            }
            return;
        }

        if (state.isTradeUiOpen) {
            if (isExtended && key == KEY_UP_CODE) state.scrollTrade(-1);
            else if (isExtended && key == KEY_DOWN_CODE) state.scrollTrade(1);
            else if (key == KEY_ENTER_CODE) {
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
            } else if (key == 27) {
                state.closeTradeUi();
            }
            return;
        }

        if (state.isInventoryOpen) {
            if ((!isExtended && (key == 'i' || key == 'I')) || key == 27) {
                state.toggleInventory();
                return;
            }
            if (isExtended && key == KEY_UP_CODE) state.scrollInventory(-1);
            else if (isExtended && key == KEY_DOWN_CODE) state.scrollInventory(1);
            else if (!isExtended && (key == 'e' || key == 'E')) {
                const int slot = state.getSelectedInventorySlot();
                if (slot != -1) {
                    outputQueue->enqueue(parseEvent(R"({"type": "EQUIP", "payload": {"slotIndex": )" + std::to_string(slot) + "}}"));
                }
            } else if (!isExtended && (key == 'u' || key == 'U')) {
                int slot = state.getSelectedInventorySlot();
                if (slot != -1) {
                    outputQueue->enqueue(parseEvent(R"({"type": "USE", "payload": {"slotIndex": )" + std::to_string(slot) + "}}"));
                }
            } else if (!isExtended && (key == 'd' || key == 'D')) {
                const int slot = state.getSelectedInventorySlot();
                if (slot != -1) {
                    outputQueue->enqueue(parseEvent(R"({"type": "DROP", "payload": {"slotIndex": )" + std::to_string(slot) + R"(, "amount": 1}})"));
                }
            }
            return;
        }

        if (key == 27) {
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
        if (keyBindings.count(bindingKey)) {
            keyBindings[bindingKey]();
        } else if (!isExtended && keyBindings.count(tolower(key))) {
            keyBindings[tolower(key)]();
        }
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
            if (isExtended && key == KEY_UP_CODE && state.selectedClassIndex > 0) state.selectedClassIndex--;
            if (isExtended && key == KEY_DOWN_CODE && state.selectedClassIndex < state.loginOptions.classes.size() - 1) state.
                    selectedClassIndex++;
        } else if (state.loginStep == 2) {
            if (isExtended && key == KEY_UP_CODE && state.selectedMapIndex > 0) state.selectedMapIndex--;
            if (isExtended && key == KEY_DOWN_CODE && state.selectedMapIndex < state.loginOptions.maps.size() - 1) state.
                    selectedMapIndex++;
        }

        if (key == KEY_BACKSPACE_CODE) {
            if (state.loginStep == 0 && !state.inputUsername.empty()) {
                state.inputUsername.pop_back();
            }
            return;
        }

        if (key == KEY_ENTER_CODE) {
            if (state.loginStep == 0 && !state.inputUsername.empty()) {
                state.loginStep++;
            } else if (state.loginStep == 1 && !state.loginOptions.classes.empty()) {
                state.loginStep++;
            } else if (state.loginStep == 2 && !state.loginOptions.maps.empty()) {
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
