// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <WinSock2.h>
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <string>
#include <sstream>
#include <thread>
// reference additional headers your program requires here
#include <WS2tcpip.h>
#include <gdiplus.h>
#include <wincodec.h>
#pragma comment(lib, "Windowscodecs.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Ws2_32.lib")