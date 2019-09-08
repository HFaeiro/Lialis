#include "stdafx.h"
#include "ListView.h"
#include "locale"


bool sortbyname(const ClientData& cData1, const ClientData& cData2) {
	
	return std::lexicographical_compare(cData1.CDS.name.begin(), cData1.CDS.name.end(), cData2.CDS.name.begin(), cData2.CDS.name.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) < towlower(bi);
	});
}
bool sortbyIP(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.IP.begin(), cData1.CDS.IP.end(), cData2.CDS.IP.begin(), cData2.CDS.IP.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) < towlower(bi);
	});
}
bool sortbyCountry(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.Country.begin(), cData1.CDS.Country.end(), cData2.CDS.Country.begin(), cData2.CDS.Country.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) < towlower(bi);
	});
}
bool sortbyOS(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.OS.begin(), cData1.CDS.OS.end(), cData2.CDS.OS.begin(), cData2.CDS.OS.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) < towlower(bi);
	});
}
bool sortbynameDec(const ClientData& cData1, const ClientData& cData2) { //sort decending

	return std::lexicographical_compare(cData1.CDS.name.begin(), cData1.CDS.name.end(), cData2.CDS.name.begin(), cData2.CDS.name.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) > towlower(bi);
	});
}
bool sortbyIPDec(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.IP.begin(), cData1.CDS.IP.end(), cData2.CDS.IP.begin(), cData2.CDS.IP.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) > towlower(bi);
	});
}
bool sortbyCountryDec(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.Country.begin(), cData1.CDS.Country.end(), cData2.CDS.Country.begin(), cData2.CDS.Country.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) > towlower(bi);
	});
}
bool sortbyOSDec(const ClientData& cData1, const ClientData& cData2) {
	return std::lexicographical_compare(cData1.CDS.OS.begin(), cData1.CDS.OS.end(), cData2.CDS.OS.begin(), cData2.CDS.OS.end(), [](wchar_t ai, wchar_t bi) {
		return towlower(ai) > towlower(bi);
	});
}
bool compClientData(ClientData cData1, ClientData cData2)
{
	int retVal, name, OS, IP, Country;
	name = _wcsnicmp(cData1.CDS.name.c_str() ,cData2.CDS.name.c_str(),99);
	OS = _wcsnicmp(cData1.CDS.OS.c_str(),cData2.CDS.OS.c_str(),99);
	IP = _wcsnicmp(cData1.CDS.IP.c_str(),cData2.CDS.IP.c_str(),99);
	Country = _wcsnicmp(cData1.CDS.Country.c_str(),cData2.CDS.Country.c_str(),99);
	retVal = name += OS += IP += Country;
	return retVal;
}

