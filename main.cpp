#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstring>
using namespace std;
using namespace sf;

enum cellState {
    Clear,
    Wall,
    Start,
    End,
};

int main()
{
    RenderWindow window(VideoMode(500, 500), "Search", Style::Titlebar | Style::Close);

    int gridSize = 50;
    int cellSize = 10;

    RectangleShape cell(Vector2f(cellSize, cellSize));
    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));
    cell.setFillColor((Color::White));
    cell.setOutlineThickness(1);
    cell.setOutlineColor(Color::Black);

    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool sKeyPressed = false;
    bool eKeyPressed = false;

    int startCellIdx[2] = {-1, -1}; // coords for start cell
    int endCellIdx[2] = {-1, -1};   // coorsd for end cell

    while (window.isOpen()) {
        Event event{};
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
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

            // run search algorithm
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (startCellIdx[0] == -1 || startCellIdx[1] == -1) continue;
                if (endCellIdx[0] == -1 || endCellIdx[1] == -1) continue;

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
            }
            else if (isRightMousePressed && Mouse::isButtonPressed(Mouse::Right)) {
                Vector2i mousePosition = Mouse::getPosition(window);
                int cellX = mousePosition.x / cellSize;
                int cellY = mousePosition.y / cellSize;
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize) {
                    cellStates[cellX][cellY] = Clear;
                }
            }
        }

        window.clear();

        // Draw the grid of cells
        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                cell.setPosition(x * cellSize, y * cellSize);
                if (cellStates[x][y] == Wall) {
                    cell.setFillColor(Color::Black);
                } else if (cellStates[x][y] == Start) {
                    cell.setFillColor(Color::Red);
                } else if (cellStates[x][y] == End) {
                    cell.setFillColor(Color::Green);
                } else {
                    cell.setFillColor(Color::White);
                }
                window.draw(cell);
            }
        }

        window.display();
    }
    return 0;
}
