#pragma once

#include "framework.h"

class Drawer
{
public:
	Drawer(HWND hwnd);
	~Drawer();
	void paint();
	void setText(const tstring& text);

private:
	HWND m_hwnd = 0;
	HDC m_dc = 0;
	HBITMAP m_bitmap = 0;
	HFONT m_font = 0;

	SIZE g_screenSize = {0, 0};
};

