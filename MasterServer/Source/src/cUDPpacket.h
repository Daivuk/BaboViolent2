#ifndef _UDPPACKET_H
#define _UDPPACKET_H

#ifdef WIN32
	#include <Memory.h>
#else
	#include "memory.h"
#endif

//type d'Interace
#define INTERFACE_SYSTEM	0	//message dedier au system p2p
#define INTERFACE_P2P		1	//message dedier au user de la p2p
#define	INTERFACE_SERVER	2	//message dedier au system server
#define	INTERFACE_CLIENT	3	//message dedier au system client


//type de SystemPacket
#define	TYPE_ACK			0	//type de packet qui contient des ack
#define TYPE_PEER			1	//contient les infos sur un nouveau peer que le fake serveur nous envoie
#define TYPE_DEMAND			2	//packet qui contient le ID de dedmande dun new peer, dois simplement lui renvoyer un TYPE_CONFIRMATION avant devoyer autre chose
#define	TYPE_CONFIRM		3	//packet qui indique que c'est une confirmation de reception (for new peer discovery)


class cUDPpacket
{
private:
public:

	cUDPpacket		*Next;
	cUDPpacket		*Previous;

	bool			Sent;			//savoir si le packet a ete envoyer
	bool			Ack;			//savoir si le packet a bien ete recu
	//bool			ReadyToPickup;	//est-ce que le packet est pret a etre pris

	float			LastCheck;		//connaitre le delai de temps ecouler
	//float			Timeout;		//nombre de temps avant que le packet soit discarter, si Timeout == 0, alros ya aucun timeout
	bool			Safe;			//le packet est safe
	
	unsigned long	ID;				//garde le ID unique du packet
	char			InterfaceID;	//garde linterface a laquelle le packet est destiner
	char			*Data;			//garde le data a envoyer
	unsigned short	TypeID;			//le type de packet
	unsigned short	Size;			//garde le size
	

	cUDPpacket();
	cUDPpacket(cUDPpacket *newPacket);
	cUDPpacket(unsigned long id,char interfaceID,char* data,int typeID,unsigned short size,bool safe=false);

	~cUDPpacket();

};


#endif
