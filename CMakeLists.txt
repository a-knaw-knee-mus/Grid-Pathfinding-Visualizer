cmake_minimum_required(VERSION 3.28)
project(cpp_search)

set(CMAKE_CXX_STANDARD 23)

add_executable(cpp_search main.cpp include/pathfinding.cpp include/window.cpp)

set_target_properties(cpp_search PROPERTIES WIN32_EXECUTABLE true) # prevent terminal from opening
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR /sfml)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(cpp_search sfml-system sfml-window sfml-graphics sfml-audio)