#include "Tests.hpp"

bool CheckIfFileExists(const std::string& path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

TEST(GameInitTests, InitInfoTest) {
  ASSERT_EQ(map1.init_map.size(), 25);
  ASSERT_EQ(map1.init_players.size(), 1);
  ASSERT_EQ(map1.init_ghosts.size(), 4);
}

TEST(RequirentmentFilesExist, FilesExistTest) {
  std::string path_to_files = "../sprites/";
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "cherry.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "dot.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "empty.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "font.otf"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "ghost_down.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "ghost_left.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "ghost_right.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "ghost_up.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "image.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "lose.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "player_down.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "player_left.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "player_right.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "player_up.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "ulta.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "wall.png"));
  ASSERT_TRUE(CheckIfFileExists(path_to_files + "win.png"));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}