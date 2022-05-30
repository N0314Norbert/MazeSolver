#pragma once
#include <iostream>
#include "GenerateMaze.h"

int main()
{
    GenerateMaze maze;
    maze.ConstructConsole(160, 100, 8, 8);
    maze.Start();

    return 0;
}

