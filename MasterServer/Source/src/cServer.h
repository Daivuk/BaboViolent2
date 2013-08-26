#ifndef _CSERVER_H
#define _CSERVER_H

#include "CClient.h"	//a server can be a master server's client

//Class defining the currently connected servers

class CServer : public CClient
{
public:
	
	bool		Registered;		//if true, that means the server can update stats with master servers
	int		ServerID;		//unique ID to identify this server, if not registered, ID == 0

	CServer();
	~CServer();
};


#endif
