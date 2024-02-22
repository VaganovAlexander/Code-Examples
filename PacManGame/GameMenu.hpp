/////////////////////////////////////////////////////////
// GAME MENU FOR PACMAN                                //
// MADE BY ALEXANDER VAGANOV                           //
// USED TO NAVIGATE IN GAME SETIINGS AND PLAYING MODS  //
/////////////////////////////////////////////////////////
#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <string>

#include "Constants.h"

namespace WindowSettings {
inline float wigth = 1920;
inline float height = 1080;
inline float tile_wight = 33;
inline float tile_height = 33;
}  // namespace WindowSettings

class GameMenu {
 public:
  GameMenu(std::vector<std::string> options_gettible, std::string image_source,
           std::vector<sf::Vector2f> coords_gettible,
           std::vector<double> sizes_gettible, size_t position_first,
           int window_id) {
    window = new sf::RenderWindow();
    font = new sf::Font();
    image = new sf::Texture();
    background = new sf::Sprite();
    set_values(options_gettible, image_source, coords_gettible, sizes_gettible,
               position_first, window_id);
    // window->setFramerateLimit(120);
  }
  GameMenu() {
    window = new sf::RenderWindow();
    window_id = 5;
  }
  ~GameMenu();
  void run();
  size_t pos;
  std::vector<sf::Text> options_drawed;
  std::vector<std::string> options;
  sf::RenderWindow* window;
  std::vector<sf::Vector2f> coords;
  std::vector<double> sizes;
  sf::Font* font;
  sf::Texture* image;
  sf::Sprite* background;
  int next_menu_id = 0;
  int window_id;

 private:
  void set_values(std::vector<std::string> options_gettible,
                  std::string image_source,
                  std::vector<sf::Vector2f> coords_gettible,
                  std::vector<double> sizes_gettible, size_t position_first,
                  int window_id);
  void loop();
  int last_pressed_;
  void CommandsHandler(int command_id);
  void MainMenuCommands(int command_id);
  void ChoosingGameModeCommands(int command_id);
  void OptionsCommands(int command_id);
};