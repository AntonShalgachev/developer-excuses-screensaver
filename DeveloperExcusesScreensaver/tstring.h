#pragma once

#include <string>
#include <codecvt>
#include <regex>

#ifdef _UNICODE
using tstring = std::wstring;
using tregex = std::wregex;
inline tstring to_tstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}
#else
using tstring = std::string;
using tregex = std::regex;
inline tstring to_tstring(const std::string& str)
{
	return str;
}
#endif
