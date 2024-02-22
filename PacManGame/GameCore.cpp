/////////////////////////////////////////////////////////
// GAME LOGIC FOR PACMAN                               //
// MADE BY PAVEL SIDLETSKIY                            //
// USED TO GENERATE NEW FRAMES AND CHECK CURRENT STATE //
/////////////////////////////////////////////////////////
#include "GameCore.hpp"

size_t Map::P(size_t i, size_t j) { return i * jsize + j; }
size_t Map::C(size_t a, size_t n) {
  if (n == 0) {
    return a - jsize;
  }
  if (n == 1) {
    return a + 1;
  }
  if (n == 2) {
    return a + jsize;
  }
  if (n == 3) {
    return a - 1;
  }
  return 0;
}

bool Map::FollowState::operator==(const FollowState& b) const {
  return (pos == b.pos) && (InitialState == b.InitialState);
}

bool Map::FollowState::operator<(const FollowState& b) const {
  return pos < b.pos;
}

Direction Map::FollowIntention(size_t a, size_t b) {
  if (a == b) {
    return Direction::Up;
  }
  std::set<size_t> touched = {a};
  std::set<FollowState> hist, act;
  if (matrix[a][0]) {
    hist.insert({a - jsize, Direction::Up});
    touched.insert(a - jsize);
  }
  if (matrix[a][1]) {
    hist.insert({a + 1, Direction::Right});
    touched.insert(a + 1);
  }
  if (matrix[a][2]) {
    hist.insert({a + jsize, Direction::Down});
    touched.insert(a + jsize);
  }
  if (matrix[a][3]) {
    hist.insert({a - 1, Direction::Left});
    touched.insert(a - 1);
  }
  for (auto& i : hist) {
    if (i.pos == b) return i.InitialState;
  }
  while (true) {
    for (auto& i : hist) {
      for (size_t j = 0; j < 4; ++j) {
        size_t n = C(i.pos, j);
        if (n == b) {
          return i.InitialState;
        }
        if (matrix[i.pos][j] && (touched.count(n) == 0)) {
          touched.insert(n);
          act.insert({n, i.InitialState});
        }
      }
    }
    hist.swap(act);
    act.clear();
  }
}

void Map::GenMatrix() {
  matrix = std::vector<std::vector<bool>>(isize * jsize,
                                          std::vector<bool>(4, false));
  for (size_t i = 0; i < isize; ++i) {
    for (size_t j = 0; j < jsize; ++j) {
      if (tiles[i][j].state == TileState::wall) {
        continue;
      }
      size_t c = P(i, j);
      if (i > 0) {
        matrix[c][0] = tiles[i - 1][j].state != TileState::wall;
      }
      if (j < jsize - 1) {
        matrix[c][1] = tiles[i][j + 1].state != TileState::wall;
      }
      if (i < isize - 1) {
        matrix[c][2] = tiles[i + 1][j].state != TileState::wall;
      }
      if (j > 0) {
        matrix[c][3] = tiles[i][j - 1].state != TileState::wall;
      }
    }
  }
}

Map::Map() {}
Map::Map(const std::vector<std::vector<Tile>>& init) : tiles(init) {
  isize = tiles.size();
  jsize = tiles[0].size();
  for (size_t i = 0; i < isize; ++i) {
    for (size_t j = 0; j < jsize; ++j) {
      if (tiles[i][j].state != TileState::wall &&
          tiles[i][j].state != TileState::empty) {
        ++coins;
      }
    }
  }
  GenMatrix();
}

Map& Map::operator=(const std::vector<std::vector<Tile>>& init) {
  tiles = init;
  isize = tiles.size();
  jsize = tiles[0].size();
  GenMatrix();
  return *this;
}

void Coord::SetI(long long i) { diff_i(i - GetI()); }
void Coord::SetJ(long long j) { diff_j(j - GetJ()); }

size_t Coord::GetTile() const {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  return ((coordinate / (map.jsize * tile_size)) / (tile_size)) * map.jsize +
         (coordinate % (map.jsize * tile_size)) / tile_size;
}
Coord Coord::GetCoord(size_t tile) {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  size_t i = tile / map.jsize;
  size_t j = tile % map.jsize;
  Coord base;
  base.diff_i(tile_size / 2 + i * tile_size);
  base.diff_j(tile_size / 2 + j * tile_size);
  return base;
}
std::pair<size_t, size_t> Coord::GetDisplayCoord() {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  // returns (x, y), not (i, j)
  return {coordinate % (map.jsize * tile_size),
          coordinate / (map.jsize * tile_size)};
}
void Coord::diff_i(long long diff) {
  coordinate += diff * GameInfo::map.jsize * GameCoreConstants::tile_size;
}
void Coord::diff_j(long long diff) { coordinate += diff; }
size_t Coord::GetI() const {
  return coordinate / (GameInfo::map.jsize * GameCoreConstants::tile_size);
}
size_t Coord::GetJ() const {
  return coordinate % (GameInfo::map.jsize * GameCoreConstants::tile_size);
}

