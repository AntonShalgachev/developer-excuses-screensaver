#include "Drawer.h"
#include "framework.h"

namespace
{
	HFONT CreateQuoteFont(const tstring& fontName, LONG height)
	{
		LOGFONT fontInfo;
		ZeroMemory(&fontInfo, sizeof(fontInfo));
		wcscpy_s(fontInfo.lfFaceName, fontName.c_str());
		fontInfo.lfQuality = ANTIALIASED_QUALITY;
		fontInfo.lfHeight = height;

		return CreateFontIndirect(&fontInfo);
	}
}

Drawer::Drawer(HWND hwnd) : m_hwnd(hwnd)
{
	//LoadConfiguration();
	RECT screenRect;
	GetClientRect(m_hwnd, &screenRect);

	g_screenSize.cy = screenRect.bottom - screenRect.top;
	g_screenSize.cx = screenRect.right - screenRect.left;

	HDC hDc = GetDC(m_hwnd);
	m_bitmap = CreateCompatibleBitmap(hDc, g_screenSize.cx, g_screenSize.cy);
	m_dc = CreateCompatibleDC(hDc);
	ReleaseDC(m_hwnd, hDc);
	SelectObject(m_dc, m_bitmap);

	SetTextColor(m_dc, RGB(255, 255, 255));
	SetBkMode(m_dc, TRANSPARENT);

	m_font = CreateQuoteFont(TEXT("Consolas"), g_screenSize.cy / 25);
	SelectObject(m_dc, m_font);
}

Drawer::~Drawer()
{
	ReleaseDC(m_hwnd, m_dc);
	DeleteObject(m_dc);
	DeleteObject(m_bitmap);
	DeleteObject(m_font);
}

void Drawer::paint()
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

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(m_dc, &screenRect, brush);

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

		FrameRect(m_dc, &textRect, CreateSolidBrush(RGB(255, 0, 0)));
	}

	InvalidateRect(m_hwnd, &screenRect, false);
}
