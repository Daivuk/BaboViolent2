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

#include "cPeer.h"


cPeer::cPeer()
{
	Next		=	0;
	Previous	=	0;

	Confirmed	=	false;

	Need2Publish	=	false;	//par defaut un peer n'a pas besoin de se publisher

	IpAdress.sin_family			=	AF_INET;
	IpAdress.sin_addr.s_addr	=	INADDR_ANY;
    IpAdress.sin_port			=	0;
    memset(&(IpAdress.sin_zero), '\0', 8);


	PacketsToSend	=	0;
	ReceivedPackets	=	0;

	ID				=	0;
	PendingID		=	1;	//par defaut on attend apres le packet 1...
	LastPacketID	=	0;
	ToKill			=	0;

	Need2Delete		=	false;
	Timeout			=	0;


	memset(AcksToSend,0,100*sizeof(UINT4));
	nbAcks	=	0;

	//fp = fopen("_peerDebug.txt","w");
	//fclose(fp);
}

cPeer::cPeer(UINT4 id)
{
	Next		=	0;
	Previous	=	0;

	IpAdress.sin_family			=	AF_INET;
	IpAdress.sin_addr.s_addr	=	INADDR_ANY;
    IpAdress.sin_port			=	0;
    memset(&(IpAdress.sin_zero), '\0', 8);

	ID			=	id;

	Need2Publish	=	false;	//par defaut un peer n'a pas besoin de se publisher
	Need2Delete		=	false;
	Confirmed		=	false;

	PacketsToSend	=	0;
	ReceivedPackets	=	0;

	PendingID		=	1;	//par defaut on attend apres le packet 1...
	LastPacketID	=	0;
	ToKill			=	0;

	memset(AcksToSend,0,100*sizeof(UINT4));
	nbAcks			=	0;
	Timeout			=	0;

	//fp = fopen("_peerDebug.txt","w");
	//fclose(fp);
}

cPeer::cPeer(struct sockaddr_in ip,UINT4 id)
{
	Next		=	0;
	Previous	=	0;

	IpAdress	=	ip;
	ID			=	id;

	Need2Publish	=	false;	//par defaut un peer n'a pas besoin de se publisher
	Need2Delete		=	false;
	Confirmed		=	false;

	PacketsToSend	=	0;
	ReceivedPackets	=	0;

	PendingID		=	1;	//par defaut on attend apres le packet 1...
	LastPacketID	=	0;
	ToKill			=	0;

	memset(AcksToSend,0,100*sizeof(UINT4));
	nbAcks	=	0;
	Timeout			=	0;

	//fp = fopen("_peerDebug.txt","w");
	//fclose(fp);
}

int cPeer::CreatePacket(cUDPpacket *newPacket)
{
	//on va ajouter le nouveau packet a la liste des packets a envoyer
	if(PacketsToSend)
	{
		//on move jusqua la fin des packets
		cUDPpacket *P=0;
		for(P=PacketsToSend;P->Next;P=P->Next){}

		//on lajoute au queue
		P->Next = newPacket;
		newPacket->Previous = P;
	}
	else	//yavait pas de packet a envoyer
	{
		PacketsToSend = newPacket;
	}

	newPacket->ID = newPacket->Safe ? ++LastPacketID : 0;	//si le packet est unsafe, on passe un id de 0

	return 0;
}

