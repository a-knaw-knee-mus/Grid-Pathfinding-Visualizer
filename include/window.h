# pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "states.h"
using namespace sf;
using namespace std;

void refreshScreen(RenderWindow& window, RectangleShape& cell, const vector<vector<cellState>>& cellStates, int gridSize, int cellSize);

void placeRandomWalls(vector<vector<cellState>>& cellStates, int gridSize, int numWalls);

void generateRandomMaze(vector<vector<cellState>>& cellStates, int gridSize);
