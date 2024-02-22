//////////////////////////////////////////////////////
// MAIN FILE FOR PACMAN GAME                        //
// MADE BY ALEKSANDER VAGANOV, SIDLETSKIY PAVEL,    //
// FOR TP PROJECT IN MIPT                           //
//////////////////////////////////////////////////////
#include "SinglePlayer.hpp"

int main() {
  bool GameplayModeWasChosen = false;
  flag_continuing = false;
  std::vector<std::vector<std::string>> options = {
      MainMenuConstants::options, ChoosingGameModeWindow::options,
      OptionsWindow::options, AboutWindow::options, GameSettings::options};
  std::vector<std::vector<sf::Vector2f>> options_coords = {
      MainMenuConstants::options_coords, ChoosingGameModeWindow::options_coords,
      OptionsWindow::options_coords, AboutWindow::options_coords,
      GameSettings::options_coords};
  std::vector<std::vector<double>> options_sizes = {
      MainMenuConstants::options_sizes, ChoosingGameModeWindow::options_sizes,
      OptionsWindow::options_sizes, AboutWindow::options_sizes,
      GameSettings::options_sizes};
  while (true) {
    int next = 1;
    GameMenu* main_menu;
    while (!GameplayModeWasChosen) {
      main_menu = new GameMenu(options[next - 1], "../sprites/image.png",
                               options_coords[next - 1],
                               options_sizes[next - 1], 1, next);
      main_menu->run();
      if (!main_menu->window->isOpen()) {
        return 0;
      }
      next = main_menu->next_menu_id;
      main_menu->window->close();
      delete main_menu;
      GameplayModeWasChosen = (next == 5 || next == 6);
      main_menu = nullptr;
    }
    if (next == 5) {
      Game();
    }
    GameplayModeWasChosen = false;
  }
}