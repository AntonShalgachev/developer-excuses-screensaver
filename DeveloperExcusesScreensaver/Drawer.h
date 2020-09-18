#pragma once

#include "framework.h"
#include "tstring.h"

class Drawer
{
public:
	Drawer(HWND hwnd, LOGFONT fontInfo, RECT monitorRect);
	~Drawer();
	void paint(HDC hdc) const;
	void setText(const tstring& text);

	void setDebugDraw(bool debugDraw) { m_debugDraw = debugDraw; }

private:
	HWND m_hwnd = 0;
	HDC m_localDc = 0;
	HBITMAP m_localBitmap = 0;
	HFONT m_font = 0;

    RECT m_windowPaintRect = { 0, 0, 0, 0 };
    RECT m_worldPaintRect = { 0, 0, 0, 0 };
    RECT m_localDcPaintRect = { 0, 0, 0, 0 };
    SIZE m_paintAreaSize = { 0, 0 };

	bool m_debugDraw = false;
};