static void
refreshButtons(HWND hWnd, Theme::ListView*& LV)
{
	
	int ActualScrollPosition = GetScrollPos(hWnd, SB_VERT);
	int tmpCounterScrollPosition = ActualScrollPosition;
	for (int i = LV->numOfPinned; i < LV->vLVButton.size(); i++)
	{

		for (int j = 0; j < LV->numOfPinned; j++)
		{
			if (tmpCounterScrollPosition >= LV->vClientData.size())
				break;
			if (!compClientData(LV->vClientData[tmpCounterScrollPosition], LV->vLVButton[j]->cData)) {
				tmpCounterScrollPosition++; // NEED TO WORK ON THIS TO FIX SCROLLING
			}
		}
		if (tmpCounterScrollPosition >= LV->vClientData.size())
			break;
		//if(LV->vLVButton[i]->)
		LV->vLVButton[i]->cData = LV->vClientData[tmpCounterScrollPosition];
		
		tmpCounterScrollPosition++;
	}
	RECT r = Theme::GetRect(hWnd);
	r.top = r.top + (LV->ButtonHeight * (LV->numOfPinned -1));
	InvalidateRect(hWnd, &r, TRUE);

}
Theme::ListViewButton*
addAndCreate(ClientData cData, HWND parent, int y, int width, int height)
{
	HWND hWnd = CreateWindow(L"LVBUTTON", NULL, WS_CHILD, 0, y, width, height, parent, NULL, (HINSTANCE)GetModuleHandle(NULL), nullptr);
	if (!hWnd) {
		MessageBox(parent, L"LVBUTTON", L"FailedToCreateWindow", NULL);
	}
	Theme::ListViewButton* tmpLVButton = (Theme::ListViewButton*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	tmpLVButton->cData = cData;
	return tmpLVButton;
}


LRESULT ListViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Theme::ListView* LV = (Theme::ListView*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	
	switch (message)
	{
	case IDB_REFRESH:
	case IDB_REMOVE:
	{
		if (wParam) {
			//std::vector<int>* invalidClients = (std::vector<int>*)(wParam);
			//std::vector<int> vInt;

			for (int i : *(std::vector<int>*)(wParam)) {
				if (i < LV->vClientData.size()) {
					for (int j = 0; j < LV->vLVButton.size(); j++)
					{
						if (LV->vClientData[i].s == LV->vLVButton[j]->cData.s) {
							if(LV->vLVButton[j]->Pinned)
								SendMessage(hWnd, IDB_PIN, NULL, lParam);
							break;
						}
					}
				}
			}
			if ((*(std::vector<int>*)(wParam))[0] == 0)
			{
				HWND rHWND = (HWND)SendMessage(GetParent(hWnd), IDB_GETREMOTEHANDLE, NULL, NULL);
				LPSTREAM iStream = (LPSTREAM)SendMessage(rHWND, M_GETSTREAM, NULL, NULL);
				while (iStream != nullptr)
					Sleep(1);
			}
			int j = 0;
			int p = NULL;
			for (int i : *(std::vector<int>*)(wParam)) {
				p = i - j;
				if (p < 0) break;
				LV->vClientData.erase(LV->vClientData.begin() + p);
				j++;
			}

			SendMessage(hWnd, IDC_CLEAR, TRUE, NULL);


		}
		else
		{

			int cMessage = 3;
			if (message == IDB_REFRESH)
				cMessage = 2;

			if (lParam == 0)
			{
				//This is the very first item in the list. since the remote stream uses this first item
				//in the list we need to make sure the istream isn't being used before deleting the item.
				HWND rHWND = (HWND)SendMessage(GetParent(hWnd), IDB_GETREMOTEHANDLE, NULL, NULL);
				LPSTREAM iStream = (LPSTREAM)SendMessage(rHWND, M_GETSTREAM, NULL, NULL);
				//while (iStream != nullptr)
				//	Sleep(1);

			}

			send(LV->vClientData[lParam].s, (char*)& cMessage, sizeof(int), 0);
			closesocket(LV->vClientData[lParam].s);
			closesocket(LV->vClientData[lParam].remoteSocket);
			LV->vClientData[lParam].s = INVALID_SOCKET;
			LV->vClientData[lParam].remoteSocket = INVALID_SOCKET;

			if (LV->vLVButton[lParam]->Pinned)
				SendMessage(hWnd, IDB_PIN, NULL, lParam);

			LV->vClientData.erase(LV->vClientData.begin() + lParam);
			SendMessage(hWnd, IDC_CLEAR, TRUE, NULL);



		}
		for (auto cData = LV->vClientData.begin() + LV->numOfPinned; cData != LV->vClientData.end(); cData++)
		{
			Theme::ListViewButton* tmpLVButton = addAndCreate(cData[0], hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
			tmpLVButton->ButtonNumber = LV->vLVButton.size();
			LV->vLVButton.push_back(tmpLVButton);
			ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);
		}
		//refreshButtons(hWnd, LV);
		

	}
	break;
	case IDR_GETCLIENTLIST:
	{
		if(LV)
		return (LRESULT)&LV->vClientData;
	}
	break;
	case IDC_CLEAR:
	{
		if (wParam != 1) {
			LV->vClientData.erase(LV->vClientData.begin(), LV->vClientData.end());
			LV->numOfPinned = 0;
		}
		for (int i = (wParam == 1 ? LV->numOfPinned : 0 ); i < LV->vLVButton.size(); i++)
			DestroyWindow(LV->vLVButton[i]->pData->hWnd);
		LV->vLVButton.erase(LV->vLVButton.begin() + (wParam == 1 ? LV->numOfPinned : 0), LV->vLVButton.end());
		
		ShowScrollBar(hWnd, SB_VERT, 0);
		LV->VSCROLL = FALSE;
		InvalidateRect(hWnd, 0, TRUE);

	}
	break;
	case IDB_PIN:
	{

		if (!LV->vLVButton[lParam]->Pinned) {
			LV->vLVButton[LV->numOfPinned]->cData = LV->vLVButton[lParam]->cData; // swap pinned item to first slot after the last pinned item;
			LV->vLVButton[LV->numOfPinned]->Pinned = TRUE;

			LV->numOfPinned++; //to be pinned item was placed. so increase this counter/
			
		}
		else
		{
			if (lParam < LV->numOfPinned - 1)
				for (int i = lParam; i < LV->numOfPinned; i++)
					LV->vLVButton[i]->cData = LV->vLVButton[i+1]->cData;
			LV->numOfPinned--;
			LV->vLVButton[LV->numOfPinned]->Pinned = FALSE;
			
		}

		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		
		si.nMin = 0;
		si.nMax = ((LV->vClientData.size() + LV->numOfPinned) - (LV->ControlHeight / LV->ButtonHeight));

		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		refreshButtons(hWnd, LV);
		SetFocus(hWnd);
		break;
	}
	

	case WM_NCCREATE:
	{
		LV = new Theme::ListView();
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)LV);
		RECT r = Theme::GetRect(hWnd);
		LV->counter = 0; 
		LV->numOfPinned = 0;
		LV->ButtonHeight = 15;
		LV->ButtonWidth = r.right;
		LV->ControlHeight = r.bottom;
	}
	return TRUE;
	case WM_MOUSEWHEEL:
	{
		SendMessage(hWnd, WM_VSCROLL, WM_MOUSEWHEEL, GET_WHEEL_DELTA_WPARAM(wParam));
		return 0;
	}
	case WM_VSCROLL:
	{

				// Get all the vertical scroll bar information.

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		// Save the position for comparison later on.
		int yPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case WM_MOUSEWHEEL:
			si.nPos += (-lParam) / (LV->ControlHeight / LV->ButtonHeight);
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			si.nPos -= 1;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			si.nPos -= 3;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			si.nPos += 3;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			RECT r = Theme::GetRect(hWnd);
			r.top = r.top + (LV->ButtonHeight * LV->numOfPinned);
			ScrollWindowEx(hWnd, 0, (yPos - si.nPos) * 18,
				&r, &r, NULL, NULL, SW_ERASE | SW_INVALIDATE);
			//ScrollWindow(hWnd, 0, 18 * (yPos - si.nPos), NULL, NULL);
			//UpdateWindow(hWnd);
			refreshButtons(hWnd, LV);
			
		}

		return 0;
	}
	case IDB_ADD:
	{
		if (LV == NULL) {
			MessageBox(hWnd, L"LV == NULL", NULL, NULL);
			return FALSE;
		}

		ClientData tmpClientData = *(ClientData*)lParam;
		if ((LV->vLVButton.size() * LV->ButtonHeight) < LV->ControlHeight + 3) {
			Theme::ListViewButton* tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size())*LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
			tmpLVButton->ButtonNumber = LV->vLVButton.size();
			LV->vLVButton.push_back(tmpLVButton);
			ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);
		}
		LV->vClientData.push_back(tmpClientData);

		
		//if (!LV->vClientData.size()) {
		//	ClientData tmpClientData = { 0,0, L"Bobby", L"192.168.0.1", L"Windows 10", L"USA" };

		//	Theme::ListViewButton* tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Armadel", L"192.168.0.4", L"Windows 8", L"France" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Lucifer", L"192.168.0.5", L"Windows 7", L"UK" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Erad", L"192.168.0.9", L"Windows XP", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Chad", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Frankie", L"192.168.0.4", L"Windows 8", L"France" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"tits mcgee", L"192.168.0.5", L"Windows 7", L"UK" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"The Chosen one", L"192.168.0.9", L"Windows XP", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Lucy", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Big Burtha", L"192.168.0.4", L"Windows 8", L"France" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"jesus", L"192.168.0.5", L"Windows 7", L"UK" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"lil jon", L"192.168.0.9", L"Windows XP", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Eradicated", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Expresso", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"LolNation", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"Gracias", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"stupid baboso", L"192.168.1.1", L"Windows Vista", L"USA" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpLVButton = addAndCreate(tmpClientData, hWnd, LV->vLVButton.size() * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
		//	tmpLVButton->ButtonNumber = LV->vLVButton.size();
		//	LV->vLVButton.push_back(tmpLVButton);
		//	ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);

		//	tmpClientData = { 0, 0, L"hombre", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"compadre", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"mi amigo", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Jack Daniels", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"My Nigga", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"yeahYA", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Lil Johnny 2 by 4", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Sarah", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Mike", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Elton", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"John", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"MAchino", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Poncho", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"AnothaOne", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"And Another", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"HolyShit", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Theres Alot", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Yup Again", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Here's", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Jones", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Who?", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Yup", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Too", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Many", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Fucking", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Names", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Yupeers", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"OHMERGERD", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"ClitNibbler", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Moist", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Taint", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Ferocious", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"lion", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"lil", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Tiger", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"UGHHH", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"lon", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"lion", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"lil", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);

		//	tmpClientData = { 0, 0, L"6thToLast", L"69.168.0.9", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"5thToLast", L"192.123.0.9", L"Windows XP", L"Columbia" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"4thToLast", L"192.500.0.9", L"Windows Vista", L"Austraila" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"3rdToLast", L"257.168.0.9", L"Windows 8", L"South America" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"2ndtoLast", L"192.168.0.9", L"Windows 98", L"Europe" };
		//	LV->vClientData.push_back(tmpClientData);
		//	tmpClientData = { 0, 0, L"Last", L"69.168.0.99", L"Windows XP", L"Russia" };
		//	LV->vClientData.push_back(tmpClientData);

		//}
		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = (LV->vClientData.size() - (LV->ControlHeight / LV->ButtonHeight));

		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		if ((LV->vClientData.size() * LV->ButtonHeight) > LV->ControlHeight + 3 && !LV->VSCROLL) {
			ShowScrollBar(hWnd, SB_VERT, 1);
			LV->VSCROLL = TRUE;
		}
		else if ((LV->vClientData.size() * LV->ButtonHeight) < LV->ControlHeight + 3 && LV->VSCROLL) {
			ShowScrollBar(hWnd, SB_VERT, 0);
			LV->VSCROLL = FALSE;
		}
		


	}
	break;
	case IDB_NAME:
		std::sort(LV->vClientData.begin(), LV->vClientData.end(), lParam == 1 ? sortbynameDec : sortbyname);
		refreshButtons(hWnd, LV);
		break;
	case IDB_IP:
		std::sort(LV->vClientData.begin(), LV->vClientData.end(), lParam == 1 ? sortbyIPDec : sortbyIP);
		refreshButtons(hWnd, LV);
		break;
	case IDB_OPSYS:
		std::sort(LV->vClientData.begin(), LV->vClientData.end(), lParam == 1 ? sortbyOSDec : sortbyOS);
		refreshButtons(hWnd, LV);
		break;
	case IDB_COUNTRY:
		std::sort(LV->vClientData.begin(), LV->vClientData.end(), lParam == 1 ? sortbyCountryDec : sortbyCountry);
		refreshButtons(hWnd, LV);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}


