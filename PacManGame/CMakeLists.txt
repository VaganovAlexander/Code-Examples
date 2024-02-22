cmake_minimum_required(VERSION 3.18)

project(Pacman)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Pacman SinglePlayer.cpp GameMenu.cpp GameCore.cpp Maps.cpp Game.cpp)
add_executable(Tests Tests.cpp GameCore.cpp Maps.cpp)

enable_testing()

gtest_discover_tests(Tests)

target_link_libraries(Pacman sfml-graphics sfml-audio)
target_link_libraries(Tests GTest::Main)