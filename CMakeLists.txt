cmake_minimum_required(VERSION 3.28)
project(cpp_search)

set(CMAKE_CXX_STANDARD 23)

add_executable(cpp_search main.cpp include/pathfinding.cpp include/window.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/Program Files/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:/Program Files/SFML/include/SFML)
target_link_libraries(cpp_search sfml-system sfml-window sfml-graphics sfml-audio)