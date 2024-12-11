#include "WndClassWin32.h"
#include <iostream>

LRESULT WINAPI WndClassWin32::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	WndClassWin32* thisIns = (WndClassWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (Msg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		thisIns = (WndClassWin32*)(createStruct->lpCreateParams);

#ifdef _DEBUG
		std::wcout << thisIns << " \t'WndProc' processing message WM_CREATE\n";
#endif
		GetWindowRect(hWnd, &thisIns->m_clientWndRect);
#ifdef _DEBUG
		std::wcout << thisIns << " \t'WndProc' window dimension" 
			<< " width: " << thisIns->m_clientWndRect.right
			<< " height: " << thisIns->m_clientWndRect.bottom
			<< "\n";
#endif
	}
		return 0;

	case WM_SIZE:
//#ifdef _DEBUG
//		std::wcout << thisIns << " \t'WndProc' processing message WM_SIZE\n";
//#endif
		thisIns->m_clientWndRect.right = LOWORD(lParam);
		thisIns->m_clientWndRect.bottom = HIWORD(lParam);
//#ifdef _DEBUG
//		std::wcout << thisIns << " \t'WndProc' window new dimension"
//			<< " width: " << thisIns->m_clientWndRect.right
//			<< " height: " << thisIns->m_clientWndRect.bottom
//			<< "\n";
//#endif
		return 0;

	case WM_PAINT:
//#ifdef _DEBUG
//		std::wcout << thisIns << " \t'WndProc' processing message WM_PAINT\n";
//#endif
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdcScreen = BeginPaint(thisIns->m_hWnd, &ps);
		HDC hdcDrawer = CreateCompatibleDC(hdcScreen);
		HBITMAP hbmpOldDrawer = (HBITMAP)SelectObject(hdcDrawer, thisIns->m_hBitBuf);

		// paint to screen
		BitBlt(hdcScreen, 0, 0, 
			thisIns->m_clientWndRect.right, thisIns->m_clientWndRect.bottom,
			hdcDrawer, 0, 0,
			SRCCOPY);

		SelectObject(hdcDrawer, hbmpOldDrawer);
		DeleteObject(hdcDrawer);
		EndPaint(thisIns->m_hWnd, &ps);
	}
		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
#ifdef _DEBUG
		std::wcout << thisIns << " \t'WndProc' processing message WM_LBUTTONDOWN or WM_RBUTTONDOWN\n";
#endif
	{
		unsigned int data = 0x0000;
		WORD x = LOWORD(lParam);
		WORD y = HIWORD(lParam);

		data = ENCODE_DWORD(x, y);
		if (Msg == WM_LBUTTONDOWN)
			thisIns->m_ptrLLQueue->enqueue(data, WNDMSGTYPE_MOUSE_L_DOWN);
		if (Msg == WM_RBUTTONDOWN)
			thisIns->m_ptrLLQueue->enqueue(data, WNDMSGTYPE_MOUSE_R_DOWN);
#ifdef _DEBUG
		std::wcout << thisIns << " \t'WndProc' enqueue"
			<< " posX: " << x
			<< " posY: " << y
			<< " data: " << data
			<< "\n";
#endif
		return 0;
	}

	case WM_CLOSE:
#ifdef _DEBUG
		std::wcout << thisIns << " \t'WndProc' processing message WM_CLOSE\n";
#endif
		// destroy window
		DestroyWindow(hWnd);
		// set window should close flag
		thisIns->m_wndShouldClose = true;
		return 0;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return LRESULT();
}

bool WndClassWin32::WndPeekMsg(void)
{
	MSG msg = {0};
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}
	return false;
}

bool WndClassWin32::WndShow(void) const
{
	// show window
	if (NULL == this->m_hWnd)
	{
#ifdef _DEBUG
		std::wcout << this << " \t'WndShow' failed (hWnd is NULL pointer)\n";
#endif
		return false;
	}
	return ShowWindow(this->m_hWnd, SW_SHOW);
}

bool WndClassWin32::WndRedraw(void) const
{
	return InvalidateRect(this->m_hWnd, NULL, FALSE);	// use FALSE to not redraw background color
}

