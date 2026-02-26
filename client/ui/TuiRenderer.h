#ifndef TUIRENDERER_H
#define TUIRENDERER_H

#include "../game/GameState.h"
#include <ftxui/dom/elements.hpp>

/**
 * @brief Responsible for rendering the game state to the terminal using FTXUI.
 *
 * This class takes the current GameState and constructs a visual representation
 * of the game world, UI panels (inventory, trade, etc.), and logs.
 */
class TuiRenderer {
public:
    TuiRenderer();

    ~TuiRenderer();

    /**
     * @brief Renders the current game state to the terminal.
     * @param state The current state of the game to render.
     */
    void render(const GameState &state);

private:
    std::string last_reset_position;

    ftxui::Element buildLoginScreen(const GameState &state);
    ftxui::Element buildMap(const GameState &state);
    ftxui::Element buildStats(const dto::PlayerDto &player);
    ftxui::Element buildInventory(const GameState &state);
    ftxui::Element buildMetroUi(const GameState &state);
    ftxui::Element buildTradeUi(const GameState &state);
    ftxui::Element buildHelp();
    ftxui::Element buildMenu(const GameState &state);
    ftxui::Element buildPayDebtUi(const GameState &state);
    ftxui::Element buildAnnouncement(const GameState &state);
    ftxui::Element buildDialog(const GameState &state);
};

#endif //TUIRENDERER_H
