#include "stdafx.h"
#include "SettingsProc.h"

LRESULT SettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Theme::Settings* pSettings = (Theme::Settings*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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
		if (pSettings->data.Applied)
		{
			SetWindowText(pSettings->HWND.Port, pSettings->data.Port);
			if (pSettings->data.MTT)
				SendMessage(pSettings->HWND.MinimizeToTray, WM_MOUSEACTIVATE, 0, 1);
			//InvalidateRect(hWnd, 0, TRUE);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_CREATE:
	{
		RECT r = { NULL };
		r.left = 14;

		pSettings = (Theme::Settings*)malloc(sizeof(Theme::Settings));
		if (pSettings == NULL)
			return FALSE;
		pSettings->data.Applied = FALSE;
		pSettings->data.MTT = FALSE;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pSettings);


		pSettings->HWND.Port = Theme::CWindow(r, L"Segoe UI", L"CEDIT", L"Port", WS_VISIBLE,
			55, 30, 87, 15, hWnd, (HMENU)IDE_PORT);

		pSettings->HWND.MinimizeToTray = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Minimize To Tray", WS_VISIBLE,
			55, 50, 87, 15, hWnd, (HMENU)IDB_TOTRAY);

		pSettings->HWND.OK = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Ok", WS_VISIBLE,
			120, 80, 20, 15, hWnd, (HMENU)IDB_OK);

		pSettings->HWND.Apply = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Apply", WS_VISIBLE,
			155, 80, 30, 15, hWnd, (HMENU)IDB_APPLY);

		pSettings->HWND.Cancel = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Cancel", WS_VISIBLE,
			20, 80, 40, 15, hWnd, (HMENU)IDM_EXIT);



	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDB_OK:
		case IDB_APPLY:
			pSettings->data.Applied = TRUE;
			GetWindowText(pSettings->HWND.Port, pSettings->data.Port, sizeof(pSettings->data.Port) / sizeof(wchar_t));
			SendMessage(pSettings->HWND.parent, WM_COMMAND, IDB_SETTINGS, IDB_SETTINGS);
			wmId == IDB_APPLY ? SendMessage(hWnd, WM_COMMAND, IDM_EXIT, 0) : SendMessage(pSettings->HWND.OK, WM_MOUSEACTIVATE, 0, 1);
			break;
		case IDB_TOTRAY:
			pSettings->data.MTT = !pSettings->data.MTT;
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
		MoveToEx(hdcMem, 0, 25, 0);
		LineTo(hdcMem, r.right, 25);
		MoveToEx(hdcMem, 0, r.bottom - 20, 0);
		LineTo(hdcMem, r.right, r.bottom -20);
		

		int i = r.right / 2;
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
