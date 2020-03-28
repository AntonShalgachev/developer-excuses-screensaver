// DeveloperExcusesScreensaver.cpp : Defines the entry point for the application.
//

#include "DeveloperExcusesScreensaver.h"
#include "framework.h"
#include "resource.h"

namespace
{
	static HDC g_hDC = nullptr;
	static HBITMAP g_hBmp = nullptr;
	static HFONT g_quoteFont = nullptr;

	static SIZE g_screenSize;

	static const int s_updateTimerId = 1;

	void UpdateFrame(HWND hWnd);

	void Init(HWND hWnd)
	{
		//LoadConfiguration();
		RECT screenRect;
		GetClientRect(hWnd, &screenRect);

		g_screenSize.cy = screenRect.bottom - screenRect.top;
		g_screenSize.cx = screenRect.right - screenRect.left;

		HDC hDc = GetDC(hWnd);
		g_hBmp = CreateCompatibleBitmap(hDc, g_screenSize.cx, g_screenSize.cy);
		g_hDC = CreateCompatibleDC(hDc);
		ReleaseDC(hWnd, hDc);
		SelectObject(g_hDC, g_hBmp);

		SetTextColor(g_hDC, RGB(255, 255, 255));
		SetBkMode(g_hDC, TRANSPARENT);

		{
			LOGFONT lf;
			ZeroMemory(&lf, sizeof(lf));
			wcscpy_s(lf.lfFaceName, TEXT("Consolas"));
			lf.lfQuality = ANTIALIASED_QUALITY;
			lf.lfHeight = g_screenSize.cy / 25;

			g_quoteFont = CreateFontIndirect(&lf);

			SelectObject(g_hDC, g_quoteFont);
		}

		SetTimer(hWnd, s_updateTimerId, 1000, nullptr);
		UpdateFrame(hWnd);
	}

	void UpdateFrame(HWND hWnd)
	{
		RECT screenRect;
		GetClientRect(hWnd, &screenRect);

		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(g_hDC, &screenRect, brush);

		{
			auto quoteText = tstring(TEXT("Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote Test Quote"));
			auto quoteLength = static_cast<int>(quoteText.length());

			auto textRect = screenRect;
			DrawText(g_hDC, quoteText.c_str(), quoteLength, &textRect, DT_CENTER | DT_NOCLIP | DT_WORDBREAK | DT_EXTERNALLEADING | DT_CALCRECT);

			auto textWidth = textRect.right - textRect.left;
			auto textHeight = textRect.bottom - textRect.top;
			auto offsetX = (g_screenSize.cx - textWidth) / 2;
			auto offsetY = (g_screenSize.cy - textHeight) / 2;
			textRect.left += offsetX;
			textRect.right += offsetX;
			textRect.bottom += offsetY;
			textRect.top += offsetY;
			DrawText(g_hDC, quoteText.c_str(), quoteLength, &textRect, DT_CENTER | DT_NOCLIP | DT_WORDBREAK | DT_EXTERNALLEADING);

			FrameRect(g_hDC, &textRect, CreateSolidBrush(RGB(255, 0, 0)));
		}

		InvalidateRect(hWnd, &screenRect, false);
	}
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			Init(hWnd);
			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hWnd, s_updateTimerId);
			ReleaseDC(hWnd, g_hDC);
			DeleteObject(g_hDC);
			DeleteObject(g_hBmp);
			DeleteObject(g_quoteFont);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, 0, 0, g_screenSize.cx, g_screenSize.cy, g_hDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			return 0;
			break;
		}
		case WM_TIMER:
		{
			if (wParam == s_updateTimerId)
				UpdateFrame(hWnd);
			break;
		}
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			//LoadConfiguration();
			//CheckDlgButton(hDlg, IDC_24HOUR_CHECK, g_is24Hour);
			//CheckDlgButton(hDlg, IDC_ANIMATEHANDS_CHECK, g_AnimatedClockHands);
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
			{
				//TCHAR szBuffer[8];

				//g_is24Hour = IsDlgButtonChecked(hDlg, IDC_24HOUR_CHECK);
				//_itow_s(g_is24Hour, szBuffer, 10);
				//WritePrivateProfileString(szAppName, szFormatOptionName, szBuffer, szIniFile);

				//g_AnimatedClockHands = IsDlgButtonChecked(hDlg, IDC_ANIMATEHANDS_CHECK);
				//_itow_s(g_AnimatedClockHands, szBuffer, 10);
				//WritePrivateProfileString(szAppName, szClockHandOptionName, szBuffer, szIniFile);
			}
			case IDCANCEL:
			{
				EndDialog(hDlg, LOWORD(wParam) == IDOK);
				return TRUE;
			}
			}
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE)
{
	return TRUE;
}
