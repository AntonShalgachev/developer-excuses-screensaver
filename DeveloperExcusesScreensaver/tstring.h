#pragma once

#include <string>
#include <codecvt>
#include <regex>

#ifdef _UNICODE
using tstring = std::wstring;
using tregex = std::wregex;
inline tstring convert_to_tstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}
inline std::string convert_to_string(const tstring& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(str);
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
