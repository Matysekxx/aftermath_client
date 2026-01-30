//
// Created by chalo on 30.01.2026.
//

#include "GameController.h"

GameController::GameController(BlockingQueue<GameEvent>* inQueue, BlockingQueue<GameEvent>* outQueue) {
    this->inputQueue = inQueue;
    this->outputQueue = outQueue;
    this->running = true;
}

bool GameController::isRunning() const {
    return running;
}

void GameController::update() {
    // TODO: Process game logic and events
}

void GameController::handleEvent(const GameEvent& event) {
    // TODO: Handle specific game event
}

void GameController::stop() {
    // TODO: Stop controller loop
}
