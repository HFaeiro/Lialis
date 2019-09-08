// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <vector>
#include <istream>
#include <windowsx.h>

#include <d2d1.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1")


#endif //PCH_H
