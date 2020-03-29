#pragma once

#include "framework.h"

class UrlDownloader : public IBindStatusCallback
{
public:
	using OnDataAvailableCallback = std::function<void(tstring)>;

	void download(const tstring& url, OnDataAvailableCallback&& onDataAvailable);
	bool isInProgress() const { return m_inProgress; }

	STDMETHOD(OnStartBinding)(DWORD, IBinding*) { return E_NOTIMPL; }
	STDMETHOD(GetPriority)(LONG*) { return E_NOTIMPL; }
	STDMETHOD(OnLowResource)(DWORD) { return E_NOTIMPL; }
	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG, ULONG, LPCWSTR) { return E_NOTIMPL; }
	STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR) { return E_NOTIMPL; }

	STDMETHOD(GetBindInfo)(DWORD* grfBINDF, BINDINFO*)
	{
		*grfBINDF = BINDF_PULLDATA | BINDF_GETNEWESTVERSION | BINDF_RESYNCHRONIZE;
		return S_OK;
	}

	STDMETHOD(OnDataAvailable)(DWORD, DWORD dwSize, FORMATETC*, STGMEDIUM __RPC_FAR *pstgmed)
	{
		onDownloadComplete(pstgmed->pstm, dwSize);
		return S_OK;
	}

	STDMETHOD(OnObjectAvailable)(REFIID, IUnknown*) { return E_NOTIMPL; }

	STDMETHOD_(ULONG, AddRef)() { return 0; }
	STDMETHOD_(ULONG, Release)() { return 0; }
	STDMETHOD(QueryInterface)(REFIID, void**) { return E_NOTIMPL; }

private:
	void onDownloadComplete(IStream* stream, DWORD length);

	OnDataAvailableCallback m_onDataAvailable;
	bool m_inProgress = false;
};

