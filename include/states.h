# pragma once

enum cellState {
    Clear, // empty cell
    Wall, // wall
    Start, // start cell
    End, // end cell
    Path, // cell part of final path
    Visited, // visited cell
    InQueue, // cell set to be visited in the future
    CurrentCell, // current cell selected from InQueue to be processed
    VisitedNoPath, // visited cells where a path wasn't found
};