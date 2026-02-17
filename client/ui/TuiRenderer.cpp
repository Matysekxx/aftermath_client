#include "TuiRenderer.h"
#include <iostream>
#include <string>
#include <algorithm>

using namespace ftxui;

std::vector<std::string> split_utf8(const std::string& str) {
    std::vector<std::string> chars;
    for (size_t i = 0; i < str.length();) {
        int cplen = 1;
        const auto c = static_cast<unsigned char>(str[i]);
        if ((c & 0xF8) == 0xF0) cplen = 4;
        else if ((c & 0xF0) == 0xE0) cplen = 3;
        else if ((c & 0xE0) == 0xC0) cplen = 2;

        if (i + cplen > str.length()) cplen = 1;
        chars.push_back(str.substr(i, cplen));
        i += cplen;
    }
    return chars;
}

Color getRarityColor(const std::string& rarity) {
    if (rarity == "LEGENDARY") return Color::Orange1;
    if (rarity == "EPIC") return Color::Magenta1;
    if (rarity == "RARE") return Color::BlueLight;
    if (rarity == "UNCOMMON") return Color::GreenLight;
    return Color::GrayLight;
}

TuiRenderer::TuiRenderer() {
}

TuiRenderer::~TuiRenderer() {
}

void TuiRenderer::render(const GameState &state) {
    std::lock_guard lock(state.stateMutex);

    Element content;

    if (state.clientState == ClientState::LOGIN_SCREEN || state.clientState == ClientState::WAITING_FOR_INIT) {
        content = buildLoginScreen(state) | flex;
    } else {
        Element stats_bar = buildStats(state.player);

        Element main_view = buildMap(state) | flex | borderStyled(ROUNDED) | color(Color::Cyan);

        if (state.isInventoryOpen) {
            main_view = dbox({main_view, buildInventory(state) | clear_under | center});
        }
        if (state.isMetroUiOpen) {
            main_view = dbox({main_view, buildMetroUi(state) | clear_under | center});
        }
        if (state.isTradeUiOpen) {
            main_view = dbox({main_view, buildTradeUi(state) | clear_under | center});
        }
        if (state.isMenuOpen) {
            main_view = dbox({main_view, buildMenu(state) | clear_under | center});
        }
        if (state.isPayDebtOpen) {
            main_view = dbox({main_view, buildPayDebtUi(state) | clear_under | center});
        }
        if (state.showHelp) {
            main_view = dbox({main_view, buildHelp(state) | clear_under | align_right});
        }

        Element logs_area = text("");
        if (state.showLogs) {
            Elements log_elements;
            for (const auto& log : state.gameLogs) {
                log_elements.push_back(text(log));
            }
            log_elements.push_back(separator());
            for (const auto& log : state.networkLogs) {
                auto c = (log.direction == "OUT") ? Color::Blue : Color::Green;
                auto arrow = (log.direction == "OUT") ? "->" : "<-";
                log_elements.push_back(hbox({
                    text(arrow) | color(c),
                    text(" " + log.type + " "),
                    text(log.payload) | dim
                }));
            }

            auto log_title = text(" SYSTEM LOGS ");
            if (!state.lastError.empty()) {
                log_title = text(" WARNING: " + state.lastError + " ") | color(Color::Red) | bold;
            }

            logs_area = window(log_title, vbox(log_elements)) | size(HEIGHT, EQUAL, 12) | color(Color::Green);
        }

        content = vbox({
            stats_bar,
            main_view,
            logs_area
        });
    }

    const Element document = content
        | size(WIDTH, EQUAL, 160)
        | size(HEIGHT, EQUAL, 50)
        | borderStyled(ROUNDED)
        | color(Color::Cyan)
        | center;

    auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
    if (screen.dimx() <= 1 || screen.dimy() <= 1) {
        screen = Screen::Create(Dimension::Fixed(160), Dimension::Fixed(50));
    }

    Render(screen, document);

    if (last_reset_position.empty()) {
        std::cout << "\033[2J\033[H";
    } else {
        std::cout << last_reset_position;
    }

    screen.Print();
    std::cout << std::flush;
    last_reset_position = screen.ResetPosition();
}

