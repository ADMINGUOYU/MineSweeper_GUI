#include "MineSweeper.h"
#include <iostream>

void MineSweeper::gen2DArray(char** &ptr, int row, int col)
{
	ptr = new char* [row];

	for (int i = 0; i < row; i++)
		ptr[i] = new char[col];

	return;
}

void MineSweeper::del2DArray(char** &ptr, int row, int col)
{
	for (int i = 0; i < row; i++)
		delete[] ptr[i];

	delete[] ptr;

	ptr = nullptr;
	return;
}

void MineSweeper::initBoard(char** &map, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			map[i][j] = CELL_EMPTY;
		}
	}

	return;
}

void MineSweeper::genMine(char** &realMap, int row, int col, int num)
{
	int count = num;
	std::srand((unsigned int)std::time(nullptr));

	while (count)
	{
		int x = std::rand() % col;
		int y = std::rand() % row;
		if (realMap[y][x] == CELL_EMPTY) {
			realMap[y][x] = CELL_MINE;
			count--;
		}
	}

	return;
}

void MineSweeper::displayMap(char** map, int row, int col)
{
	int r, c;

	// display column numbers
	std::cout << "  ";
	for (c = 0; c < col; c++)
		std::cout << c % 10 << " ";
	std::cout << std::endl;

	// display map and row numbers (on the left)
	for (r = 0; r < row; r++)
	{
		std::cout << r % 10 << " ";
		for (c = 0; c < col; c++)
			std::cout << map[r][c] << " ";
		std::cout << std::endl;
	}

	return;
}

int MineSweeper::countChar(char** map, int startRow, int startCol, int endRow, int endCol, char ch)
{
	// LOOP approach
	int count = 0;
	for (int i = startRow; i <= endRow; i++)
	{
		for (int j = startCol; j <= endCol; j++)
		{
			if (map[i][j] == ch)
				count++;
		}
	}

	return count;

	// RECURSION approach

	//// ending case
	//if ((startRow == endRow) && (startCol == endCol))
	//	if (ch == map[startRow][startCol])
	//		return 1;
	//	else
	//		return 0;


	//int splitROW = (endRow + startRow) / 2;
	//int splitCOL = (endCol + startCol) / 2;
	//int count = 0;

	////           *
	//// ______|______|______|______
	//// ______|______|______|______  *
	//// ______|______|______|______
	//
	//count += countChar(map, startRow, startCol, splitROW, splitCOL, ch);

	//if (endCol - (splitCOL + 1) >= 0)
	//	count += countChar(map, startRow, splitCOL + 1, splitROW, endCol, ch);

	//if (endRow - (splitROW + 1) >= 0)
	//{
	//	count += countChar(map, splitROW + 1, startCol, endRow, splitCOL, ch);

	//	if (endCol - (splitCOL + 1) >= 0)
	//		count += countChar(map, splitROW + 1, splitCOL + 1, endRow, endCol, ch);
	//}

	//return count;

}

MineSweeper::MineSweeper(LL_Queue* ll_queue, int row, int col, int numMine)
	:
	m_msgGame(ll_queue),
	m_row(row),
	m_col(col),
	m_numOfMine(numMine),
	m_realMap(nullptr),
	m_currentMap(nullptr),
	m_gameFinish(false)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeper' constructor being called\n";
	std::wcout << this << " CLASS: 'MineSweeper'"
		<< " row: " << row
		<< " col: " << col
		<< " number of mine: " << numMine
		<< " ll_queue: " << ll_queue
		<< "\n";
#endif

	// dynamically allocate memory
	this->gen2DArray(this->m_currentMap, row, col);
	this->gen2DArray(this->m_realMap, row, col);

	// init board
	this->initBoard(this->m_currentMap, row, col);
	this->initBoard(this->m_realMap, row, col);

	// generate mine to realMap
	this->genMine(this->m_realMap, row, col, numMine);

	// a message of success
	int unexplored = this->countChar(this->m_currentMap, 0, 0, this->m_row - 1, this->m_col - 1, CELL_EMPTY);
	this->m_msgGame->enqueue(
		ENCODE_DWORD((this->m_row * this->m_col) - unexplored, unexplored),
		MINESWEEPER_INFO);

#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeper' constructor execution completes\n";
#endif
	return;
}

MineSweeper::~MineSweeper(void)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeper' destructor being called\n";
#endif

	// delete dynamically allocated arrays
	this->del2DArray(this->m_currentMap, this->m_row, this->m_col);
	this->del2DArray(this->m_realMap, this->m_row, this->m_col);

#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeper' destructor execution completes\n";
#endif
	return;
}

