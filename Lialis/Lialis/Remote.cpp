#include "stdafx.h"
#include "Remote.h"
#include <mutex>
std::mutex m;
BOOL usingVECT =  FALSE;

Remote::Remote()
{
	int count = NULL;
	
	do {


		rData.iStream[count] = nullptr;
		count++;
	} while (count < 5);
}

Remote::~Remote()
{
}


void Remote::D2DUpdater(std::vector<RemoteSession>*& vRemoteSessions)
{
	
	int sessionSize = vRemoteSessions->size();
	if (sessionSize)
	{

	/*	ID2D1Factory* d2dFactory = NULL;
		IWICImagingFactory* d2dWICFactory;*/
		ID2D1HwndRenderTarget* d2drender = NULL;

		//CoInitialize(nullptr);


				RECT rc = Theme::GetRect(vRemoteSessions->begin()->rDraw);
				D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);


				HRESULT hr = d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(vRemoteSessions->begin()->rDraw, size), &d2drender);


				if (SUCCEEDED(hr))
				{
					RemoteSession rData;
					if (vRemoteSessions->size())
					{

								rData = *vRemoteSessions->begin();
								int count = 0;
								
									if (vRemoteSessions->begin()->iStream[count] != nullptr) {
										usingVECT = TRUE;
										D2D1_SIZE_F renderSize = d2drender->GetSize();
										rc = Theme::GetRect(vRemoteSessions->begin()->rDraw);
										size = D2D1::SizeU(rc.right, rc.bottom);

										if (renderSize.height != size.height || renderSize.width != size.width)
										{
											SetWindowPos(vRemoteSessions->begin()->rDraw, 0, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
											d2drender->Resize(size);
										}
										//do {
										DrawD2DImage(d2dFactory, d2dWICFactory, d2drender, vRemoteSessions->begin()->rDraw, vRemoteSessions->begin()->iStream[count]);
										if (vRemoteSessions->begin()->iStream[count] != nullptr) {
											vRemoteSessions->begin()->iStream[count]->Release();
											vRemoteSessions->begin()->iStream[count] = nullptr;
										}
										//count++;
										//} while (count < 2);
										usingVECT = FALSE;
									}
									else
										Sleep(1);
								
					}


					d2drender->Release();

		}
	}

	return;
}

void Remote::RemoteHandler(std::vector<RemoteSession>* vRemoteSessions)
{
	//Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, NULL);
	//GetEncoderClsid(L"image/jpeg", &gdiCLSID);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	while (vRemoteSessions->begin()->RemoteReceiveSocket == INVALID_SOCKET) //wait for SendSocket to be valid.
		Sleep(100);

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory), NULL, (void**)& d2dFactory);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IWICImagingFactory), (void**)(&d2dWICFactory));
		if (SUCCEEDED(hr))
		{
			/*std::thread t1(&Remote::D2DUpdater, this, vRemoteSessions);

			t1.detach();*/
			RemoteSession rData;
			HWND tab = nullptr;
			while (vRemoteSessions->begin()->WINDOWACTIVE)
			{
				BOOL BEGIN = TRUE;

				m.lock();
				if (vRemoteSessions->size())
				{
					if (vRemoteSessions->size() > 1)
					{
						if (!vRemoteSessions->begin()->ACTIVE) {
							auto tmpRSesh = vRemoteSessions->begin();
							for (tmpRSesh; tmpRSesh != vRemoteSessions->end(); tmpRSesh++)
							{
								if (tmpRSesh->ACTIVE)
								{
									rData = *tmpRSesh;

									if (tmpRSesh->RemoteReceiveSocket == INVALID_SOCKET)
									{
										int count = 0;
										while (tmpRSesh->RemoteReceiveSocket == INVALID_SOCKET) //wait for SendSocket to be valid.
										{
											Sleep(100);
											if (count > 10)
											{

												MessageBox(NULL, L"Socket Timeout", NULL, NULL);
												break;
											}
											count++;

										}
										rData.RemoteReceiveSocket = tmpRSesh->RemoteReceiveSocket;

									}
									tab = tmpRSesh->tab;
									BEGIN = FALSE;
									break;
								}
							}
						}
					}
					if (BEGIN) {
						rData = *vRemoteSessions->begin();
						tab = rData.tab;
					}

					fd_set remoteSockets;
					timeval wait;
					wait.tv_sec = 2;
					wait.tv_usec = 0;
					FD_ZERO(&remoteSockets);
					FD_SET(rData.RemoteReceiveSocket, &remoteSockets);
					FD_SET(rData.RemoteSendSocket, &remoteSockets);
					int sel = select(NULL, &remoteSockets, NULL, NULL, &wait);

					int retVal = NULL;

					if (sel > 0)
					{
						if (FD_ISSET(rData.RemoteReceiveSocket, &remoteSockets))
						{
							retVal = RemoteReceive(rData);
							if (retVal > 0) {
								if (vRemoteSessions->size()) {
									vRemoteSessions->begin()->iStream[0] = rData.iStream[0];
									D2DUpdater(vRemoteSessions);
									m.unlock();
								}
							}
							else
							{
								m.unlock();
								SendMessage(rData.hWnd, WM_COMMAND, IDM_EXIT, NULL);
								break;
							}

						}

					}
					else if (sel <= 0 && retVal < 0)
					{
						//client closed or lost connection. make another client active. 
						if (vRemoteSessions->begin()->WINDOWACTIVE) {
							if (!SendMessage(rData.hWnd, WM_COMMAND, IDB_CLOSEREMOTE, (LPARAM)tab)) {
								//if this was the last open remote connection
								m.unlock();
								SendMessage(rData.hWnd, WM_COMMAND, IDM_EXIT, NULL);
								break;
							}


						}

					}
				}
				m.try_lock();
				m.unlock();
			}
		}
	}
	//m.unlock();
	d2dFactory->Release();
	d2dWICFactory->Release();
	CoUninitialize();
	return;
}

