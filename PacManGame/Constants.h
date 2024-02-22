#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

namespace CommonConstants {
    inline const double font_size = 50;
    inline const double font_name_size = 80;
    inline const size_t position_first = 1;
}

namespace MainMenuConstants {
    inline const std::vector<std::string> options = {"Pacman!", "Play", "Options", "About", "Quit"};
    inline const std::vector<sf::Vector2f> options_coords = {{680, 40}, {200, 700}, {600, 700}, {1100, 700}, {1500, 700}};
    inline const std::vector<double> options_sizes = {80, 50, 50, 50, 50};
}

namespace ChoosingGameModeWindow {
    inline const std::vector<std::string> options = {"Pacman!", "SinglePlayer"};
    inline const std::vector<sf::Vector2f> options_coords = {{680, 40}, {200, 700}};
    inline const std::vector<double> options_sizes = {80, 50};
}

namespace OptionsWindow {
    inline const std::vector<std::string> options = {"Pacman!", "HardMode", "Enabled? (Red if 'Yes')"};
    inline const std::vector<sf::Vector2f> options_coords = {{680, 40}, {200, 700}, {850, 700}};
    inline const std::vector<double> options_sizes = {80, 50, 50};
}

namespace AboutWindow {
    inline const std::vector<std::string> options = {"Pacman!", "THIS GAME WAS MADE BY", "ALEXANDER VAGANOV, PAVEL SIDLETSKIY ", "FOR TP COURSE IN MIPT"};
    inline const std::vector<sf::Vector2f> options_coords = {{680, 40}, {550, 200}, {100, 400}, {100, 600}};
    inline const std::vector<double> options_sizes = {80, 50, 50, 50, 50};
}

namespace GameSettings {
    inline bool hardmode_enabled = false;
    inline const std::vector<std::string> options = {"Pacman!"};
    inline const std::vector<sf::Vector2f> options_coords = {{680, 40}};
    inline const std::vector<double> options_sizes = {80};
}