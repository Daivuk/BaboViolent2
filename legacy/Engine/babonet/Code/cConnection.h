/*
	Copyright 2012 bitHeads inc.

	This file is part of the BaboViolent 2 source code.

	The BaboViolent 2 source code is free software: you can redistribute it and/or 
	modify it under the terms of the GNU General Public License as published by the 
	Free Software Foundation, either version 3 of the License, or (at your option) 
	any later version.

	The BaboViolent 2 source code is distributed in the hope that it will be useful, 
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with the 
	BaboViolent 2 source code. If not, see http://www.gnu.org/licenses/.
*/

#ifndef _CONNECTION__
#define	_CONNECTION__

#include "platform_types.h"
#include <fcntl.h>

#ifdef WIN32
	#define amax max
	#include "Winsock2.h"
	#include "ws2tcpip.h"
#else
	#include "LinuxHeader.h"
#endif

#include "cUDPpacket.h"
#include "cPacket.h"

class cConnection
{
private:

	#ifdef USING_LOG
		FILE * LogFile;
	#endif

	float			ConTimeout;			//timeout for the connection, if it reached 5, we kill the thread connect()ing
	int			ConnectResult;			// 0 = success  1 = echec
	bool			isConnecting;		// currently connecting of not

	int	StartConnection(); // initaite the connection process
	int	UpdateConnecting(float elapsed);	//update the connection process ( check result if its finished )

	fd_set			m_conSet; // temp fdset for connection polling

public:


	float			LastCheck;			//savoir le nombre de temps qui s'est ecouler

	char			RecvBuf[64];		//buffer qui va recevoir le incoming data
	int			ToRecv;					//nombre de byte a recevoir

	int			State;					//savoir on est rendu ou avec notre connection

	UINT4		connID;			//garde le ID de la remote connection
	UINT4		*Lpid;			//LastPAcketID
	//les info du client qui essaye de se connecter

	timeval		*Timeout;			//pointeur sur une struct qui tien le timeout
	sockaddr_in *RemoteIP;			//pointeur sur les infos du host auquel on se connect
	int			*UDPfd;				//pointeur sur le filedescriptor UDP du client
	int			*FileDescriptor;	//pointeur sur le filedescriptor du client
	bool		isServer;			//est-ce que notre client est serveur ?
	char		*LastError;			//derniere erreur cote client
	fd_set		*Master;
	fd_set		*Write;
	fd_set		*Read;
	int			*FDmax;
	bool		*UDPenabled;


	cConnection(bool *UDPenabled,timeval *Timeout,sockaddr_in *remoteIP,int *filedescriptor,int *udpFD,char *lastError,fd_set *master,fd_set *read,fd_set *write,int *fdmax,UINT4 *LastPacketID);
	
	int			Update(float elapsed);			// donne de lattention anotre connection process
	int			PrepareSocket();			//va setter les param de notre socket TCP
	int			PrepareUDP();				//socket UDP
	void			CloseSocket(int fd);			//closes a socket (cross platform)
};

#endif