Element TuiRenderer::buildLoginScreen(const GameState &state) {
    auto logo = vbox({
        text(R"(    ___     ______ ______ ______ ____  __  ___  ___  ______ __  __  )") | color(Color::Cyan),
        text(R"(   /   |   / ____//_  __// ____// __ \/  |/  / /   |/_  __/ / / /   )") | color(Color::Cyan),
        text(R"(  / /| |  / /_     / /  / __/  / /_/ / /|_/ / / /| | / /   / /_/ /  )") | color(Color::Cyan),
        text(R"( / ___ | / __/    / /  / /___ / _, _/ /  / / / ___ |/ /   / __  /   )") | color(Color::BlueLight),
        text(R"(/_/  |_|/_/      /_/  /_____//_/ |_/_/  /_/ /_/  |_/_/   /_/ /_/    )") | color(Color::BlueLight),
    }) | bold | center;

    if (state.clientState == ClientState::WAITING_FOR_INIT) {
        return vbox({logo, text(state.connectionStatus) | center | blink}) | center;
    }

    Elements classes;
    for (int i = 0; i < state.loginOptions.classes.size(); ++i) {
        bool isSelected = (i == state.selectedClassIndex);
        bool isActiveStep = (state.loginStep == 1);
        auto t = text((isSelected ? " > " : "   ") + state.loginOptions.classes[i]);
        if (isSelected) t = t | inverted | color(Color::Yellow);
        if (!isActiveStep && !isSelected) t = t | dim;
        classes.push_back(t);
    }

    Elements maps;
    for (int i = 0; i < state.loginOptions.maps.size(); ++i) {
        const bool isSelected = (i == state.selectedMapIndex);
        const bool isActiveStep = (state.loginStep == 2);
        auto t = text((isSelected ? " > " : "   ") + state.loginOptions.maps[i].mapName);
        if (isSelected) t = t | inverted | color(Color::Yellow);
        if (!isActiveStep && !isSelected) t = t | dim;
        maps.push_back(t);
    }

    const auto status_element = (state.loginStep == 3)
                              ? text(" Authenticating... ") | bgcolor(Color::Green) | color(Color::Black) | blink
                              : text(" [ENTER] Confirm | [ARROWS] Select ") | dim;

    const auto content = vbox({
               logo | center,
               separator(),
               hbox(text(" [1] IDENTITY: "),
                    text(state.inputUsername.empty() ? "Enter Name..." : state.inputUsername)
                        | bold | color(state.loginStep == 0 ? Color::Yellow : Color::White) | (state.loginStep == 0 ? inverted : flex)),
               separator(),
               text(" [2] SPECIALIZATION:"),
               vbox(classes) | borderStyled(ROUNDED),
               text(" [3] DEPLOYMENT ZONE:"),
               vbox(maps) | borderStyled(ROUNDED),
               separator(),
               status_element | center
           });
    return content | size(WIDTH, EQUAL, 80) | borderStyled(ROUNDED) | color(Color::Cyan) | center;
}

