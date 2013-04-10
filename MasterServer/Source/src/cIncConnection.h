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

	unsigned long	ID;		//id unique de notre connection

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

	unsigned long	Pid;

	//char junk[32];

	cIncConnection(unsigned long id,unsigned char *netBitField,bool UDPenabled,int tcpListener,int *nbClient,int *maxClient,char *lastError);
	~cIncConnection(){}	

	void			CloseSocket(int socketFD);
	
	int				Update();

};












#endif
