#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <thread>
#include "include/states.h"
#include "include/window.h"
#include "include/astar.h"
using namespace std;
using namespace sf;

string getTitle(string searchType, int speed, int cellSize) {
    return "PATHFINDING - Search Algo: " + searchType + " | Speed: " + to_string(speed) + "ms | Cell Size: " + to_string(cellSize);
}

int main() {
    string searchType = "AStar";
    int speed = 20; // ms delay between frames
    const int minSpeed = 0, maxSpeed = 200;
    int cellSize = 10;
    const int minCellSize = 4, maxCellSize = 20;
    const int windowSize = 500;
    int gridSize = windowSize / cellSize;
    bool allowDiagonal = false;

    string title = getTitle(searchType, speed, cellSize);
    RenderWindow window(VideoMode(windowSize+1, windowSize+1), title, Style::Titlebar | Style::Close);

    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));

    RectangleShape cell{};
    cell.setSize(Vector2f(cellSize-1, cellSize-1));
    cell.setPosition(1, 1);
    cell.setOutlineColor(Color::Black);
    cell.setOutlineThickness(1);

    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool sKeyPressed = false;
    bool eKeyPressed = false;
    bool searching = false;

    int startCellIdx[2] = {-1, -1}; // coords for start cell
    int endCellIdx[2] = {-1, -1};   // coorsd for end cell

    while (window.isOpen()) {
        Event event{};
        while (window.pollEvent(event) && !searching) {
            if (event.type == Event::Closed) {
                window.close();
            }

            // change cell size
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Up) {
                if (cellSize + 2 > maxCellSize) continue;
                cellSize += 2;
                gridSize = windowSize / cellSize;
                title = getTitle(searchType, speed, cellSize);
                window.setTitle(title);

                cellStates.resize(gridSize);
                for (vector<cellState>& row: cellStates) {
                    row.resize(gridSize, Clear);
                }

                for (int i=0; i<gridSize; i++) {
                    for (int j=0; j<gridSize; j++) {
                        cell.setSize(Vector2f(cellSize-1, cellSize-1));
                        cell.setPosition(i * cellSize, j * cellSize);
                        cellStates[i][j] = Clear;
                    }
                }
                refreshScreen(window, cell, cellStates, gridSize, cellSize);
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Down) {
                if (cellSize - 2 < minCellSize) continue;
                cellSize -= 2;
                gridSize = windowSize / cellSize;
                title = getTitle(searchType, speed, cellSize);
                window.setTitle(title);

                cellStates.resize(gridSize);
                for (vector<cellState>& row: cellStates) {
                    row.resize(gridSize, Clear);
                }

                for (int i=0; i<gridSize; i++) {
                    for (int j=0; j<gridSize; j++) {
                        cell.setSize(Vector2f(cellSize-1, cellSize-1));
                        cellStates[i][j] = Clear;
                    }
                }
                refreshScreen(window, cell, cellStates, gridSize, cellSize);
            }

            // change animation speed
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Right) {
                if (speed + 10 > maxSpeed) continue;
                speed += 10;
                title = getTitle(searchType, speed, cellSize);
                window.setTitle(title);
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Left) {
                if (speed - 10 < minSpeed) continue;
                speed -= 10;
                title = getTitle(searchType, speed, cellSize);
                window.setTitle(title);
            }

            // reset cells
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        cellStates[x][y] = Clear;
                    }
                }
                startCellIdx[0] = -1, startCellIdx[1] = -1;
                endCellIdx[0] = -1, endCellIdx[1] = -1;
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::T) {
                generateRandomMaze(cellStates, gridSize);
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Y) {
                placeRandomWalls(cellStates, gridSize, gridSize*gridSize/2); // half of the cells should be walls
            }

            // run search algorithm
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (startCellIdx[0] == -1 || startCellIdx[1] == -1) continue;
                if (endCellIdx[0] == -1 || endCellIdx[1] == -1) continue;
                searching = true;
                cout << "run search" << endl;
            }

            // set start cell
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::S) {
                sKeyPressed = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::S) {
                sKeyPressed = false;
            }
            else if (sKeyPressed && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePosition = Mouse::getPosition(window);
                int cellX = mousePosition.x / cellSize;
                int cellY = mousePosition.y / cellSize;
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize) {
                    cellStates[cellX][cellY] = Start;
                }
                if (startCellIdx[0] != -1 && startCellIdx[1] != -1) { // remove last selected start cell
                    cellStates[startCellIdx[0]][startCellIdx[1]] = Clear;
                }
                // save new location for start cell
                startCellIdx[0] = cellX;
                startCellIdx[1] = cellY;

                // remove Visited and Path cells
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        if (cellStates[x][y] == Visited || cellStates[x][y] == Path) {
                            cellStates[x][y] = Clear;
                        }
                    }
                }
            }

            // set end cell
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                eKeyPressed = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::E) {
                eKeyPressed = false;
            }
            else if (eKeyPressed && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePosition = Mouse::getPosition(window);
                int cellX = mousePosition.x / cellSize;
                int cellY = mousePosition.y / cellSize;
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize) {
                    cellStates[cellX][cellY] = End;
                }
                if (endCellIdx[0] != -1 && endCellIdx[1] != -1) { // remove last selected end cell
                    cellStates[endCellIdx[0]][endCellIdx[1]] = Clear;
                }
                // save new location for end cell
                endCellIdx[0] = cellX;
                endCellIdx[1] = cellY;

                // remove Visited and Path cells
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        if (cellStates[x][y] == Visited || cellStates[x][y] == Path) {
                            cellStates[x][y] = Clear;
                        }
                    }
                }
            }

            // draw/undraw walls
            else if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    isLeftMousePressed = true;
                }
                else if (event.mouseButton.button == Mouse::Right) {
                    isRightMousePressed = true;
                }
            }
            else if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == Mouse::Left) {
                    isLeftMousePressed = false;
                }
                else if (event.mouseButton.button == Mouse::Right) {
                    isRightMousePressed = false;
                }
            }
            else if (isLeftMousePressed && Mouse::isButtonPressed(Mouse::Left)) {
                Vector2i mousePosition = Mouse::getPosition(window);
                int cellX = mousePosition.x / cellSize;
                int cellY = mousePosition.y / cellSize;
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize) {
                    cellStates[cellX][cellY] = Wall;
                }

                // remove Visited and Path cells
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        // adding wall means nodes that were visited may not be visitable anymore so clear
                        if (cellStates[x][y] == Path || cellStates[x][y] == Visited) {
                            cellStates[x][y] = Clear;
                        }
                    }
                }
            }
            else if (isRightMousePressed && Mouse::isButtonPressed(Mouse::Right)) {
                Vector2i mousePosition = Mouse::getPosition(window);
                int cellX = mousePosition.x / cellSize;
                int cellY = mousePosition.y / cellSize;
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize) {
                    cellStates[cellX][cellY] = Clear;
                }

                // remove Visited and Path cells
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        // removing wall means any that were visited can still be visitable so dont clear visited
                        if (cellStates[x][y] == Path) {
                            cellStates[x][y] = Visited;
                        }
                    }
                }
            }
        }

        if (searching) {
            vector<Vector2i> path = findPath(cellStates, {startCellIdx[0], startCellIdx[1]}, {endCellIdx[0], endCellIdx[1]}, allowDiagonal, cell, window, cellSize, speed);
            if (path.empty()) {
                cout << "No Path Found" << endl;
            } else {
                for (const auto& node : path) {
                    cellStates[node.x][node.y] = Path;
                    chrono::milliseconds duration(speed);
                    this_thread::sleep_for(duration);
                    refreshScreen(window, cell, cellStates, gridSize, cellSize);
                }
            }
            searching = false;
        }

        refreshScreen(window, cell, cellStates, gridSize, cellSize);
    }
    return 0;
}
