#define _CRT_SECURE_NO_WARNINGS
#include "MineSweeperGame.h"
#include <iostream>
#include <climits>

RECT MineSweeperGame::valid = { WND_RES_X * (1 - WND_VALID_RATIO) / 2,
				WND_RES_Y * (1 - WND_VALID_RATIO) / 2, 
				WND_RES_X * WND_VALID_RATIO, 
				WND_RES_Y * WND_VALID_RATIO };

RECT MineSweeperGame::gameBoard = { valid.left,
				valid.top, 
				valid.bottom / WND_BOARD_RATIO, 
				valid.bottom };

RECT MineSweeperGame::titleBar = { valid.left + gameBoard.right + WND_PADDING_X,
				valid.top, 
				valid.right - gameBoard.right, 
				WND_TITLE_HEIGHT };

RECT MineSweeperGame::ctrlSet = { titleBar.left + (titleBar.right * WND_PADDING_X_RATIO / 2),
				(titleBar.top + titleBar.bottom) + WND_PADDING_Y, 
				titleBar.right * (1 - WND_PADDING_X_RATIO), 
				WND_BTN_HEIGHT };

RECT MineSweeperGame::ctrlReset = { ctrlSet.left,
				ctrlSet.top + ctrlSet.bottom + WND_PADDING_Y,
				ctrlSet.right,
				WND_BTN_HEIGHT };

RECT MineSweeperGame::infoBox = { titleBar.left,
				ctrlReset.top + ctrlReset.bottom + WND_PADDING_Y,
				titleBar.right,
				valid.bottom - (ctrlReset.top + ctrlReset.bottom + WND_PADDING_Y) };

MineSweeperGame::boardINFO MineSweeperGame::gameBoardInfo = { GAME_BOARD_ROW, GAME_BOARD_COL, GAME_BOARD_MINE, min((float)gameBoard.right / GAME_BOARD_COL, (float)gameBoard.bottom / GAME_BOARD_ROW) };

MineSweeperGame::MineSweeperGame(void)
	:
	m_gameLogic(nullptr)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeperGame' constructor being called\n";
#endif
	this->m_msgGame = new LL_Queue();
	this->m_msgQueueWnd = new LL_Queue();
	this->m_gameWindow = new WndClassWin32(L"MineSweeper", WND_RES_X, WND_RES_Y, this->m_msgQueueWnd);

#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeperGame' constructor execution completes\n";
#endif
	return;
}

MineSweeperGame::~MineSweeperGame(void) 
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeperGame' destructor being called\n";
#endif
	delete this->m_gameWindow;
	delete this->m_msgQueueWnd;
	delete this->m_msgGame;
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'MineSweeperGame' destructor execution completes\n";
#endif
}

void MineSweeperGame::paintBoardBac(HDC hdc, void* posX, void* posY)
{
	SetBkMode(hdc, TRANSPARENT);
	HGDIOBJ original = NULL;
	original = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, GAME_BOARD_BACK_COLOR);

	for (int i = 0; i < gameBoardInfo.row; i++)
	{
		for (int j = 0; j < gameBoardInfo.col; j++)
		{
			Rectangle(hdc,
				gameBoard.left + (j * gameBoardInfo.size),
				gameBoard.top + (i * gameBoardInfo.size),
				gameBoard.left + ((j + 1) * gameBoardInfo.size),
				gameBoard.top + ((i + 1) * gameBoardInfo.size));
		}
	}
	SelectObject(hdc, original);
	return;
}

