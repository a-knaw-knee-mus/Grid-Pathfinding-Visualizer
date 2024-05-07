#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>
#include <queue>
#include <cmath>
#include <chrono>
#include <thread>
using namespace std;
using namespace sf;

enum cellState {
    Clear,
    Wall,
    Start,
    End,
    Path,
    Visited,
};

struct Node {
    int x, y;  // Node position
    int costFromStart;  // Cost from start to this node
    int costToEnd;
    Node* parent;  // Parent node

    Node(const int x, const int y, const int costFromStart, const int costToEnd, Node* parent) : x(x), y(y), costFromStart(costFromStart), costToEnd(costToEnd), parent(parent) {}
};

// Calculate Manhattan distance between two points
int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

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

// A* algorithm implementation
vector<Vector2i> findPath(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, const Vector2i endCellIdx, const bool allowDiagonal, vector<vector<RectangleShape>>& cells, RenderWindow& window) {
    vector<Vector2i> path;
    vector<vector<bool>> visited(cellStates.size(), vector<bool>(cellStates[0].size(), false));
    priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)>> pq([](Node* a, Node* b) {
        return (a->costToEnd + a->costFromStart) > (b->costToEnd + b->costFromStart);  // Min heap based on node cost
    });
    auto* start = new Node(startCellIdx.x, startCellIdx.y, 0, manhattanDistance(startCellIdx.x, startCellIdx.y, endCellIdx.x, endCellIdx.y), nullptr); // address of start node
    pq.push(start);

    // Define possible movement directions (up, down, left, right, diagonal, topleft, bottomleft, topright, bottomright)
    constexpr int dx[] = {-1, 1, 0, 0, -1, 1, -1, 1};
    constexpr int dy[] = {0, 0, -1, 1, -1, -1, 1, 1};

    while (!pq.empty()) {
        Node* current = pq.top();
        pq.pop();

        if (current->x == endCellIdx.x && current->y == endCellIdx.y) {
            // Reached the end cell, reconstruct the path
            while (current != nullptr) {
                if (cellStates[current->x][current->y] == Visited) {
                    path.emplace_back(current->x, current->y);
                }
                current = current->parent;
            }
            ranges::reverse(path.begin(), path.end());  // Reverse to get correct path order
            cout << "path found" << endl;
            break;
        }

        visited[current->x][current->y] = true;

        // mark cell as visited except if already marked as Start or End
        if (cellStates[current->x][current->y] == Clear) {
            cellStates[current->x][current->y] = Visited;
            refreshScreen(window, cells, cellStates, cellStates.size());
            chrono::milliseconds duration(20);
            this_thread::sleep_for(duration);
        }

        for (int i = 0; i < (allowDiagonal ? 8 : 4); ++i) {  // Check all directions if allowDiagonal is true
            const int nextX = current->x + dx[i];
            const int nextY = current->y + dy[i];

            // new cell in range, and not a wall
            if (!(nextX >= 0 && nextX < cellStates.size() && nextY >= 0 && nextY < cellStates[0].size())) continue;
            if (visited[nextX][nextY]) continue;
            if (cellStates[nextX][nextY] == Wall) continue;

            //int newCost = current->cost + (i < 4 ? 1 : 2);  // Uniform cost for non-diagonal, greater cost for diagonal
            const int newCostFromStart = current->costFromStart+1;
            const int newCostToEnd = manhattanDistance(nextX, nextY, endCellIdx.x, endCellIdx.y);

            Node* nextNode = new Node(nextX, nextY, newCostFromStart, newCostToEnd, current);
            pq.push(nextNode);
        }

    }

    // Cleanup memory
    while (!pq.empty()) {
        delete pq.top();
        pq.pop();
    }

    return path;
}

int main() {
    RenderWindow window(VideoMode(500, 500), "Search", Style::Titlebar | Style::Close);

    int gridSize = 50;
    int cellSize = 10;

    vector<vector<cellState>> cellStates(gridSize, vector<cellState>(gridSize, Clear));

    vector<vector<RectangleShape>> cells(gridSize, vector<RectangleShape>(gridSize));
    for (int i=0; i<gridSize; i++) {
        for (int j=0; j<gridSize; j++) {
            cells[i][j].setSize(Vector2f(cellSize-1, cellSize-1));
            cells[i][j].setPosition(i * cellSize, j * cellSize);
            cells[i][j].setOutlineThickness(1);
            cells[i][j].setOutlineColor(Color::Black);
        }
    }

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
                refreshScreen(window, cells, cellStates, gridSize);
            }

            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Y) {
                placeRandomWalls(cellStates, gridSize, gridSize*gridSize/2); // half of the cells should be walls
                refreshScreen(window, cells, cellStates, gridSize);
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

        if (searching) {
            vector<Vector2i> path = findPath(cellStates, {startCellIdx[0], startCellIdx[1]}, {endCellIdx[0], endCellIdx[1]}, false, cells, window);
            if (path.empty()) {
                cout << "No Path Found" << endl;
            } else {
                for (const auto& node : path) {
                    cellStates[node.x][node.y] = Path;
                }
            }
            searching = false;
        }

        refreshScreen(window, cells, cellStates, gridSize);
    }
    return 0;
}
