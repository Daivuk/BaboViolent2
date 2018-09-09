#ifndef _PEER2PEER_H
#define _PEER2PEER_H

#include "cPeer.h"
#include "stdio.h"

#ifdef WIN32
	#include "ws2tcpip.h"
#else
	#include "LinuxHeader.h"
#endif
#include "baboNet.h"
#include "cDNSquery.h"


//le port par defaut pour broadcast
#define BB_BROADCAST_PORT	11111


class cPeer2Peer
{
private:

	int				PrepareSockets();			//prepare les sockets par defaut
	char*			GetBroadcastAdress();		//retourne le Broadcast adress selon le subnet mask

public:

	//FILE			*fp;	//debug file

	unsigned long	BytesSent;
	unsigned long	BytesReceived;

	fd_set			master;				//fdset pour les socket udp(communication socket + broadcast socket)
	fd_set			fdread;
	fd_set			fdwrite;
	timeval			Timeout;
	int				fdmax;

	sockaddr_in		ipAdress;			//adress IP de lordi courant, incluant le port


	char			LastMessage[100];	//garde le dernier message
	char			LastError[100];		//garde la derniere erreur

	unsigned long	LastPeerID;			//id unique qu'on va emmetre au peers
	//unsigned long	LastPacketID;		//id unique qu'on emmet aux packets

	int				UDPfd;				//socket sur lequel on va envoyer les packet udp
	int				BCfd;				//socket sur lequel on va broadcaster
	unsigned short	BindedPort;			//port qu'on va binder, pour faker le serveur

	unsigned char	NetBitField;		//garde les option du baboNet p2p sous 8 bit
	
	cDNSquery		*DNSqueries;		//pointer on a list of queries
	cPeer			*PeerList;			//list qui contient les peers
	cPeer			*BCPeer;			//peer qui va stocker / envoyer les broadcast packets
	
	cPeer2Peer();						//constructeur par defaut
	~cPeer2Peer();



	int				AddDNSquery(char *domain,unsigned long id);											//add a dns query to the list, returns the ID giving to this queried peer
	void			AnalyzeSystemDatagram(cUDPpacket *packet,cPeer *fromPeer);							//va decortiquer un packet quier dedier a la babonet
	void			AnalyzeSystemDatagram(char *buffer,int *nread,char *fromIP,unsigned short port);	//va decortiquer un packet quier dedier a la babonet
	
	void			ApplyAcks(char *ackBuffer,int bufferSize,cPeer* fromPeer);		//va dire aux packets qu'ils ont ete recus
	int				CheckUnpublishedPeers(float elapsed,bool &isNew);				//permet d'indiquer au user la presence d'un nouveau peer

	cUDPpacket*		ExtractPacket(char* buffer,int *nread);							//va extraire un packet

	cPeer*			GetPeerByID(unsigned long id);									//retourne la reference a un peer selon son ID
	cPeer*			GetPeerByIP(sockaddr_in ip);									//retourne la reference a un peer selon son ip/port
	cPeer*			GetPeerByIPport(char *ip,unsigned short port);					//retourne la reference a un peer selon son ip/port

	int				SendToPeers(float elapsed);										//envoie les packets en attente aux peers
	int				ReceiveFromPeers();												//permet de recevoir les infos des Peers
	void			ReceiveDatagram(cUDPpacket *packet,sockaddr_in fromIP);			//permet de redirigez et de prendre une action selon le packet recu

	void			CloseSocket(int socketFD);
	int				BindPort(unsigned short port);										//permet de binder un port sur lequel on va ecouter les p2p packets
	int				CreatePeer(struct sockaddr_in ip,bool *exist);						//create un new peer dans notre liste, et retourne un ID pour communiquer avec par la suite, exist va contenir si oui ou non le peer existait
	int				DeletePeer(unsigned long peerID);									//permet de retirer un peer dla liste
	int				SendTo(long peerID,cUDPpacket *newPacket,unsigned long exception=0);//envoie d'un packet a un peer, sauf celui quia le ID comme exception

};

#endif
