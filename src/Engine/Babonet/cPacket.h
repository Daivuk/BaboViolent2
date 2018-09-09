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

//	Class packet
//	ce sont ces packet qui seront envoyes/recu avec send() et recv()
//
#ifndef PACKET_H
#define PACKET_H

#ifdef WIN32
	#include <Memory.h>
	#include "winsock2.h"
#else
	#include "LinuxHeader.h"
#endif

#include "stdio.h"
#include "cUDPpacket.h"

//un header de packet TCP est composer de 4 bytes
#define TCP_HEADER_SIZE		4

//un header de packet UDP est composer de 4 bytes
#define UDP_HEADER_SIZE		4


	//notre packet header
	struct stHeader
	{
		unsigned short Size;
		unsigned short typeID;
	};

		

class cPacket
{
private:


public:

	
	//une struct qu'on va se servir pour envoyer du data
	struct stPacket
	{
		unsigned short	Size;		//le Size du packet TOTAL
		unsigned short	TypeID;		//le typeID du packet
        char			*data;		//un array dynamique qui tien le data a envoyer
	};


	//bool			isAlive;		//savoir si le packet courrant est valide, pour ne pas ecrire par dessus

	//FILE *fp;		//fichier de debug

	cPacket			*Next;		//permet de chainer les packets (pour les queues)
	cPacket			*Previous;

	unsigned short	TypeID;		//type du packet, pour savoir avec quoi on deal rendu a l'autre bout
	unsigned short	Size;		//connaitre la grosseur du Data ( et non du packet )
	int				Remaining;	//combien de byte y reste a envoyer du Data ( et non du packet )

	
	char			*Data;		//pointeur vers le data du packet
		

	cPacket();
	cPacket(cPacket *packet);
	cPacket(char *data,int size,unsigned short typeID); // packet de recpetion par un client
	//cPacket(unsigned short Size,char *data,unsigned short typeID); //packet d'envoie d'un client
	~cPacket();


	int				Send(int socketFD);					//le packet s'envoie au socket passer en param, retourne le nombre de byte envoyer
	int				SendUDP(int socketFD,sockaddr_in *ip);	//envoie d'un packet UDP
};


#endif


