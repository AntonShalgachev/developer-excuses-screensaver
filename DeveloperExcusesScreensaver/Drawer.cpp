#include "Drawer.h"
#include "framework.h"

namespace
{
	const auto referenceHeight = 1080;
	const auto textColor = RGB(255, 255, 255);
	const auto backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	const auto outlineBrush = CreateSolidBrush(RGB(255, 0, 0));
}

Drawer::Drawer(HWND hwnd, LOGFONT fontInfo, RECT monitorRect) : m_hwnd(hwnd)
{
    RECT windowRect;
    GetWindowRect(m_hwnd, &windowRect);

    IntersectRect(&m_worldPaintRect, &monitorRect, &windowRect);

    m_paintAreaSize.cx = m_worldPaintRect.right - m_worldPaintRect.left;
	m_paintAreaSize.cy = m_worldPaintRect.bottom - m_worldPaintRect.top;

    m_localDcPaintRect = { 0, 0, m_paintAreaSize.cx, m_paintAreaSize.cy };

    LONG windowPaintLeft = m_worldPaintRect.left - windowRect.left;
    LONG windowPaintTop = m_worldPaintRect.top - windowRect.top;
    m_windowPaintRect = { windowPaintLeft, windowPaintTop, windowPaintLeft + m_paintAreaSize.cx, windowPaintTop + m_paintAreaSize.cy };

	HDC hDc = GetDC(m_hwnd);
	m_localBitmap = CreateCompatibleBitmap(hDc, m_paintAreaSize.cx, m_paintAreaSize.cy);
	m_localDc = CreateCompatibleDC(hDc);
	ReleaseDC(m_hwnd, hDc);
	SelectObject(m_localDc, m_localBitmap);

	SetTextColor(m_localDc, textColor);
	SetBkMode(m_localDc, TRANSPARENT);

	fontInfo.lfHeight = fontInfo.lfHeight * m_paintAreaSize.cy / referenceHeight;

	m_font = CreateFontIndirect(&fontInfo);
	SelectObject(m_localDc, m_font);
}

Drawer::~Drawer()
{
	ReleaseDC(m_hwnd, m_localDc);
	DeleteObject(m_localDc);
	DeleteObject(m_localBitmap);
	DeleteObject(m_font);
}

void Drawer::paint(HDC hdc) const
{
	BitBlt(hdc, m_windowPaintRect.left, m_windowPaintRect.top, m_paintAreaSize.cx, m_paintAreaSize.cy, m_localDc, 0, 0, SRCCOPY);
}

void Drawer::setText(const tstring& text)
{
	FillRect(m_localDc, &m_localDcPaintRect, backgroundBrush);

    {
		auto quoteLength = static_cast<int>(text.length());
		auto textFlags = DT_CENTER | DT_NOCLIP | DT_WORDBREAK | DT_EXTERNALLEADING;

		auto textRect = m_localDcPaintRect;
		DrawText(m_localDc, text.c_str(), quoteLength, &textRect, textFlags | DT_CALCRECT);

		auto textWidth = textRect.right - textRect.left;
		auto textHeight = textRect.bottom - textRect.top;
		auto offsetX = (m_paintAreaSize.cx - textWidth) / 2;
		auto offsetY = (m_paintAreaSize.cy - textHeight) / 2;
		textRect.left += offsetX;
		textRect.right += offsetX;
		textRect.bottom += offsetY;
		textRect.top += offsetY;

		DrawText(m_localDc, text.c_str(), quoteLength, &textRect, textFlags);

		if (m_debugDraw)
			FrameRect(m_localDc, &textRect, outlineBrush);
	}

	InvalidateRect(m_hwnd, &m_windowPaintRect, false);
}