Element TuiRenderer::buildMap(const GameState &state) {
    int rangeX = state.map.rangeX;
    int rangeY = state.map.rangeY;

    if (rangeX == 0 && rangeY == 0 && !state.map.layers.empty()) {
        for (const auto& [key, rows] : state.map.layers) {
            if (!rows.empty()) {
                rangeY = rows.size() / 2;
                if (!rows[0].empty()) {
                    rangeX = split_utf8(rows[0]).size() / 2;
                }
                break;
            }
        }
    }

    int height = 20;
    int width = 60;

    const int max_height = state.showLogs ? 30 : 40;
    if (rangeY > 0) height = std::min(rangeY * 2 + 1, max_height);
    if (rangeX > 0) width = std::min(rangeX * 2 + 1, 154);

    const int clientTopLeftX = state.map.centerX - (width / 2);
    const int clientTopLeftY = state.map.centerY - (height / 2);

    const int serverTopLeftX = state.map.centerX - rangeX;
    const int serverTopLeftY = state.map.centerY - rangeY;

    const int offsetX = clientTopLeftX - serverTopLeftX;
    const int offsetY = clientTopLeftY - serverTopLeftY;

    std::vector display_grid(height, std::vector<std::string>(width, " "));

    std::string currentLayerKey = std::to_string(state.map.centerZ);
    if (state.map.layers.count(currentLayerKey)) {
        const auto& rows = state.map.layers.at(currentLayerKey);

        for (int y = 0; y < height; ++y) {
            const int serverY = y + offsetY;
            if (serverY >= 0 && serverY < rows.size()) {
                std::vector<std::string> row_chars = split_utf8(rows[serverY]);
                for (int x = 0; x < width; ++x) {
                    const int serverX = x + offsetX;
                    if (serverX >= 0 && serverX < row_chars.size()) {
                        if (row_chars[serverX] != " ") {
                            display_grid[y][x] = row_chars[serverX];
                        }
                    }
                }
            }
        }
    }

    for (const auto &obj: state.objects) {
        const int relX = obj.x - clientTopLeftX;
        const int relY = obj.y - clientTopLeftY;
        if (relY >= 0 && relY < height && relX >= 0 && relX < width) {
            std::string sym = "?";
            if (obj.type == "CONTAINER") sym = "■";
            else if (obj.type == "EXIT") sym = ">";
            else if (obj.type == "BED") sym = "=";
            display_grid[relY][relX] = sym;
        }
    }

    for (const auto &npc: state.npcs) {
        const int relX = npc.x - clientTopLeftX;
        const int relY = npc.y - clientTopLeftY;
        if (relY >= 0 && relY < height && relX >= 0 && relX < width) {
            std::string symbol = "E";
            if (!npc.name.empty()) symbol = (npc.aggressive ? "M" : "N");
            display_grid[relY][relX] = symbol;
        }
    }

    for (const auto &other : state.otherPlayers) {
        if (other.z != state.player.layerIndex) continue;
        const int relX = other.x - clientTopLeftX;
        const int relY = other.y - clientTopLeftY;
        if (relY >= 0 && relY < height && relX >= 0 && relX < width) {
            display_grid[relY][relX] = "P";
        }
    }

    int pRelX = state.player.x - clientTopLeftX;
    int pRelY = state.player.y - clientTopLeftY;
    if (pRelY >= 0 && pRelY < height && pRelX >= 0 && pRelX < width) {
        display_grid[pRelY][pRelX] = "@";
    }

    Elements rows_elements;
    for (const auto &row_vec : display_grid) {
        Elements line_elems;
        for (const auto &cell : row_vec) {
            auto t = text(cell);
            if (cell == "@") t = t | color(Color::GreenLight) | bold;
            else if (cell == "P") t |= color(Color::BlueLight) | bold;
            else if (cell == "M") t |=  color(Color::Red) | bold;
            else if (cell == "N") t |= color(Color::Green) | bold;
            else if (cell == "■") t |= color(Color::Yellow);
            else if (cell == ">") t |= color(Color::Green);
            else if (cell == "=") t |= color(Color::Cyan);
            else if (cell == "#" || cell == "┌" || cell == "┐" || cell == "└" || cell == "┘" || cell == "─" || cell == "│") t |= color(Color::Cyan);
            else if (cell == ".") t |= color(Color::GrayDark);

            line_elems.push_back(t);
        }
        rows_elements.push_back(hbox(line_elems));
    }

    auto map_title = text(" SECTOR: " + state.map.mapName + " ") | hcenter | bold;

    return vbox({
        map_title | bgcolor(Color::Cyan) | color(Color::Black),
        vbox(rows_elements) | center | flex
    });
}

