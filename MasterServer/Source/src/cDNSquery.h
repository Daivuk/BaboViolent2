#ifndef _DNS_QUERY_
#define _DNS_QUERY_

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
	unsigned long	PeerID;		//keep the givin 


    cDNSquery(char *domain,unsigned long id);

	void		execute(void *pArg);	//execute the thread
    int			Update();				//check if thread has finished, return 1 if peer was discovered, return 0 if not finished, return -1 on error

};


#endif