bool WndClassWin32::WndDrawElements(void (*drawFunction)(HDC hdc, void* dataX, void* dataY), void* dataX, void* dataY, WndDrawType drawBufType, bool clear) const
{
#ifdef _DEBUG
	std::wcout << this << " \t'WndDrawElements' called\n";
#endif
	if (this->m_wndShouldClose)
	{
#ifdef _DEBUG
		std::wcout << this << " \t'WndDrawElements' failed window should close\n";
		return false;
#endif
	}

	// prepare device context
	HDC hdcScreen = GetDC(this->m_hWnd);
	HDC hdcDrawer = CreateCompatibleDC(hdcScreen);
	HDC hdcMixer = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmpOldDrawer = NULL;
	HBITMAP hbmpOldMixer = NULL;

	// bind bitmap to dc
	switch (drawBufType)
	{
	case WNDDRAWTYPE_BACKBUF:
		hbmpOldDrawer = (HBITMAP)SelectObject(hdcDrawer, this->m_hBitBufBackgroud);
		break;
	case WNDDRAWTYPE_FOREBUF:
		hbmpOldDrawer = (HBITMAP)SelectObject(hdcDrawer, this->m_hBitBufForeground);
		break;
	default:
#ifdef _DEBUG
		std::wcout << this << " \t'WndDrawElements' invalid draw buffer type\n";
#endif
		return false;
	}

	// check whether to clear buffer
	if (clear) {
		RECT fillRect = this->m_clientWndRect;
		fillRect.left = 0;
		fillRect.top = 0;
		FillRect(hdcDrawer, &fillRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	// call draw function (if there is one)
	if (NULL != drawFunction)
	{
#ifdef _DEBUG
		std::wcout << this << " \t'WndDrawElements' calling draw function\n";
#endif
		(*drawFunction)(hdcDrawer, dataX, dataY);
#ifdef _DEBUG
		std::wcout << this << " \t'WndDrawElements' draw function ends\n";
#endif
	}

	// prepare mixer
	hbmpOldMixer = (HBITMAP)SelectObject(hdcMixer, this->m_hBitBuf);

	// mix bitmaps
	SelectObject(hdcDrawer, this->m_hBitBufBackgroud);
	BitBlt(hdcMixer, 0, 0, this->m_clientWndRect.right, this->m_clientWndRect.bottom,
		hdcDrawer, 0, 0, SRCCOPY);
	SelectObject(hdcDrawer, this->m_hBitBufForeground);
	TransparentBlt(hdcMixer, 0, 0, this->m_clientWndRect.right, this->m_clientWndRect.bottom,
		hdcDrawer, 0, 0, this->m_clientWndRect.right, this->m_clientWndRect.bottom,
		RGB(255, 255, 255));

	// unbind bitmap buffer
	SelectObject(hdcDrawer, hbmpOldDrawer);
	SelectObject(hdcMixer, hbmpOldMixer);
	DeleteObject(hdcDrawer);
	DeleteObject(hdcMixer);
	ReleaseDC(this->m_hWnd, hdcScreen);

#ifdef _DEBUG
	std::wcout << this << " \t'WndDrawElements' ends\n";
#endif
	return true;
}

WndClassWin32::WndClassWin32(const WCHAR* className, UINT wndWidth, UINT wndHeight, LL_Queue* msgQueue)
	:
	// init member variables (using constructors)
	m_className(className),
	m_hIns(NULL),
	m_hWnd(NULL),
	m_hBitBufBackgroud(NULL),
	m_hBitBufForeground(NULL),
	m_hBitBuf(NULL),
	m_clientWndRect(),
	m_wndShouldClose(false),
	m_ptrLLQueue(msgQueue)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' constructor being called\n";
#endif // debug message (you will see this message printed when Visual Studio is in DEBUG config)
	
	// check init
	if ((NULL == this->m_className) || (NULL == this->m_ptrLLQueue))
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' class name or message queue not valid\n";
#endif
		this->m_wndShouldClose = true;
		return;
	}

	// set up buffer and client RECT
	this->m_clientWndRect.left = 0;
	this->m_clientWndRect.top = 0;
	this->m_clientWndRect.right = wndWidth;
	this->m_clientWndRect.bottom = wndHeight;

	// retrieve hIns of this program
	if (NULL == (this->m_hIns = GetModuleHandle(NULL)))
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' constructor failed to retrieve handle of this program\n";
#endif
		this->m_wndShouldClose = true;
		return;
	}
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' retrieved hIns " << this->m_hIns << "\n";
#endif

	//  register window class
	WNDCLASS wc = { 0 };
	wc.lpszClassName = this->m_className;
	wc.hInstance = this->m_hIns;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);

	if (NULL == RegisterClass(&wc))
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' constructor failed to register window class\n";
#endif
		this->m_wndShouldClose = true;
		return;
	}

	// calculate window rect (adjust)
	AdjustWindowRectEx(&this->m_clientWndRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	this->m_clientWndRect.right -= this->m_clientWndRect.left;
	this->m_clientWndRect.bottom -= this->m_clientWndRect.top;

	// create window
	this->m_hWnd = CreateWindowEx(
		NULL,							// dwExStyle: extended window style
		this->m_className,				// lpClassName: long pointer to window class name
		this->m_className,				// lpWindowName: long pointer to window name
		WS_OVERLAPPED | WS_SYSMENU,		// dwStyle: window style
		CW_USEDEFAULT,					// X: X location of window
		CW_USEDEFAULT,					// Y: Y location of window
		this->m_clientWndRect.right,	// nWidth: width of the window
		this->m_clientWndRect.bottom,	// nHeight: height of the window
		NULL,							// hWndParent: handle to parent window
		NULL,							// hMenu: handle to menu bar (resource)
		this->m_hIns,					// hInstance: handle of current program instance
		this							// lpParam: additional data passed when WM_CREATE is generated
	);
	if (NULL == this->m_hWnd)
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' constructor failed to create window\n";
#endif
		this->m_wndShouldClose = true;
		return;
	}
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' window created hWnd " << this->m_hWnd << "\n";
#endif

	// load instance address to the window user data
	SetWindowLongPtr(this->m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	// creates bitmap buffers
	HDC hdcScreen = GetDC(this->m_hWnd);
	this->m_hBitBufBackgroud = CreateCompatibleBitmap(hdcScreen, wndWidth, wndHeight);
	this->m_hBitBufForeground = CreateCompatibleBitmap(hdcScreen, wndWidth, wndHeight);
	this->m_hBitBuf = CreateCompatibleBitmap(hdcScreen, wndWidth, wndHeight);
	ReleaseDC(this->m_hWnd, hdcScreen);
	if ((NULL == this->m_hBitBufBackgroud) || (NULL == this->m_hBitBufForeground) || (NULL == this->m_hBitBuf))
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' failed to create bitmap buffers\n";
#endif
		this->m_wndShouldClose = true;
		return;
	}

	// constructor finished
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' constructor execution completes\n";
#endif
	return;
}