Element TuiRenderer::buildStats(const dto::PlayerDto &player) {
    const float hp_perc = (player.maxHp > 0) ? static_cast<float>(player.hp) / static_cast<float>(player.maxHp) : 0.0f;
    const float rad_perc = std::clamp(static_cast<float>(player.rads) / 100.0f, 0.0f, 1.0f);

    return hbox({
        text(" " + player.name + " ") | bold | color(Color::White) | bgcolor(Color::Blue),
        separator(),
        text(" HP ") | color(Color::Red) | bold,
        gauge(hp_perc) | color(Color::Red) | flex,
        text(" " + std::to_string(player.hp) + "/" + std::to_string(player.maxHp) + " ") | color(Color::Red),
        separator(),
        text(" RAD ") | color(Color::Yellow) | bold,
        gauge(rad_perc) | color(Color::Yellow) | flex,
        text(" " + std::to_string(player.rads) + "% ") | color(Color::Yellow),
        separator(),
        text(" " + std::to_string(player.credits) + " CR ") | color(Color::Black) | bgcolor(Color::Gold1) | bold,
        separator(),
        text(" DEBT: " + std::to_string(player.debt) + " CR ") | color(Color::White) | bgcolor(Color::Red) | bold,
        separator(),
        text(" GLOBAL DEBT: " + std::to_string(player.globalDebt) + " CR ") | color(Color::White) | bgcolor(Color::Magenta) | bold
    }) | borderStyled(ROUNDED) | color(Color::Cyan) | size(HEIGHT, EQUAL, 3);
}

Element TuiRenderer::buildInventory(const GameState &state) {
    const auto& inventory = state.player.inventory;
    Elements items;
    int idx = 0;
    for (const auto &[slot, item]: inventory.slots) {
        auto row = hbox({
            text(" " + std::to_string(slot) + " ") | color(Color::Cyan),
            text(item.name) | color(getRarityColor(item.rarity)),
            filler(),
            text("x" + std::to_string(item.quantity) + " ") | color(Color::Yellow)
        });

        if (idx == state.selectedInventoryIndex) {
            row = row | bgcolor(Color::Cyan) | color(Color::Black) | bold;
        }
        items.push_back(row);
        idx++;
    }

    if (items.empty()) {
        items.push_back(text("Empty") | center | dim);
    }

    return window(text(" CARGO HOLD ") | hcenter | bold, vbox(items))
        | size(WIDTH, EQUAL, 50) | size(HEIGHT, EQUAL, 15) | borderStyled(ROUNDED) | color(Color::Cyan);
}

Element TuiRenderer::buildMetroUi(const GameState &state) {
    Elements stations;
    int idx = 0;
    for (const auto &station: state.metroUi.stations) {
        auto t = text(" " + station.name + " ");
        if (idx == state.metroSelectionIndex) {
            t = t | inverted | color(Color::Yellow) | bold;
        }
        stations.push_back(t);
        idx++;
    }

    Color lineColor = Color::Green;
    if (state.metroUi.lineId == "line_b") lineColor = Color::Yellow;
    else if (state.metroUi.lineId == "line_c") lineColor = Color::Red;
    else if (state.metroUi.lineId == "line_d") lineColor = Color::Blue;
    else if (state.metroUi.lineId == "line_e") lineColor = Color::Magenta;
    else if (state.metroUi.lineId == "line_f") lineColor = Color::GrayLight;
    else if (state.metroUi.lineId == "line_g") lineColor = Color::Orange1;

    return window(text(" METRO: " + state.metroUi.lineId + " ") | hcenter | bold, vbox(stations))
        | size(WIDTH, EQUAL, 40) | borderStyled(ROUNDED) | color(lineColor);
}

