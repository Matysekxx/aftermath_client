//
// Created by chalo on 30.01.2026.
//

#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>

#include "EventType.h"
#include <nlohmann/json.hpp>

class GameEvent {
    EventType type;
        nlohmann::json payload;
    public:
    GameEvent(EventType type, const nlohmann::json &payload);
    [[nodiscard]] EventType getType() const;
    [[nodiscard]] nlohmann::json getPayload() const;
};

static GameEvent parseEvent(std::string& message);



#endif //GAMEEVENT_H
