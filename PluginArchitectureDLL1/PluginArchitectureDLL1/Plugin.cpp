#include "pch.h"
#include "Plugin.h"
#include "Remote.h"

//void aPlugin::Initialize()
//{
//	printf("%s", PluginName);
//
//}

aPlugin::aPlugin()
{
}

aPlugin::~aPlugin()
{
}

void aPlugin::Initialize(SOCKET s)
{
	
	Theme::RClass(CS_HREDRAW | CS_VREDRAW, RemoteProc, (HINSTANCE)GetModuleHandle(NULL),
		LoadCursor(nullptr, IDC_ARROW) , LoadIcon((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)), CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C)), L"Remote Desktop", L"Remote", sizeof(Remote::RemoteSession));

	Theme::RClass(CS_HREDRAW | CS_VREDRAW, DrawProc, (HINSTANCE)GetModuleHandle(NULL),
		LoadCursor(nullptr, IDC_ARROW), LoadIcon((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)), CreateSolidBrush(RGB(0x2C, 0x2C, 0x2C)), L"Remote Desktop", L"RemoteDraw", sizeof(Remote::RemoteSession));

	BOOL rSocket = FALSE;
	int check = recv(s, (char*)& rSocket, sizeof(int), 0); //receive whether or not client already has a connection socket.
	if (rSocket) //check if client has socket 
	{
		//client already has a socket. see if this socket is replacing old send socket or creating a send socket. 
		//FALSE for replacing, TRUE for recv socket.
		recv(s, (char*)& rSocket, sizeof(BOOL), 0);

		//check for socket in our vector.
		SOCKET clientsSocket = NULL;
		recv(s, (char*)& clientsSocket, sizeof(SOCKET), 0); //get clients remote socket.
		for (auto RemoteSeshSearch = this->vRemoteSessionsData->begin(); RemoteSeshSearch != vRemoteSessionsData->end(); RemoteSeshSearch++) //check for matching socket
		{
			if (RemoteSeshSearch->ClientsSocket == clientsSocket) //socket matched. overwrite previous remote socket.
				if (rSocket)
				{
					RemoteSeshSearch->RemoteReceiveSocket = s;
					break;
				}
				else
				{
					RemoteSeshSearch->RemoteSendSocket = s;

					break;
				}
		}
	}
	else //client had no previous remote session active. we need to create one.
	{
		
		Remote::RemoteSession rData;
		rData.RemoteSendSocket = INVALID_SOCKET;
		recv(s, (char*)& rData.ClientsSocket, sizeof(SOCKET), 0); //receive clients socket


		recv(s, (char*)& rData.numScreens, sizeof(int), 0); //receive number of screens on client end

		if (!GetCDS(s, rData.CD))
			return;


	
		rData.RemoteReceiveSocket = INVALID_SOCKET;

		
		Remote::RemoteSession* tmpRData = nullptr; 

		BOOL hWndShared = FALSE;
		if (!this->vRemoteSessionsData || this->vRemoteSessionsData->size() < 1)
		{

			rData.hWnd = CreateWindow(L"Remote", L"Remote Session", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, nullptr, (HINSTANCE)GetModuleHandle(NULL), nullptr);

			this->vRemoteSessionsData = (std::vector<Remote::RemoteSession>*)GetWindowLongPtr(rData.hWnd, GWLP_USERDATA);
			this->vRemoteSessionsData->begin()->CD = rData.CD;
			this->vRemoteSessionsData->begin()->ClientsSocket = rData.ClientsSocket;
			this->vRemoteSessionsData->begin()->numScreens = rData.numScreens;
			this->vRemoteSessionsData->begin()->RemoteSendSocket = s;
			this->vRemoteSessionsData->begin()->RemoteReceiveSocket = INVALID_SOCKET;
			this->vRemoteSessionsData->begin()->ACTIVE = TRUE;

			SendMessage(rData.hWnd, WM_COMMAND, WM_CREATE, (LPARAM)FALSE);

			SendMessage(this->vRemoteSessionsData->begin()->hWnd, WM_INIT_REMOTE, NULL, (LPARAM)this->vRemoteSessionsData);
			ShowWindow(rData.hWnd, SW_SHOW);
			UpdateWindow(rData.hWnd);
		}
		//
		else {

			auto tmpRData = this->vRemoteSessionsData->begin();

			rData.hWnd = tmpRData->hWnd;
			rData.RemoteSendSocket = s;
			rData.rDraw = tmpRData->rDraw;
			rData.maxi = tmpRData->maxi;
			rData.mini = tmpRData->mini;
			rData.exit = tmpRData->exit;
			

			auto tmpRSesh = this->vRemoteSessionsData->begin();
			for (tmpRSesh; tmpRSesh != this->vRemoteSessionsData->end(); tmpRSesh++)
			{
				if (tmpRSesh->ACTIVE) {
					tmpRSesh->ACTIVE = FALSE;
					SendMessage(tmpRSesh->tab, WM_MOUSEACTIVATE, NULL, 1);
				}
			}

			rData.ACTIVE = TRUE;
			this->vRemoteSessionsData->push_back(rData);
			SendMessage(rData.hWnd, WM_COMMAND, WM_CREATE, (LPARAM)& rData);



		}

		int erro = GetLastError();


		BOOL ready = TRUE;
		send(s, (char*)& ready, sizeof(int), 0);
	}
	
	return;

}

void aPlugin::destroy()
{
}


