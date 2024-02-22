#include "SinglePlayer.hpp"

void CheckTouches() {
  using namespace GameInfo;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    players_int[0] = Direction::Left;
    // D("Left");
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    players_int[0] = Direction::Up;
    // D("Up");
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    players_int[0] = Direction::Right;
    // D("Right");
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    players_int[0] = Direction::Down;
    // D("Down");
  }
}

namespace score_visualization {
inline const std::vector<sf::Vector2f> options_coords = {{100, 400},
                                                         {100, 500}};
inline const std::vector<double> options_sizes = {40, 40};
}  // namespace score_visualization

void Running(GameMenu* game_menu, sf::Texture& texture,
             std::vector<sf::Text>& score_visual, GameState& status,
             sf::Sprite& player) {
  using namespace WindowSettings;
  using namespace GameInfo;
  using namespace GameCoreConstants;
  GenNextFrame();
  game_menu->window->clear();
  double offset_x = static_cast<double>((wigth - tile_wight * map.jsize) / 2);
  double offset_y = static_cast<double>((height - tile_height * map.isize) / 2);
  for (size_t i = 0; i < map.isize; ++i) {
    for (size_t j = 0; j < map.jsize; ++j) {
      sf::Sprite square;
      // D(square.getSize().x);
      // D(square.getSize().y);
      square.setPosition(offset_x + tile_wight * j, offset_y + tile_height * i);
      if (map.tiles[i][j].state == TileState::wall) {
        texture.loadFromFile("../sprites/wall.png");
        square.setTexture(texture);
        // std::cout << "Texture was loaded" << std::endl;
      } else if (map.tiles[i][j].state == TileState::empty) {
        texture.loadFromFile("../sprites/empty.png");
        square.setTexture(texture);
      } else if (map.tiles[i][j].state == TileState::dot) {
        texture.loadFromFile("../sprites/dot.png");
        square.setTexture(texture);
      } else if (map.tiles[i][j].state == TileState::cherry) {
        texture.loadFromFile("../sprites/cherry.png");
        square.setTexture(texture);
      } else if (map.tiles[i][j].state == TileState::ulta) {
        texture.loadFromFile("../sprites/ulta.png");
        square.setTexture(texture);
      }
      game_menu->window->draw(square);
    }
  }
  std::pair<size_t, size_t> player_display_coords =
      GameInfo::players[0].coord.GetDisplayCoord();
  player.setPosition(offset_x - 15 + player_display_coords.first,
                     offset_y - 15 + player_display_coords.second);
  if (players[0].dir == Direction::None || players[0].dir == Direction::Right) {
    texture.loadFromFile("../sprites/player_right.png");
  } else if (players[0].dir == Direction::Up) {
    texture.loadFromFile("../sprites/player_up.png");
  } else if (players[0].dir == Direction::Down) {
    texture.loadFromFile("../sprites/player_down.png");
  } else if (players[0].dir == Direction::Left) {
    texture.loadFromFile("../sprites/player_left.png");
  }
  player.setTexture(texture);
  game_menu->window->draw(player);
  for (size_t i = 0; i < GameInfo::ghosts.size(); ++i) {
    std::pair<size_t, size_t> ghost_display_coords =
        GameInfo::ghosts[i].coord.GetDisplayCoord();
    sf::Sprite ghost;
    sf::Texture texture;
    ghost.setPosition(offset_x - 15 + ghost_display_coords.first,
                      offset_y - 15 + ghost_display_coords.second);
    if (ghosts[i].dir == Direction::None || ghosts[i].dir == Direction::Right) {
      texture.loadFromFile("../sprites/ghost_right.png");
    } else if (ghosts[i].dir == Direction::Up) {
      texture.loadFromFile("../sprites/ghost_up.png");
    } else if (ghosts[i].dir == Direction::Down) {
      texture.loadFromFile("../sprites/ghost_down.png");
    } else if (ghosts[i].dir == Direction::Left) {
      texture.loadFromFile("../sprites/ghost_left.png");
    }
    ghost.setTexture(texture);
    game_menu->window->draw(ghost);
  }
  CheckTouches();
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
    status = GameState::Paused;
    game_menu->window->close();
    return;
  }
  for (int i = 0; i < 2; ++i) {
    score_visual[i].setFont(*game_menu->font);
    score_visual[i].setPosition(score_visualization::options_coords[i]);
    if (i == 0) {
      score_visual[0].setString("Score:");
    } else {
      score_visual[1].setString(std::to_string(GameInfo::players[0].score));
    }
    score_visual[i].setCharacterSize(score_visualization::options_sizes[i]);
    game_menu->window->draw(score_visual[i]);
  }
  game_menu->window->display();
}

