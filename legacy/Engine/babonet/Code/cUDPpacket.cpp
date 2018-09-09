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

#include "cUDPpacket.h"


cUDPpacket::cUDPpacket()
{
	ID				=	0;
	InterfaceID		=	0;
	//Timeout			=	0;
	Safe			=	false;
	Data			=	0;
	Size			=	0;
	TypeID			=	0;
	Ack				=	false;
	Sent			=	false;
	//ReadyToPickup	=	false;
	LastCheck		=	0;

	Next		=	0;
	Previous	=	0;
}

cUDPpacket::cUDPpacket(cUDPpacket *p)
{
	ID				=	p->ID;
	InterfaceID		=	p->InterfaceID;
	//Timeout			=	p->Timeout;
	LastCheck		=	0;
	Safe			=	p->Safe;
	Size			=	p->Size;

	if(Size)
	{
		Data		=	new char[Size];
		memcpy(Data,p->Data,Size);
	}
	else
	{
		Data		=	0;
	}
		
	
	TypeID			=	p->TypeID;
	Ack				=	false;
	Sent			=	false;


	Next		=	0;
	Previous	=	0;
}

cUDPpacket::cUDPpacket(UINT4 id,char interfaceID,char* data,int typeID,unsigned short size,bool safe)
{
	ID				=	id;
	InterfaceID		=	interfaceID;
	Size			=	size;
    TypeID			=	(unsigned short)(typeID);
	Ack				=	false;
	Sent			=	false;
	LastCheck		=	0;

	if(Size)
	{
		Data	=	new char[Size];
		memcpy(Data,data,Size);
	}
	else
	{
		Data	=	0;
	}

	Safe		=	safe;

	Next		=	0;
	Previous	=	0;
}

cUDPpacket::~cUDPpacket()
{
	if(Data) delete [] Data;
}
