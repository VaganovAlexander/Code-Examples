/////////////////////////////////////////////////////////
// GAME LOGIC FOR PACMAN                               //
// MADE BY PAVEL SIDLETSKIY                            //
// USED TO GENERATE NEW FRAMES AND CHECK CURRENT STATE //
/////////////////////////////////////////////////////////
#pragma once
#include <chrono>
#include <iostream>
#include <set>
#include <thread>
#include <unordered_set>
#include <vector>

template <typename T>
void D(T n) {
  std::cout << "DEB " << n << std::endl;
}

enum class PlayerState { usual, cherry };
enum class GameMode { Normal, Hard };
enum class GameState { Running, Paused, Win, Lose, Caught, End };
enum class Colours { Red, Blue, White, Yellow };
enum class Direction { None, Up, Right, Down, Left };
enum class TileState { wall, empty, dot, cherry, ulta };

namespace GameCoreConstants {
const size_t tile_size = 33;  // need central pixel? Yes
const size_t def_speed = 2;
const size_t collision = 200;
const size_t collision1 = 1000;
};  // namespace GameCoreConstants

struct Tile {
  TileState state;
};

struct Map {
  size_t isize = 0;
  size_t jsize = 0;
  size_t coins = 0;
  std::vector<std::vector<Tile>> tiles;
  std::vector<std::vector<bool>> matrix;

  size_t P(size_t i, size_t j);
  size_t C(size_t a, size_t n);

  struct FollowState {
    size_t pos;
    Direction InitialState = Direction::None;
    bool operator==(const FollowState& b) const;
    bool operator<(const FollowState& b) const;
  };

  Direction FollowIntention(size_t a, size_t b);

  void GenMatrix();

  Map();
  Map(const std::vector<std::vector<Tile>>& init);

  Map& operator=(const std::vector<std::vector<Tile>>& init);
};

struct Coord {
  size_t coordinate = 0;

  size_t GetTile() const;
  static Coord GetCoord(size_t tile);
  std::pair<size_t, size_t> GetDisplayCoord();

  void diff_i(long long diff);
  void diff_j(long long diff);

  size_t GetI() const;
  size_t GetJ() const;

  void SetI(long long i);
  void SetJ(long long j);
};

class Creature {
 public:
  Coord coord;
  Direction dir = Direction::None;
  size_t speed = GameCoreConstants::def_speed;

  void GenNextStep(const Direction& new_dir);
};

class Ghost : public Creature {
 public:
  using Creature::coord;
  size_t ticks_to_follow = 0;
  Colours colour = Colours::Red;

  void ThinkAboutLife(size_t i);
};

class Player : public Creature {
 public:
  size_t coins = 0;
  size_t score = 0;
  size_t lifes = 2;  // не обязательно
  PlayerState state = PlayerState::usual;
  size_t ticks_for_state = 0;

  void CheckCollisions();
};

namespace GameInfo {
inline Map map;
inline std::vector<Player> players;
inline std::vector<Ghost> ghosts;
inline std::vector<Coord> def_players;
inline std::vector<Coord> def_ghosts;
inline std::vector<Direction> players_int;
inline std::vector<Direction> ghosts_int;
inline GameMode mode;
inline GameState game_state;
};  // namespace GameInfo

struct GameInitInfo {
  std::vector<std::vector<Tile>> init_map;
  std::vector<size_t> init_players;
  std::vector<size_t> init_ghosts;
};

// not needed?
size_t FindFreePosition();
void ResetPos();
void InitGame(const GameInitInfo& init);
void GenNextFrame();
