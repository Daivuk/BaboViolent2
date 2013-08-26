#ifndef _PEER_H
#define	_PEER_H

#ifdef WIN32
	#include "winsock2.h"
#else
	#include "memory.h"
	#include <sys/select.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#include "cUDPpacket.h"


#define		RESEND_DELAY	0.1f
#include	"stdio.h"

class cPeer
{
private:
public:

	//FILE			*fp;					//fichier de debug

	unsigned long	ID;						//id unique du peer
	sockaddr_in		IpAdress;				//garde ladresse et le port du peer
	bool			Confirmed;				//est-ce qu'on a recu le vrai port de communication du peer


	float			Timeout;				//ca fait combien de temps que le peer ne nous a pas parler

	cPeer			*Next;
	cPeer			*Previous;

	bool			Need2Publish;			//est-ce qu'on doit dire a l'usager qu'on existe ?
	bool			Need2Delete;			//est-ce qu'on doit se faire delter bientot ?
    
	
	int				nbAcks;					//garde le nombre de ack a envoyer
	unsigned long	AcksToSend[100];		//array des acks a envoyer

	unsigned long	LastPacketID;			//garde le dernier ID de packet
	unsigned long	PendingID;				//le ID du prochain packet attendu
	cUDPpacket		*ReceivedPackets;		//liste des packets recus, si packet ID < Pendig ID, il est prennable au user
	cUDPpacket		*PacketsToSend;			//liste des packets a envoyer
	cUDPpacket		*ToKill;				//yavait til un packet a detruire ?

	cPeer();
	cPeer(unsigned long ID);
	cPeer(struct sockaddr_in ip,unsigned long ID);

	~cPeer();


	int				CreatePacket(cUDPpacket *newPacket);
	void			AddReceivedPacket(cUDPpacket *newPacket);

	int				SendPackets(int fd,float elapsed,fd_set master,unsigned long & nbBytes);		//return 0 on success, 1 on failure
	void			DeletePacket(cUDPpacket *packet);

	int				PackToBuffer(char *buf,cUDPpacket *packet);			//ajoute un packet au send buffer, retourne le nombre de byte d'ecris
	void			AddAck(unsigned long id);							//ajoute un ack a envoyer
	cUDPpacket*		GetReadyPacket();									//retourne un packet pret a donner au user
	void			DebugAcks();										//va printer les ack dnas un file


};


#endif
