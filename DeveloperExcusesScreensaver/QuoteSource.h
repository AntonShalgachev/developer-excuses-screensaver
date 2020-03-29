#pragma once

#include "framework.h"
#include "UrlDownloader.h"

class QuoteSource
{
public:
	using OnDataAvailableCallback = std::function<void(tstring)>;
	void fetchQuote(OnDataAvailableCallback&& onQuoteAvailable);

private:
	UrlDownloader m_downloader;
};

