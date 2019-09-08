#include "stdafx.h"
#include "ConnectionHandler.h"
#include "Plugin.h"

ConnectionHandler::ConnectionHandler()
{
	ListViewHWND = NULL;
}


ConnectionHandler::~ConnectionHandler()
{
}

void ConnectionHandler::onClientConnect(SOCKET s) //called when a client connects with a new connection. 
{
	
	int packetType = NULL;
	recv(s, (char*)&packetType, sizeof(int), 0);
	switch (packetType) {
	case 0: //recv first connect client data
	{

		ClientData clientData;
		if (!GetCDS(s, clientData))
			return;
		if (clientData.CDS.name.empty()) { //if bad info close socket.
			closesocket(s);
			break;
		}
		send(s, (char*)&s, sizeof(SOCKET), 0); //send the client back its socket. 
		clientData.s = s;
		SendMessage(ListViewHWND, IDB_ADD, 0, (LPARAM)&clientData); //tell listview to add this item.
	}
	break;
	
	default: //undefined behavior
		auto vSearch = vPlugins.begin();
		int i = 0;
		for (vSearch; vSearch != vPlugins.end(); vSearch++, i++)
		{
			if (vSearch[0]->usingListView == packetType)
			{
				//Normally here we would setup the remote client data. nah this if for the birds. tell the plugin to do it like it should.
				SendMessage(GetParent(ListViewHWND), WM_PLUGININIT, i, s);
				return;
			}
		}

		
		closesocket(s);
		break;
	}
	return;
}

void ConnectionHandler::ClientActivator(LPVOID pcData, int type)
{
	ClientData* cData = (ClientData*)pcData;

	send(cData->s, (char*)& type, sizeof(int), 0);
	int isSocket = -1;
	int check = recv(cData->s, (char*)& isSocket, sizeof(int), 0);
	if (isSocket == 0)
	{

		return;
	}
	if (check < 0)
		return;
	
	//need to handle else code that isn't hardcoded for remote.

	return;
}

wchar_t* ConnectionHandler::serialize(ClientData cData)
{
	wchar_t* serBuffer = new wchar_t[1048]; //TODO: get exact string lengths before allocations in future
	int bufferoffset = 0;
	int stringLength = cData.CDS.name.length() + 1;

	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData.CDS.name[i];

	bufferoffset += stringLength;
	stringLength = cData.CDS.IP.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData.CDS.IP[i];

	bufferoffset += stringLength;
	stringLength = cData.CDS.OS.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData.CDS.OS[i];

	bufferoffset += stringLength;
	stringLength = cData.CDS.Country.length() + 1;
	for (int i = 0; i < stringLength; i++)
		serBuffer[i + bufferoffset] = cData.CDS.Country[i];

	return serBuffer;
}

ClientData ConnectionHandler::deSerialize(wchar_t* data)
{
	ClientData cData;
	int bufferOffset = 0;
	for (int i = 0; data[i] != '\0'; i++)
		cData.CDS.name += data[i];
	bufferOffset += cData.CDS.name.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.CDS.IP += data[i + bufferOffset];
	bufferOffset += cData.CDS.IP.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.CDS.OS += data[i + bufferOffset];
	bufferOffset += cData.CDS.OS.length() + 1;
	for (int i = 0; data[i + bufferOffset] != '\0'; i++)
		cData.CDS.Country += data[i + bufferOffset];
	
	return cData;
}

BOOL ConnectionHandler::GetCDS(SOCKET s, ClientData& clientData)
{
	int sel = NULL;
	ULONG BytesToRead = 0;
	int count = 0;
	while (sel == NULL) {

		fd_set byteCheck;
		timeval conTime;
		conTime.tv_sec = 2;
		conTime.tv_usec = 0;

		FD_ZERO(&byteCheck);
		FD_SET(s, &byteCheck);

		if ((sel = select(NULL, &byteCheck, NULL, NULL, &conTime)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAECONNABORTED)
				return FALSE;
		}

		else if (sel > 0) {
			ioctlsocket(s, FIONREAD, &BytesToRead); //Get # of bytes to read //# of bytes being sent from client 
			if (BytesToRead == NULL)
				return FALSE;
		}
		if (count > 10)
		{
			MessageBox(NULL, L"Socket Timeout", NULL, NULL);
			break;
		}
		count++;
	}
	wchar_t* buffer = new wchar_t[BytesToRead + (1 * sizeof(wchar_t))]; //allocate butter to how many bytes will be needed. 
	int read = NULL;
	int checkRead = NULL;
	while (read != BytesToRead) //receive bytes untill all data is received.
	{
		checkRead = recv(s, (char*)buffer + read, BytesToRead, 0);
		if (checkRead == -1)
		{
			delete[] buffer;
			return FALSE;
		}
		read += checkRead;

	}
	clientData = deSerialize(buffer); //fill data structure from buffer.
	delete[] buffer;
	return TRUE;
}