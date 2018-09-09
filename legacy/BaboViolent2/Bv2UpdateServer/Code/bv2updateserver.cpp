/*
	Marc Durocher
	RndLabs Inc.
	2006-07

	Bv2UpdateServer is the app running on a dedicated server, where bv2 checks for available update,
	and update files in case it is needed.

*/


#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <memory.h>
#include "dkc.h"

#ifndef WIN32
	#include "sys/times.h"
#else
	#include <windows.h>
#endif

#include "baboNet.h"
#include "sqlite3.h"
#include "CClient.h"
#include "UpdateStructs.h"


#ifndef WIN32
	#define TIME_SPLIT 100.0f	//linux time denominator
	#define stricmp strcasecmp //helper define
#else
	#define TIME_SPLIT 1000.0f	//Windows time denominator
#endif

//#define DBPATH "../../../fl.db"

#define DBPATH "fl.db"



sqlite3* db;
CClient *Clients=0;	//list of currently connected clients

float 	LastVersionCheck=0;	//how much time since last version check
char	CurrentVersion[6];	//keep the current version of the game
unsigned short CurrentDBVersion = 0; // keep current db version

unsigned char CurrentLoad=0;	//when the server is busy the load goes up!

void InitDB();
void InitNetwork();
void CleanUp();
bool Update(float elapsed);

void AddClient(unsigned long id,char *IP);
void RemoveClient(unsigned long id);

void RetreiveNetData();
void UpdateClients(float elapsed);
void GetLatestVersion();
unsigned char GetBaseLoad();

CClient *GetClientByID(unsigned long ID);



int main()
{
	InitDB();

	CurrentLoad = GetBaseLoad();

	//let's init babonet so people can connect to us
	InitNetwork();

	
	//main program here
	bool isRunning = true;
	
	dkcInit(60);

	//linux time struct
	#ifndef WIN32
		timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = 1000000;
	#endif

	GetLatestVersion();

	while(isRunning)
	{
		// On va updater notre timer
		int nbFrameElapsed = dkcUpdateTimer();

		// On va chercher notre delay
		float delay = dkcGetElapsedf();
		
		while (nbFrameElapsed)
		{ 
			//let's update our NetManager
			isRunning	=	Update(delay);

			nbFrameElapsed--;
		}
		//linux sleep
		#ifndef WIN32
			nanosleep(&ts,0);
			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		//windows sleep
		#else
			Sleep(1);
		#endif
		
	}


	//cleanup
	CleanUp();
		
	return 0;
}

unsigned char GetBaseLoad()
{
	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];

	sprintf(SQL,"Select Value From Settings Where SettingName = 'BaseScoreLoad';");
	
	int dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	unsigned char load = atoi(azResult[1]);

	sqlite3_free_table(azResult);

	return load;	
}

void InitDB()
{
	int rc = sqlite3_open(DBPATH,&db);

	if(rc)
	{
		perror("sqlite3_open");
		exit(0);
	}
	else
	{
		printf("Opened database successfully\n");
	}

	//testing purposes only
	//sprintf(CurrentVersion,"2.03");
}

void InitNetwork()
{
	if(bb_init())
	{
		perror("Babonet Init\n");
		return;
	}
	else
	{
		printf("BaboNet Loaded\n");
	}


	//let's start the server hosting
	if(bb_serverCreate(false,1020,11207))
	{
		perror("bb_serverCreate");
	}
	else
	{
		printf("Server up n running\n");
	}
}

void CleanUp()
{
	sqlite3_close(db);

	bb_shutdown();

	//let's delete all clients
	CClient *toKill=0;
	for(CClient *C=Clients;C;delete toKill)
	{
		toKill = C;
		C = C->Next;
	}
}

bool Update(float elapsed)
{

	//let's update our network
	char IP[16];
	long newID = bb_serverUpdate(elapsed,UPDATE_SEND_RECV,IP);
	
	if(newID > 0) //new client
	{
		AddClient(newID,IP);
		printf("Client with ID %li ; IP : %s has connected\n",newID,IP);
	}
	else if(newID == BBNET_ERROR) //error
	{
		perror("bb_serverUpdate");
		return false;
	}
	else if(newID < 0)	//client has closed the connection
	{
		printf(" > Client with ID %li disconnected\n",newID * -1);
		RemoveClient(newID *-1);
	}

	//retreive newly available data
	RetreiveNetData();

	//update Clients queries
	UpdateClients(elapsed);

	//check if we need to get latest version
	LastVersionCheck+=elapsed;
	if(LastVersionCheck >= 60)
	{
		GetLatestVersion();
		LastVersionCheck = 0;
	}

	return true;
}

