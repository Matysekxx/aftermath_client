#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <nlohmann/json.hpp>
#include "../dto/GameResponses.h"
#include <string>
#include <vector>

namespace utils {
    using json = nlohmann::json;

    class JsonParser {
    public:
        static std::string safeString(const json &j, const std::string &key, const std::string &def) {
            if (j.contains(key) && !j[key].is_null()) {
                return j.value(key, def);
            }
            return def;
        }

        static int safeInt(const json &j, const std::string &key, int def) {
            if (j.contains(key) && !j[key].is_null() && j[key].is_number()) {
                return j[key].get<int>();
            }
            return def;
        }

        static long safeLong(const json &j, const std::string &key, long def) {
            if (j.contains(key) && !j[key].is_null() && j[key].is_number()) {
                return j[key].get<long>();
            }
            return def;
        }

        static dto::ItemDto parseItem(const json &j) {
            if (j.is_null()) return {};
            dto::ItemDto item;
            item.id = safeString(j, "id", "");
            item.name = safeString(j, "name", "Unknown");
            item.type = safeString(j, "type", "MISC");
            item.description = safeString(j, "description", "");
            item.rarity = safeString(j, "rarity", "COMMON");
            item.quantity = safeInt(j, "quantity", 1);
            item.price = safeInt(j, "price", 0);
            return item;
        }

        static dto::MapDataResponse parseMap(const json &j) {
            dto::MapDataResponse m;
            if (j.is_null()) return m;

            m.mapName = safeString(j, "mapName", "Unknown Map");
            m.centerX = safeInt(j, "centerX", 0);
            m.centerY = safeInt(j, "centerY", 0);
            m.centerZ = safeInt(j, "centerZ", 0);
            m.rangeX = safeInt(j, "rangeX", 0);
            m.rangeY = safeInt(j, "rangeY", 0);
            if (j.contains("layers") && j["layers"].is_object()) {
                for (auto &[key, val]: j["layers"].items()) {
                    if (val.is_array()) {
                        m.layers[key] = val.get<std::vector<std::string> >();
                    }
                }
            }
            return m;
        }

        static dto::LoginOptionsResponse parseLoginOptions(const json &j) {
            dto::LoginOptionsResponse r;
            if (j.is_null()) return r;

            if (j.contains("classes") && !j["classes"].is_null()) {
                r.classes = j["classes"].get<std::vector<std::string> >();
            }
            if (j.contains("maps") && !j["maps"].is_null()) {
                for (const auto &m: j["maps"]) {
                    if (!m.is_null()) {
                        r.maps.push_back({safeString(m, "mapId", ""), safeString(m, "mapName", "")});
                    }
                }
            }
            return r;
        }

        static std::vector<dto::NpcDto> parseNpcs(const json &j) {
            std::vector<dto::NpcDto> npcs;
            if (j.is_array()) {
                for (const auto &npcJson: j) {
                    if (!npcJson.is_null()) {
                        npcs.push_back({
                            safeString(npcJson, "id", ""),
                            safeString(npcJson, "name", ""),
                            safeString(npcJson, "type", ""),
                            safeInt(npcJson, "x", 0),
                            safeInt(npcJson, "y", 0),
                            safeInt(npcJson, "z", 0),
                            safeInt(npcJson, "hp", 0),
                            safeInt(npcJson, "maxHp", 0),
                            npcJson.value("aggressive", false),
                            safeString(npcJson, "interaction", "TALK")
                        });
                    }
                }
            }
            return npcs;
        }

        static std::vector<dto::MapObjectDto> parseMapObjects(const json &j) {
            std::vector<dto::MapObjectDto> objects;
            if (j.is_array()) {
                for (const auto &objJson : j) {
                    if (!objJson.is_null()) {
                        dto::MapObjectDto obj;
                        obj.id = safeString(objJson, "id", "");
                        obj.type = safeString(objJson, "type", "");
                        obj.action = safeString(objJson, "action", "");
                        obj.description = safeString(objJson, "description", "");
                        obj.x = safeInt(objJson, "x", 0);
                        obj.y = safeInt(objJson, "y", 0);
                        obj.z = safeInt(objJson, "z", 0);
                        objects.push_back(obj);
                    }
                }
            }
            return objects;
        }

        static std::vector<dto::OtherPlayerDto> parseOtherPlayers(const json &j, const std::string& selfId) {
            std::vector<dto::OtherPlayerDto> players;
            if (j.is_array()) {
                for (const auto &pJson : j) {
                    if (!pJson.is_null()) {
                        std::string pid = safeString(pJson, "id", "");
                        if (pid != selfId) {
                            players.push_back({
                                pid,
                                safeString(pJson, "name", "Unknown"),
                                safeInt(pJson, "x", 0),
                                safeInt(pJson, "y", 0),
                                safeInt(pJson, "z", 0)
                            });
                        }
                    }
                }
            }
            return players;
        }
    };
}

#endif //JSONPARSER_H