int cPeer::SendPackets(int fd,float elapsed,fd_set master, UINT4 & nbBytes)
{
	//pour chaque packet on va calculer combien ca fait qu'on envoie et limiter a 400 bytes par packet
	//pour garder de lespace pour nos ack

	int sent = 0;
	char sendBuf[800];	//buffer dans lequel on va packer les packets

	cUDPpacket *toKill = 0;

	fd_set fdwrite = master; // copy it

	timeval Timeout;
	Timeout.tv_sec	= 0;
	Timeout.tv_usec = 0;

	if (select(fd+1, NULL, &fdwrite, NULL, &Timeout) == -1)
	{
		//char LastError[100];
		//sprintf(LastError,"Error select()ing while cPeer::SendPackets WSA : %i",WSAGetLastError());
		return 1;
	}

	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;
	
    if(FD_ISSET(fd,&fdwrite))
	{

		//on debug lenvoie
			//char ip[16];
			//unsigned short port;

			//sprintf(ip,inet_ntoa(IpAdress.sin_addr));
			//port	=	ntohs(IpAdress.sin_port);

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"> Sending Packets to peer with IP : %s:%i\n",ip,port);
			//fclose(fp);


		for(cUDPpacket *P=PacketsToSend;P;P=P->Next)
		{	
			nbBytes += P->Size;

			if(toKill)
			{
				if(PacketsToSend == toKill) PacketsToSend = toKill->Next;
				DeletePacket(toKill);
				toKill = 0;
			}

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"	>>interfaceID:%i ID:%i typeID:%i size:%i sent:%i\n",P->InterfaceID,P->ID,P->TypeID,P->Size,(int)P->Sent);
			//fclose(fp);

			if(P->Sent) //si yavait deja ete envoyer
			{
				if(P->Ack) //est-ce qu'on a confirmation de reception de packet ?
				{
					//on lenleve du queue
					toKill = P;
					continue;
				}

				P->LastCheck += elapsed;

				if(P->LastCheck >= RESEND_DELAY)
				{
					P->LastCheck = 0;

					sent += PackToBuffer(sendBuf + sent,P);
				}

			}
			else //packet jamais envoyer
			{
				if(P->TypeID == 1) //un ping
				{
					//fp = fopen("_netDebug.txt","a");
					//fprintf(fp,"	>> SENDING PING\n",ip,port);
					//fclose(fp);
				}


				sent += PackToBuffer(sendBuf + sent,P);
				P->Sent	= true;
				if(!(P->Safe)) toKill = P; //si le packet n'est pas safe, on le delete
			}

			if(sent >= 400) break;	//si on a atteint la limite de grosseur de packet on sors
		}

		if(toKill)
		{
			if(PacketsToSend == toKill) PacketsToSend = toKill->Next;
			DeletePacket(toKill);
			toKill = 0;
		}

		int Acks=0;

		//on attach ici les ack des packets recu
		if(nbAcks)
		{
			Acks = nbAcks;
			//DebugAcks();

			//on va se fair eun ti packet a ajouter au buffer
			cUDPpacket ackPacket;
			ackPacket.ID = 0;
			ackPacket.InterfaceID	=	INTERFACE_SYSTEM;
			ackPacket.TypeID		=	TYPE_ACK;
			ackPacket.Size			=	(unsigned short)(nbAcks * sizeof(UINT4));
			ackPacket.Data			=	new char[ackPacket.Size];
			memcpy(ackPacket.Data,AcksToSend,ackPacket.Size);

			sent += PackToBuffer(sendBuf + sent,&ackPacket);

			nbBytes += (nbAcks * sizeof(UINT4));

			nbAcks = 0;

			memset(AcksToSend,0,100*sizeof(UINT4));
		}



		//on est pret a envoyer notre packet
		int len			=	sizeof(sockaddr);
		int sentto		=	0;
		int Remaining	=	sent;

		while(Remaining)
		{
			sentto = sendto(fd,sendBuf + (sent - Remaining),Remaining,0,(sockaddr*)&IpAdress,len);
			if(sentto == -1)
			{
				//int i = WSAGetLastError();
				return 1; //error
			}
			Remaining -= sentto;
		}

		//fp = fopen("_netDebug.txt","a");
		//fprintf(fp,"	>> Sent : %i bytes and %i Acks\n",sent,Acks);
		//fclose(fp);
	}

	
	return 0;
}

int cPeer::PackToBuffer(char *buf,cUDPpacket *p)
{
	int sent = 0;

	//on pack l'interface ID
	memcpy(buf + sent,&(p->InterfaceID),sizeof(char));
	sent += sizeof(char);

	//on pack le ID
	memcpy(buf + sent,&(p->ID),sizeof(UINT4));
	sent += sizeof(UINT4);

	//on pack le typeID de packet
	memcpy(buf + sent,&(p->TypeID),sizeof(unsigned short));
	sent += sizeof(unsigned short);

	//on pack le size du data
	memcpy(buf + sent,&(p->Size),sizeof(unsigned short));
	sent += sizeof(unsigned short);

	//on pack le data
	memcpy(buf + sent,p->Data,p->Size);
	sent += p->Size;

	return sent;
}

