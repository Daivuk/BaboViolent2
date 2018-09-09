#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include "cServer.h"
#include "cClient.h"
#include "cPeer2Peer.h"

class cUDPserver
{
private:


public:

	//FILE		*fp;						//fichier de debug

	timeval		Timeout;					//timeout pour notre select()

	//cPeer2Peer	*P2P;						//pointeur sur notre module p2p
	//cServer		*Server;					//pointeur sur notre serveur
	//cClient		*Client;					//pointeur sur notre client

	fd_set		master;						//le fdset qui va contenir le socket udp
    
	int			UDPfd;						//notre socket UDP principal
	int			BCfd;						//socket pour le broadcasting
	int			fdmax;						//garde le socket quia le # le plus haut

	cUDPserver();
	

	int				SetDefaultSocket();			//setup le socket udp de base
	int				ReceiveFromSocket(int nbTimes);		//permet de recevoir toute les packet udp destines a notre socket nbTime est le nombre de fois a checker le socket
	int				BindPort(unsigned short port);
	cUDPpacket*		ExtractP2Ppacket(char* buf,int *nread);
	cPacket*		ExtractCSpacket(char* buf,int *nread);


};


#endif