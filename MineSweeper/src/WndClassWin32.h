#pragma once
#ifndef _WNDCLASSWIN32_H_
#define _WNDCLASSWIN32_H_

#include <Windows.h>
#include "LL_Queue.h"

enum WndDrawType
{
	// enum -> draw buffer type
	WNDDRAWTYPE_BACKBUF,	// background buffer
	WNDDRAWTYPE_FOREBUF		// foreground buffer
};

enum WndMsgType
{
	WNDMSGTYPE_MOUSE_L_DOWN = 100,
	WNDMSGTYPE_MOUSE_R_DOWN = 101
};

class WndClassWin32
{
private:
	HINSTANCE			m_hIns;						// handle to current Windows program instance
	HWND				m_hWnd;						// handle to this Windows window
	const WCHAR*		m_className;				// pointer to the class name WCHAR
	HBITMAP				m_hBitBufBackgroud;			// handle of Background bitmap buffer
	HBITMAP				m_hBitBufForeground;		// handle of Foreground bitmap buffer
	HBITMAP				m_hBitBuf;					// handle of back buffer of the screen
	RECT				m_clientWndRect;			// dimensions of current Windows window
	bool				m_wndShouldClose;			// bool flag for termination

	LL_Queue*			m_ptrLLQueue;				// pointer to Queue

public:
	// constructor (window class name, window width, window height)
	WndClassWin32(const WCHAR* className, UINT wndWidth, UINT wndHeight, LL_Queue* msgQueue);
	// destructor
	~WndClassWin32(void);

private:
	// static function -> window message handler
	static LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	// static function -> peek message from system message queue
	// this method does NOT block (immediately returns)
	static bool WndPeekMsg(void);

public:
	// member function -> show Windows window
	bool WndShow(void) const;
	// member function -> redraw client area
	bool WndRedraw(void) const;
	// member function -> draws on buffers
	bool WndDrawElements(void (*drawFunction)(HDC hdc, void* dataX, void* dataY), void* dataX, void* dataY, WndDrawType drawBufType, bool clear) const;
	// inline function -> return should close status
	bool WndShouldClose(void) { return this->m_wndShouldClose; }
};

#endif
