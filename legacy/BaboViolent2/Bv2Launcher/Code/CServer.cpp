#include "CServer.h"
#include "windows.h"

CServer::CServer()
{
	IP[0] = '\0';
	Port = 0;

	Next = 0;
	Previous = 0;

	isConnected = false;
	bbID = 0;
	LoadScore = 0;
}

CServer::CServer( char * ip, unsigned short port )
{
	sprintf( IP , ip );
	Port = port;

	Next = 0;
	Previous = 0;

	isConnected = false;
	bbID = 0;
	LoadScore = 0;
}

void CServer::Update( float elapsed )
{
	//let's connect to it
	if(!isConnected)
	{
		//if server was not responding, no need for updating it
		if( LoadScore == -1 ) return;
		
		//are we trying to connect
		if(!bbID)
		{
			//l;ets connect to it
			bbID = bb_clientConnect( IP , Port );
			//printf(" Connect attempt to %s , %i \n", IP , Port );
			
			//send the server a score query
			bb_clientSend( bbID , 0 , 0 , LOAD_QUERY , NET_TCP ); 
		}

		int rc = bb_clientUpdate( bbID , elapsed );

		switch( rc )
		{
			case CLIENT_CONNECT_FAILURE:
			{
				LoadScore = -1;
				break;
			}
			case SERVER_CLOSED_CONNECTION:
			{
				LoadScore = -1;
				break;
			}
			case CLIENT_CONNECT_SUCCESS:
			{
				isConnected = true;
				LoadScore = 0;
				break;
			}
		}

	}
	else
	{
		//we are connected, let's update server
		if( (bb_clientUpdate( bbID , elapsed )) > 0 )
		{
			printf(" > Connection problem, please restart \n");
			Sleep(3000);
			exit(0);
		}
		//retreive newly available data
		RetreiveNetData();
	}
}

void CServer::RetreiveNetData()
{
	
	char *buffer;
	int typeID;

	while(buffer = bb_clientReceive(bbID,&typeID))
	{
		switch(typeID)
		{
			case LOAD_SCORE:
			{
				stLoadScore ls;
				memcpy(&ls,buffer,sizeof( stLoadScore ));

				LoadScore = ls.LoadScore;

				//printf( "Server Id %li has %i score\n",bbID,LoadScore);
				break;
			}
		}
	}

}

void CServer::Disconnect()
{
	isConnected = false;
	bb_clientDisconnect( bbID );
}
