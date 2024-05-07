# pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace sf;
using namespace std;

void refreshScreen(RenderWindow& window, vector<vector<RectangleShape>>& cells, const vector<vector<cellState>>& cellStates, int gridSize);

void placeRandomWalls(vector<vector<cellState>>& cellStates, int gridSize, int numWalls);

void generateRandomMaze(vector<vector<cellState>>& cellStates, int gridSize);