void cPeer::DeletePacket(cUDPpacket *p)
{
	if(p->Previous)		p->Previous->Next	=	p->Next;
	if(p->Next)			p->Next->Previous	=	p->Previous;
	
	delete p;
}

cUDPpacket* cPeer::GetReadyPacket()
{
	if(ToKill)
	{
		if(ReceivedPackets == ToKill) ReceivedPackets = ReceivedPackets->Next;
		DeletePacket(ToKill);
		ToKill = 0;
	}

	if(ReceivedPackets)
	{
		//si on est en rpesence d'un packet unsafe, il est disponible c clair
		if(ReceivedPackets->ID == 0)
		{
			ToKill = ReceivedPackets;
			return ReceivedPackets;
		}

		//on va checker si c le packet qu'on attendait
		if(ReceivedPackets->ID == PendingID)
		{
            //on est rendu au bon packet
			PendingID++;
			ToKill = ReceivedPackets;

			/*fp = fopen("_peerDebug.txt","a");
			fprintf(fp,"Current Pending ID : %i",(int)PendingID);
			fclose(fp);*/

			return ReceivedPackets;
		}
	}
	return 0;
}

void cPeer::AddReceivedPacket(cUDPpacket *newPacket)
{
	//on ajoute le packet a la liste des packet en attente de reception

	//on test si le packet avait deja ete recu
	if(newPacket->ID && newPacket->ID < PendingID)
	{
		AddAck(newPacket->ID);
		delete newPacket;
		return;
	}


	//si ya deja un packet dans liste, on va aller le placer a bonne place
	if(ReceivedPackets)
	{
		for(cUDPpacket *P = ReceivedPackets;P;P=P->Next)
		{
			if(newPacket->ID == 0) //si on a recu un unsafe packet, on lajoute au debut
			{
					//on le place devant
					ReceivedPackets->Previous = newPacket;
					newPacket->Next = ReceivedPackets;
					ReceivedPackets = newPacket;
	                
					return;
			}
			else
			{

				if(newPacket->ID < P->ID)	//si le nouveau packet a un ID < que le packet courant
				{
					//on le place devant
					if(P->Previous)		P->Previous->Next	= newPacket;
					newPacket->Previous	= P->Previous;

					newPacket->Next = P;
					P->Previous		=	newPacket;

					if(P == ReceivedPackets) ReceivedPackets = newPacket;

					//on va envoyer un ack pour ce packet
					if(newPacket->ID)	AddAck(newPacket->ID);
	                
					return;
				}
				else if(newPacket->ID > P->ID)
				{
					if(P->Next) continue;
					
					P->Next = newPacket;
					newPacket->Previous = P;

					//on va envoyer un ack pour ce packet
					if(newPacket->ID)	AddAck(newPacket->ID);

					return;
				}
				else	//ici c que le packet existe deja, on le discarte simplement
				{
					if(newPacket->ID) AddAck(newPacket->ID);
					delete newPacket;
					return ;
				}
			}
		}
	}
	else
	{

		ReceivedPackets = newPacket;

		if(newPacket->ID > 0) AddAck(newPacket->ID);
	}



}

void cPeer::AddAck(UINT4 id)
{
	for(int i=0;i<nbAcks;i++)
	{
		if(AcksToSend[i] == id) return;
	}

	AcksToSend[nbAcks++] = id;
}

void cPeer::DebugAcks()
{
	//if(!nbAcks) return;

	////fp = fopen("_peerDebug.txt","a");

	//
	////fprintf(fp,"Acks a envoyer : \n");
	//for(int i=0;i<nbAcks;i++)
	//{
	//	fprintf(fp,"%i ; ",(int)AcksToSend[i]);
	//}
	//fprintf(fp,"\n");
	//fclose(fp);

}

cPeer::~cPeer()
{

	cUDPpacket *toKill=0;
	cUDPpacket *P = 0;
	for(P=PacketsToSend;P;delete toKill)
	{
		toKill = P;
		P=P->Next;
	}

	toKill=0;
	for(P=ReceivedPackets;P;delete toKill)
	{
		toKill = P;
		P=P->Next;
	}
}
