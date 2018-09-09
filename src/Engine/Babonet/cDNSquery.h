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

#ifndef _DNS_QUERY_
#define _DNS_QUERY_

#include "platform_types.h"

#include "CThread.h"

#ifdef WIN32
	#include "Winsock2.h"
#else
	#include "memory.h"
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#include "stdio.h"

//a dns query is a simple gethostbyname() command, in a thread so it will never freeze the computer



class cDNSquery : public CThread
{
private:

	

public:
	
	struct hostent		*phe;		//keep the host entity interface
	cDNSquery		*Next;
	cDNSquery		*Previous;

	sockaddr_in		IP;			//keep the returned IP on success
	char			*Domain;	//keep the queried domain name
	UINT4	PeerID;		//keep the givin 


    cDNSquery(char *domain,UINT4 id);

	void		execute(void *pArg);	//execute the thread
    int			Update();				//check if thread has finished, return 1 if peer was discovered, return 0 if not finished, return -1 on error

};


#endif
