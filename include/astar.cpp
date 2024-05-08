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
#include "states.h"
#include "window.h"
using namespace std;
using namespace sf;

struct Node {
    int x, y;  // Node position
    int costFromStart;  // Cost from start to this node
    int costToEnd;      // Cost from this node to end
    Node* parent;  // Parent node

    Node(const int x, const int y, const int costFromStart, const int costToEnd, Node* parent) : x(x), y(y), costFromStart(costFromStart), costToEnd(costToEnd), parent(parent) {}
};

// Calculate Manhattan distance between two points
int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool isStartOrEnd(const int x, const int y, const Vector2i start, const Vector2i end) {
    if (x == start.x && y == start.y) return true;
    if (x == end.x && y == end.y) return true;
    return false;
}

// A* algorithm implementation
vector<Vector2i> findPath(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, const Vector2i endCellIdx, const bool allowDiagonal, RectangleShape& cell, RenderWindow& window, Event& event, int cellSize, int speed, vector<string>& legendParams) {
    vector<Vector2i> path;
    vector<vector<bool>> visited(cellStates.size(), vector<bool>(cellStates[0].size(), false));
    priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)>> pq([](Node* a, Node* b) {
        return (a->costToEnd + a->costFromStart) > (b->costToEnd + b->costFromStart);  // Min heap based on node cost
    });
    auto* start = new Node(startCellIdx.x, startCellIdx.y, 0, manhattanDistance(startCellIdx.x, startCellIdx.y, endCellIdx.x, endCellIdx.y), nullptr); // address of start node
    pq.push(start);

    // Define possible movement directions (left, right, up, down, topleft, topright, bottomleft, bottomright)
    constexpr int dx[] = {-1, 1, 0, 0, -1, 1, -1, 1};
    constexpr int dy[] = {0, 0, -1, 1, -1, -1, 1, 1};

    bool stopLoop = false;

    while (!pq.empty()) {
        // check for search cancel
        while (window.pollEvent(event)) {
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                stopLoop = true;
                cout << "Paused" << endl;
                break;
            }
        }

        if (stopLoop) break;

        Node* current = pq.top();
        pq.pop();

        visited[current->x][current->y] = true;

        if (!isStartOrEnd(current->x, current->y, startCellIdx, endCellIdx)) {
            cellStates[current->x][current->y] = Visited;
            refreshScreen(window, cell, cellStates, cellStates.size(), cellSize, legendParams);
            chrono::milliseconds duration(speed);
            this_thread::sleep_for(duration);
        }

        if (current->x == endCellIdx.x && current->y == endCellIdx.y) {
            // Reached the end cell, reconstruct the path
            while (current != nullptr) {
                if (!isStartOrEnd(current->x, current->y, startCellIdx, endCellIdx)) {
                    path.emplace_back(current->x, current->y);
                }
                current = current->parent;
            }
            ranges::reverse(path.begin(), path.end());  // Reverse to get correct path order
            cout << "Path Found" << endl;
            break;
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
            if (!isStartOrEnd(nextX, nextY, startCellIdx, endCellIdx)) {
                cellStates[nextX][nextY] = VisitedInQueue;
            }
            pq.push(nextNode);
        }

    }

    // Cleanup memory
    while (!pq.empty()) {
        delete pq.top();
        pq.pop();
    }

    if (!stopLoop && path.empty()) {
        cout << "No Path Found. Remove Walls and Try Again." << endl;
    }
    return path;
}
