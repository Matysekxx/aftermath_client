#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>

#include "EventType.h"
#include <nlohmann/json.hpp>

/**
 * @brief Represents a single event in the game system.
 *
 * A GameEvent consists of a type (EventType) and a payload (JSON).
 * It is the standard unit of communication between the server and client,
 * and between different components of the client application.
 */
class GameEvent {
private:
    EventType type;
    nlohmann::json payload;

public:
    /**
     * @brief Constructs a GameEvent.
     * @param type The type of the event.
     * @param payload The data associated with the event.
     */
    GameEvent(EventType type, const nlohmann::json &payload);

    /**
     * @brief Gets the type of the event.
     * @return The EventType enum value.
     */
    [[nodiscard]] EventType getType() const;

    /**
     * @brief Gets the payload of the event.
     * @return The JSON object containing event data.
     */
    [[nodiscard]] nlohmann::json getPayload() const;
};

/**
 * @brief Parses a raw JSON string into a GameEvent object.
 * @param message The raw JSON string received from the network.
 * @return A constructed GameEvent.
 */
GameEvent parseEvent(const std::string &message);


#endif //GAMEEVENT_H
