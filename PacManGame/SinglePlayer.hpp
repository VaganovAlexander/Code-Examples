#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameMenu.hpp"
#include "Maps.hpp"

inline bool flag_continuing = false;

inline bool end_of_the_game = false;

void CheckTouches();

void Running(GameMenu* game_menu, sf::Texture& texture,
             std::vector<sf::Text>& score_visual, GameState& status,
             sf::Sprite& player);

void Paused(GameMenu* game_menu, GameState& status, sf::Texture& texture,
            sf::RectangleShape& next_menu);

void Win(GameMenu* game_menu, GameState& status, sf::Texture& texture,
         sf::RectangleShape& next_menu);

void Lose(GameMenu* game_menu, GameState& status, sf::Texture& texture,
          sf::RectangleShape& next_menu);

void Game();