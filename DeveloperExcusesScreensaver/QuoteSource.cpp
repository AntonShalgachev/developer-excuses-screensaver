#include "QuoteSource.h"
#include "framework.h"

void QuoteSource::fetchQuote(std::function<void(tstring)> onQuoteAvailable)
{
	auto text = tstring(TEXT("Some quote"));
	auto part = tstring(TEXT(" some quote"));
	for (auto i = 0; i < m_count; i++)
	{
		text += part;
	}

	onQuoteAvailable(text);

	m_count++;
}