void Paused(GameMenu* game_menu, GameState& status, sf::Texture& texture,
            sf::RectangleShape& next_menu) {
  sf::Event event;
  next_menu.setSize(sf::Vector2f(1920, 1080));
  texture.loadFromFile("../sprites/paused.png");
  next_menu.setTexture(&texture);
  game_menu->window->draw(next_menu);
  game_menu->window->display();
  // next_menu.setTexture(texture);
  while (status == GameState::Paused) {
    if (event.type == sf::Event::KeyPressed) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        game_menu->next_menu_id = 1;
        flag_continuing = false;
        D(flag_continuing);
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        status = GameState::Running;
        break;
      }
    }
  }
}

void Win(GameMenu* game_menu, GameState& status, sf::Texture& texture,
         sf::RectangleShape& next_menu) {
  sf::Event event;
  next_menu.setSize(sf::Vector2f(1920, 1080));
  texture.loadFromFile("../sprites/win.png");
  // next_menu.setTexture(texture);
  while (status == GameState::Win) {
    if (event.type == sf::Event::KeyPressed) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        game_menu->next_menu_id = 1;
        flag_continuing = false;
        break;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        GameInfo::game_state = GameState::Running;
      }
    }
  }
}

void Lose(GameMenu* game_menu, GameState& status, sf::Texture& texture,
          sf::RectangleShape& next_menu) {
  sf::Event event;
  next_menu.setSize(sf::Vector2f(1920, 1080));
  texture.loadFromFile("../sprites/lose.png");
  next_menu.setTexture(&texture);
  // next_menu.setTexture(texture);
  while (status == GameState::Lose) {
    game_menu->window->clear();
    game_menu->window->draw(next_menu);
    game_menu->window->display();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      GameInfo::game_state = GameState::End;
      game_menu->next_menu_id = 1;
      flag_continuing = false;
      break;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
      GameInfo::game_state = GameState::Running;
    }
  }
}

void Game() {
  GameMenu* game_menu = new GameMenu(
      GameSettings::options, "../sprites/wall.png",
      GameSettings::options_coords, GameSettings::options_sizes, 1, 5);
  using namespace GameInfo;

  if (!flag_continuing || end_of_the_game) {
    D("Initialising game");
    InitGame(map1);
    flag_continuing = true;  // TODO diff between first and second variants
    end_of_the_game = false;
  }

  // GameState status = GameState::Running; инициализация должна быть
  std::cout << "Entering Game" << std::endl;
  using namespace WindowSettings;
  using namespace GameInfo;
  using namespace GameCoreConstants;
  GameState status = GameState::Running;
  D(static_cast<int>(status));
  D(game_menu->window_id);
  std::vector<sf::Text> score_visual;
  score_visual.resize(2);
  while (game_menu->window_id == 5) {
    sf::Event event;
    game_menu->window->clear();
    sf::RectangleShape next_menu;
    sf::Sprite player;
    sf::Texture texture;
    status = GameInfo::game_state;
    if (status == GameState::Running) {
      Running(game_menu, texture, score_visual, status, player);
    } else if (status == GameState::Paused) {
      Paused(game_menu, status, texture, next_menu);
    } else if (status == GameState::Win) {
      Win(game_menu, status, texture, next_menu);
    } else if (status == GameState::Lose) {
      Lose(game_menu, status, texture, next_menu);
    } else if (status == GameState::Caught) {
      GameInfo::game_state = GameState::Running;
    }
    if (event.type == sf::Event::KeyPressed || status == GameState::End) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        status = GameState::Paused;
      }
    }
  }
}