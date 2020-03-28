// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#define _USE_MATH_DEFINES
#define VC_EXTRALEAN

#include "targetver.h"
#include <windows.h>
#include <commctrl.h>
#include <scrnsave.h>
#include <cmath>
#include <string>

#ifdef _UNICODE
using tstring = std::wstring;
#else
using tstring = std::string;
#endif
