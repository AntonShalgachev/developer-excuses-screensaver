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
    struct MonitorRects
    {
        std::vector<RECT> rects;

        static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
        {
            auto pThis = reinterpret_cast<MonitorRects*>(pData);
            pThis->rects.push_back(*lprcMonitor);
            return TRUE;
        }

        MonitorRects()
        {
            EnumDisplayMonitors(0, 0, MonitorEnum, reinterpret_cast<LPARAM>(this));
        }
    };

	std::unique_ptr<QuoteSource> quoteSource = nullptr;

    std::vector<std::shared_ptr<Drawer>> drawers;

	constexpr auto defaultUpdatePeriod = 7000;
	constexpr auto defaultFontSize = 43;
	constexpr auto defaultFontName = TEXT("Courier New");
	constexpr auto defaultSeparateQuote = true;

	constexpr int updateTimerId = 1;

	constexpr bool debugTextDraw = false;
	constexpr bool debugBehaveLikeWindow = false;

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
		return ConfigurationManager::Configuration{ defaultUpdatePeriod, getDefaultFontData(), defaultSeparateQuote };
	}

    ConfigurationManager configManager{ getDefaultConfiguration() };

    void fetchNewQuoteForAllDrawers()
    {
        quoteSource->fetchQuote([](const tstring& text) {
            for (const auto& drawer : drawers)
                drawer->setText(text);
        });
    }

    void fetchNewQuoteForEachDrawer()
    {
        for (const auto& drawer : drawers)
        {
            quoteSource->fetchQuote([drawer](const tstring& text) {
                drawer->setText(text);
            });
        }
    }

    void fetchNewQuote()
    {
        if (configManager.getConfiguration().separateQuote)
            fetchNewQuoteForEachDrawer();
        else
            fetchNewQuoteForAllDrawers();
    }

    const std::vector<RECT>& getMonitorRects()
    {
        static const MonitorRects monitorRects;

        return monitorRects.rects;
    }
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			if constexpr (debugBehaveLikeWindow)
			{
				RECT r;
				GetWindowRect(hWnd, &r);

				auto width = r.right - r.left;
				auto height = r.bottom - r.top;

				// to make it non-topmost
				SetWindowPos(hWnd, HWND_NOTOPMOST, r.left, r.top, width, height, 0);

				// to make it visible in the taskbar
				auto exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
				SetWindowLong(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
			}

            for (const auto& monitorRect : getMonitorRects())
            {
                auto drawer = std::make_shared<Drawer>(hWnd, deserializeFontDescription(configManager.getConfiguration().fontData), monitorRect);
                drawer->setDebugDraw(debugTextDraw);
                drawers.push_back(std::move(drawer));
            }

			quoteSource = std::make_unique<QuoteSource>();

			SetTimer(hWnd, updateTimerId, configManager.getConfiguration().timerPeriod, nullptr);
			fetchNewQuote();

			break;
		case WM_DESTROY:
            drawers.clear();
			quoteSource = nullptr;
			KillTimer(hWnd, updateTimerId);
			break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            for (const auto& drawer : drawers)
                drawer->paint(hdc);

            EndPaint(hWnd, &ps);
            break;
        }
		case WM_TIMER:
			if (wParam == updateTimerId)
				fetchNewQuote();
			break;

		case WM_ACTIVATEAPP:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if constexpr (debugBehaveLikeWindow)
				return DefWindowProc(hWnd, message, wParam, lParam);
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

            auto separateQuote = configManager.getConfiguration().separateQuote;

            SendMessage(GetDlgItem(hDlg, IDC_CHECK_SEPARATE_QUOTE), BM_SETCHECK, separateQuote ? BST_CHECKED : BST_UNCHECKED, 0);

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

                auto separateQuote = SendMessage(GetDlgItem(hDlg, IDC_CHECK_SEPARATE_QUOTE), BM_GETCHECK, NULL, NULL) == BST_CHECKED;

				configManager.getConfiguration().timerPeriod = timerPeriod;
                configManager.getConfiguration().separateQuote = separateQuote;
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
