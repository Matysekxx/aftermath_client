#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <mutex>
#include "../dto/GameResponses.h"
#include <nlohmann/json.hpp>

enum class ClientState {
    WAITING_FOR_INIT,
    LOGIN_SCREEN,
    PLAYING
};

enum class TradeMode {
    BUY,
    SELL
};

class GameState {
public:
    mutable std::mutex stateMutex;

    ClientState clientState = ClientState::WAITING_FOR_INIT;
    dto::LoginOptionsResponse loginOptions;
    std::string inputUsername;
    int selectedClassIndex = 0;
    int selectedMapIndex = 0;
    int loginStep = 0;

    dto::PlayerDto player;
    std::vector<dto::OtherPlayerDto> otherPlayers;

    dto::MapDataResponse map;
    dto::NpcsUpdateResponse npcs;
    dto::MapObjectsUpdateResponse objects;

    std::string connectionStatus = "Connecting to server...";
    std::vector<std::string> gameLogs;
    std::vector<dto::NetworkLogDto> networkLogs;
    std::string lastError;
    std::vector<dto::ChatMessageResponse> chatHistory;
    dto::MetroUiResponse metroUi;
    dto::TradeUiLoadResponse tradeUi;
    int metroSelectionIndex = 0;
    int tradeSelectionIndex = 0;
    bool isMetroUiOpen = false;
    bool isTradeUiOpen = false;
    TradeMode tradeMode = TradeMode::BUY;
    bool isInventoryOpen = false;
    int selectedInventoryIndex = 0;
    bool showLogs = false;
    bool showHelp = false;
    bool isMenuOpen = false;
    int menuSelectionIndex = 0;
    bool exitRequested = false;

    bool isPayDebtOpen = false;
    std::string debtInput;

    void updatePlayer(const dto::PlayerDto &playerDto);

    void updateMap(const dto::MapDataResponse &newMap);

    void updateNpcs(const dto::NpcsUpdateResponse &newNpcs);

    void updateObjects(const dto::MapObjectsUpdateResponse &newObjects);

    void updateOtherPlayers(const std::vector<dto::OtherPlayerDto> &players);

    void addChatMessage(const dto::ChatMessageResponse &msg);

    void setMetroUi(const dto::MetroUiResponse &uiData);

    void setTradeUi(const dto::TradeUiLoadResponse &uiData);

    void toggleInventory();
    void scrollInventory(int delta);
    int getSelectedInventorySlot() const;
    void scrollMetro(int delta);
    dto::StationDto getSelectedMetroStation();
    void closeMetroUi();
    void scrollTrade(int delta);
    dto::ItemDto getSelectedTradeItem();
    void closeTradeUi();
    void toggleTradeMode();
    void toggleLogs();
    void toggleHelp();
    void toggleMenu();
    void scrollMenu(int delta);
    void togglePayDebt();

    void addGameLog(const std::string& msg);
    void addNetworkLog(const std::string& dir, const std::string& type, const std::string& payload);
    void setError(const std::string& err);
    void clearError();
};

#endif //GAMESTATE_H
