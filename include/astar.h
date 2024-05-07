# pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace std;
using namespace sf;

vector<Vector2i> findPath(vector<vector<cellState>>& cellStates, const Vector2i startCellIdx, const Vector2i endCellIdx, const bool allowDiagonal, vector<vector<RectangleShape>>& cells, RenderWindow& window);
