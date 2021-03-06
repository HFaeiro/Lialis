// LialisClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LialisClient.h"
#include "Winsock.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND PORT;
HWND URL;
BOOL START = FALSE;
Winsock wSock;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LIALISCLIENT, szWindowClass, MAX_LOADSTRING);

	WCHAR IP[MAX_LOADSTRING];
	WCHAR PORT[MAX_LOADSTRING];
	LoadStringW(hInstance, IDR_IP, IP, MAX_LOADSTRING);
	LoadStringW(hInstance, IDR_PORT, PORT, MAX_LOADSTRING);

	//wSock = Winsock(std::wcstol(PORT, nullptr, 10), IP);

    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LIALISCLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LIALISCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+5);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LIALISCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 230, 275, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   wSock.cHandler.mainHWND = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		CreateWindow(L"Button", L"Start", WS_CHILD | WS_VISIBLE, 50, 75, 100, 50, hWnd, (HMENU)IDB_START, NULL, NULL);
		CreateWindow(L"Button", L"ScreenShot", WS_CHILD | WS_VISIBLE,50, 130, 100,50, hWnd, (HMENU)IDB_SCREEN, NULL, NULL);
		PORT = CreateWindow(L"EDIT", L"4444", WS_CHILD | WS_VISIBLE, 50, 25, 100, 20, hWnd, (HMENU)IDE_PORT, NULL, NULL);
		URL = CreateWindow(L"EDIT", L"127.0.0.1", WS_CHILD | WS_VISIBLE, 50, 50, 100, 20, hWnd, (HMENU)IDE_URL, NULL, NULL);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDB_SCREEN:
			{
				POINT cursor = { 0 };
				GetCursorPos(&cursor);
				HMENU popupMenu = CreatePopupMenu();
				int screens = GetSystemMetrics(SM_CMONITORS);
				if (screens)
				{
					for (int i = 1; i <= screens; i++) {
						wchar_t* string = new wchar_t[10];

						swprintf(string, 10, L"Screen %d", i);
						// LPCWSTR string = OVERLOAD(L"Screen %d", i);
						InsertMenu(popupMenu, NULL, MF_STRING, i, string);
					}
					int item = TrackPopupMenu(popupMenu, TPM_LEFTALIGN | TPM_RETURNCMD, cursor.x, cursor.y, 0, hWnd, NULL);
					if (item) {
						RemoteView r;
						r.ScreenShot(item);
					}
				}
				break;
			}
			case IDB_START:
			{
				if (START) {
					SetWindowText(GetDlgItem(hWnd, IDB_START), L"Start");
					START = FALSE;
					wSock.Deinitialize();
				}
				else {
					SetWindowText(GetDlgItem(hWnd, IDB_START), L"Stop");
					START = TRUE;
					wchar_t lpbuf[100];
					GetWindowText(PORT, lpbuf, 100);
					if (lpbuf != L"Port") {
						int iPort = std::wcstol(lpbuf, nullptr, 10);
						if (iPort != 0) {
							GetWindowText(GetDlgItem(hWnd, IDE_URL), lpbuf, 100);
							if (lpbuf != L"URL/IP") {
								wSock = Winsock(iPort, lpbuf);
								wSock.cHandler.mainHWND = hWnd;
							}
						}
					}
					wSock.Initialize();
				}
			}
			break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
