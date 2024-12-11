#pragma once
#ifndef _MINESWEEPER_H_
#define _MINESWEEPER_H_

#include "LL_Queue.h"

enum MINESWEEPER_MAP_TYPE
{
	MAP_CURRENT,
	MAP_REAL
};

enum MINESWEEPER_MSG_TYPE
{
	MINESWEEPER_FINISH = 0,
	MINESWEEPER_WIN,
	MINESWEEPER_LOSE,
	MINESWEEPER_INFO,

};

enum MINESWEEPER_CELL_STATUS
{
	CELL_EMPTY = '.',
	CELL_MINE = '*',
	CELL_EMPTY_EXPLORED = ' ',
	CELL_USER_MARKED = '?'
};

class MineSweeper {
private:
	LL_Queue* m_msgGame;

	int m_row;
	int m_col;
	int m_numOfMine;

	char** m_realMap;
	char** m_currentMap;

	bool m_gameFinish;

private:
	static void gen2DArray(char** &ptr, int row, int col);
	static void del2DArray(char** &ptr, int row, int col);
	static void initBoard(char** &map, int row, int col);
	static void genMine(char** &realMap, int row, int col, int num);
	static void displayMap(char** map, int row, int col);

	static int countChar(char** map, int startRow, int startCol, int endRow, int endCol, char ch);

public:
	MineSweeper(LL_Queue* ll_queue, int row, int col, int numMine);
	~MineSweeper(void);

public:
	void exploreMap(int x, int y, bool initCall = true);
	void markCell(int x, int y) const;
	char** getMap(MINESWEEPER_MAP_TYPE type)
	{
		switch (type)
		{
		case MAP_CURRENT:
			return this->m_currentMap;
		case MAP_REAL:
			return this->m_realMap;
		default:
			return nullptr;
		}
	}
	bool gameFinish(void) { return this->m_gameFinish; }
};

#endif // !_MINESWEEPER_H_
