#include "GameState.h"
#include <chrono>
#include <iomanip>
#include <sstream>

void GameState::updatePlayer(const dto::PlayerDto &playerDto) {
    this->player = playerDto;
}

void GameState::updateMap(const dto::MapDataResponse &newMap) {
    this->map = newMap;
}

void GameState::updateNpcs(const dto::NpcsUpdateResponse &newNpcs) {
    this->npcs = newNpcs;
}

void GameState::updateObjects(const dto::MapObjectsUpdateResponse &newObjects) {
    this->objects = newObjects;
}

void GameState::updateOtherPlayers(const std::vector<dto::OtherPlayerDto> &players) {
    this->otherPlayers = players;
}

void GameState::addChatMessage(const dto::ChatMessageResponse &msg) {
    chatHistory.push_back(msg);
    if (chatHistory.size() > 10) {
        chatHistory.erase(chatHistory.begin());
    }
}

void GameState::setMetroUi(const dto::MetroUiResponse &uiData) {
    this->metroUi = uiData;
    this->metroSelectionIndex = 0;
    this->isMetroUiOpen = true;
}

void GameState::setTradeUi(const dto::TradeUiLoadResponse &uiData) {
    this->tradeUi = uiData;
    this->tradeSelectionIndex = 0;
    this->isTradeUiOpen = true;
    this->tradeMode = TradeMode::BUY;
}

void GameState::toggleInventory() {
    this->isInventoryOpen = !this->isInventoryOpen;
}

void GameState::scrollInventory(const int delta) {
    if (player.inventory.slots.empty()) return;
    selectedInventoryIndex += delta;
    if (selectedInventoryIndex < 0) selectedInventoryIndex = (int)player.inventory.slots.size() - 1;
    if (selectedInventoryIndex >= player.inventory.slots.size()) selectedInventoryIndex = 0;
}

int GameState::getSelectedInventorySlot() const {
    if (player.inventory.slots.empty()) return -1;
    if (selectedInventoryIndex >= player.inventory.slots.size()) return -1;
    auto it = player.inventory.slots.begin();
    std::advance(it, selectedInventoryIndex);
    return it->first;
}

void GameState::scrollMetro(int delta) {
    if (metroUi.stations.empty()) return;
    metroSelectionIndex = (metroSelectionIndex + delta);
    if (metroSelectionIndex < 0) metroSelectionIndex = (int)metroUi.stations.size() - 1;
    if (metroSelectionIndex >= metroUi.stations.size()) metroSelectionIndex = 0;
}

dto::StationDto GameState::getSelectedMetroStation() {
    if (metroUi.stations.empty()) return {};
    if (metroSelectionIndex >= 0 && metroSelectionIndex < metroUi.stations.size()) {
        return metroUi.stations[metroSelectionIndex];
    }
    return {};
}

void GameState::closeMetroUi() {
    isMetroUiOpen = false;
}

void GameState::scrollTrade(int delta) {
    if (tradeMode == TradeMode::BUY) {
        if (tradeUi.items.empty()) return;
        tradeSelectionIndex = (tradeSelectionIndex + delta);
        if (tradeSelectionIndex < 0) tradeSelectionIndex = (int)tradeUi.items.size() - 1;
        if (tradeSelectionIndex >= tradeUi.items.size()) tradeSelectionIndex = 0;
    } else {
        scrollInventory(delta);
    }
}

dto::ItemDto GameState::getSelectedTradeItem() {
    if (tradeUi.items.empty()) return {};
    if (tradeSelectionIndex >= 0 && tradeSelectionIndex < tradeUi.items.size()) {
        return tradeUi.items[tradeSelectionIndex];
    }
    return {};
}

void GameState::closeTradeUi() {
    isTradeUiOpen = false;
}

void GameState::toggleTradeMode() {
    if (tradeMode == TradeMode::BUY) tradeMode = TradeMode::SELL;
    else tradeMode = TradeMode::BUY;
}

void GameState::toggleLogs() {
    showLogs = !showLogs;
}

void GameState::toggleHelp() {
    showHelp = !showHelp;
}

void GameState::toggleMenu() {
    isMenuOpen = !isMenuOpen;
    menuSelectionIndex = 0;
    if (isMenuOpen) {
        isInventoryOpen = false;
        isMetroUiOpen = false;
        isTradeUiOpen = false;
        isPayDebtOpen = false;
        isDialogOpen = false;
    }
}

void GameState::scrollMenu(int delta) {
    menuSelectionIndex = (menuSelectionIndex + delta + 3) % 3;
}

void GameState::togglePayDebt() {
    isPayDebtOpen = !isPayDebtOpen;
    debtInput.clear();
    if (isPayDebtOpen) {
        isInventoryOpen = false;
        isMetroUiOpen = false;
        isTradeUiOpen = false;
        isMenuOpen = false;
        isDialogOpen = false;
    }
}

void GameState::showAnnouncement(const std::string& msg) {
    announcementMessage = msg;
    isAnnouncementOpen = true;
    isInventoryOpen = false;
    isMetroUiOpen = false;
    isTradeUiOpen = false;
    isMenuOpen = false;
    isPayDebtOpen = false;
    isDialogOpen = false;
}

void GameState::closeAnnouncement() {
    isAnnouncementOpen = false;
    announcementMessage.clear();
}

void GameState::openDialog(const dto::DialogResponse& dialog) {
    currentDialog = dialog;
    isDialogOpen = true;
    isInventoryOpen = false;
    isMetroUiOpen = false;
    isTradeUiOpen = false;
    isMenuOpen = false;
    isPayDebtOpen = false;
    isAnnouncementOpen = false;
}

void GameState::closeDialog() {
    isDialogOpen = false;
    currentDialog = {};
}

void GameState::addGameLog(const std::string &msg) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << msg;
    gameLogs.push_back(ss.str());
    if (gameLogs.size() > 8) {
        gameLogs.erase(gameLogs.begin());
    }
}

void GameState::addNetworkLog(const std::string &dir, const std::string &type, const std::string &payload) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    
    std::string p_short = payload;
    if (p_short.length() > 60) p_short = p_short.substr(0, 60) + "...";

    networkLogs.push_back({ss.str(), dir, type, p_short});
    if (networkLogs.size() > 8) {
        networkLogs.erase(networkLogs.begin());
    }
}

void GameState::setError(const std::string &err) {
    lastError = err;
}

void GameState::clearError() {
    lastError.clear();
}
