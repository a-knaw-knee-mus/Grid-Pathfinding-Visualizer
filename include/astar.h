# pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace std;
using namespace sf;

vector<Vector2i> findPath(vector<vector<cellState>>& cellStates, Vector2i startCellIdx, Vector2i endCellIdx, bool allowDiagonal, RectangleShape& cell, RenderWindow& window, int cellSize, int speed, vector<string>& legendParams);