void AddClient(unsigned long id,char *IP)
{
	if(Clients)	//already ahve some client in the queues
	{
		CClient *C=0;
		for(C=Clients;C->Next;C=C->Next) {}

		C->Next = new CClient((unsigned long)id,IP,db,CurrentVersion);
		C->Next->Previous = C;

	}
	else		//first client in the queue
	{
		Clients = new CClient((unsigned long)id,IP,db,CurrentVersion);
	}

	CurrentLoad++;

}

void RemoveClient(unsigned long id)
{
	for(CClient *C=Clients;C;C=C->Next)
	{
		if(C->bbID == id) //we found the client
		{
			if(C == Clients)
			{

				if(C->Next)
				{
					C->Next->Previous = C->Previous;
				}

				Clients = C->Next;
				
				CurrentLoad--;		
				delete C;
				return;
			
			}
			else
			{
				if(C->Next)	C->Next->Previous = C->Previous;
				if(C->Previous)	C->Previous->Next = C->Next;
				
				CurrentLoad--;
				delete C;
				return;
			}			
		}
	}
}

void RetreiveNetData()
{
	
	char *buffer;
	unsigned long bbID;
	int typeID;

	while(buffer = bb_serverReceive(bbID,typeID))
	{
		switch(typeID)
		{
			case VERSION:
			{
				stVersion ver;
				memcpy(&ver,buffer,sizeof(stVersion));

				stStatus status;
				

				if(stricmp(ver.Version,CurrentVersion)) //the client doesnt have latest version
				{
					//the client needs to get updated
					CClient *C = GetClientByID(bbID);
					
					if(!C) continue;

					sprintf(C->Version,ver.Version);
					C->Updating = true;
					C->NeedList = true;
					C->OnlyDB = false;
					
					status.Status = 1;
				}
				else //no update needed
				{
					//telling the client he is up to date
					status.Status = 0;					
					//RemoveClient(bbID);
				}
				bb_serverSend((char*)&status,sizeof(stStatus),STATUS,bbID,NET_TCP);

				break;
			}
			case DB_VERSION:
			{
				stDBVersion ver;
				memcpy(&ver,buffer,sizeof(stDBVersion));
				stStatus status;

				// client sent us his db version
				if( ver.Version != CurrentDBVersion ) //the client db is out of sync
				{
					CClient *C = GetClientByID(bbID);
					if(!C) continue;

					// send him the latest database
					C->Updating = true;
					C->NeedList = true;
					C->OnlyDB = true;
					
					status.Status = 1;
					
				}
				else
				{
					//client db is up to date
					
					status.Status = 0;					
					RemoveClient(bbID);
				}
				bb_serverSend((char*)&status,sizeof(stStatus),STATUS,bbID,NET_TCP);
				break;
			}
			case PING: //a client wants to ping us
			{
				//let's send him a pong
				printf(" > Received a PING query, sending PONG\n");
				bb_serverSend(0,0,PONG,bbID,NET_TCP);
				break;
			}
			case LOAD_QUERY: //a client wants to know our current load
			{
				stLoadScore ls;
				ls.LoadScore = CurrentLoad;

				bb_serverSend((char*)&ls,sizeof(stLoadScore),LOAD_SCORE,bbID,NET_TCP);
				break;
			}
			case ACK:
			{
				//we receive an ACK from a client
				CClient *C = GetClientByID(bbID);
				if(!C) continue;

				C->Acked = true;
				C->mfElaspedSinceLastAck = 0.0f;
				break;
			}
			default:
			{
				//unknown packet type, kick the client and ignore it
				printf(" > Unknown packet type, disconnecting client\n");
				bb_serverDisconnectClient(bbID);
				RemoveClient(bbID);
			}
		}
	}

}

void UpdateClients(float elapsed)
{
	for(CClient *C = Clients;C;C=C->Next)
	{
		C->TimeOut += elapsed;

		if(C->TimeOut > 6000) //6000 secondes of inactivity ?
		{
			//let's force the client to disconnect
			printf(" > Disconnecting client with ID %li ... 6000 second timeout reached\n",C->bbID);

			bb_serverDisconnectClient(C->bbID);
			RemoveClient(C->bbID);
			
			return;
		}

		//update the client
		C->Update( elapsed );
	}

}

void GetLatestVersion()
{
	printf("Fetching latest version...\n");


	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];

	// game version
	sprintf(SQL,"Select Value From Settings Where SettingName = 'Version';");
	int dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	sprintf(CurrentVersion,azResult[1]);
	sqlite3_free_table(azResult);

	// db version
	sprintf(SQL,"Select Value From Settings Where SettingName = 'DBVersion';");
	dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	CurrentDBVersion = (unsigned short)atoi(azResult[1]);
	sqlite3_free_table(azResult);



	printf("Latest game version is %s  latest db version is %i\n",CurrentVersion,CurrentDBVersion);


}

CClient *GetClientByID(unsigned long ID)
{
	for(CClient* C=Clients;C;C=C->Next)
	{
		if(C->bbID == ID)
		{
			return C;
		}
	}
	
	return 0;
}