Element TuiRenderer::buildTradeUi(const GameState &state) {
    const auto& tradeOffer = state.tradeUi;
    Elements items;

    if (state.tradeMode == TradeMode::BUY) {
        int idx = 0;
        for (const auto &item: tradeOffer.items) {
            auto row = hbox({
                text(" " + item.name + " ") | color(getRarityColor(item.rarity)),
                filler(),
                text(std::to_string(item.price) + " CR ") | color(Color::Gold1)
            });

            if (idx == state.tradeSelectionIndex) {
                row = row | bgcolor(Color::Green) | color(Color::Black) | bold;
            }
            items.push_back(row);
            idx++;
        }
    } else {
        int idx = 0;
        for (const auto &[slot, item]: state.player.inventory.slots) {
            auto row = hbox({
                text(" " + item.name + " ") | color(getRarityColor(item.rarity)),
                filler(),
                text("x" + std::to_string(item.quantity) + " ") | color(Color::Yellow)
            });

            if (idx == state.selectedInventoryIndex) {
                row = row | bgcolor(Color::Red) | color(Color::Black) | bold;
            }
            items.push_back(row);
            idx++;
        }
    }

    if (items.empty()) {
        items.push_back(text("No items") | center | dim);
    }

    auto title = (state.tradeMode == TradeMode::BUY)
        ? text(" TRADER: " + tradeOffer.npcName + " [BUYING] ") | color(Color::Green) | bold
        : text(" TRADER: " + tradeOffer.npcName + " [SELLING] ") | color(Color::Red) | bold;

    return window(hbox({title, filler(), text("[S] SWITCH MODE ")}), vbox(items))
        | size(WIDTH, EQUAL, 60) | size(HEIGHT, EQUAL, 20) | borderStyled(ROUNDED) | color(Color::White);
}

Element TuiRenderer::buildHelp(const GameState &state) {
    auto key_row = [](const std::string& key, const std::string& desc) {
        return hbox({
            text(" " + key + " ") | color(Color::Black) | bgcolor(Color::Cyan) | bold,
            text(" " + desc) | color(Color::White)
        });
    };

    return window(text(" MANUAL ") | hcenter | bold, vbox({
        key_row("ARROWS", "Move Character"),
        key_row("SPACE ", "Attack / Fire Weapon"),
        key_row("E     ", "Interact / Loot"),
        key_row("I     ", "Open Cargo Hold"),
        key_row("  U   ", "Use Item (in Inventory)"),
        key_row("  E   ", "Equip Item (in Inventory)"),
        key_row("  D   ", "Drop Item (in Inventory)"),
        key_row("P     ", "Pay Debt"),
        key_row("L     ", "Toggle Logs"),
        key_row("H     ", "Close Help"),
        key_row("ESC   ", "System Menu")
    })) | size(WIDTH, EQUAL, 50) | borderStyled(ROUNDED) | color(Color::Cyan);
}

Element TuiRenderer::buildMenu(const GameState &state) {
    auto option = [&](const std::string& label, const int index) {
        if (state.menuSelectionIndex == index) {
            return text(" > " + label + " < ") | bold | color(Color::Black) | bgcolor(Color::Green);
        }
        return text("   " + label + "   ") | color(Color::White);
    };

    return window(text(" PAUSED ") | hcenter | bold, vbox({
        option("RESUME", 0),
        option("MANUAL", 1),
        option("QUIT", 2)
    })) | size(WIDTH, EQUAL, 30) | borderStyled(ROUNDED) | color(Color::Green);
}

Element TuiRenderer::buildPayDebtUi(const GameState &state) {
    return window(text(" DEBT REPAYMENT ") | hcenter | bold, vbox({
        text("Current Debt: " + std::to_string(state.player.debt) + " CR") | color(Color::Red),
        separator(),
        text("Enter amount to pay:"),
        text(state.debtInput + "_") | bold | color(Color::Yellow) | inverted,
        separator(),
        text("[ENTER] Confirm | [ESC] Cancel") | dim
    })) | size(WIDTH, EQUAL, 40) | borderStyled(ROUNDED) | color(Color::Magenta);
}
