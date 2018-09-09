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

#ifndef _INC_CONN_H
#define _INC_CONN_H

#include "baboNet.h"
#include "time.h"

#ifdef WIN32
	#include "Winsock2.h"
#else
	#include "LinuxHeader.h"
#endif

#include "md5class.h"
#include "CThread.h"

class cIncConnection
{
private:
public:

	//FILE			*fp;

	fd_set			master;
	fd_set			fdwrite;

	timeval			Timeout;

	int				State;	//garde on est rendu ou dans la connection, 0 = entrein de connecter, doit envoyer la connID, 1 = en attente du port udp, 2 = renvoie du ok pour le port udp
	bool			UDPenabled;

	UINT4	ID;		//id unique de notre connection

	cIncConnection	*Next;
	cIncConnection	*Previous;

	bool			isConnected;

	unsigned short	UDPport;
	sockaddr_in		IP;
	int				NewFD;			//TCP new file descriptor

	int				*NbClients;
	int				*MaxClients;


	unsigned char	*NetBitField;
	int				TCPlistener;
	
	char			LastMessage[100];
	char			*LastError;

	UINT4	Pid;

	//char junk[32];

	cIncConnection(UINT4 id,unsigned char *netBitField,bool UDPenabled,int tcpListener,int *nbClient,int *maxClient,char *lastError);
	~cIncConnection(){}	

	void			CloseSocket(int socketFD);
	
	int				Update();

};












#endif
