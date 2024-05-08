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

// Remove Visited, VisitedInQueue and Path Cells
void resetPathfinding(const int gridSize, vector<vector<cellState>>& cellStates) {
    for (int x = 0; x < gridSize; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            if (cellStates[x][y] == Visited || cellStates[x][y] == Path || cellStates[x][y] == VisitedInQueue) {
                cellStates[x][y] = Clear;
            }
        }
    }
}

int main() {
    string searchType = "AStar";
    int speed = 0; // ms delay between frames
    const int minSpeed = 0, maxSpeed = 200;
    int cellSize = 10;
    const int minCellSize = 4, maxCellSize = 30;
    const int windowSize = 500;
    int gridSize = windowSize / cellSize;
    bool allowDiagonal = false;

    // extra 350px horizontal for instructions
    RenderWindow window(VideoMode(windowSize + 350, windowSize+1), "Pathfinding Visualizer", Style::Titlebar | Style::Close);

    Image icon;
    icon.loadFromFile("../images/bullsEye.jpg");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));

    RectangleShape cell{};
    cell.setSize(Vector2f(cellSize-1, cellSize-1));
    cell.setPosition(1, 1);
    cell.setOutlineColor(Color::Black);
    cell.setOutlineThickness(1);

    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool setStart = false;
    bool setEnd = false;
    bool searching = false;

    Vector2i startCell = {-1, -1}; // coords for start cell
    Vector2i endCell = {-1, -1};   // coorsd for end cell

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

                startCell = {-1, -1};
                endCell = {-1, -1};
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Down) {
                if (cellSize - 2 < minCellSize) continue;
                cellSize -= 2;
                gridSize = windowSize / cellSize;

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

                startCell = {-1, -1};
                endCell = {-1, -1};
            }

            // change animation speed
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Right) {
                if (speed + 10 > maxSpeed) continue;
                speed += 10;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Left) {
                if (speed - 10 < minSpeed) continue;
                speed -= 10;
            }

            // toggle diagonal only
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::D) {
                allowDiagonal = !allowDiagonal;
            }

            // reset cells
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                for (int x = 0; x < gridSize; ++x) {
                    for (int y = 0; y < gridSize; ++y) {
                        cellStates[x][y] = Clear;
                    }
                }
                startCell = {-1, -1};
                endCell = {-1, -1};
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::W) {
                generateRandomMaze(cellStates, gridSize);
                startCell = {-1, -1};
                endCell = {-1, -1};
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
                placeRandomWalls(cellStates, gridSize, gridSize*gridSize*0.35); // 35% of the cells should be walls
                startCell = {-1, -1};
                endCell = {-1, -1};
            }

            // run search algorithm
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (startCell.x == -1 || startCell.y == -1) continue;
                if (endCell.x == -1 || endCell.y == -1) continue;
                searching = true;
                resetPathfinding(gridSize, cellStates);
                cout << "Run Search" << endl;
            }

            // set start cell
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::A) {
                setStart = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::A) {
                setStart = false;
            }
            else if (setStart && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSize, mousePosition.y / cellSize};
                if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) continue;
                cellStates[cell.x][cell.y] = Start;
                if (startCell.x != -1 && startCell.y != -1) { // remove last selected start cell
                    cellStates[startCell.x][startCell.y] = Clear;
                }
                if (cell.x == endCell.x && cell.y == endCell.y) {
                    endCell = {-1, -1};
                }
                // save new location for start cell
                startCell = cell;

                resetPathfinding(gridSize, cellStates);
            }

            // set end cell
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::S) {
                setEnd = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::S) {
                setEnd = false;
            }
            else if (setEnd && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSize, mousePosition.y / cellSize};
                if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) continue;
                cellStates[cell.x][cell.y] = End;
                if (endCell.x != -1 && endCell.y != -1) { // remove last selected end cell
                    cellStates[endCell.x][endCell.y] = Clear;
                }
                if (cell.x == startCell.x && cell.y == startCell.y) {
                    startCell = {-1, -1};
                }
                // save new location for end cell
                endCell = cell;

                resetPathfinding(gridSize, cellStates);
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
                Vector2i cell = {mousePosition.x / cellSize, mousePosition.y / cellSize};
                if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) continue;
                if (cell.x >= 0 && cell.x < gridSize && cell.y >= 0 && cell.y < gridSize) {
                    cellStates[cell.x][cell.y] = Wall;
                }

                if (cell.x == startCell.x && cell.y == startCell.y) {
                    startCell = {-1, -1};
                }
                if (cell.x == endCell.x &&
                    cell.y == endCell.y) {
                    endCell = {-1, -1};
                }

                resetPathfinding(gridSize, cellStates);
            }
            else if (isRightMousePressed && Mouse::isButtonPressed(Mouse::Right)) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSize, mousePosition.y / cellSize};
                if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) continue;
                if (cell.x >= 0 && cell.x < gridSize && cell.y >= 0 && cell.y < gridSize) {
                    cellStates[cell.x][cell.y] = Clear;
                }

                if (cell.x == endCell.x &&  cell.y == endCell.y) {
                    startCell = {-1, -1};
                }
                if (cell.x == endCell.x &&  cell.y == endCell.y) {
                    endCell = {-1, -1};
                }

                resetPathfinding(gridSize, cellStates);
            }
        }

        vector<string> legendParams = {searchType, to_string(speed), to_string(cellSize), allowDiagonal ? "Yes" : "No"};

        if (searching) {
            vector<Vector2i> path = findPath(cellStates, startCell, endCell, allowDiagonal, cell, window, event, cellSize, speed, legendParams);
            for (const auto& node : path) {
                cellStates[node.x][node.y] = Path;
                chrono::milliseconds duration(speed);
                this_thread::sleep_for(duration);
                refreshScreen(window, cell, cellStates, gridSize, cellSize, legendParams);
            }
            searching = false;
        }

        refreshScreen(window, cell, cellStates, gridSize, cellSize, legendParams);
    }
    return 0;
}