int Remote::RemoteReceive(RemoteSession& rData)
{

	int check = NULL;
	int read = NULL;
	int size = NULL;



	int sel = NULL;
	std::vector<char> vBuffer;
	BOOL BTRBool = FALSE;
	int iterations = 0;

	check = recv(rData.RemoteReceiveSocket, (char*)& size, sizeof(int), 0);
	if (check < 0)
		return -1;
	if (size > 10000000)
	{
		return -1;
	}
	vBuffer.resize(100000);
	do
	{
		check = recv(rData.RemoteReceiveSocket, &vBuffer[read], 100000, 0);
		read += check;
		if (check < 1)
		{
			int err = WSAGetLastError();
			return -1;

		}
		vBuffer.resize(read + 100000);
	} while (read != size);

	int count = 0;
	//do {
		if (CreateStreamOnHGlobal(NULL, true, &rData.iStream[count]) == S_OK)
		{

			ULONG inumBytes = NULL;
			if (rData.iStream[count]->Write(&vBuffer[0], vBuffer.size(), &inumBytes) == S_OK)
			{
				LARGE_INTEGER li;
				li.QuadPart = NULL;
				if (rData.iStream[count]->Seek(li, STREAM_SEEK_SET, NULL) != S_OK)
					return -1;
			}
		}
	//	count++;
	//} while (count < 2);
	int cheeky = 2;
	int sent = send(rData.RemoteSendSocket, (char*)& cheeky, sizeof(int), 0);
	return sent;
}

