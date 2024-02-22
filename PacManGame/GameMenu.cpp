/////////////////////////////////////////////////////////
// GAME MENU FOR PACMAN                                //
// MADE BY ALEXANDER VAGANOV                           //
// USED TO NAVIGATE IN GAME SETIINGS AND PLAYING MODS  //
/////////////////////////////////////////////////////////
#include "GameMenu.hpp"

GameMenu::~GameMenu() {
  delete window;
  delete font;
  delete image;
  delete background;
}

void options_drawed_setting(sf::Text& option, sf::Font* font,
                            const std::string& options,
                            const sf::Vector2f& coord, double size) {
  option.setFont(*font);
  option.setOutlineColor(sf::Color::Blue);
  option.setPosition(coord);
  option.setString(options);
  option.setCharacterSize(size);
}

void GameMenu::set_values(std::vector<std::string> options_gettible,
                          std::string image_source,
                          std::vector<sf::Vector2f> coords_gettible,
                          std::vector<double> sizes_gettible,
                          size_t position_first, int window_id_) {
  window->create(sf::VideoMode(1920, 1080), "Pacman");
  window->setPosition(sf::Vector2i(0, 0));
  font->loadFromFile("../sprites/font.otf");
  image->loadFromFile(image_source);
  background->setTexture(*image);
  options = options_gettible;
  options_drawed.resize(options_gettible.size());
  coords = coords_gettible;
  sizes = sizes_gettible;
  pos = position_first;
  for (size_t i = 0; i < options_drawed.size(); ++i) {
    options_drawed_setting(options_drawed[i], font, options[i], coords[i],
                           sizes[i]);
  }
  if (window_id_ == 3) {
    if (GameSettings::hardmode_enabled) {
      options_drawed[2].setOutlineColor(sf::Color::Red);
    } else {
      options_drawed[2].setOutlineColor(sf::Color::Green);
    }
    options_drawed[2].setOutlineThickness(4);
  }
  options_drawed[pos].setOutlineThickness(4);
  window_id = window_id_;
}

void GameMenu::loop() {
  sf::Event event;
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window->close();
      break;
    }

    if (event.type == sf::Event::KeyPressed) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (pos > options_drawed.size() - 2 || window_id == 3 ||
            window_id == 4) {
          continue;
        }
        options_drawed[pos].setOutlineThickness(0);
        options_drawed[++pos].setOutlineThickness(4);
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (pos < 2 || window_id == 4) {
          continue;
        }
        options_drawed[pos].setOutlineThickness(0);
        options_drawed[--pos].setOutlineThickness(4);
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        last_pressed_ = pos;
        CommandsHandler(last_pressed_);
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        if (window_id == 1) {
          window->close();
        } else {
          next_menu_id = 1;
        }
        break;
      }
    }
  }
}

void GameMenu::run() {
  while (window->isOpen()) {
    window->clear();
    window->draw(*background);
    for (auto t : options_drawed) {
      window->draw(t);
    }
    window->display();
    loop();
    if (bool(next_menu_id) && last_pressed_) {
      break;
    }
  }
}

void GameMenu::MainMenuCommands(int command_id) {
  if (command_id < 1 || command_id > 4) {
    throw std::bad_alloc();
  }
  switch (command_id) {
    case 1:
      std::cout << "PLAY"
                << "\n";
      next_menu_id = 2;
      break;
    case 2:
      std::cout << "OPTIONS"
                << "\n";
      next_menu_id = 3;
      break;
    case 3:
      std::cout << "ABOUT"
                << "\n";
      next_menu_id = 4;
      break;
    case 4:
      std::cout << "QUIT"
                << "\n";
      window->close();
      break;
  }
}

void GameMenu::ChoosingGameModeCommands(int command_id) {
  if (command_id < 1 || command_id > 2) {
    throw std::bad_alloc();
  }
  switch (command_id) {
    case 1:
      std::cout << "SINGLEPLAYER"
                << "\n";
      next_menu_id = 5;
      break;
    case 2:
      std::cout << "MULTYPLAYER"
                << "\n";
      next_menu_id = 6;
      break;
  }
}

void GameMenu::OptionsCommands(int command_id) {
  if (command_id < 1 || command_id > 2) {
    throw std::bad_alloc();
  }
  switch (command_id) {
    case 1:
      if (!GameSettings::hardmode_enabled) {
        options_drawed[2].setOutlineColor(sf::Color::Red);
      } else {
        options_drawed[2].setOutlineColor(sf::Color::Green);
      }
      GameSettings::hardmode_enabled = !GameSettings::hardmode_enabled;
      break;
  }
}

void GameMenu::CommandsHandler(int command_id) {
  try {
    if (window_id < 0) {
      throw std::bad_alloc();
    }
    switch (window_id) {
      case 1:
        MainMenuCommands(command_id);
        break;
      case 2:
        ChoosingGameModeCommands(command_id);
        break;
      case 3:
        OptionsCommands(command_id);
        break;
      case 4:
        break;
    }
  } catch (...) {
    std::cout << "An error has occured!"
              << "\n";
  }
}
