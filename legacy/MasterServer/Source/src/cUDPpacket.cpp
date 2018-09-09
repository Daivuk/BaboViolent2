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

cUDPpacket::cUDPpacket(unsigned long id,char interfaceID,char* data,int typeID,unsigned short size,bool safe)
{
	ID				=	id;
	InterfaceID		=	interfaceID;
	Size			=	size;
	TypeID			=	typeID;
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
