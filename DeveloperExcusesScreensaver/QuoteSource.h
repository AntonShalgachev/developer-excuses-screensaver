#pragma once

#include <functional>
#include "tstring.h"

class QuoteSource
{
public:
	void fetchQuote(std::function<void(tstring)> onQuoteAvailable);

private:
	int m_count = 0;
};

