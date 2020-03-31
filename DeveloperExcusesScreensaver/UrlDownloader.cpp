#include "UrlDownloader.h"

void UrlDownloader::download(const tstring& url, OnDataAvailableCallback&& onDataAvailable, OnErrorCallback&& onError)
{
	m_onDataAvailable = onDataAvailable;
	m_onError = onError;
	m_inProgress = true;

	DeleteUrlCacheEntry(url.c_str());
	auto result = URLOpenPullStream(nullptr, url.c_str(), 0, this);

	if (FAILED(result))
	{
		std::stringstream ss;
		ss << "ERROR: Could not connect. HRESULT: 0x" << std::hex << result << std::dec << std::endl;
		throw std::runtime_error(ss.str());
	}
}

void UrlDownloader::onDownloadComplete(IStream* stream, DWORD length)
{
	auto buffer = std::make_unique<char[]>(length);

	ULONG bytesRead = 0;
	auto result = stream->Read(buffer.get(), length, &bytesRead);

	if (m_onDataAvailable)
		m_onDataAvailable(convert_to_tstring(std::string{ buffer.get(), length }));

	m_onDataAvailable = {};
}

void UrlDownloader::onDownloadError()
{
	if (m_onError)
		m_onError();
	m_onError = {};
}
