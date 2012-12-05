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

#include "cPacket.h"


cPacket::cPacket()
{
	//isAlive	=	false;
	TypeID	=	0;
	Data	=	0;
	Size	=	0;

	Remaining	=	0;

	Next		=	0;
	Previous	=	0;
}

cPacket::cPacket(cPacket *packet)
{
	TypeID	=	packet->TypeID;
	Size	=	packet->Size;

	//si on passe pas de data
	if(!Size)
	{
		Data = 0;
	}
	else
	{
		Data	=	new char[Size];
		memcpy(Data,packet->Data,Size);	
	}

	Remaining	=	Size;

	Next		=	0;
	Previous	=	0;
}

cPacket::cPacket(char *data,int size,unsigned short typeID)
{
	TypeID	=	typeID;
	Size	=	(unsigned short)size;
	//isAlive	=	true;

	//si on passe pas de data
	if(!Size)
	{
		Data = 0;
	}
	else
	{
		Data	=	new char[size];
		memcpy(Data,data,size);	
	}

	Remaining	=	Size;

	Next		=	0;
	Previous	=	0;
}


int cPacket::Send(int socketFD)
{
	//on setup une struct pour envoyer

	stPacket Packet;
	stHeader header;
	
	header.Size		=	Size;
	header.typeID	=	TypeID;
	
	//si c juste un typeID qu'on veut passer
	if(!Size)
	{
		Packet.data		=	new char[TCP_HEADER_SIZE];
		memcpy(Packet.data, &header, TCP_HEADER_SIZE);
	}
	else
	{
        Packet.data		=	new char[Remaining + TCP_HEADER_SIZE];

		memcpy(Packet.data, &header, TCP_HEADER_SIZE);
		memcpy(Packet.data + TCP_HEADER_SIZE, Data, header.Size);
	}

	
	//on envoie le packet
	int sent = 0;	

	//tant qui reste du data on va envoyer le size
	Remaining = header.Size + TCP_HEADER_SIZE;
	int total = Remaining;

	//on a 3 essaie sinon on sort en erreur
	int tries = 3;

	while(Remaining)
	{
		if(!tries)
		{
			delete [] Packet.data;
			return 2;
		}

		sent = send(socketFD,(const char*)(Packet.data + (total-Remaining)),Remaining,0);

		if(sent < 0)
		{
			//sprintf(LastError,"Error : could not send() packets to Server");
			delete [] Packet.data;
			return 1;
		}
		
		Remaining -= sent;
		tries -= 1;
	}

	delete [] Packet.data;

	return 0;
}

int cPacket::SendUDP(int socketFD,sockaddr_in *ip)
{
	//on setup une struct pour envoyer

	stPacket Packet;
	stHeader header;
	
	header.Size			=	Size;
	header.typeID		=	TypeID;
	//char interfaceID	=	toClient ? INTERFACE_CLIENT : INTERFACE_SERVER;
	
	//si c juste un typeID qu'on veut passer
	if(!Size)
	{
		Packet.data		=	new char[UDP_HEADER_SIZE];
		memcpy(Packet.data, &header, UDP_HEADER_SIZE);
	}
	else
	{
        Packet.data		=	new char[Remaining + UDP_HEADER_SIZE];

		memcpy(Packet.data, &header, UDP_HEADER_SIZE);
		memcpy(Packet.data + UDP_HEADER_SIZE, Data, header.Size);
	}

	
	//on envoie le packet
	int sent = 0;	

	//tant qui reste du data on va envoyer le size
	Remaining = header.Size + UDP_HEADER_SIZE;
	int total = Remaining;

	//on a 5 essaie sinon on sort en erreur
	int tries = 5;

	while(Remaining)
	{
		if(!tries)
		{
			delete [] Packet.data;
			return 2;
		}

		//unsigned short port = ntohs(ip->sin_port);
		//char *IP			=	inet_ntoa(ip->sin_addr);


		sent = sendto(socketFD,(const char*)(Packet.data + (total-Remaining)),Remaining,0,(sockaddr*)ip,sizeof(sockaddr));

		if(sent < 0)
		{
			delete [] Packet.data;
			return 1;
		}
		
		Remaining -= sent;
		tries -= 1;
	}

	delete [] Packet.data;

	return 0;
}

cPacket::~cPacket()
{
	Next		=	0;
	Previous	=	0;

	if(Data) delete Data;
}