void MineSweeperGame::paintCtrlBac(HDC hdc, void* posX, void* posY)
{
	SetBkMode(hdc, TRANSPARENT);
	HGDIOBJ original = NULL;

	// print title
	HFONT titleFont = CreateFont(titleBar.bottom, titleBar.bottom * 0.618, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, L"Consolas");
	original = SelectObject(hdc, titleFont);

	RECT rectangle = { titleBar.left, titleBar.top, titleBar.left + titleBar.right, titleBar.top + titleBar.bottom };
	DrawText(hdc, TITLE, wcslen(TITLE), &rectangle, DT_CENTER);

	// print buttons -> set
	HFONT btnFont = CreateFont(ctrlSet.bottom, ctrlSet.bottom * 0.618, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, L"Consolas");
	
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, GAME_BTN_SET_COLOR);

	rectangle.left = ctrlSet.left;
	rectangle.top = ctrlSet.top;
	rectangle.right = ctrlSet.left + ctrlSet.right;
	rectangle.bottom = ctrlSet.top + ctrlSet.bottom;

	Rectangle(hdc, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);

	SelectObject(hdc, btnFont);
	DrawText(hdc, BTN_SET_TEXT, wcslen(BTN_SET_TEXT), &rectangle, DT_CENTER);

	// -> reset
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, GAME_BTN_RESET_COLOR);

	rectangle.left = ctrlReset.left;
	rectangle.top = ctrlReset.top;
	rectangle.right = ctrlReset.left + ctrlReset.right;
	rectangle.bottom = ctrlReset.top + ctrlReset.bottom;

	Rectangle(hdc, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);

	SelectObject(hdc, btnFont);
	DrawText(hdc, BTN_RESET_TEXT, wcslen(BTN_RESET_TEXT), &rectangle, DT_CENTER);

	// -> infobox
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, GAME_INFOBOX_COLOR);

	rectangle.left = infoBox.left;
	rectangle.top = infoBox.top;
	rectangle.right = infoBox.left + infoBox.right;
	rectangle.bottom = infoBox.top + infoBox.bottom;

	Rectangle(hdc, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);

	// tidy up
	SelectObject(hdc, original);
	DeleteObject(titleFont);
	return;
}

void MineSweeperGame::paintInfoBox(HDC hdc, void* dataA, void* dataB)
{
	// get address of this instance
	MineSweeperGame* thisIns = (MineSweeperGame*)dataA;
	// unpack data
	GAME_STATUS status = (GAME_STATUS)GET_LOWORD((int)dataB);
	int explored = GET_HIWORD((int)dataB);

	SetBkMode(hdc, TRANSPARENT);
	HGDIOBJ original = NULL;
	HFONT logFont = CreateFont(GAME_INFOBOX_FONT_SIZE, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, L"Consolas");
	original = SelectObject(hdc, logFont);

	RECT rectangle = { 0 };
	rectangle.left = infoBox.left;
	rectangle.top = infoBox.top;
	rectangle.right = infoBox.left + infoBox.right;
	rectangle.bottom = infoBox.top + infoBox.bottom;

	char buff[256] = { 0 };

	switch (status)
	{
	case GAME_ONGOING:
		std::sprintf(buff, "INFO: GAME ONGOING\n\nexplored cell: %d\nunexplored cell: %d\n\n< click on the cell to explore it >\n< right click to mark cell >\n< right click again to unmark >", explored, (thisIns->gameBoardInfo.row * thisIns->gameBoardInfo.col) - explored);
		break;
	case GAME_WIN:
		std::sprintf(buff, "INFO: YOU WIN!\n\n< click 'reset' to play again >");
		break;
	case GAME_LOSE:
		std::sprintf(buff, "INFO: YOU LOSE!\n\n< click 'reset' to play again >");
		break;
	default:
		return;
	}

	DrawTextA(hdc, buff, 
		strlen(buff), &rectangle, DT_LEFT);

	// tidy up
	SelectObject(hdc, original);
	DeleteObject(logFont);
	return;
}

