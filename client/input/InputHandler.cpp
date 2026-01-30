//
// Created by chalo on 30.01.2026.
//

#include "InputHandler.h"
#include <conio.h>


InputHandler::InputHandler(BlockingQueue<GameEvent>* outQueue) : outputQueue(outQueue) {
    setupBindings();
}

void InputHandler::setupBindings() {
    keyBindings['w'] = [this]()  { outputQueue->enqueue(parseEvent("{\"direction\": \"UP\"}"));};
    keyBindings['a'] = [this]()  { outputQueue->enqueue(parseEvent("{\"direction\": \"LEFT\"}"));};
    keyBindings['s'] = [this]()  { outputQueue->enqueue(parseEvent("{\"direction\": \"DOWN\"}"));};
    keyBindings['d'] = [this]()  { outputQueue->enqueue(parseEvent("{\"direction\": \"RIGHT\"}"));};
}

void InputHandler::processInput() {
    if (_kbhit()) {
        const char key = tolower(getch());
        if (keyBindings.count(key)) {
            keyBindings[key]();
        }
    }
}
