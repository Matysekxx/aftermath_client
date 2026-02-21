#ifndef GAMERESPONSES_H
#define GAMERESPONSES_H

#include <string>
#include <vector>
#include <map>

namespace dto {

    struct MapInfo {
        std::string mapId;
        std::string mapName;
    };

    struct NpcDto {
        std::string id;
        std::string name;
        std::string type;
        int x = 0;
        int y = 0;
        int z = 0;
        int hp = 0;
        int maxHp = 0;
        bool aggressive = false;
        std::string interaction;
    };

    struct StationDto {
        std::string id;
        std::string name;
    };

    struct ItemDto {
        std::string id;
        std::string name;
        std::string type;
        std::string description;
        std::string rarity;
        int quantity = 0;
        int price = 0;
    };

    struct InventoryDto {
        int capacity;
        double maxWeight;
        std::map<int, ItemDto> slots;
    };

    struct PlayerDto {
        std::string id;
        std::string name;
        int x = 0;
        int y = 0;
        int layerIndex = 0;
        int hp = 0;
        int maxHp = 100;
        int rads = 0;
        int radsLimit = 0;
        int credits = 0;
        int debt = 0;
        long globalDebt = 0;
        InventoryDto inventory;
        std::string equippedWeaponSlot;
        std::string equippedMaskSlot;
        std::string state;
    };

    struct OtherPlayerDto {
        std::string id;
        std::string name;
        int x = 0;
        int y = 0;
        int z = 0;
    };

    struct MapObjectDto {
        std::string id;
        std::string type;
        int x = 0;
        int y = 0;
        int z = 0;
        std::string action;
        std::string description;
        std::vector<ItemDto> items;
    };

    struct LoginOptionsResponse {
        std::vector<std::string> classes;
        std::vector<MapInfo> maps;
    };

    struct MapDataResponse {
        std::string mapName;
        int centerX = 0;
        int centerY = 0;
        int centerZ = 0;
        int rangeX = 0;
        int rangeY = 0;
        std::map<std::string, std::vector<std::string> > layers;
    };

    using NpcsUpdateResponse = std::vector<NpcDto>;

    struct MetroUiResponse {
        std::string lineId;
        std::vector<StationDto> stations;
    };

    struct TradeUiLoadResponse {
        std::string npcId;
        std::string npcName;
        std::vector<ItemDto> items;
        std::string welcomeMessage;
    };

    using MapObjectsUpdateResponse = std::vector<MapObjectDto>;

    struct ChatMessageResponse {
        std::string message;
    };

    struct NetworkLogDto {
        std::string timestamp;
        std::string direction;
        std::string type;
        std::string payload;
    };

    struct DialogResponse {
        std::string npcName;
        std::string text;
    };
}

#endif //GAMERESPONSES_H
