#include "stdafx.h"
#include "Winsock.h"
#pragma comment(lib,"winmm.lib")

Winsock::Winsock(HWND ListViewHWND)
{
	result = NULL;
	ptr = NULL;
	Settings.port = 4444;//some kind of default port? whatever
	
	cHandler.ListViewHWND = ListViewHWND;
}


Winsock::~Winsock()
{
	closesocket(ListenSocket);
	WSACleanup();
}

int Winsock::Initialize()
{

	int iResult = NULL;
	ListenSocket = INVALID_SOCKET;

	if ((iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		MessageBox(NULL, L"WSAStartup Failed!", NULL , NULL);//convert iResult to wstring for proper error messaging
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::wstringstream wss;
	wss << this->Settings.port;
	std::wstring wstr = wss.str();

	if ((iResult = GetAddrInfoW(this->Settings.URL, wstr.c_str(), &hints, &result)) != 0) {
		MessageBox(NULL, L"GetAddrInfoW Failed!", NULL, NULL);//convert iResult to wstring for proper error messaging
		WSACleanup();
		return 1;
	}

	((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr = htonl(INADDR_ANY);

	if ((ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET) {
		MessageBox(NULL, L"CreateSocket Failed!", NULL, NULL);//convert iResult to wstring for proper error messaging
		FreeAddrInfoW(result);
		WSACleanup();
		return 1;
	}

	if ((iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen)) == SOCKET_ERROR) {
		MessageBox(NULL, L"Bind Failed!", NULL, NULL);//convert iResult to wstring for proper error messaging
		FreeAddrInfoW(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	FreeAddrInfoW(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		MessageBox(NULL, L"Listen Failed!", NULL, NULL);//convert iResult to wstring for proper error messaging
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	std::thread t1(&Winsock::ListenThread, this);
	t1.detach();

	return 0;
}

void Winsock::ListenThread()
{

	
	struct sockaddr_in ClientInfo = { 0 };
	int size = sizeof(ClientInfo);
	socklen_t sSize = sizeof(ClientInfo);
	std::thread t1(&Winsock::TimeOutThread, this);
	t1.detach();
	while (TRUE) {
		SOCKET tmpSocket;
		
		if ((tmpSocket = accept(this->ListenSocket, (sockaddr*)& ClientInfo, &size)) != INVALID_SOCKET) {
			//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sInit._cHandler->Handle_Client, (LPVOID)_Handle, NULL, NULL);
			std::thread t2(&ConnectionHandler::onClientConnect, std::ref(cHandler), tmpSocket);
			t2.detach();
		}
		if ((WSAGetLastError()) == WSANOTINITIALISED) break;

		//


		//}
	}
	return;
}
void Winsock::TimeOutThread()
{
	DWORD time = timeGetTime();
	DWORD CheckTime = time + 1500;
	while (true) {

		time = timeGetTime();
		if (time >= CheckTime) {
			std::vector<ClientData>* cData = (std::vector<ClientData>*)SendMessage(this->cHandler.ListViewHWND, IDR_GETCLIENTLIST, NULL, NULL);

			int check = 0;
			int ping = 500;
			std::vector<int> Clients;
			if (cData->size())
			{
				auto tmpCD = cData->begin();
				for (int i = 0; tmpCD != cData->end(); tmpCD++, i++)
				{

					if ((send(tmpCD->s, (char*)& ping, sizeof(int), 0) < 0))
					{
						
						Clients.push_back(i);
						//tell listview to remove this client from the list.
						//SendMessage(this->cHandler.ListViewHWND, IDB_REMOVE, TRUE, i);
					}

				}
				if(Clients.size())
					SendMessage(this->cHandler.ListViewHWND, IDB_REMOVE, (WPARAM)&Clients, NULL);
			}

			CheckTime += 1500;
		}
		Sleep(10);
	}
}
void Winsock::Deinitialize()
{
	closesocket(ListenSocket);
	ListenSocket = NULL;
	WSACleanup();
	
}


