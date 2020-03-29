#include "QuoteSource.h"
#include "framework.h"

namespace
{
	const auto url = tstring(TEXT("http://developerexcuses.com/"));

	const auto regex = tregex(TEXT("<a href=\"/\" rel=\"nofollow\" style=\"text-decoration: none; color: #333;\">(.+)</a>"));

	tstring findQuote(const tstring& html)
	{
		std::wsmatch match;
		
		while (std::regex_search(html, match, regex))
		{
			if (match.size() < 2)
				continue;
			return match[1];
		}

		return TEXT("");
	}
}

void QuoteSource::fetchQuote(OnDataAvailableCallback&& onQuoteAvailable)
{
	if (m_downloader.isInProgress())
		return;

	m_downloader.download(url, [onQuoteAvailable = std::forward<OnDataAvailableCallback>(onQuoteAvailable)](const tstring& result) {
		onQuoteAvailable(findQuote(result));
	});
}
