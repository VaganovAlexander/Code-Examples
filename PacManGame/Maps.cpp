#include "Maps.hpp"

GameInitInfo GettingMap() {
  using std::vector;
  vector<vector<Tile>> map;
  vector<size_t> players;
  vector<size_t> ghosts;

  std::ifstream inp("../Maps.txt");
  size_t isize, jsize, pl_n, gh_n;
  inp >> isize >> jsize;
  map.assign(isize, vector<Tile>(jsize));
  for (size_t i = 0; i < isize; ++i) {
    for (size_t j = 0; j < jsize; ++j) {
      int tmp;
      inp >> tmp;
      map[i][j] = {TileState(tmp)};
    }
  }

  inp >> pl_n;
  players.resize(pl_n);
  for (size_t i = 0; i < pl_n; ++i) {
    inp >> players[i];
  }

  inp >> gh_n;
  ghosts.resize(gh_n);
  for (size_t i = 0; i < gh_n; ++i) {
    inp >> ghosts[i];
  }

  return {map, players, ghosts};
}
