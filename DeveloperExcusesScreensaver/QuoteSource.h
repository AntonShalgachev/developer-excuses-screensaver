#pragma once

#include "framework.h"
#include "UrlDownloader.h"

class QuoteSource
{
public:
	using OnDataAvailableCallback = std::function<void(tstring)>;
	void fetchQuote(OnDataAvailableCallback&& onQuoteAvailable);

    QuoteSource();

private:
    void processNextJob();

    UrlDownloader m_downloader;

    std::stack<OnDataAvailableCallback> m_jobs;
};

