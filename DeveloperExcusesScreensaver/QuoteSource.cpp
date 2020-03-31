#include "QuoteSource.h"
#include "framework.h"

namespace
{
	const auto url = tstring(TEXT("http://developerexcuses.com/"));

	const auto regex = tregex(TEXT("<a href=\"/\" rel=\"nofollow\" style=\"text-decoration: none; color: #333;\">(.+)</a>"));

	std::vector<tstring> cachedQuotes = {
		TEXT("Maybe somebody forgot to pay our hosting company"),
		TEXT("I thought I fixed that"),
	};

	std::default_random_engine re{ std::random_device{}() };

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

	m_downloader.download(url, [onQuoteAvailable](const tstring& result) {
		auto quote = findQuote(result);
		cachedQuotes.push_back(quote);
		onQuoteAvailable(quote);
	}, [onQuoteAvailable]() {
		if (cachedQuotes.empty())
		{
			onQuoteAvailable({});
			return;
		}

		std::uniform_int_distribution<std::size_t> dist{ 0, cachedQuotes.size() - 1 };
		onQuoteAvailable(cachedQuotes[dist(re)]);
	});
}
