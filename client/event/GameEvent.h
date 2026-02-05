#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>

#include "EventType.h"
#include <nlohmann/json.hpp>

class GameEvent {
private:
    EventType type;
    nlohmann::json payload;

public:
    GameEvent(EventType type, const nlohmann::json &payload);

    [[nodiscard]] EventType getType() const;

    [[nodiscard]] nlohmann::json getPayload() const;
};

GameEvent parseEvent(const std::string &message);


#endif //GAMEEVENT_H
