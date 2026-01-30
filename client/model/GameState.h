//
// Created by chalo on 30.01.2026.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <string>

struct Entity {
    int id;
    float x, y;
    std::string type;
};

class GameState {
private:
    std::vector<Entity> entities;

public:
    GameState();

    void updateEntity(int id, float x, float y);
    void addEntity(const Entity& entity);
    void removeEntity(int id);

    const std::vector<Entity>& getEntities() const;
};

#endif //GAMESTATE_H
