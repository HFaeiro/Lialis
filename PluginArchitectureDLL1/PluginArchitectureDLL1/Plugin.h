#pragma once
#include <string>
#include "GlobalStructs.h"
#include "Remote.h"


class __declspec(dllexport) Plugin
{
public:

	Plugin() {};
	virtual ~Plugin() {};
	std::wstring PluginName;
	std::wstring ListViewName;
	bool usingListView = FALSE;
	bool activateOnLoad = FALSE;
	HMODULE PluginHandle = NULL;
	virtual void Initialize( SOCKET) = 0;
	virtual void destroy() = 0;


};

//#define DECLARE_PLUGIN(x) extern "C"{__declspec(dllexport) \
//PluginInterface * makePlugin(){ return new x;}}







class aPlugin : public Plugin {
public:
	aPlugin();
	virtual ~aPlugin();

	void Initialize( SOCKET);
	void destroy();

	std::vector<Remote::RemoteSession>* vRemoteSessionsData;
	//wchar_t* serialize(ClientData);
	ClientData deSerialize(wchar_t* data);
	BOOL GetCDS(SOCKET, ClientData&);
};

//wchar_t* aPlugin::serialize(ClientData cData)
//{
//	wchar_t* serBuffer = new wchar_t[1048]; //TODO: get exact string lengths before allocations in future
//	int bufferoffset = 0;
//	int stringLength = cData.CDS.name.length() + 1;
//
//	for (int i = 0; i < stringLength; i++)
//		serBuffer[i + bufferoffset] = cData.CDS.name[i];
//
//	bufferoffset += stringLength;
//	stringLength = cData.CDS.IP.length() + 1;
//	for (int i = 0; i < stringLength; i++)
//		serBuffer[i + bufferoffset] = cData.CDS.IP[i];
//
//	bufferoffset += stringLength;
//	stringLength = cData.CDS.OS.length() + 1;
//	for (int i = 0; i < stringLength; i++)
//		serBuffer[i + bufferoffset] = cData.CDS.OS[i];
//
//	bufferoffset += stringLength;
//	stringLength = cData.CDS.Country.length() + 1;
//	for (int i = 0; i < stringLength; i++)
//		serBuffer[i + bufferoffset] = cData.CDS.Country[i];
//
//	return serBuffer;
//}
inline ClientData aPlugin::deSerialize(wchar_t* data)
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

inline BOOL aPlugin::GetCDS(SOCKET s, ClientData& clientData)
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
