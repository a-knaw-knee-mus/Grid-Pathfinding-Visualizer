#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "states.h"
using namespace sf;
using namespace std;

void addLegend(RenderWindow& window, vector<string>& legendParams) {
    Font regularFont, boldFont;
    Text text;
    int startX = 520;
    int startY = 28; // vertical offset

    if (!regularFont.loadFromFile("../fonts/JetBrainsMono-Regular.ttf")) {
        cerr << "Failed to load JetBrainsMono-Regular.ttf" << endl;
    }
    if (!boldFont.loadFromFile("../fonts/JetBrainsMono-Bold.ttf")) {
        cerr << "Failed to load JetBrainsMono-Bold.ttf" << endl;
    }

    text.setFillColor(Color::Black);

    text.setFont(boldFont);
    text.setString("Keybinds");
    text.setCharacterSize(20);
    text.setPosition(startX, startY+0);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Algorithm Name = " + legendParams[0]);
    text.setCharacterSize(14);
    text.setPosition(startX, startY+35);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("A*: 1");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+52);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Animation Speed = " + legendParams[1] + "ms");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+80);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Increase: <-   Decrease: ->");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+97);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Cell Size = " + legendParams[2] + "px");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+125);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Increase: ^    Decrease: v");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+142);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Allow Diagonal Paths = " + legendParams[3]);
    text.setCharacterSize(14);
    text.setPosition(startX, startY+170);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Toggle: D");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+187);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Generate Walls: Q");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+230);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Generate Maze:  W");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+255);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Restart:        R");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+280);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Set Wall:       Hold Left Click");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+305);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Remove Wall:    Hold Right Click");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+330);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Set Start:      S + Left Click");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+355);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Set End:        E + Right Click");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+380);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Begin Search:   Enter");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+405);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Pause Search:   Esc");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+430);
    window.draw(text);
}

// clear everything from the screen
void refreshScreen(RenderWindow& window, RectangleShape& cell, const vector<vector<cellState>>& cellStates, int gridSize, int cellSize, vector<string>& legendParams) {
    window.clear(Color::White);

    // Draw the grid of cells
    for (int x = 0; x < gridSize; x++) {
        for (int y = 0; y < gridSize; y++) {
            cell.setPosition(x * cellSize + 1, y * cellSize + 1);
            if (cellStates[x][y] == Wall) {
                cell.setFillColor(Color::Black);
            } else if (cellStates[x][y] == Start) {
                cell.setFillColor(Color::Red);
            } else if (cellStates[x][y] == End) {
                cell.setFillColor(Color::Green);
            } else if (cellStates[x][y] == Visited) {
                cell.setFillColor(Color::Cyan);
            } else if (cellStates[x][y] == Path) {
                cell.setFillColor(Color::Magenta);
            } else {
                cell.setFillColor(Color::White);
            }

            window.draw(cell);
        }
    }

    addLegend(window, legendParams);

    window.display();
}

// Function to generate a random maze using Recursive Backtracking algorithm
void generateRandomMaze(vector<vector<cellState>>& cellStates, int gridSize) {
    // Reset cellStates to Clear
    for (int x = 0; x < gridSize; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            cellStates[x][y] = Clear;
        }
    }

    // Seed the random number generator
    srand(time(nullptr));

    // Recursive Backtracking algorithm for maze generation
    function<void(int, int)> recursiveBacktrack = [&](int x, int y) {
        // Mark the current cell as visited
        cellStates[x][y] = Wall;

        // Define random order for visiting neighbors
        vector<int> directions = {0, 1, 2, 3};  // Up, Down, Left, Right
        random_shuffle(directions.begin(), directions.end());

        // Visit neighbors in random order
        for (const int dir : directions) {
            int nx = x, ny = y;
            if (dir == 0) nx -= 2;  // Up
            else if (dir == 1) nx += 2;  // Down
            else if (dir == 2) ny -= 2;  // Left
            else if (dir == 3) ny += 2;  // Right

            if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize && cellStates[nx][ny] == Clear) {
                // Carve a passage between current cell and neighbor
                cellStates[nx][ny] = Wall;
                cellStates[x + (nx - x) / 2][y + (ny - y) / 2] = Wall;

                // Recursively visit the neighbor
                recursiveBacktrack(nx, ny);
            }
        }
    };

    // Start maze generation from a random cell
    const int startX = rand() % (gridSize / 2) * 2 + 1;
    const int startY = rand() % (gridSize / 2) * 2 + 1;
    recursiveBacktrack(startX, startY);
}

void placeRandomWalls(vector<vector<cellState>>& cellStates, int gridSize, int numWalls) {
    // Reset cellStates to Clear
    for (auto& row : cellStates) {
        for (auto& cell : row) {
            cell = Clear;
        }
    }

    // Seed the random number generator
    srand(time(nullptr));

    // Randomly place walls
    for (int i = 0; i < numWalls; ++i) {
        const int x = rand() % gridSize;
        const int y = rand() % gridSize;
        cellStates[x][y] = Wall;
    }
}
