#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <ctime>
#include <thread>
#include <filesystem>
#include "include/states.h"
#include "include/window.h"
#include "include/pathfinding.h"
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
    int speedIdx = 1;
    const vector<int> speeds = {0, 1, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200}; // ms delay between frames
    int cellSizeIdx = 3;
    const vector<int> cellSizes = {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30}; // px size per cell
    const int windowSize = 500;
    int gridSize = windowSize / cellSizes[cellSizeIdx];
    bool allowDiagonal = false;

    // extra 350px horizontal for instructions
    RenderWindow window(VideoMode(windowSize + 325, windowSize+1), "Pathfinding Visualizer", Style::Titlebar | Style::Close);

    Image icon;
    icon.loadFromFile("../images/bullsEye.jpg");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));

    RectangleShape cell{};
    cell.setSize(Vector2f(cellSizes[cellSizeIdx]-1, cellSizes[cellSizeIdx]-1));
    cell.setPosition(1, 1);
    cell.setOutlineColor(Color::Black);
    cell.setOutlineThickness(1);

    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool isShiftPressed = false;
    bool searching = false;

    Vector2i startCell = {-1, -1}; // coords for start cell
    Vector2i endCell = {-1, -1};   // coorsd for end cell

    while (window.isOpen()) {
        Event event{};
        while (window.pollEvent(event) && !searching) {
            if (event.type == Event::Closed) {
                window.close();
            }

            // set pathfinding algorithm
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::Num1) {
                searchType = "AStar";
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::Num2) {
                searchType = "BFS";
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::Num3) {
                searchType = "Greedy BFS";
            }

            // change cell size
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Up) {
                if (cellSizeIdx+1 >= cellSizes.size()) continue;
                cellSizeIdx += 1;
                gridSize = windowSize / cellSizes[cellSizeIdx];

                cellStates.resize(gridSize);
                for (vector<cellState>& row: cellStates) {
                    row.resize(gridSize, Clear);
                }

                for (int i=0; i<gridSize; i++) {
                    for (int j=0; j<gridSize; j++) {
                        cell.setSize(Vector2f(cellSizes[cellSizeIdx]-1, cellSizes[cellSizeIdx]-1));
                        cell.setPosition(i * cellSizes[cellSizeIdx], j * cellSizes[cellSizeIdx]);
                        cellStates[i][j] = Clear;
                    }
                }

                startCell = {-1, -1};
                endCell = {-1, -1};

                window.setTitle("Pathfinding Visualizer");
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Down) {
                if (cellSizeIdx-1 < 0) continue;
                cellSizeIdx -= 1;
                gridSize = windowSize / cellSizes[cellSizeIdx];

                cellStates.resize(gridSize);
                for (vector<cellState>& row: cellStates) {
                    row.resize(gridSize, Clear);
                }

                for (int i=0; i<gridSize; i++) {
                    for (int j=0; j<gridSize; j++) {
                        cell.setSize(Vector2f(cellSizes[cellSizeIdx]-1, cellSizes[cellSizeIdx]-1));
                        cellStates[i][j] = Clear;
                    }
                }

                startCell = {-1, -1};
                endCell = {-1, -1};

                window.setTitle("Pathfinding Visualizer");
            }

            // change animation speed
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Right) {
                if (speedIdx+1 >= speeds.size()) continue;
                speedIdx += 1;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Left) {
                if (speedIdx-1 < 0) continue;
                speedIdx -= 1;
            }

            // toggle diagonal only
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::D) {
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
                window.setTitle("Pathfinding Visualizer");
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::W) {
                generateRandomMaze(cellStates, gridSize);
                startCell = {-1, -1};
                endCell = {-1, -1};
                window.setTitle("Pathfinding Visualizer");
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
                placeRandomWalls(cellStates, gridSize, gridSize*gridSize*0.35); // 35% of the cells should be walls
                startCell = {-1, -1};
                endCell = {-1, -1};
                window.setTitle("Pathfinding Visualizer");
            }

            // run search algorithm
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (startCell.x == -1 || startCell.y == -1) continue;
                if (endCell.x == -1 || endCell.y == -1) continue;
                searching = true;
                window.setTitle("Pathfinding Visualizer");
                resetPathfinding(gridSize, cellStates);
                cout << "Run Search" << endl;
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::LShift) {
                isShiftPressed = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::LShift) {
                isShiftPressed = false;
            }
            // set start cell
            else if (isShiftPressed && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSizes[cellSizeIdx], mousePosition.y / cellSizes[cellSizeIdx]};
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

                window.setTitle("Pathfinding Visualizer");
                resetPathfinding(gridSize, cellStates);
            }
            // set end cell
            else if (isShiftPressed && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSizes[cellSizeIdx], mousePosition.y / cellSizes[cellSizeIdx]};
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

                window.setTitle("Pathfinding Visualizer");
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
                Vector2i cell = {mousePosition.x / cellSizes[cellSizeIdx], mousePosition.y / cellSizes[cellSizeIdx]};
                if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) continue;
                if (cell.x >= 0 && cell.x < gridSize && cell.y >= 0 && cell.y < gridSize) {
                    cellStates[cell.x][cell.y] = Wall;
                }

                if (cell.x == startCell.x && cell.y == startCell.y) {
                    startCell = {-1, -1};
                }
                if (cell.x == endCell.x && cell.y == endCell.y) {
                    endCell = {-1, -1};
                }

                window.setTitle("Pathfinding Visualizer");
                resetPathfinding(gridSize, cellStates);
            }
            else if (isRightMousePressed && Mouse::isButtonPressed(Mouse::Right)) {
                Vector2i mousePosition = Mouse::getPosition(window);
                Vector2i cell = {mousePosition.x / cellSizes[cellSizeIdx], mousePosition.y / cellSizes[cellSizeIdx]};
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

                window.setTitle("Pathfinding Visualizer");
                resetPathfinding(gridSize, cellStates);
            }

            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::P) {
                Texture texture;
                texture.create(window.getSize().x, window.getSize().y);
                texture.update(window);

                time_t now = time(nullptr);
                tm localTime = *localtime(&now);
                stringstream ss;
                ss << put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
                const string suffix = ss.str();

                if (!filesystem::exists("screenshots")) {
                    filesystem::create_directory("screenshots");
                }
                if (texture.copyToImage().saveToFile("screenshots/path-" + suffix + ".png")) {
                    cout << "Screenshot saved" << endl;
                } else {
                    cerr << "Failed to save screenshot" << endl;
                }
            }
        }

        vector<string> legendParams = {searchType, to_string(speeds[speedIdx]), to_string(cellSizes[cellSizeIdx]), allowDiagonal ? "Yes" : "No"};

        if (searching) {
            vector<Vector2i> path{};
            if (searchType == "AStar") {
                path = findPathAStar(cellStates, startCell, endCell, allowDiagonal, cell, window, event, cellSizes[cellSizeIdx], speeds[speedIdx], legendParams);
            } else if (searchType == "BFS") {
                path = findPathBFS(cellStates, startCell, endCell, allowDiagonal, cell, window, event, cellSizes[cellSizeIdx], speeds[speedIdx], legendParams);
            } else if (searchType == "Greedy BFS") {
                path = findPathGreedyBFS(cellStates, startCell, endCell, allowDiagonal, cell, window, event, cellSizes[cellSizeIdx], speeds[speedIdx], legendParams);
            } else {
                cerr << "invalid search" << endl;
            }
            for (const auto& node : path) {
                cellStates[node.x][node.y] = Path;
                chrono::milliseconds duration(speeds[speedIdx]);
                this_thread::sleep_for(duration);
                refreshScreen(window, cell, cellStates, gridSize, cellSizes[cellSizeIdx], legendParams);
            }
            searching = false;
        }

        refreshScreen(window, cell, cellStates, gridSize, cellSizes[cellSizeIdx], legendParams);
    }
    return 0;
}
