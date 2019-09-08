#pragma once
#include "ConnectionHandler.h"
class Winsock
{
public:
	Winsock();
	Winsock(int, wchar_t*);
	~Winsock();
	int Initialize();
	
	void InitialConnectThread();
	void Deinitialize();
	struct Settings
	{
		int port;
		std::wstring URL;
	}Settings;
	std::thread* listenThread;
	ConnectionHandler cHandler;
	
private:
	WSADATA wsaData;
	struct addrinfoW *result, *ptr, hints; //look into addrinfoexW
	SOCKET ConnectSocket;
	


};

