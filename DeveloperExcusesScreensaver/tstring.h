#pragma once

#include <string>

#ifdef _UNICODE
using tstring = std::wstring;
#else
using tstring = std::string;
#endif
