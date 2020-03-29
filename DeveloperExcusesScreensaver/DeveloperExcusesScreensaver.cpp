// DeveloperExcusesScreensaver.cpp : Defines the entry point for the application.
//

#include "DeveloperExcusesScreensaver.h"
#include "framework.h"
#include "Drawer.h"
#include "QuoteSource.h"

namespace
{
	std::unique_ptr<QuoteSource> quoteSource = nullptr;
	std::unique_ptr<Drawer> drawer = nullptr;

	// TODO extract to config dialog
	const int updateTimerPeriod = 5000;
	const int fontSize = 43;

	const bool debugDraw = false;
	const int updateTimerId = 1;

	void fetchNewQuote()
	{
		quoteSource->fetchQuote([](const tstring& text) {
			drawer->setText(text);
		});
	}
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			drawer = std::make_unique<Drawer>(hWnd, fontSize);
			quoteSource = std::make_unique<QuoteSource>();

			drawer->setDebugDraw(debugDraw);

			SetTimer(hWnd, updateTimerId, updateTimerPeriod, nullptr);
			fetchNewQuote();

			break;
		case WM_DESTROY:
			drawer = nullptr;
			quoteSource = nullptr;
			KillTimer(hWnd, updateTimerId);
			break;
		case WM_PAINT:
			drawer->paint();
			break;
		case WM_TIMER:
			if (wParam == updateTimerId)
				fetchNewQuote();
			break;
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
