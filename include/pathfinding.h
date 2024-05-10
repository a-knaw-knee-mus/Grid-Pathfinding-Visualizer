# pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace std;
using namespace sf;

vector<Vector2i> findPathAStar(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, const Vector2i endCellIdx, bool allowDiagonal, RectangleShape& cell, RenderWindow& window, Event& event, const int cellSize, const int speed, vector<string>& legendParams);

vector<Vector2i> findPathBFS(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, Vector2i endCellIdx, bool allowDiagonal, RectangleShape& cell, RenderWindow& window, Event& event, const int cellSize, const int speed, vector<string>& legendParams);

vector<Vector2i> findPathDFS(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, Vector2i endCellIdx, bool allowDiagonal, RectangleShape& cell, RenderWindow& window, Event& event, const int cellSize, const int speed, vector<string>& legendParams);

vector<Vector2i> findPathGreedyBFS(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, const Vector2i endCellIdx, const bool allowDiagonal, RectangleShape& cell, RenderWindow& window, Event& event, const int cellSize, const int speed, vector<string>& legendParams);