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

#ifndef _SERVER_H
#define _SERVER_H

#include "baboNet.h"
#include "cClient.h"
#include "cIncConnection.h"


#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif


//notre class serveur
class cServer
{
private:

	fd_set			UDPmaster;			// master file descriptor list for Select() UDP
	fd_set			master;				// master file descriptor list for Select()
	fd_set			read_fds;			// temp file descriptor list for select()
	fd_set			write_fds;			// temp file descriptor list for select()

	int			fdmax;				// garde lindex le plus haut des fd
	int			newfd;				// garde lindex du dernier file descriptor connecter
	timeval			Timeout;			// garde le nombre de temps a attendre apres les fd_Set


	int			Listener;			// file descriptor qui va ecouter pour les nouvelle connection
	int			UDPfd;				// file descriptor qui ecoute les packets udp

	UINT4		LastNetID;			// garde le dernier ID emis a un client
	unsigned short		ListenPort;			// le port sur lequel on ecoute pour les new connections

	UINT4		NewConnID;			// un ID qu'on emet a une nouvelle connection
	
	bool			UDPenabled;			// on est a tu UDP pour le server ?

public:

	UINT4	BytesSent;			// nombre total de byte que le serveur a envoyer
	UINT4	BytesReceived;		// nombre total de byte recu par le serveur


	//FILE			*fp;

	cIncConnection	*PendingConnections;// un queue qui garde les connections en cours
	//bool			isListen;			// est-ce qu'on roule un client en meme temps

	char			LastError[256];		// garde la derniere erreur cote serveur
	char			LastMessage[256];	// garde le dernier message cote serveur

	unsigned char		NetBitField;		// garde les option de la baboNet sous 8 bit


	cClient			*Clients;		//liste des clients connecter
	int			maxClients;		//garde le nombre maximal de client ici
	int			nbClient;		//garde le nombre de client connecter en ce moment

	sockaddr_in		ipAdress;		//garde le IP et le Port du serveur

	cServer();													//constructeur par defaut
	cServer(bool udpenabled,int maxClient,unsigned short listenPort);			//constructeur parametrer du nombre maximal de client et du listenport

	~cServer();

	
	int		Send(char* dataToSend,int dataSize,int typeID,INT4 destination,int protocol=0);		//pour envoyer du server vers un client, protocol 0 = TCP(safe), 1 = UDP(unsafe)
	

	INT4		ReceivePacketsFromClients();						//recevoir les packet qui viennent des clients
	int		ReceiveDatagram(cPacket *packet,sockaddr_in fromIP);//permet de recevoir un packet UDP d'un client
	int		SendPacketsToClients();								//envoyer les packet vers les clients
	int		PrepareHosting();									//va setter les trucs par defaut quand on spawn le serveur
	int		CreateListener();									//setup un listener, le add au master set, et le bind sur notre listen port
	INT4		CreateClient(sockaddr_in *ip, int fileDescriptor,unsigned short udpPort,UINT4 pid);	//permet d'ajouter un client
	void		CloseSocket(int socketFD);

	int 		GetQueueCount(int protocol);						//permet de savoir combien ya de packet dnas le queues

	cClient*	getClientByFD(int fd);								//permet d'obtenir la reference a un client grace a son file descriptor, le bool determine si c un file descriptor udp ou tcp
	cClient*	getClientByID(UINT4 netid);					//permet d'obtenir un client par son baboNet ID
	cClient*	getClientByIP(char *ip,unsigned short port);		//permet d'obtenir un client par son IP/port
	
	char*		getLastError()		{	return LastError;	}		//permet dobtenir une version textuel de la derniere erreur cote serveur
	char*		getLastMessage()	{	return LastMessage;	}		//permet dobtenir une version textuel de ce qui c passer au dernier cycle

	int		RemoveClient(cClient *clientToKill);				//permet de retirer un client du serveur ( le disconnecter )
	void		AcceptConnection(UINT4 id);					//va starter une thread pour accepter la nouvelle connection, avec son id unique
	INT4		UpdateConnections(char *newIP);						//va checker si tel tel connection a terminer, retourne le new babonetID si yen a une quia reussi
	int		GetMaxFD();							// simple function to update our max FD
};	













#endif