void Creature::GenNextStep(const Direction& new_dir) {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  switch (new_dir) {
    case (Direction::Up):
      if (map.matrix[coord.GetTile()][0]) dir = new_dir;
      break;
    case (Direction::Right):
      if (map.matrix[coord.GetTile()][1]) dir = new_dir;
      break;
    case (Direction::Down):
      if (map.matrix[coord.GetTile()][2]) dir = new_dir;
      break;
    case (Direction::Left):
      if (map.matrix[coord.GetTile()][3]) dir = new_dir;
      break;
    case (Direction::None):
      break;
  }
  size_t tile = coord.GetTile();
  Coord other_coord = Coord::GetCoord(tile);
  size_t i = other_coord.GetI();
  size_t j = other_coord.GetJ();
  size_t i_cur = coord.GetI();
  size_t j_cur = coord.GetJ();
  long long to_go = def_speed;
  if (i != i_cur && (dir == Direction::Right || dir == Direction::Left)) {
    size_t dist = i + i_cur - std::min(i, i_cur) * 2;
    if (to_go < dist) {
      if (i_cur < i) {
        coord.diff_i(to_go);
        return;
      }
      coord.diff_i(-to_go);
      return;
    }
    coord.SetI(i);
    to_go -= dist;
  }
  if (j != j_cur && (dir == Direction::Up || dir == Direction::Down)) {
    size_t dist = j + j_cur - std::min(j, j_cur) * 2;
    if (to_go < dist) {
      if (j_cur < j) {
        coord.diff_j(to_go);
        return;
      }
      coord.diff_j(-to_go);
      return;
    }
    coord.SetJ(j);
    to_go -= dist;
  }
  switch (dir) {
    case (Direction::Up):
      if (i_cur > i || map.matrix[tile][0]) {
        coord.diff_i(-to_go);
      }
      break;
    case (Direction::Right):
      if (j_cur < j || map.matrix[tile][1]) {
        coord.diff_j(to_go);
      }
      break;
    case (Direction::Down):
      if (i_cur < i || map.matrix[tile][2]) {
        coord.diff_i(to_go);
      }
      break;
    case (Direction::Left):
      if (j_cur > j || map.matrix[tile][3]) {
        coord.diff_j(-to_go);
      }
      break;
    case (Direction::None):
      break;
  }
}

void Ghost::ThinkAboutLife(size_t i) {
  using namespace GameInfo;
  if (mode == GameMode::Hard) {
    ghosts_int[i] =
        map.FollowIntention(coord.GetTile(), players[0].coord.GetTile());
  }
}

void Player::CheckCollisions() {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  long long cur_i = coord.GetI();
  long long cur_j = coord.GetJ();
  for (size_t i = 0; i < ghosts.size(); ++i) {
    long long a = ghosts[i].coord.GetI();
    long long b = ghosts[i].coord.GetJ();
    if ((a - cur_i) * (a - cur_i) + (b - cur_j) * (b - cur_j) <= collision) {
      GameInfo::game_state = GameState::Caught;
      D("CAUGHT");
      if (lifes == 0) {
        GameInfo::game_state = GameState::Lose;
        return;
      }
      --lifes;
      D(static_cast<int>(GameInfo::game_state));
      return;
    }
  }
  size_t tile_number = coord.GetTile();
  auto tile = Coord::GetCoord(tile_number);
  long long a = tile.GetI();
  long long b = tile.GetJ();
  size_t tile_i = tile_number / map.jsize;
  size_t tile_j = tile_number % map.jsize;
  if (map.tiles[tile_i][tile_j].state != TileState::empty &&
      (a - cur_i) * (a - cur_i) + (b - cur_j) * (b - cur_j) <= collision1) {
    switch (map.tiles[tile_i][tile_j].state) {
      case (TileState::dot):
        score += 10;
        --map.coins;
        break;
      case (TileState::cherry):
        score += 100;
        --map.coins;
        break;
      case (TileState::ulta):
        score += 50;
        --map.coins;
        break;
      case (TileState::wall):
        score -= 100000;
        break;
      case (TileState::empty):
        score += 1;
        break;
    }
    map.tiles[tile_i][tile_j] = {TileState::empty};
    if (map.coins == 0) {
      GameInfo::game_state = GameState::Win;
      return;
    }
  }
}

// not needed?
size_t FindFreePosition() {
  return 0;
  using namespace GameInfo;
  while (true) {
    size_t ans = rand();
    for (auto& i : players) {
      continue;
    }
  }
}

void ResetPos() {
  using namespace GameInfo;
  for (size_t i = 0; i < ghosts.size(); ++i) {
    ghosts[i].coord = def_ghosts[i];
    ghosts[i].dir = Direction::Up;
    ghosts[i].ticks_to_follow = 0;
  }
  for (size_t i = 0; i < players.size(); ++i) {
    players[i].coord = def_players[i];
    players_int[i] = players[i].dir = Direction::None;
    players[i].state = PlayerState::usual;
    players[i].ticks_for_state = 0;
  }
}

void InitGame(const GameInitInfo& init) {
  using namespace GameInfo;
  using std::vector;
  map = (init.init_map);
  def_players.resize(init.init_players.size());
  for (size_t i = 0; i < def_players.size(); ++i) {
    def_players[i] = Coord::GetCoord(init.init_players[i]);
  }
  def_ghosts.resize(init.init_ghosts.size());
  for (size_t i = 0; i < def_ghosts.size(); ++i) {
    def_ghosts[i] = Coord::GetCoord(init.init_ghosts[i]);
  }
  players = std::vector<Player>(init.init_players.size());
  ghosts = std::vector<Ghost>(init.init_ghosts.size());
  players_int = std::vector<Direction>(players.size(), Direction::None);
  ghosts_int = std::vector<Direction>(ghosts.size(), Direction::Up);
  game_state = GameState::Running;
  mode = GameMode::Hard;
  ResetPos();
}

void GenNextFrame() {
  using namespace GameInfo;
  using namespace GameCoreConstants;
  for (size_t i = 0; i < players.size(); ++i) {
    players[i].GenNextStep(players_int[i]);
  }
  for (size_t i = 0; i < ghosts.size(); ++i) {
    ghosts[i].ThinkAboutLife(i);
    ghosts[i].GenNextStep(ghosts_int[i]);
  }
  for (size_t i = 0; i < players.size(); ++i) {
    players[i].CheckCollisions();
    if (game_state == GameState::Caught) {
      ResetPos();
      game_state = GameState::Running;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
