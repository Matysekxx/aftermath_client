#ifndef GAMEREQUESTS_H
#define GAMEREQUESTS_H

#include <string>
#include "GameEventTypes.h"

namespace dto {
    struct GameRequest {
        virtual ~GameRequest() = default;

        [[nodiscard]] virtual std::string getType() const = 0;
    };

    struct InitRequest : public GameRequest {
        [[nodiscard]] std::string getType() const override { return ACTION_INIT; }
    };

    struct LoginRequest : public GameRequest {
        std::string username;
        std::string playerClass;
        std::string startingMapId;

        [[nodiscard]] std::string getType() const override { return ACTION_LOGIN; }
    };

    struct MoveRequest : public GameRequest {
        std::string direction;

        [[nodiscard]] std::string getType() const override { return ACTION_MOVE; }
    };

    struct ChatRequest : public GameRequest {
        std::string message;

        [[nodiscard]] std::string getType() const override { return ACTION_CHAT; }
    };

    struct AttackRequest : public GameRequest {
        [[nodiscard]] std::string getType() const override { return ACTION_ATTACK; }
    };

    struct InteractRequest : public GameRequest {
        [[nodiscard]] std::string getType() const override { return ACTION_INTERACT; }
    };

    struct UseRequest : public GameRequest {
        int slotIndex;

        [[nodiscard]] std::string getType() const override { return ACTION_USE; }
    };

    struct EquipRequest : public GameRequest {
        int slotIndex;

        [[nodiscard]] std::string getType() const override { return ACTION_EQUIP; }
    };

    struct DropRequest : public GameRequest {
        int slotIndex;
        int amount;
        [[nodiscard]] std::string getType() const override { return ACTION_DROP; }
    };

    struct BuyRequest : public GameRequest {
        std::string npcId;
        int itemIndex;

        [[nodiscard]] std::string getType() const override { return ACTION_BUY; }
    };

    struct SellRequest : public GameRequest {
        std::string npcId;
        int slotIndex;

        [[nodiscard]] std::string getType() const override { return ACTION_SELL; }
    };

    struct TravelRequest : public GameRequest {
        std::string mapId;
        std::string lineId;

        [[nodiscard]] std::string getType() const override { return ACTION_TRAVEL; }
    };
}

#endif //GAMEREQUESTS_H
