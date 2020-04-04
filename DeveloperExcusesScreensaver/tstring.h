#pragma once

#include <string>
#include <Windows.h>
#include <regex>

#ifdef _UNICODE
using tstring = std::wstring;
using tregex = std::wregex;
inline tstring convert_to_tstring(const std::string& str)
{
	if (str.empty())
		return {};

	auto sourceSize = static_cast<int>(str.size());
	auto targetSize = MultiByteToWideChar(CP_UTF8, 0, str.data(), sourceSize, nullptr, 0);
	auto result = tstring(targetSize, TEXT('\0'));
	MultiByteToWideChar(CP_UTF8, 0, str.data(), sourceSize, result.data(), targetSize);
	return result;
}
inline std::string convert_to_string(const tstring& str)
{
	if (str.empty())
		return {};

	auto sourceSize = static_cast<int>(str.size());
	auto targetSize = WideCharToMultiByte(CP_UTF8, 0, str.data(), sourceSize, nullptr, 0, nullptr, nullptr);
	auto result = std::string(targetSize, '\0');
	WideCharToMultiByte(CP_UTF8, 0, str.data(), sourceSize, result.data(), targetSize, nullptr, nullptr);
	return result;
}
template<typename T>
inline tstring to_tstring(T val)
{
	return std::to_wstring(val);
}
#else
using tstring = std::string;
using tregex = std::regex;
inline tstring convert_to_tstring(const std::string& str)
{
	return str;
}

inline std::string convert_to_string(const tstring& str)
{
	return str;
}
template<typename T>
inline tstring to_tstring(T val)
{
	return std::to_string(val);
}
#endif