void DrawD2DImage(ID2D1Factory* d2dFactory, IWICImagingFactory* d2dWICFactory, ID2D1HwndRenderTarget* d2drender, HWND hWnd, LPSTREAM& istream)
{
	
	if (istream == nullptr)
		return;

	IWICBitmapDecoder* d2dDecoder = NULL;
	IWICFormatConverter* d2dConverter = NULL;

	IWICBitmapFrameDecode* d2dBmpSrc = NULL;
	ID2D1Bitmap* d2dBmp = NULL;
	LARGE_INTEGER li;
	li.QuadPart = NULL;
	if (istream->Seek(li, STREAM_SEEK_SET, NULL) == S_OK)
	{

		HRESULT hr = d2dWICFactory->CreateDecoderFromStream(istream, NULL, WICDecodeMetadataCacheOnLoad, &d2dDecoder);
		if (SUCCEEDED(hr))
		{
			hr = d2dWICFactory->CreateFormatConverter(&d2dConverter);
			if (SUCCEEDED(hr))
			{
				hr = d2dDecoder->GetFrame(0, &d2dBmpSrc);
				if (SUCCEEDED(hr))
				{
					d2dDecoder->Release();
					hr = d2dConverter->Initialize(d2dBmpSrc, GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
					if (SUCCEEDED(hr))
					{
						hr = d2drender->CreateBitmapFromWicBitmap(d2dConverter, NULL, &d2dBmp);
						if (SUCCEEDED(hr))
						{
							d2dConverter->Release();
							RECT rc = Theme::GetRect(hWnd);
							D2D1_RECT_F rect = D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom);
							d2drender->BeginDraw();
							d2drender->SetTransform(D2D1::Matrix3x2F::Identity());
							d2drender->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray, 0.1f));
							d2drender->DrawBitmap(d2dBmp, &rect);
							d2drender->EndDraw();
							rc = Theme::GetRect(hWnd);
							rc.bottom = 52;
							d2dBmp->Release();
						}
					}
					d2dBmpSrc->Release();
				}

			}

		}

	}
	else
	{
		int point = 1;
		return;

	}
}
int resettablocations(std::vector<Remote::RemoteSession>* vRS)
{
	int i = 0;
	auto vrs = vRS->begin();
	for (vrs; vrs != vRS->end(); vrs++,i++)
	{
		if (vrs->tab)
		{
			//, 31, 75, 20,
			SetWindowPos(vrs->tab, 0, 75 * i, 31, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else i - 1;
	}
	return 1;
}
LRESULT CALLBACK DrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK RemoteProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::vector<Remote::RemoteSession>* vRemoteSessions = (std::vector<Remote::RemoteSession>*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
	case M_GETSTREAM:
	{
		return (LRESULT)vRemoteSessions->begin()->iStream[0];
	}
	break;
	case WM_SIZE:
	{
		SetWindowPos(vRemoteSessions->begin()->rDraw, NULL, 5, 52, LOWORD(lParam) - 8, HIWORD(lParam) - 54, SWP_NOZORDER | SWP_NOACTIVATE);
		if (vRemoteSessions->begin()->MAX && wParam != 1  && wParam != 0)
		{
			vRemoteSessions->begin()->MAX = FALSE;
		}
	}
	break;
	case IDB_SWITCHSCREEN:
	{
		HWND tab = vRemoteSessions->begin()->tab;
		if (wParam) {
			tab = (HWND)wParam;
		}
		SOCKET SendSocket = vRemoteSessions->begin()->RemoteSendSocket;
		if (vRemoteSessions->size() > 1)
		{
				auto tmpRSesh = vRemoteSessions->begin();
				BOOL FOUND = FALSE;
				for (tmpRSesh; tmpRSesh != vRemoteSessions->end(); tmpRSesh++)
				{
					if (tmpRSesh->ACTIVE)
					{
							if (wParam)
							{
								if (tmpRSesh->tab != tab) {

									SendMessage(tmpRSesh->tab, WM_MOUSEACTIVATE, NULL, 1);
									tmpRSesh->ACTIVE = FALSE;
									FOUND = TRUE;
									tmpRSesh = vRemoteSessions->begin();
									if (tmpRSesh->tab == tab) { //found match
										tmpRSesh->ACTIVE = TRUE;
										SendSocket = tmpRSesh->RemoteSendSocket;
										break;
									}

								}
							}
							else {

								SendSocket = tmpRSesh->RemoteSendSocket;
								break;

							}

					}
					if (FOUND)
						if (tmpRSesh->tab == tab) { //found match
							tmpRSesh->ACTIVE = TRUE;
							SendSocket = tmpRSesh->RemoteSendSocket;
							break;
						}
				}
			//}
		}
		int screen = lParam;
		int i = 1;
		send(SendSocket, (char*)&i, sizeof(int), 0);
		send(SendSocket, (char*)& screen, sizeof(int),0);

	//	SendMessage(tab, WM_MOUSEACTIVATE, NULL, 1);
		break;
	}
	case WM_GETNUMSCREEN:
	{
		if (vRemoteSessions->size() > 1)
		{
			auto RemoteSeshSearch = vRemoteSessions->begin();
			for (RemoteSeshSearch; RemoteSeshSearch != vRemoteSessions->end(); RemoteSeshSearch++) //check for matching socket
			{
				if (RemoteSeshSearch->tab == (HWND)lParam) { //found match
					return RemoteSeshSearch->numScreens;
				}
			}
		}
		return vRemoteSessions->begin()->numScreens;
		break;
	}
	case WM_MAXIMIZE:
	{
		ShowWindow(hWnd, SW_MAXIMIZE);

		break;
	}
	case WM_NCHITTEST: //if window was clicked.
	{
		UINT uHitTest;
		POINT mPos;
		RECT r;
		GetWindowRect(hWnd, &r);
		mPos.x = GET_X_LPARAM(lParam) - r.left;
		mPos.y = GET_Y_LPARAM(lParam) - r.top;

		if (mPos.x > r.right - r.left - 10) { //mouse is on right wall
			return HTRIGHT;
		}
		else if (mPos.x < 5) { //mouse is on left wall
			return HTLEFT;
		}
		else if (mPos.x < 10 && mPos.y < 10) { //mouse is on top left corner
			return HTTOPLEFT;
		}
		else if (mPos.x > r.right - r.left - 10 && mPos.y < 10) { //mouse is on top right corner
			return HTTOPRIGHT;
		}
		else if (mPos.x > r.right - r.left - 10 && mPos.y > r.bottom - r.top - 10)
			return HTBOTTOMRIGHT;
		else if (mPos.x < 10 && mPos.y > r.bottom - r.top - 10)
			return HTBOTTOMLEFT;
		else if (mPos.y < 5)
			return HTTOP;

		else if (mPos.y > r.bottom - r.top - 10)
			return HTBOTTOM;

		else if (mPos.y <= 30) //mouse is on top bar
				return HTCAPTION;

		else return DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		//return uHitTest;
	}
	break;
	case WM_INIT_REMOTE:
	{
		Remote re;
		
		std::thread t(&Remote::RemoteHandler, re, (std::vector<Remote::RemoteSession>*)lParam);
		t.detach();
		break;
	}
	case WM_CREATE:
	{
		std::vector<Remote::RemoteSession>* newRData = new std::vector<Remote::RemoteSession>;
		Remote::RemoteSession rData;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)newRData);
		rData.defaultSize = Theme::GetRect(hWnd);
		RECT r = { NULL };
		r.left = 14;
		r.right = 0;
		rData.exit = Theme::CWindow(r, NULL, L"CBUTTON", NULL, WS_VISIBLE | CS_EXIT, 768, 0, 30, 30, hWnd, (HMENU)IDM_EXIT);
		rData.mini = Theme::CWindow(r, NULL, L"CBUTTON", NULL, WS_VISIBLE | CS_MINI, 708, 0, 30, 30, hWnd, (HMENU)IDM_MINI);
		rData.maxi = Theme::CWindow(r, NULL, L"CBUTTON", NULL, WS_VISIBLE | CS_MAXI, 738, 0, 30, 30, hWnd, (HMENU)IDM_MAXI);
		rData.hWnd = hWnd;
		newRData->push_back(rData);

	}

	break;
	case WM_COMMAND:
	{
		
		int wmID = LOWORD(wParam);
		switch (wmID)
		{
		case IDB_CLOSEREMOTE:
		{
			if (vRemoteSessions->size() > 1)
			{
				int i = 0;
				BOOL FOUND = FALSE;
				auto rSS = vRemoteSessions->begin();
				for (rSS; rSS != vRemoteSessions->end(); rSS++, i++)
				{
					if (rSS->tab == (HWND)lParam)
					{
						DestroyWindow((HWND)lParam);
						rSS->tab = nullptr;
						if (rSS->ACTIVE) {
							rSS->ACTIVE = FALSE;
							while (vRemoteSessions->begin()->iStream[0] != nullptr)
								Sleep(1);
							vRemoteSessions->erase(vRemoteSessions->begin() + i);
							rSS = vRemoteSessions->begin();
							if (rSS->tab != nullptr)
							{
								if (!rSS->ACTIVE)
									SendMessage(rSS->tab, WM_MOUSEACTIVATE, NULL, 1);
								rSS->ACTIVE = TRUE;
							}
							else
								continue;
						}
						else {
							while (vRemoteSessions->begin()->iStream[0] != nullptr)
								Sleep(1);
							vRemoteSessions->erase(vRemoteSessions->begin() + i);
						}
						break;
					}
					else if (FOUND)

						if (rSS->tab != nullptr) {
							if (!rSS->ACTIVE)
								SendMessage(rSS->tab, WM_MOUSEACTIVATE, NULL, 1);
							rSS->ACTIVE = TRUE;
							break;
						}

					//else if (rSS->tab != nullptr) {
					//	rSS->ACTIVE = TRUE;
					//}
				}
				//reset tab location function
				resettablocations(vRemoteSessions);
				return 1;
			}
			else
				return 0;
		}
		break;
		case IDM_RTAB:
		{
			if (vRemoteSessions->size() > 1)
			{
				auto RemoteSeshSearch = vRemoteSessions->begin();
				BOOL FOUND = FALSE;
				for (RemoteSeshSearch; RemoteSeshSearch != vRemoteSessions->end(); RemoteSeshSearch++) //check for matching socket
				{

					if (RemoteSeshSearch->ACTIVE && RemoteSeshSearch->tab != (HWND)lParam) {
						SendMessage(RemoteSeshSearch->tab, WM_MOUSEACTIVATE, NULL, 1);
						RemoteSeshSearch->ACTIVE = FALSE;
						FOUND = TRUE;
						RemoteSeshSearch = vRemoteSessions->begin();
						if (RemoteSeshSearch->tab == (HWND)lParam) { //found match
							if (!RemoteSeshSearch->ACTIVE) {
								SendMessage(RemoteSeshSearch->tab, WM_MOUSEACTIVATE, NULL, 1);
							}
							RemoteSeshSearch->ACTIVE = TRUE;
							break;
						}

						//break;
					}
					if(FOUND)
						if (RemoteSeshSearch->tab == (HWND)lParam) { //found match
							if (!RemoteSeshSearch->ACTIVE) {
								SendMessage(RemoteSeshSearch->tab, WM_MOUSEACTIVATE, NULL, 1);
							}
							RemoteSeshSearch->ACTIVE = TRUE;
							break;
						}


					//break;
				}
			}

			//if (vRemoteSessions->begin()->ACTIVE) 
			//	SendMessage(vRemoteSessions->begin()->tab, WM_MOUSEACTIVATE, NULL, 1);
		}
			break;
		case WM_CREATE:
		{
			RECT r = { NULL };
			r.left = 14;
			r.right = 0;
			
			if (!lParam)
			{
				vRemoteSessions->begin()->tab = Theme::CWindow(r, NULL, L"CBUTTON", vRemoteSessions->begin()->CD.CDS.name.c_str(), WS_VISIBLE | CS_TAB, 0, 31, 75, 20, hWnd, (HMENU)IDM_RTAB);
				SendMessage(vRemoteSessions->begin()->tab, WM_MOUSEACTIVATE, NULL, 1);
				r = Theme::GetRect(hWnd);
				r.right -= 5;
				r.bottom -= 54;
				vRemoteSessions->begin()->rDraw = CreateWindow(L"RemoteDraw", vRemoteSessions->begin()->CD.CDS.name.c_str(), WS_VISIBLE | WS_CHILD, 3, r.top + 52, r.right, r.bottom, hWnd, nullptr, (HINSTANCE)GetModuleHandle(NULL), nullptr);
				
			}
			else
			{
				if (vRemoteSessions->size() > 1)
				{
					auto RemoteSeshSearch = vRemoteSessions->begin();

					for (RemoteSeshSearch; RemoteSeshSearch != vRemoteSessions->end(); RemoteSeshSearch++) //check for matching socket
					{
						if (RemoteSeshSearch->ACTIVE) { //found match
							int i = vRemoteSessions->size();

							RemoteSeshSearch->tab = Theme::CWindow(r, NULL, L"CBUTTON", RemoteSeshSearch->CD.CDS.name.c_str(), WS_VISIBLE | CS_TAB, 75 * (i - 1), 31, 75, 20, hWnd, (HMENU)IDM_RTAB);
							SendMessage(RemoteSeshSearch->tab, WM_MOUSEACTIVATE, NULL, 1);
							if(RemoteSeshSearch->MAX)
								ShowWindow(hWnd, SW_RESTORE);
							BringWindowToTop(hWnd);
							
								break;
						}
					}
				}

			}
			break;
		}
		case IDM_MAXI:
		{
			if (vRemoteSessions->begin()->MAX) {
				ShowWindow(hWnd, SW_SHOWNORMAL);
			}
			else{
				ShowWindow(hWnd, SW_MAXIMIZE);
			}
			vRemoteSessions->begin()->MAX = !vRemoteSessions->begin()->MAX;
			break;
		}
		case IDM_MINI: //minimize button case.
		{
			//TODO: add check for minimize to tray.
			SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL); //send minimize command to window. 
			break;
		}
		case IDM_EXIT: //exit button //need to handle closing and deleting remote sockets 

			DestroyWindow(hWnd);
			break;
		default:

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{

		RECT rect = Theme::GetRect(hWnd);
		SetWindowPos(vRemoteSessions->begin()->exit, 0, rect.right - 32, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		SetWindowPos(vRemoteSessions->begin()->maxi, 0, rect.right - 62, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		SetWindowPos(vRemoteSessions->begin()->mini, 0, rect.right - 92, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		int iOldState = SaveDC(hdc);
		HPEN hp = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		SelectObject(hdc, hp);
		MoveToEx(hdc, 0, 30, 0);
		LineTo(hdc, rect.right, 30);


		int i = rect.right / 2;
		SetTextColor(hdc, RGB(200, 200, 200));
		SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));


		WCHAR staticText[99];
		int len = SendMessage(hWnd, WM_GETTEXT,
			_ARRAYSIZE(staticText), (LPARAM)staticText);

		if (len == NULL)
			return FALSE;
		Theme::DrawTextC({ 11,7,0,15 }, hdc, staticText, len, 0);

		RestoreDC(hdc, iOldState);
		EndPaint(hWnd, &ps);
	}
		break;
	case WM_DESTROY: //need to handle closing and deleting remote sockets otherwise if you call remote again it will say its already a socket.
	{
		///TODO Move this chunk to the area where disconnected or lost clients are cleaned up.
		//Maybe?
		auto RemoteSeshSearch = vRemoteSessions->begin();
		
		int i = NULL;
		BOOL found = FALSE;
		vRemoteSessions->begin()->WINDOWACTIVE = FALSE;
		int s = 3;
		for (RemoteSeshSearch; RemoteSeshSearch != vRemoteSessions->end(); RemoteSeshSearch++, i++) //check for matching socket
		{
			
			send(RemoteSeshSearch->RemoteSendSocket, (char*)& s, sizeof(int), 0);
			closesocket(RemoteSeshSearch->RemoteReceiveSocket); //close sockets
			closesocket(RemoteSeshSearch->RemoteSendSocket);
			RemoteSeshSearch->RemoteReceiveSocket = INVALID_SOCKET;
			RemoteSeshSearch->RemoteSendSocket = INVALID_SOCKET;

		}

		m.lock();
		/*while (vRemoteSessions->begin()->iStream[0] != nullptr)
			Sleep(1);*/
		vRemoteSessions->clear();
		
		//std::vector<Remote::RemoteSession>().swap(*vRemoteSessions);
		vRemoteSessions = NULL;
		m.unlock();
		//DestroyWindow(hWnd);

		break;

	}



	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}




//int Remote::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
//{
//
//	UINT num = 0;
//	UINT size = 0;
//
//	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
//	Gdiplus::GetImageEncodersSize(&num, &size);
//	if (size == 0)
//		return -1;
//	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
//	if (pImageCodecInfo == NULL)
//		return -1;
//	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
//	for (UINT j = 0; j < num; ++j)
//	{
//		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
//		{
//			*pClsid = pImageCodecInfo[j].Clsid;
//			free(pImageCodecInfo);
//			return j;
//		}
//	}
//	free(pImageCodecInfo);
//	return 0;
//}