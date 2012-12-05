#ifndef _C_SERVER_H
#define _C_SERVER_H

#include "baboNet.h"
#include <stdio.h>
#include <memory.h>
#include "UpdateStructs.h"


class CServer
{
private:

	char IP[16];
	unsigned short Port;

	bool			isConnected;




public:

	unsigned long	bbID;
	int				LoadScore;

	CServer *Next;
	CServer *Previous;

	CServer();
	CServer( char * ip, unsigned short port );

	void	Update( float elapsed );
	void	RetreiveNetData();

	void	Disconnect();

};



#endif