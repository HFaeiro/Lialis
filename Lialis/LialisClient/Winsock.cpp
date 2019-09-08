#include "stdafx.h"
#include "Winsock.h"


Winsock::Winsock()
{
	cHandler.ConnectionInfo = NULL;
	ConnectSocket = INVALID_SOCKET;
	//result = NULL;
	//ptr = NULL;
	Settings.port = 4444;//some kind of default port? whatever
	Settings.URL = L"127.0.0.1";
}

Winsock::Winsock(int port, wchar_t* URL)
{
	cHandler.ConnectionInfo = NULL;
	ConnectSocket = INVALID_SOCKET;
	Settings.port = port;
	Settings.URL = URL;
}

Winsock::~Winsock()
{
	if (ConnectSocket != INVALID_SOCKET) {
		closesocket(ConnectSocket);
		WSACleanup();
	}
}

int Winsock::Initialize()
{

	int iResult = NULL;
	ConnectSocket = INVALID_SOCKET;

	if ((iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
		return 1;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::wstringstream wss;
	wss << this->Settings.port;
	std::wstring wstr = wss.str();

	if ((iResult = GetAddrInfoW(this->Settings.URL.c_str(), wstr.c_str(), &hints, &cHandler.ConnectionInfo)) != 0) {
		WSACleanup();
		return 1;
	}

	if ((ConnectSocket = socket(cHandler.ConnectionInfo->ai_family, cHandler.ConnectionInfo->ai_socktype, cHandler.ConnectionInfo->ai_protocol)) == INVALID_SOCKET) {
		FreeAddrInfoW(cHandler.ConnectionInfo);
		WSACleanup();
		return 1;
	}
	cHandler.ConnectSocket = ConnectSocket;
	std::thread t1(&Winsock::InitialConnectThread, this);
	t1.detach();

	return 0;
}

void Winsock::InitialConnectThread()
{

	
	struct sockaddr_in ClientInfo = { 0 };
	int size = sizeof(ClientInfo);
	socklen_t sSize = sizeof(ClientInfo);

	while (TRUE) {
		SOCKET tmpSocket;
		if ((connect(ConnectSocket, cHandler.ConnectionInfo->ai_addr, cHandler.ConnectionInfo->ai_addrlen)) != SOCKET_ERROR) {
			//std::thread t2(&ConnectionHandler::OnConnect,cHandler, this->ConnectSocket);
			//t2.join();
			cHandler.OnConnect();
			int i = 5;
		}
		int wsaerr = WSAGetLastError();
		if (wsaerr == WSANOTINITIALISED || wsaerr == WSAENOTSOCK) break; // make sure to reinitialize
		if (wsaerr != WSAECONNREFUSED && wsaerr != 0) {

			Deinitialize();

			Initialize(); // if i do a full reinitialization i need to make sure i tell the server that this is a reconnect and to delete the previous connection from clientList
							//or save previous sockets to send and replace old sockets in server for "seemless reconnect"
			break;
		}
		//Sleep(5);
	}
	return;
}

void Winsock::Deinitialize()
{
	closesocket(ConnectSocket);
	ConnectSocket = INVALID_SOCKET;
	closesocket(cHandler.Remote.rData.sSend);
	cHandler.Remote.rData.sSend = INVALID_SOCKET;
	closesocket(cHandler.Remote.rData.sRecv);
	cHandler.Remote.rData.sRecv = INVALID_SOCKET;
	WSACleanup();
	cHandler.ConnectionInfo = { NULL };
	
}


