//
// Created by chalo on 30.01.2026.
//

#include "GameState.h"

GameState::GameState() {
}

void GameState::updateEntity(const int id, const float x, const float y) {
    for (Entity& entity : entities) {
        if (entity.id == id) {
            entity.x = x;
            entity.y = y;
        }
    }
}

void GameState::addEntity(const Entity& entity) {
    entities.push_back(entity);
}

void GameState::removeEntity(int id) {
    for (int i = 0; i < entities.size(); ++i) {
        if (entities.at(i).id == id) {
            entities.erase(entities.begin() + i);
            return;
        }
    }
}

const std::vector<Entity>& GameState::getEntities() const {
    return entities;
}
