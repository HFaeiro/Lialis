// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <WinSock2.h>
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>
#include <math.h>
#include <string>
#include <sstream>
//#include <ctime>
//#include <fstream>
//#include <limits>
#include <vector>
#include <algorithm>
//#include <cwctype>
#include <thread>
// TODO: reference additional headers your program requires here
#include <gdiplus.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "gdiplus.lib")

#include <d2d1.h>
#include <wincodec.h>
//
#pragma comment(lib, "d2d1")
#include "GlobalStructs.h"