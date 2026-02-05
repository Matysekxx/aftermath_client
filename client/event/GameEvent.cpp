#include "GameEvent.h"

GameEvent::GameEvent(const EventType type, const nlohmann::json &payload) {
    this->type = type;
    this->payload = payload;
}

EventType GameEvent::getType() const {
    return type;
}

nlohmann::json GameEvent::getPayload() const {
    return payload;
}

GameEvent parseEvent(const std::string &message) {
    nlohmann::json json = nlohmann::json::parse(message);
    const auto type = json.value("type", "UNKNOWN");
    const EventType eventType = stringToType.count(type) == 1 ? stringToType[type] : EventType::UNKNOWN;
    return {eventType, json};
}
