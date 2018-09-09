#include "cDNSquery.h"


cDNSquery::cDNSquery(char *domain,unsigned long id)
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
	phe = gethostbyname(Domain);
	if(!phe) return;
	memcpy(&IP.sin_addr, phe->h_addr_list[0], sizeof(struct in_addr));
}
