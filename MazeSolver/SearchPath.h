#pragma once
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class SolveMaze: public olcConsoleGameEngine
{
public:
	SolveMaze()
	{
		m_sAppName = L"Path Finding";
	}

private:
	struct sNode
	{
		bool bObstacle = false;			
		bool bVisited = false;			
		float fGlobalGoal;			
		float fLocalGoal;				
		int x;							
		int y;
		vector<sNode*> vecNeighbours;	
		sNode* parent;
	};

	sNode* nodes = nullptr;
	int nMapWidth = 160;
	int nMapHeight = 100;

	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;
};

