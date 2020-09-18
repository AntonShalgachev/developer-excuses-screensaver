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
    m_jobs.push(std::forward<OnDataAvailableCallback>(onQuoteAvailable));

    if (!m_downloader.isInProgress())
        processNextJob();
}

QuoteSource::QuoteSource()
{
    m_downloader.setOnDownloaderReadyCallback([this]() {
        processNextJob();
    });
}

void QuoteSource::processNextJob()
{
    if (m_downloader.isInProgress())
    {
        throw std::runtime_error("Trying to process next job while the downloader is still in progress");
    }

    if (m_jobs.empty())
    {
        return;
    }

    auto onQuoteAvailable = m_jobs.top();
    m_jobs.pop();

    auto onDataAvailable = [onQuoteAvailable](const tstring& result)
    {
        auto quote = findQuote(result);
        cachedQuotes.push_back(quote);
        onQuoteAvailable(quote);
    };

    auto onError = [onQuoteAvailable]()
    {
        if (cachedQuotes.empty())
        {
            onQuoteAvailable({});
            return;
        }

        std::uniform_int_distribution<std::size_t> dist{ 0, cachedQuotes.size() - 1 };
        onQuoteAvailable(cachedQuotes[dist(re)]);
    };

    m_downloader.download(url, onDataAvailable, onError);
}
