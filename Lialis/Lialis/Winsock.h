#pragma once
#include "ConnectionHandler.h"
class Winsock
{
public:
	Winsock(HWND);
	~Winsock();
	int Initialize();
	void ListenThread();
	void Deinitialize();
	struct Settings
	{
		int port;
		wchar_t URL[99] = L"127.0.0.1";
	}Settings;
	std::thread* listenThread;
	ConnectionHandler cHandler;
private:
	WSADATA wsaData;
	struct addrinfoW *result, *ptr, hints; //look into addrinfoexW
	SOCKET ListenSocket;
	void TimeOutThread();


};

