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
		// scrolling breaks get rid of && tmpCounterScrollPosition < LV->vClientData.size()
		for (int j = 0; j < LV->numOfPinned && tmpCounterScrollPosition < LV->vClientData.size(); j++)
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
		return nullptr;
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
	case WM_PLUGINLVACTIVATE:
	{
		auto vSearch = LV->vLVDDM.begin();
		int i = 0;
		for (vSearch; vSearch != LV->vLVDDM.end(); vSearch++ , i ++)
		{
			if (vSearch->uIDNewItem == wParam)
			{
				SendMessage(GetParent(hWnd), WM_PLUGINLVACTIVATE, wParam, (LPARAM)&LV->vLVButton[lParam]->cData);
			}
		}
	}
		break;
	case WM_ADDDROPMENUITEM:
	{
		Theme::LVDropDownMenu tmpLVDDM;
		tmpLVDDM.MenuName = (wchar_t*)lParam;
		tmpLVDDM.uIDNewItem = LV->vLVDDM.size() -1; //subtract default menu item amount to create count of plugins. 
		LV->vLVDDM.push_back(tmpLVDDM);
	}
	break;
	case WM_GETLVDDM:
	{
		return (LRESULT)&LV->vLVDDM;
	}
	break;
	case IDB_REFRESH:
	case IDB_REMOVE:
	{
		if (wParam) {

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



			send(LV->vClientData[lParam].s, (char*)& cMessage, sizeof(int), 0);
			closesocket(LV->vClientData[lParam].s);
			LV->vClientData[lParam].s = INVALID_SOCKET;

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
		
		Theme::LVDropDownMenu tmpLVDDM;
		tmpLVDDM.MenuName = L"Remove";
		tmpLVDDM.uIDNewItem = IDB_REMOVE;
		LV->vLVDDM.push_back(tmpLVDDM);
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