WndClassWin32::~WndClassWin32(void)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' destructor being called\n";
#endif


	// delete bitmap buffers
	if (NULL != this->m_hBitBufBackgroud)
	{
		if (!DeleteObject(this->m_hBitBufBackgroud))
		{
#ifdef _DEBUG
			std::wcout << this << " CLASS: 'WndClassWin32' failed to delete m_hBitBufBackgroud\n";
#endif
		}

	}
	if (NULL != this->m_hBitBufForeground)
	{
		if (!DeleteObject(this->m_hBitBufForeground))
		{
#ifdef _DEBUG
			std::wcout << this << " CLASS: 'WndClassWin32' failed to delete m_hBitBufForeground\n";
#endif
		}

	}
	if (NULL != this->m_hBitBuf)
	{
		if (!DeleteObject(this->m_hBitBuf))
		{
#ifdef _DEBUG
			std::wcout << this << " CLASS: 'WndClassWin32' failed to delete m_hBitBuf\n";
#endif
		}

	}
	
	// unregister window class
	if (!UnregisterClass(this->m_className, this->m_hIns))
	{
#ifdef _DEBUG
		std::wcout << this << " CLASS: 'WndClassWin32' failed to unregister window class\n";
#endif
	}

	// destructor finished
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'WndClassWin32' destructor execution completes\n";
#endif
	return;
}



