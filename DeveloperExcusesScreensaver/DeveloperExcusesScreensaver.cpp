// DeveloperExcusesScreensaver.cpp : Defines the entry point for the application.
//

#include "DeveloperExcusesScreensaver.h"
#include "framework.h"
#include "Drawer.h"
#include "QuoteSource.h"
#include "ConfigurationManager.h"
#include "resource.h"
#include "tstring.h"

namespace
{
	std::unique_ptr<QuoteSource> quoteSource = nullptr;
	std::unique_ptr<Drawer> drawer = nullptr;

	const auto defaultUpdatePeriod = 7000;
	const auto defaultFontSize = 43;
	const auto defaultFontName = TEXT("Courier New");

	const bool debugDraw = false;
	const int updateTimerId = 1;

	void fetchNewQuote()
	{
		quoteSource->fetchQuote([](const tstring& text) {
			drawer->setText(text);
		});
	}

	std::vector<unsigned char> serializeFontDescription(const LOGFONT& logFont)
	{
		auto dataSize = sizeof(logFont);

		std::vector<unsigned char> fontData;
		fontData.resize(dataSize);
		CopyMemory(fontData.data(), &logFont, dataSize);

		return fontData;
	}

	LOGFONT deserializeFontDescription(const std::vector<unsigned char>& serializedFontDescription)
	{
		LOGFONT logFont;
		CopyMemory(&logFont, serializedFontDescription.data(), sizeof(logFont));
		return logFont;
	}

	std::vector<unsigned char> getDefaultFontData()
	{
		LOGFONT logFont;

		ZeroMemory(&logFont, sizeof(logFont));
		wcscpy_s(logFont.lfFaceName, defaultFontName);
		logFont.lfQuality = ANTIALIASED_QUALITY;
		logFont.lfHeight = defaultFontSize;

		return serializeFontDescription(logFont);
	}

	ConfigurationManager::Configuration getDefaultConfiguration()
	{
		return ConfigurationManager::Configuration{ defaultUpdatePeriod, getDefaultFontData() };
	}

	ConfigurationManager configManager{ getDefaultConfiguration() };
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			drawer = std::make_unique<Drawer>(hWnd, deserializeFontDescription(configManager.getConfiguration().fontData));
			quoteSource = std::make_unique<QuoteSource>();

			drawer->setDebugDraw(debugDraw);

			SetTimer(hWnd, updateTimerId, configManager.getConfiguration().timerPeriod, nullptr);
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
			auto timerPeriod = configManager.getConfiguration().timerPeriod;
			auto timerPeriodText = to_tstring(timerPeriod); // string is null-terminated in C++11
			SetDlgItemText(hDlg, IDC_TIMER_PERIOD_EDIT, timerPeriodText.c_str());

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
			{
				auto timerPeriodEdit = GetDlgItem(hDlg, IDC_TIMER_PERIOD_EDIT);
				auto timerPeriodStringLength = GetWindowTextLength(timerPeriodEdit);
				auto timerPeriodString = std::vector<TCHAR>(timerPeriodStringLength + 1);
				GetWindowText(timerPeriodEdit, timerPeriodString.data(), static_cast<DWORD>(timerPeriodString.size()));
				auto timerPeriod = StrToInt(timerPeriodString.data());

				configManager.getConfiguration().timerPeriod = timerPeriod;
				configManager.save();

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			case IDCANCEL:
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			case IDC_CHOOSE_FONT:
			{
				auto logFont = deserializeFontDescription(configManager.getConfiguration().fontData);

				CHOOSEFONT chooseFont;
				ZeroMemory(&chooseFont, sizeof(chooseFont));
				chooseFont.lStructSize = sizeof(chooseFont);
				chooseFont.hwndOwner = hDlg;
				chooseFont.lpLogFont = &logFont;
				chooseFont.iPointSize = 120;
				chooseFont.Flags = CF_BOTH | CF_EFFECTS | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT;
				
				ChooseFont(&chooseFont);
				configManager.getConfiguration().fontData = serializeFontDescription(logFont);

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
