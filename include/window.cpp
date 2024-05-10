#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "states.h"
using namespace sf;
using namespace std;

void addLegend(RenderWindow& window, unordered_map<string, string>& legendParams) {
    Font regularFont, boldFont;
    Text text;
    int startX = 520;
    int startY = 15; // vertical offset

    if (!regularFont.loadFromFile("fonts/JetBrainsMono-Regular.ttf")) {
        cerr << "Failed to load JetBrainsMono-Regular.ttf" << endl;
    }
    if (!boldFont.loadFromFile("fonts/JetBrainsMono-Bold.ttf")) {
        cerr << "Failed to load JetBrainsMono-Bold.ttf" << endl;
    }

    text.setFillColor(Color::Black);

    text.setFont(boldFont);
    text.setString("Keybinds");
    text.setCharacterSize(20);
    text.setPosition(startX, startY+0);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Algorithm Name = " + legendParams["searchType"]);
    text.setCharacterSize(14);
    text.setPosition(startX, startY+35);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("A*: 1  BFS: 2  Greedy BFS: 3");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+52);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Animation Speed = " + legendParams["speed"] + "ms");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+80);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Increase: <-   Decrease: ->");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+97);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Cell Size = " + legendParams["cellSize"] + "px");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+125);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Increase: ^    Decrease: v");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+142);
    window.draw(text);

    text.setFont(boldFont);
    text.setString("Allow Diagonal Paths = " + legendParams["allowDiagonal"]);
    text.setCharacterSize(14);
    text.setPosition(startX, startY+170);
    window.draw(text);

    text.setFont(regularFont);
    text.setString("Toggle: D");
    text.setCharacterSize(14);
    text.setPosition(startX, startY+187);
    window.draw(text);

    int offset = 135;
    text.setCharacterSize(14);
    text.setFont(boldFont);

    text.setString("Generate Walls:");
    text.setPosition(startX, startY+230);
    window.draw(text);
    text.setString("Q");
    text.setPosition(startX+offset, startY+230);
    window.draw(text);

    text.setString("Generate Maze:");
    text.setPosition(startX, startY+255);
    window.draw(text);
    text.setString("W");
    text.setPosition(startX+offset, startY+255);
    window.draw(text);

    text.setString("Restart:");
    text.setPosition(startX, startY+280);
    window.draw(text);
    text.setString("R");
    text.setPosition(startX+offset, startY+280);
    window.draw(text);

    text.setString("Set Wall:");
    text.setPosition(startX, startY+305);
    window.draw(text);
    text.setString("Hold Left Click");
    text.setPosition(startX+offset, startY+305);
    window.draw(text);

    text.setString("Remove Wall:");
    text.setPosition(startX, startY+330);
    window.draw(text);
    text.setString("Hold Right Click");
    text.setPosition(startX+offset, startY+330);
    window.draw(text);

    text.setString("Set Start:");
    text.setPosition(startX, startY+355);
    window.draw(text);
    text.setString("LShft + Left Click");
    text.setPosition(startX+offset, startY+355);
    window.draw(text);

    text.setString("Set End:");
    text.setPosition(startX, startY+380);
    window.draw(text);
    text.setString("LShft + Right Click");
    text.setPosition(startX+offset, startY+380);
    window.draw(text);

    text.setString("Begin Search:");
    text.setPosition(startX, startY+405);
    window.draw(text);
    text.setString("Enter");
    text.setPosition(startX+offset, startY+405);
    window.draw(text);

    text.setString("Pause Search:");
    text.setPosition(startX, startY+430);
    window.draw(text);
    text.setString("Esc");
    text.setPosition(startX+offset, startY+430);
    window.draw(text);

    text.setString("Screenshot:");
    text.setPosition(startX, startY+455);
    window.draw(text);
    text.setString("P");
    text.setPosition(startX+offset, startY+455);
    window.draw(text);
}

// clear everything from the screen
void refreshScreen(RenderWindow& window, RectangleShape& cell, const vector<vector<cellState>>& cellStates, int gridSize, int cellSize, unordered_map<string, string>& legendParams) {
    window.clear(Color::White);

    // Draw the grid of cells
    for (int x = 0; x < gridSize; x++) {
        for (int y = 0; y < gridSize; y++) {
            cell.setPosition(x * cellSize + 1, y * cellSize + 1);
            if (cellStates[x][y] == Wall) {
                cell.setFillColor(Color::Black);
            } else if (cellStates[x][y] == Start) {
                cell.setFillColor(Color(255, 102, 102));
            } else if (cellStates[x][y] == End) {
                cell.setFillColor(Color(255, 178, 102));
            } else if (cellStates[x][y] == Visited) {
                cell.setFillColor(Color(175, 238, 238));
            } else if (cellStates[x][y] == Path) {
                cell.setFillColor(Color(204, 153, 255));
            } else if (cellStates[x][y] == VisitedInQueue) {
                cell.setFillColor(Color(152, 251, 152));
            } else if (cellStates[x][y] == CurrentCell) {
                cell.setFillColor(Color(178, 102, 255));
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
