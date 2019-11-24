#include "stdafx.h"
#include "RemoteView.h"


RemoteView::RemoteView()
{
	rData.sRecv = INVALID_SOCKET;
	rData.sSend = INVALID_SOCKET;
	hGlobal = NULL;
	iStream = NULL;
	screen = 1;
	gLockStream = nullptr;
	bscreenShot = FALSE;
	prevSize = NULL;
	screenFlag = FALSE;
}


RemoteView::~RemoteView()
{

}

void RemoteView::RemoteHandler()
{

	Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, NULL);
	GetEncoderClsid(L"image/jpeg", &gdiCLSID);
	int size = NULL;

	this->RemoteSend();
	this->RemoteRecv();
	//void* buffer = GrabScreen( size);
	//GrabScreen();
	//std::thread t1(&RemoteView::RemoteRecv, this);
	//t1.detach();
	//this->RemoteSend();

}

void RemoteView::RemoteSend()
{
	std::vector<char> vBuffer;
	int count = 0;
	int vBSize = 0;

	//do {

		GrabScreen();
		if (prevSize)
		{
			if (prevSize == size && !screenFlag)
			{
				GlobalUnlock(hGlobal);
				DeleteObject(hGlobal);
				GlobalUnlock(gLockStream);
				iStream->Release();
				RemoteSend();

				return;
			}
		}
		if (!gLockStream)
			return;
		if (size > 0) {
			vBuffer.resize(size + vBSize + 1);

			for (int i = 0; i <= size; i++)
				vBuffer[vBSize + i] = ((char*)gLockStream)[i];
			//vBSize += size;
			//count++;
			int err = WSAGetLastError();
			GlobalUnlock(hGlobal);
			DeleteObject(hGlobal);
			GlobalUnlock(gLockStream);
			iStream->Release();
			//} while (count < 2);
			vBSize = vBuffer.size();
			int sent = send(this->rData.sSend, (char*)& vBSize, sizeof(int), 0);
			sent = send(this->rData.sSend, (char*)& vBuffer[0], vBSize, 0);



			if (sent < 0)
			{

				closesocket(this->rData.sRecv);
				closesocket(this->rData.sSend);
				this->rData.sRecv = INVALID_SOCKET;
				this->rData.sSend = INVALID_SOCKET;
				return;

			}
		}
		
		//Sleep(20);

	//}
		screenFlag = FALSE;
		prevSize = size;
	return;
}
BOOL CALLBACK MonitorEnumProcCallback(_In_  HMONITOR hMonitor, _In_  HDC DevC, _In_  LPRECT lprcMonitor, _In_  LPARAM dwData)
{
	reinterpret_cast<RemoteView*>(dwData)->EnumProcCallback(hMonitor, DevC, lprcMonitor);
	return true;
}
BOOL CALLBACK RemoteView::EnumProcCallback(_In_  HMONITOR hMonitor, _In_  HDC DevC, _In_  LPRECT lprcMonitor) {
	screenCounter++;

	std::wstring bmpName;

	if (((screenCounter == screen) && (bscreenShot == FALSE)) || ((screenCounter == ssScreen) && (bscreenShot == TRUE))) {
		
		MONITORINFO  info;
		info.cbSize = sizeof(MONITORINFO);

		BOOL monitorInfo = GetMonitorInfo(hMonitor, &info);
		if (monitorInfo) {

			DWORD Width = info.rcMonitor.right - info.rcMonitor.left;
			DWORD Height = info.rcMonitor.bottom - info.rcMonitor.top;

			DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (Width * Height * 4));
			char* BmpFileData = (char*)GlobalAlloc(0x0040, FileSize);

			PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)BmpFileData;
			PBITMAPINFOHEADER  BInfoHeader = (PBITMAPINFOHEADER)& BmpFileData[sizeof(BITMAPFILEHEADER)];

			BFileHeader->bfType = 0x4D42; // BM
			BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
			BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
			BInfoHeader->biPlanes = 1;
			BInfoHeader->biBitCount = 24;
			BInfoHeader->biCompression = BI_RGB;
			BInfoHeader->biHeight = Height;
			BInfoHeader->biWidth = Width;


			HDC CaptureDC = CreateCompatibleDC(DevC);
			HBITMAP screenShot = CreateCompatibleBitmap(DevC, Width, Height);
			SelectObject(CaptureDC, screenShot);
			BitBlt(CaptureDC, 0, 0, Width, Height, DevC, info.rcMonitor.left, info.rcMonitor.top, SRCCOPY | CAPTUREBLT);


			if (Gdiplus::DllExports::GdipCreateBitmapFromHBITMAP(screenShot, NULL, &pBitmap))
			{
				DeleteObject(pBitmap);
				DeleteObject(CaptureDC);
				DeleteObject(screenShot);
				
				Gdiplus::GdiplusShutdown(gdiToken);
				return FALSE;

			}
			CreateStreamOnHGlobal(NULL, TRUE, &iStream);
			GetHGlobalFromStream(iStream, &hGlobal);
			if (bscreenShot) {

				std::wstringstream wss;
				wss << L"screen ";
				wss << ssScreen;
				wss << L".png";
				bmpName = wss.str();
				Gdiplus::DllExports::GdipSaveImageToFile(pBitmap, bmpName.c_str(), &gdiCLSID, NULL);
				DeleteObject(CaptureDC);
				DeleteObject(screenShot);
				Gdiplus::DllExports::GdipDisposeImage(pBitmap);
				GlobalFree(BmpFileData);
				bscreenShot = FALSE;
				return FALSE;

			}

			Gdiplus::DllExports::GdipSaveImageToStream(pBitmap, iStream, &gdiCLSID, NULL);
			
			gLockStream = (char*)GlobalLock(hGlobal);
			
			STATSTG stat;
			iStream->Stat(&stat, NULL);
			size = stat.cbSize.LowPart;
			if (size > 500000000)
			{
				size++;
				return FALSE;
			}
			///////////////////////////////////////////////////////////////////////////////////////
			
			DeleteObject(CaptureDC);
			DeleteObject(screenShot);
			Gdiplus::DllExports::GdipDisposeImage(pBitmap);
			GlobalFree(BmpFileData);
			return FALSE;
		}
	}
	return TRUE;
}
void RemoteView::ScreenShot(int i)
{
	Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, NULL);
	GetEncoderClsid(L"image/png", &gdiCLSID);
	ssScreen = i;
	bscreenShot = TRUE;
	HDC DevC = GetDC(NULL);
	screenCounter = 0;

	EnumDisplayMonitors(DevC, NULL, MonitorEnumProcCallback, (LPARAM)this);
	return;
}
void RemoteView::RemoteRecv()
{
	while (true) {
		int i = NULL;
		int check = recv(this->rData.sRecv, (char*)&i, sizeof(int), 0);
		if (check < 0)
			return;
		switch (i)
		{
		case 1://switch screens
		{
			i = NULL;
			check = recv(this->rData.sRecv, (char*)&screen, sizeof(int), 0);
			if (check < 0)
				return;
			screenFlag = TRUE;
			break;
		}
		case 2://ready for screen
		{
			std::thread t1(&RemoteView::RemoteSend, this);
			t1.detach();
		}
		break;
		case 3:
		{
			closesocket(this->rData.sRecv);
			closesocket(this->rData.sSend);
			this->rData.sRecv = INVALID_SOCKET;
			this->rData.sSend = INVALID_SOCKET;
			return;
		}
		default:
			break;
		}
		Sleep(5);
	}
	return;
}
void* RemoteView::GrabScreen()
{
	RECT r;
	HDC DevC = GetDC(NULL);
	screenCounter = 0;
	
	EnumDisplayMonitors(DevC, NULL, MonitorEnumProcCallback, (LPARAM)this);

	return (char*)gLockStream;
	

}

int RemoteView::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{

	UINT num = 0;
	UINT size = 0;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return 0;
}