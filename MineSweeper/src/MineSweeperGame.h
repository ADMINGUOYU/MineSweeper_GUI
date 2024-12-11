#pragma once
#ifndef _MINESWEEPERGAME_H_
#define _MINESWEEPERGAME_H_

#include <Windows.h>
#include "LL_Queue.h"
#include "WndClassWin32.h"
#include "MineSweeper.h"

#define WND_RES_X 960
#define WND_RES_Y 540
#define WND_VALID_RATIO 0.90
#define WND_BOARD_RATIO 0.9
#define WND_TITLE_HEIGHT 35
#define WND_BTN_HEIGHT 35
#define WND_PADDING_X_RATIO 0.35
#define WND_PADDING_X 25
#define WND_PADDING_Y 25

#define GAME_BOARD_ROW 7
#define GAME_BOARD_COL 8
#define GAME_BOARD_MINE 10

#define TITLE L"MineSweeper GUI"
#define BTN_SET_TEXT L"SET"
#define BTN_RESET_TEXT L"RESET"

#define GAME_BOARD_BACK_COLOR RGB(255, 255, 255)
#define CELL_EMPTY_COLOR RGB(200, 200, 200)
#define GAME_BTN_SET_COLOR RGB(3, 252, 219)
#define GAME_BTN_RESET_COLOR RGB(249, 138, 255)
#define GAME_INFOBOX_COLOR RGB(250, 250, 250)

#define GAME_CELL_FONT_COLOR(c) \
	( c == '1' ? RGB(44, 60, 229) \
	: c == '2' ? RGB(44, 223, 89) \
	: c == '3' ? RGB(254, 77, 23) \
	: c == '4' ? RGB(0, 65, 180) \
	: RGB(200, 2, 0))

#define GAME_INFOBOX_FONT_SIZE 20

// for data transmit
//#define GET_LOPTR(p) (INT32)((INT64)p & 0xffffffff)
//#define GET_HIPTR(p) (INT32)(((INT64)p >> 32))
//#define ENCODE_PTR(l,h) (((INT64)l | ((INT64)h << 32)))
#define ENCODE_DWORD(l,h) (l | (h << 16))
#define GET_LOWORD(l) ((WORD)(l) & 0xffff)
#define GET_HIWORD(h) ((WORD)(h >> 16) & 0xffff)

enum GAME_STATUS
{
	GAME_ONGOING = 0,
	GAME_WIN,
	GAME_LOSE
};

class MineSweeperGame
{
private:
	struct boardINFO {
		int row;
		int col;
		int numMine;
		float size;
	};

private:
	WndClassWin32* m_gameWindow;
	LL_Queue* m_msgQueueWnd;
	LL_Queue* m_msgGame;
	MineSweeper* m_gameLogic;

	static RECT valid;
	static RECT titleBar;
	static RECT gameBoard;
	static RECT ctrlSet;
	static RECT ctrlReset;
	static RECT infoBox;

	static boardINFO gameBoardInfo;

private:
	static void paintBoardBac(HDC hdc, void* posX, void* posY);
	static void paintCtrlBac(HDC hdc, void* posX, void* posY);
	static void paintInfoBox(HDC hdc, void* dataA, void* dataB);
	static void paintBoard(HDC hdc, void* dataA, void* dataB);
	static void setBoardInfo(int row, int col, int numMine);

private:
	void paintClearBuffers(void) const;
	void paintBackground(void) const;

private:
	void btn_set(void);
	void btn_reset(void);

private:
	void gameDelete(void);
	void gameInit(int row = gameBoardInfo.row, int col = gameBoardInfo.col, int numMine = gameBoardInfo.numMine);
	void processWndMsg(void);
	void processGameMsg(void);

public:
	void run(void);

public:
	MineSweeperGame(void);
	~MineSweeperGame(void);


};

#endif
