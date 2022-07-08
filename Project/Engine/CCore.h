#pragma once

#include "CSingleton.h"

class CCore
	: public CSingleton<CCore>
{
	SINGLE(CCore);

private:
	HWND		m_hWnd;			// 메인 윈도우 핸들
	POINT		m_ptWndRes;		// 메인 윈도우 해상도

public:
	int init(HWND _hWnd, POINT _ptResolution, bool _bWindow = true);
	void progress();

public:
	HWND GetWndHwnd() { return m_hWnd; }
	POINT GetWndResolution() { return m_ptWndRes; }
};