void MineSweeperGame::paintBoard(HDC hdc, void* dataA, void* dataB)
{
	// get address of this instance
	MineSweeperGame* thisIns = (MineSweeperGame*)dataA;

	// get gameboard map
	char** mapCurrent = thisIns->m_gameLogic->getMap(MAP_CURRENT);
	char** mapReal = thisIns->m_gameLogic->getMap(MAP_REAL);

	// start drawing
	SetBkMode(hdc, TRANSPARENT);
	HGDIOBJ original = NULL;
	RECT rectangle = { 0 };
	HFONT cellFont = CreateFont(gameBoardInfo.size, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, L"Consolas");
	original = SelectObject(hdc, GetStockObject(DC_BRUSH));

	for (int i = 0; i < gameBoardInfo.row; i++)
	{
		for (int j = 0; j < gameBoardInfo.col; j++)
		{
			rectangle.left = gameBoard.left + (j * gameBoardInfo.size);
			rectangle.top = gameBoard.top + (i * gameBoardInfo.size);
			rectangle.right = gameBoard.left + ((j + 1) * gameBoardInfo.size);
			rectangle.bottom = gameBoard.top + ((i + 1) * gameBoardInfo.size);

			switch (mapCurrent[i][j])
			{
			case CELL_EMPTY_EXPLORED:
				// draw nothing
				break;
			case CELL_EMPTY:
			case CELL_USER_MARKED:
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SetDCBrushColor(hdc, CELL_EMPTY_COLOR);
				Rectangle(hdc, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
				
				if (CELL_EMPTY == mapCurrent[i][j])
					break;
			default:
				// here we have something to write
				SelectObject(hdc, cellFont);
				SetTextColor(hdc, GAME_CELL_FONT_COLOR(mapCurrent[i][j]));
				DrawTextA(hdc, &mapCurrent[i][j], 1, &rectangle, DT_CENTER);
				SetTextColor(hdc, RGB(0, 0, 0));
				break;
			}

			if (thisIns->m_gameLogic->gameFinish() 
#ifdef _DEBUG
				|| true
#endif
				)
			{
				// print mine map
				switch (mapReal[i][j])
				{
				case CELL_MINE:
					SelectObject(hdc, cellFont);
					DrawTextA(hdc, &mapReal[i][j], 1, &rectangle, DT_CENTER);
					break;
				default:
					break;
				}
			}

		}
	}
	
	// finishing off
	SelectObject(hdc, original);
	DeleteObject(cellFont);
	return;
}

void MineSweeperGame::setBoardInfo(int row, int col, int numMine)
{
	gameBoardInfo.row = row;
	gameBoardInfo.col = col;
	gameBoardInfo.numMine = numMine;
	gameBoardInfo.size = min((float)gameBoard.right / col, (float)gameBoard.bottom / row);

	return;
}

void MineSweeperGame::paintClearBuffers(void) const
{
	this->m_gameWindow->WndDrawElements(NULL, NULL, NULL, WNDDRAWTYPE_BACKBUF, true);
	this->m_gameWindow->WndDrawElements(NULL, NULL, NULL, WNDDRAWTYPE_FOREBUF, true);
	return;
}

void MineSweeperGame::paintBackground(void) const
{
	this->m_gameWindow->WndDrawElements(paintBoardBac, NULL, NULL, WNDDRAWTYPE_BACKBUF, false);
	this->m_gameWindow->WndDrawElements(paintCtrlBac, NULL, NULL, WNDDRAWTYPE_BACKBUF, false);
	return;
}

void MineSweeperGame::btn_set(void)
{
	std::cout << "Input new game parameters: (row, col, numMine)\n";
	int row = 0;
	int col = 0;
	int numMine = 0;
	std::cin >> row >> col >> numMine;

	// parameter check
	if (row < 1 || col < 1 || numMine >(row * col)) {
		std::cout << "Invalid parameters\n";
		return;
	}

	this->gameInit(row, col, numMine);
	return;
}

void MineSweeperGame::btn_reset(void)
{
	this->gameInit();
	return;
}

void MineSweeperGame::gameDelete(void)
{
	if (nullptr == this->m_gameLogic)
		return;

	delete this->m_gameLogic;
	this->m_gameLogic = nullptr;
	return;
}

void MineSweeperGame::gameInit(int row, int col, int numMine)
{
	this->gameDelete();
	setBoardInfo(row, col, numMine);
	this->m_gameLogic = new MineSweeper(this->m_msgGame, gameBoardInfo.row, gameBoardInfo.col, gameBoardInfo.numMine);

	// issue redraw call
	this->paintClearBuffers();
	this->paintBackground();
	return;
}

void MineSweeperGame::processWndMsg(void)
{
	// check if there is message to process
	if (this->m_msgQueueWnd->isEmpty())
		return;

	unsigned int data = 0;
	
	switch (this->m_msgQueueWnd->dequeue(&data))
	{
	case WNDMSGTYPE_MOUSE_L_DOWN:
	{
#ifdef _DEBUG
		std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN'\n";
#endif

		// unpack data
		int x = GET_LOWORD(data);
		int y = GET_HIWORD(data);

		if (
			(x >= gameBoard.left) &&
			(x <= (gameBoard.left + gameBoard.right)) &&
			(y >= gameBoard.top) &&
			(y <= (gameBoard.top + gameBoard.bottom))
			)
		{
#ifdef _DEBUG
			std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN' inside gameboard area\n";
#endif

			// now the click is in the game board area
			int x_to_board = x - gameBoard.left;
			int y_to_board = y - gameBoard.top;
			int x_cell = x_to_board / gameBoardInfo.size;
			int y_cell = y_to_board / gameBoardInfo.size;

			// if is a valid cell
			if ((x_cell < gameBoardInfo.col) && (y_cell < gameBoardInfo.row))
			{

#ifdef _DEBUG
				std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN' inside gameboard area is a valid cell\n";
#endif
				if ((nullptr != this->m_gameLogic) && (!this->m_gameLogic->gameFinish()))
				{
#ifdef _DEBUG
					std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN' inside gameboard area is a valid cell, issuing command"
						<< " x_cell: " << x_cell
						<< " y_cell: " << y_cell
						<< "\n";
#endif
					// if game is not finish and valid issue command
					this->m_gameLogic->exploreMap(x_cell, y_cell);
				}
			}
			return;
		}
		
		// set is pressed
		if (
			(x >= ctrlSet.left) &&
			(x <= (ctrlSet.left + ctrlSet.right)) &&
			(y >= ctrlSet.top) &&
			(y <= (ctrlSet.top + ctrlSet.bottom))
			)
		{
#ifdef _DEBUG
			std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN' inside set_btn area\n";
#endif
			this->btn_set();
			return;
		}

		// reset is pressed
		if (
			(x >= ctrlReset.left) &&
			(x <= (ctrlReset.left + ctrlReset.right)) &&
			(y >= ctrlReset.top) &&
			(y <= (ctrlReset.top + ctrlReset.bottom))
			)
		{
#ifdef _DEBUG
			std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_L_DOWN' inside reset_btn area\n";
#endif
			this->btn_reset();
			return;
		}

		break;
	}

	case WNDMSGTYPE_MOUSE_R_DOWN:
	{
#ifdef _DEBUG
		std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_R_DOWN'\n";

#endif
		// unpack data
		int x = GET_LOWORD(data);
		int y = GET_HIWORD(data);

		if (
			(x >= gameBoard.left) &&
			(x <= (gameBoard.left + gameBoard.right)) &&
			(y >= gameBoard.top) &&
			(y <= (gameBoard.top + gameBoard.bottom))
			)
		{
#ifdef _DEBUG
			std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_R_DOWN' inside gameboard area\n";
#endif

			// now the click is in the game board area
			int x_to_board = x - gameBoard.left;
			int y_to_board = y - gameBoard.top;
			int x_cell = x_to_board / gameBoardInfo.size;
			int y_cell = y_to_board / gameBoardInfo.size;

			// if is a valid cell
			if ((x_cell < gameBoardInfo.col) && (y_cell < gameBoardInfo.row))
			{

#ifdef _DEBUG
				std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_R_DOWN' inside gameboard area is a valid cell\n";
#endif
				if ((nullptr != this->m_gameLogic) && (!this->m_gameLogic->gameFinish()))
				{
#ifdef _DEBUG
					std::wcout << this << " \t 'processWndMsg' processing 'WNDMSGTYPE_MOUSE_R_DOWN' inside gameboard area is a valid cell, issuing command"
						<< " x_cell: " << x_cell
						<< " y_cell: " << y_cell
						<< "\n";
#endif
					// if game is not finish and valid issue command
					this->m_gameLogic->markCell(x_cell, y_cell);
				}
			}
			return;
		}
	}


	default:
		// no message (not recognized message)
		return;
	}
}

void MineSweeperGame::processGameMsg(void)
{
	// check if there is message to process
	if (this->m_msgGame->isEmpty())
		return;

	unsigned int data = 0;

	switch (this->m_msgGame->dequeue(&data))
	{
	case MINESWEEPER_FINISH:
#ifdef _DEBUG
		std::wcout << this << " \t 'processGameMsg' processing 'MINESWEEPER_FINISH'\n";
#endif

		return;

	case MINESWEEPER_WIN:
#ifdef _DEBUG
		std::wcout << this << " \t 'processGameMsg' processing 'MINESWEEPER_WIN'\n";
#endif
		// issue redraw call
		this->m_gameWindow->WndDrawElements(paintBoard, this, (void*)ENCODE_DWORD(GAME_WIN, NULL), WNDDRAWTYPE_FOREBUF, true);
		this->m_gameWindow->WndDrawElements(paintInfoBox, this, (void*)ENCODE_DWORD(GAME_WIN, NULL), WNDDRAWTYPE_FOREBUF, false);
		this->m_gameWindow->WndRedraw();

		return;

	case MINESWEEPER_LOSE:
#ifdef _DEBUG
		std::wcout << this << " \t 'processGameMsg' processing 'MINESWEEPER_LOSE'\n";
#endif
		// issue draw call
		this->m_gameWindow->WndDrawElements(paintBoard, this, (void*)ENCODE_DWORD(GAME_LOSE, NULL), WNDDRAWTYPE_FOREBUF, true);
		this->m_gameWindow->WndDrawElements(paintInfoBox, this, (void*)ENCODE_DWORD(GAME_LOSE, NULL), WNDDRAWTYPE_FOREBUF, false);
		this->m_gameWindow->WndRedraw();

		return;

	case MINESWEEPER_INFO:
	{
#ifdef _DEBUG
		std::wcout << this << " \t 'processGameMsg' processing 'MINESWEEPER_INFO'\n";
#endif
		// unpack data
		int explored = GET_LOWORD(data);
		int unexplored = GET_HIWORD(data);

#ifdef _DEBUG
		std::wcout << this << " \t 'processGameMsg' 'MINESWEEPER_INFO'"
			<< " explored: " << explored
			<< " unexplored: " << unexplored
			<< "\n";
#endif

		// issue redraw call
		this->m_gameWindow->WndDrawElements(paintBoard, this, (void*)ENCODE_DWORD(GAME_ONGOING, explored), WNDDRAWTYPE_FOREBUF, true);
		this->m_gameWindow->WndDrawElements(paintInfoBox, this, (void*)ENCODE_DWORD(GAME_ONGOING, explored), WNDDRAWTYPE_FOREBUF, false);
		this->m_gameWindow->WndRedraw();
		return;
	}

	default:
		return;
	}
}


void MineSweeperGame::run(void)
{
	// window init here -> init both buffers
	this->m_gameWindow->WndShow();
	this->gameInit();
	
	while (!this->m_gameWindow->WndShouldClose())
	{
		// game body here
		this->processWndMsg();
		this->processGameMsg();

		this->m_gameWindow->WndPeekMsg();

		// thread sleep -> reduce CPU comsumption -> idle
		Sleep(1);
	}

	this->gameDelete();
	return;
}


