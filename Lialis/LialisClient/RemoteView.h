#pragma once
#include <vector>
class RemoteView
{
public:
	RemoteView();
	~RemoteView();


	struct RemoteData
	{
		SOCKET sSend;
		SOCKET sRecv;
	}rData;
BOOL CALLBACK EnumProcCallback(_In_  HMONITOR hMonitor, _In_  HDC DevC, _In_  LPRECT lprcMonitor);
	void RemoteHandler();
	void ScreenShot(int);
private:
	int screenCounter;
	HGLOBAL hGlobal;
	LPSTREAM iStream;
	int screen;
	CLSID gdiCLSID;
	void RemoteSend();
	void RemoteRecv();
	int GetEncoderClsid(const WCHAR*, CLSID*);
	void* GrabScreen();
	void* gLockStream;
	int size;
	Gdiplus::GdiplusStartupInput gdiInput;
	ULONG_PTR gdiToken;
	Gdiplus::GpBitmap* pBitmap;
	BOOL bscreenShot;
	int ssScreen;
	int prevSize;
	BOOL screenFlag;

	


};

