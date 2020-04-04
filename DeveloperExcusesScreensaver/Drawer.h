#pragma once

#include "framework.h"
#include "tstring.h"

class Drawer
{
public:
	Drawer(HWND hwnd, LOGFONT fontInfo);
	~Drawer();
	void paint() const;
	void setText(const tstring& text);

	void setDebugDraw(bool debugDraw) { m_debugDraw = debugDraw; }

private:
	HWND m_hwnd = 0;
	HDC m_dc = 0;
	HBITMAP m_bitmap = 0;
	HFONT m_font = 0;

	SIZE g_screenSize = {0, 0};

	bool m_debugDraw = false;
};