void MineSweeper::exploreMap(int x, int y, bool initCall)
{
	// check if the game is done
	if (this->m_gameFinish)
		return;

	// out of range do nothing (for now)
	if (y < 0 || y >= this->m_row || x < 0 || x >= this->m_col)
		return;

	// check if it has already been explored -> do nothing (for now)
	if (CELL_EMPTY != this->m_currentMap[y][x])
		return;

	// if this is a user marked cell
	if (CELL_USER_MARKED == this->m_currentMap[y][x])
		return;

	// check if we are on mine
	if (CELL_MINE == this->m_realMap[y][x])
	{
		if (initCall)
		{
			// NOOOOO! we are on mine
#ifdef _DEBUG
			std::wcout << this << " \t'exploreMap' on mine game finished\n";
#endif
			this->m_gameFinish = true;
			this->m_msgGame->enqueue(ENCODE_DWORD(x, y), MINESWEEPER_LOSE);
			this->m_msgGame->enqueue(ENCODE_DWORD(NULL, NULL), MINESWEEPER_FINISH);
		}
		return;
	}

	//since we have checked that the current [row][col] is NOT mine, we do not check again here
	int mineCount = countChar(this->m_realMap,
		(y == 0 ? 0 : y - 1),
		(x == 0 ? 0 : x - 1),
		(y == (this->m_row - 1) ? (this->m_row - 1) : y + 1),
		(x == (this->m_col - 1) ? (this->m_col - 1) : x + 1),
		CELL_MINE);

	//now we need to explore more blocks if necessary
	if (mineCount == 0) {
		this->m_currentMap[y][x] = CELL_EMPTY_EXPLORED;

		this->exploreMap(x - 1, y - 1, false);
		this->exploreMap(x - 1, y, false);
		this->exploreMap(x - 1, y + 1, false);

		this->exploreMap(x, y - 1, false);
		this->exploreMap(x, y + 1, false);

		this->exploreMap(x + 1, y - 1, false);
		this->exploreMap(x + 1, y, false);
		this->exploreMap(x + 1, y + 1, false);
	}
	else {
		this->m_currentMap[y][x] = mineCount + '0'; //we need a character here!!!
	}

	// check if this is the initial caller
	if (initCall)
	{
#ifdef _DEBUG
		std::wcout << this << " \t'exploreMap' process complete -> print mapCurrent\n";
		std::wcout << "____________________________________________________________\n";
		displayMap(this->m_currentMap, this->m_row, this->m_col);
		std::wcout << "------------------------------------------------------------\n";
		std::wcout << this << " \t'exploreMap' process complete -> print done\n";
#endif
		// we send a message to the queue
		// LOWORD is the cell explored
		// HIWORD is the cell unexplored
		int unexplored = this->countChar(this->m_currentMap, 0, 0, this->m_row - 1, this->m_col - 1, CELL_EMPTY) + this->countChar(this->m_currentMap, 0, 0, this->m_row - 1, this->m_col - 1, CELL_USER_MARKED);
		this->m_msgGame->enqueue(
			ENCODE_DWORD((this->m_row * this->m_col) - unexplored, unexplored),
			MINESWEEPER_INFO);
		// check if you win
		if (unexplored <= this->m_numOfMine) {
#ifdef _DEBUG
			std::wcout << this << " \t'exploreMap' 'win' game finished\n";
#endif
			// update status
			this->m_gameFinish = true;
			// you win
			this->m_msgGame->enqueue(
				ENCODE_DWORD(x, y),
				MINESWEEPER_WIN);
			// send a finish message as well
			this->m_msgGame->enqueue(
				ENCODE_DWORD(NULL, NULL), 
				MINESWEEPER_FINISH);
		}
		
	}
	return;
}

void MineSweeper::markCell(int x, int y) const
{
	// check if the game is done
	if (this->m_gameFinish)
		return;

	// out of range do nothing (for now)
	if (y < 0 || y >= this->m_row || x < 0 || x >= this->m_col)
		return;


	// if not marked -> mark
	switch (this->m_currentMap[y][x])
	{
	case CELL_EMPTY:
#ifdef _DEBUG
		std::wcout << this << " \t'markCell' marked"
			<< " ( " << x << " , " << y << " )"
			<< "\n";
#endif
		this->m_currentMap[y][x] = CELL_USER_MARKED;
		break;

	case CELL_USER_MARKED:
#ifdef _DEBUG
		std::wcout << this << " \t'markCell' unmarked"
			<< " ( " << x << " , " << y << " )"
			<< "\n";
#endif
		this->m_currentMap[y][x] = CELL_EMPTY;
		break;

	default:
		return;
	}

	// send message
	int unexplored = this->countChar(this->m_currentMap, 0, 0, this->m_row - 1, this->m_col - 1, CELL_EMPTY) + this->countChar(this->m_currentMap, 0, 0, this->m_row - 1, this->m_col - 1, CELL_USER_MARKED);
	this->m_msgGame->enqueue(
		ENCODE_DWORD((this->m_row * this->m_col) - unexplored, unexplored),
		MINESWEEPER_INFO);

	return;
}

