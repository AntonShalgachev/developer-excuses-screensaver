#include "Drawer.h"
#include "framework.h"

namespace
{
	const auto referenceHeight = 1080;
	const auto textColor = RGB(255, 255, 255);
	const auto backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	const auto outlineBrush = CreateSolidBrush(RGB(255, 0, 0));
}

Drawer::Drawer(HWND hwnd, LOGFONT fontInfo) : m_hwnd(hwnd)
{
	RECT screenRect;
	GetClientRect(m_hwnd, &screenRect);

	g_screenSize.cy = screenRect.bottom - screenRect.top;
	g_screenSize.cx = screenRect.right - screenRect.left;

	HDC hDc = GetDC(m_hwnd);
	m_bitmap = CreateCompatibleBitmap(hDc, g_screenSize.cx, g_screenSize.cy);
	m_dc = CreateCompatibleDC(hDc);
	ReleaseDC(m_hwnd, hDc);
	SelectObject(m_dc, m_bitmap);

	SetTextColor(m_dc, textColor);
	SetBkMode(m_dc, TRANSPARENT);

	fontInfo.lfHeight = fontInfo.lfHeight * g_screenSize.cy / referenceHeight;

	m_font = CreateFontIndirect(&fontInfo);
	SelectObject(m_dc, m_font);
}

Drawer::~Drawer()
{
	ReleaseDC(m_hwnd, m_dc);
	DeleteObject(m_dc);
	DeleteObject(m_bitmap);
	DeleteObject(m_font);
}

void Drawer::paint() const
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);
	BitBlt(hdc, 0, 0, g_screenSize.cx, g_screenSize.cy, m_dc, 0, 0, SRCCOPY);
	EndPaint(m_hwnd, &ps);
}

void Drawer::setText(const tstring& text)
{
	RECT screenRect;
	GetClientRect(m_hwnd, &screenRect);

	FillRect(m_dc, &screenRect, backgroundBrush);

	{
		auto quoteLength = static_cast<int>(text.length());
		auto textFlags = DT_CENTER | DT_NOCLIP | DT_WORDBREAK | DT_EXTERNALLEADING;

		auto textRect = screenRect;
		DrawText(m_dc, text.c_str(), quoteLength, &textRect, textFlags | DT_CALCRECT);

		auto textWidth = textRect.right - textRect.left;
		auto textHeight = textRect.bottom - textRect.top;
		auto offsetX = (g_screenSize.cx - textWidth) / 2;
		auto offsetY = (g_screenSize.cy - textHeight) / 2;
		textRect.left += offsetX;
		textRect.right += offsetX;
		textRect.bottom += offsetY;
		textRect.top += offsetY;
		DrawText(m_dc, text.c_str(), quoteLength, &textRect, textFlags);

		if (m_debugDraw)
			FrameRect(m_dc, &textRect, outlineBrush);
	}

	InvalidateRect(m_hwnd, &screenRect, false);
}
