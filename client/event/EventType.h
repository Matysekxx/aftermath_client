//
// Created by chalo on 30.01.2026.
//

#ifndef EVENTTYPE_H
#define EVENTTYPE_H

enum class EventType {
    PLAYER_MOVED,
    STATS_UPDATE,
    MAP_OBJECTS_UPDATE,
    GAME_OVER,
    NOTIFICATION,
    UNKNOWN
};

static std::map<std::string, EventType> stringToType = {
    {"PLAYER_MOVED", EventType::PLAYER_MOVED},
    {"STATS_UPDATE", EventType::STATS_UPDATE},
    {"MAP_OBJECTS_UPDATE", EventType::MAP_OBJECTS_UPDATE},
    {"GAME_OVER",  EventType::GAME_OVER},
    {"NOTIFICATION", EventType::NOTIFICATION}
};

#endif //EVENTTYPE_H
