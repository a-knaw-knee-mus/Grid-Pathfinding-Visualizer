#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace sf;
using namespace std;

// clear everything from the screen
void refreshScreen(RenderWindow& window, vector<vector<RectangleShape>>& cells, const vector<vector<cellState>>& cellStates, int gridSize) {
    window.clear();

    // Draw the grid of cells
    for (int x = 0; x < gridSize; x++) {
        for (int y = 0; y < gridSize; y++) {
            if (cellStates[x][y] == Wall) {
                cells[x][y].setFillColor(Color::Black);
            } else if (cellStates[x][y] == Start) {
                cells[x][y].setFillColor(Color::Red);
            } else if (cellStates[x][y] == End) {
                cells[x][y].setFillColor(Color::Green);
            } else if (cellStates[x][y] == Visited) {
                cells[x][y].setFillColor(Color::Cyan);
            } else if (cellStates[x][y] == Path) {
                cells[x][y].setFillColor(Color::Magenta);
            } else {
                cells[x][y].setFillColor(Color::White);
            }

            window.draw(cells[x][y]);
        }
    }
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
