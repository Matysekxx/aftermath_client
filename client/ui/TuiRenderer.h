#ifndef TUIRENDERER_H
#define TUIRENDERER_H

#include "../game/GameState.h"
#include <ftxui/dom/elements.hpp>


class TuiRenderer {
public:
    TuiRenderer();

    ~TuiRenderer();

    void render(const GameState &state);

private:
    std::string last_reset_position;

    ftxui::Element buildLoginScreen(const GameState &state);

    ftxui::Element buildMap(const GameState &state);

    ftxui::Element buildStats(const dto::PlayerDto &player);

    ftxui::Element buildInventory(const GameState &state);

    ftxui::Element buildMetroUi(const GameState &state);

    ftxui::Element buildTradeUi(const GameState &state);

    ftxui::Element buildHelp(const GameState &state);
    ftxui::Element buildMenu(const GameState &state);
};

#endif //TUIRENDERER_H
