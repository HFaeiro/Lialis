#pragma once
#include "Resource.h"
#include "Theme.h"
#include "GlobalStructs.h"

LRESULT CALLBACK RemoteProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DrawProc(HWND, UINT, WPARAM, LPARAM);

void DrawD2DImage(ID2D1Factory* d2dFactory, IWICImagingFactory* d2dWICFactory, ID2D1HwndRenderTarget* d2drender, HWND hWnd, LPSTREAM& istream);
class Remote
{
public:
	Remote();
	~Remote();

	struct RemoteSession
	{
		BOOL ACTIVE;
		BOOL WINDOWACTIVE;
		RECT defaultSize;
		SOCKET RemoteSendSocket;
		SOCKET ClientsSocket;
		SOCKET RemoteReceiveSocket;
		int numScreens;
		HWND hWnd;
		//LPSTREAM iStream;
		HWND exit;
		HWND mini;
		HWND maxi;
		HWND tab;
		HWND rDraw;
		BOOL MAX;
		BOOL WindowSizeChange;
		ClientData CD;
		std::vector<LPSTREAM> iStream;
		RemoteSession()
		{
			WINDOWACTIVE = TRUE;
			ACTIVE = FALSE;
			defaultSize = { 0 };
			RemoteSendSocket = INVALID_SOCKET;
			ClientsSocket = INVALID_SOCKET;
			RemoteReceiveSocket = INVALID_SOCKET;
			numScreens = NULL;
			hWnd = NULL;
			iStream.resize(5);
			exit = NULL;
			mini = NULL;
			maxi = NULL;
			tab = NULL;
			rDraw = NULL;
			MAX = FALSE;
			WindowSizeChange = FALSE;
		}
	}rData;
	 std::vector<RemoteSession>* Vector;
	void RemoteHandler(std::vector<RemoteSession>*);
private:

	ID2D1Factory* d2dFactory = NULL;
	IWICImagingFactory* d2dWICFactory;
	//ID2D1HwndRenderTarget* d2drender = NULL;
	//CLSID gdiCLSID;
	//ULONG_PTR gdiToken;
	//Gdiplus::GdiplusStartupInput gdiInput;
	
	enum PacketType { Screen, Mouse};
	int GetEncoderClsid(const WCHAR*, CLSID*);

	void RemoteSend(std::vector<RemoteSession>*);
	int RemoteReceive(RemoteSession&);
	void D2DUpdater(std::vector<RemoteSession>*&);
};

