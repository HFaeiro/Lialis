#pragma once
#include "Resource.h"
//#include "Remote.h"
#include <mutex>
class ConnectionHandler
{
public:
	std::mutex mvRSD; 
	ConnectionHandler();
	~ConnectionHandler();
	HWND ListViewHWND; 
	
private:

	//struct ClientData {
	//	SOCKET s;// , remoteSocket;
	//	struct ClientDataStrings
	//	{
	//		std::wstring name, IP, OS, Country;
	//	}CDS;
	//};

	//std::vector<Remote::RemoteSession>* vRemoteSessionsData;

	wchar_t* serialize(ClientData);
	ClientData deSerialize(wchar_t* data);
	BOOL GetCDS(SOCKET, ClientData&);
public:
	void onClientConnect(SOCKET);
	void ClientActivator(LPVOID, int);

};

