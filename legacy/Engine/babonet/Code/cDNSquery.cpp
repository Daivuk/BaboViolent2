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

#include "cDNSquery.h"


cDNSquery::cDNSquery(char *domain,UINT4 id)
{
	Domain		=	domain;
	phe			=	0;

	PeerID		=	id;


	Next		=	0;
	Previous	=	0;

    start();	
}

int cDNSquery::Update()
{

	if(!isRunning()) //la thread a fini
	{
		if(!phe) return -1;
		
		//memcpy(&IP.sin_addr, phe->h_addr_list[0], sizeof(struct in_addr));

		//char ip[16];
		//sprintf(ip,inet_ntoa(IP.sin_addr));

		return 1;
	}


	return 0;
}

void cDNSquery::execute(void *pArg)
{
	(void)pArg;
	phe = gethostbyname(Domain);
	if(!phe) return;
	memcpy(&IP.sin_addr, phe->h_addr_list[0], sizeof(struct in_addr));
}
