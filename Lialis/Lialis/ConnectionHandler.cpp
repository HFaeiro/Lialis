#include "stdafx.h"
#include "ConnectionHandler.h"


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
	case tRemote: //activate remote session
	{
		BOOL rSocket = FALSE;
		int check = recv(s, (char*)&rSocket, sizeof(int), 0); //receive whether or not client already has a connection socket.
		if (rSocket) //check if client has socket 
		{
			//client already has a socket. see if this socket is replacing old send socket or creating a send socket. 
			//FALSE for replacing, TRUE for recv socket.
			recv(s, (char*)&rSocket, sizeof(BOOL), 0);

			//check for socket in our vector.
			SOCKET clientsSocket = NULL;
			recv(s, (char*)&clientsSocket, sizeof(SOCKET), 0); //get clients remote socket.
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
			//Remote* remoteSesh = new Remote();
			Remote::RemoteSession rData;// = new Remote::RemoteSession();
			rData.RemoteSendSocket = INVALID_SOCKET;
			recv(s, (char*)& rData.ClientsSocket, sizeof(SOCKET), 0); //receive clients socket


			recv(s, (char*)& rData.numScreens, sizeof(int), 0); //receive number of screens on client end

			if (!GetCDS(s, rData.CD))
				return;


			//rData.RemoteSendSocket = s;
			rData.RemoteReceiveSocket = INVALID_SOCKET;

			//Remote::RemoteSession* rData = new Remote::RemoteSession();
			Remote::RemoteSession* tmpRData = nullptr; //(Remote::RemoteSession*)GetWindowLongPtr(rData->hWnd, GWLP_USERDATA);

			BOOL hWndShared = FALSE;
			if (!this->vRemoteSessionsData || this->vRemoteSessionsData->size() < 1)
			{
				
				rData.hWnd = (HWND)SendMessage(GetParent(this->ListViewHWND), WM_CREATE_REMOTE_WINDOW, NULL, NULL); //create remote window
				
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
				
				//rData = (Remote::RemoteSession*)GetWindowLongPtr(remoteSesh->rData.hWnd, GWLP_USERDATA);
				//rData->hWnd = remoteSesh->rData.hWnd;
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
				//rData->iStream = tmpRData->iStream;
				//SetWindowLongPtr(rData->hWnd, GWLP_USERDATA, (LONG_PTR)rData);
				//hWndShared = TRUE;
				
				auto tmpRSesh = this->vRemoteSessionsData->begin();
				for (tmpRSesh; tmpRSesh != this->vRemoteSessionsData->end(); tmpRSesh++)
				{
					if (tmpRSesh->ACTIVE) {
						tmpRSesh->ACTIVE = FALSE;
						SendMessage(tmpRSesh->tab, WM_MOUSEACTIVATE, NULL, 1);
					}
				}

				rData.ACTIVE = TRUE;
				//while (this->vRemoteSessionsData->begin()->iStream[0] != nullptr)
				//	Sleep(1);
				this->vRemoteSessionsData->push_back(rData);
				SendMessage(rData.hWnd, WM_COMMAND, WM_CREATE, (LPARAM)&rData);
				

				
			}

			int erro = GetLastError();


			BOOL ready = TRUE;
			send(s, (char*)& ready, sizeof(int), 0);


		}

		break;
	}
	default: //undefined behavior
		closesocket(s);
		break;
	}
	return;
}

void ConnectionHandler::ClientActivator(LPVOID pcData, int type)
{
	ClientData* cData = (ClientData*)pcData;
	switch (type)
	{
	case tRemote: //Tell Client to activate remote session. TODO: check if remote session is already active.
	{
		send(cData->s, (char*)&type, sizeof(int), 0);
		int isSocket = -1;
		int check = recv(cData->s, (char*)&isSocket, sizeof(int), 0);
		if (isSocket == 0) //socket is being created. nothing more to do here.
			break;
		if (check < 0 )	break;
		else //client says socket already exists, lets check.
		{
			SOCKET remoteSocket = NULL;
			recv(cData->s, (char*)&remoteSocket, sizeof(SOCKET), 0); //recv socket number from client.
			if (this->vRemoteSessionsData->size() > 0)
			{
				BOOL scanResult = FALSE;
				//scan through this vector for the socket.			//check our vRemoteSessionData vector for this socket. 
				auto remoteVectorScan = vRemoteSessionsData->begin();
				for ( remoteVectorScan ; remoteVectorScan != vRemoteSessionsData->end(); remoteVectorScan++)
				{
					if (remoteVectorScan->ClientsSocket == remoteSocket)
					{
						scanResult = TRUE;
						ShowWindow(remoteVectorScan->hWnd, SW_SHOW);
						break;
					}
				}
				//if this socket is not valid or does not exist tell the client by sending false and break
				//else send true and tell user that this client already has a remote session and break
				send(cData->s, (char*)&scanResult, sizeof(BOOL), 0);
				if (scanResult)
					MessageBox(ListViewHWND, L"Remote Session already open for this Client.", NULL, MB_OK);
				else
					ClientActivator(pcData, type);
				break;
			}
			else
			{
				isSocket = 0;
				send(cData->s, (char*)&isSocket, sizeof(int), 0);
				recv(cData->s, (char*)& isSocket, sizeof(int), 0);
				if(isSocket == WSAEISCONN)
					ClientActivator(pcData, ConnectionHandler::PacketType::tRemote);
				break;
			}
		}
	}
		break;
	default:
		return;
	}
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