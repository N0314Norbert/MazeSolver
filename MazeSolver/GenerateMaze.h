#pragma once

#include "olcConsoleGameEngine.h"
#include <stack>

using namespace std;

class GenerateMaze: public olcConsoleGameEngine
{
public:
	GenerateMaze()
	{
		m_sAppName = L"MAZE SOLVER";
	}

private:
	int mazeWidth;
	int mazeHeight;
	int* maze;

	struct sNode
	{
		bool bVisited = false;
		float fGlobalGoal;
		float fLocalGoal;
		int x;
		int y;
		vector<sNode*> vecNeighbours;
		sNode* parent;
	};

	sNode* nodes = nullptr;
	int nMapWidth = 25;
	int nMapHeight = 25;

	bool mazeFinished = false;

	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;

	enum
	{
		CELL_PATH_N	= 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};

	int visitedCells;

	stack<pair<int, int>> m_stack;

	int pathWidth;


protected:
	virtual bool OnUserCreate()
	{
		mazeWidth = 25;
		mazeHeight = 25;

		srand(time(0));

		maze = new int[mazeWidth * mazeHeight];

		memset(maze, 0x00, mazeWidth * mazeHeight * sizeof(int));

		m_stack.push(make_pair(0, 0));
		maze[0] = CELL_VISITED;
		visitedCells = 1;

		pathWidth = 2;

		nodes = new sNode[nMapWidth * nMapHeight];

		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].x = x; 
				nodes[y * nMapWidth + x].y = y;
				nodes[y * nMapWidth + x].parent = nullptr;
				nodes[y * nMapWidth + x].bVisited = false;
			}


		nodeStart = &nodes[(nMapHeight / 2 + 2) * nMapWidth-8];
		nodeEnd = &nodes[(nMapHeight / 2) * nMapWidth - 2];

		return true;

	}

	void AStar()
	{
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].parent = nullptr;
				nodes[y * nMapWidth + x].bVisited = false;
				nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
				nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;

			}

		auto distance = [](sNode* a, sNode* b) 
		{
			return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
		};

		auto heuristic = [distance](sNode* a, sNode* b) 
		{
			return distance(a, b);
		};

		sNode* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

		list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		while(!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
		{
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
			{
				listNotTestedNodes.pop_front();
			}

			if (listNotTestedNodes.empty())
			{
				break;
			}
			int nNodeSize = 3;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true;

			for (auto nodeNeighbour : nodeCurrent -> vecNeighbours)
			{
				if (!nodeNeighbour->bVisited)
				{
					listNotTestedNodes.push_back(nodeNeighbour);
					
				}
					

				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;
					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}

			}
				
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		

		

		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * mazeWidth + (m_stack.top().first + x);
		};

		if (visitedCells < mazeWidth * mazeHeight)
		{
			vector<int> neighbours;

			if (m_stack.top().second > 0 && (maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);

			if (m_stack.top().first < mazeWidth - 1 && (maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);

			if (m_stack.top().second < mazeHeight - 1 && (maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);

			if (m_stack.top().first > 0 && (maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			if (!neighbours.empty())
			{
				int nextCellDir = neighbours[rand() % neighbours.size()];

				switch (nextCellDir)
				{
				case 0:
					maze[offset(0, 0)] |= CELL_PATH_N;
					maze[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
					nodes[offset(0, 0)].vecNeighbours.push_back(&nodes[offset(0, -1)]);
					nodes[offset(0, -1)].vecNeighbours.push_back(&nodes[offset(0, 0)]);
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
						break;

				case 1:
					maze[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
					maze[offset(0, 0)] |= CELL_PATH_E;
					nodes[offset(0, 0)].vecNeighbours.push_back(&nodes[offset(+1, 0)]);
					nodes[offset(+1, 0)].vecNeighbours.push_back(&nodes[offset(0, 0)]);
					m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second - 0)));
					break;

				case 2:
					maze[offset(0, 0)] |= CELL_PATH_S;
					maze[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
					nodes[offset(0, 0)].vecNeighbours.push_back(&nodes[offset(0, +1)]);
					nodes[offset(0, +1)].vecNeighbours.push_back(&nodes[offset(0, 0)]);
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;

				case 3:
					maze[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
					maze[offset(0, 0)] |= CELL_PATH_W;
					nodes[offset(0, 0)].vecNeighbours.push_back(&nodes[offset(-1, 0)]);
					nodes[offset(-1, 0)].vecNeighbours.push_back(&nodes[offset(0, 0)]);
					m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;

				}

				visitedCells++;
			}
			else
			{
				m_stack.pop();
			}
		}
		else 
		{
			mazeFinished = true;
		}


		Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ');


		for (int x = 0; x < mazeWidth; x++)
		{
			for (int y = 0; y < mazeHeight; y++)
			{
				for (int py = 0; py < pathWidth; py++)
				{
					for (int px = 0; px < pathWidth; px++)
					{
						if (maze[y * mazeWidth + x] & CELL_VISITED)
						{
							Draw(x * (pathWidth + 1) + px, y * (pathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
						}
						else
						{
							Draw(x * (pathWidth + 1) + px, y * (pathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
						}
					}
				}
				for (int p = 0; p < pathWidth; p++)
				{
					if (maze[y * mazeWidth + x] & CELL_PATH_S)
					{
						Draw(x * (pathWidth + 1) + p, y * (pathWidth + 1) + pathWidth);
					}
					if (maze[y * mazeWidth + x] & CELL_PATH_E)
					{
						Draw(x * (pathWidth + 1) + pathWidth, y * (pathWidth + 1) + p);
					}						
				}
				

			}

		}

		int nNodeSize = 3;
		int nNodeBorder = 1;

		int nSelectedNodeX = m_mousePosX / nNodeSize;
		int nSelectedNodeY = m_mousePosY / nNodeSize;

		if (m_mouse[0].bReleased) 
		{
			if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)
				if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)
				{
					if (m_keys[VK_SHIFT].bHeld)
						nodeStart = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
					else if (m_keys[VK_CONTROL].bHeld)
						nodeEnd = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];

					
				}
		}

			for (int x = 0; x < nMapWidth; x++)
			{
				for (int y = 0; y < nMapHeight; y++)
				{
					for (int py = 0; py < pathWidth; py++)
					{
						for (int px = 0; px < pathWidth; px++)
						{
							if (&nodes[y * nMapWidth + x] == nodeStart)
							{
								Fill(x * nNodeSize + nNodeBorder - px, y * nNodeSize + nNodeBorder - py, (x + 1) * nNodeSize - nNodeBorder - px, (y + 1) * nNodeSize - nNodeBorder - py, PIXEL_SOLID, FG_RED);
							}
							if (&nodes[y * nMapWidth + x] == nodeEnd)
							{
								Fill(x * nNodeSize + nNodeBorder - px, y * nNodeSize + nNodeBorder - py, (x + 1) * nNodeSize - nNodeBorder - px, (y + 1) * nNodeSize - nNodeBorder - py, PIXEL_SOLID, FG_GREEN);
							}
						}
					}


				}
			}

			if (mazeFinished)
			{
				AStar();
			}

			if (nodeEnd != nullptr)
			{
				sNode* p = nodeEnd;
				while (p->parent != nullptr)
				{
					DrawLine(p->x * nNodeSize + nNodeSize / 2, p->y * nNodeSize + nNodeSize / 2,
						p->parent->x * nNodeSize + nNodeSize / 2, p->parent->y * nNodeSize + nNodeSize / 2, PIXEL_SOLID, FG_DARK_BLUE);


					p = p->parent;

				}
			}
		

		return true;
	}
};

