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
    Stop,
};

int main()
{
    RenderWindow window(VideoMode(500, 500), "Search");

    int gridSize = 50;
    int cellSize = 10;

    RectangleShape cell(Vector2f(cellSize, cellSize));
    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));
    cell.setFillColor((Color::White));
    cell.setOutlineThickness(1);
    cell.setOutlineColor(Color::Black);

    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isLeftMousePressed = true;
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    isRightMousePressed = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isLeftMousePressed = false;
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
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

        window.clear(Color::Black);

        // Draw the grid of cells
        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                cell.setPosition(x * cellSize, y * cellSize);
                if (cellStates[x][y] == Wall) {
                    cell.setFillColor(Color::Black);
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
