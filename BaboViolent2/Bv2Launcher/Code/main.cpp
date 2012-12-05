/*	BV2 LAUNCHER APP

	RndLabs inc, All right reserved
	by Marc Durocher
	07-12-06
*/


#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <memory.h>
#include "baboNet.h"
#include "sqlite3.h"
#include "UpdateStructs.h"
#include "dkc.h"
#include "CServer.h"
#include "windows.h"
#include "CString.h"

#ifndef WIN32
	#define stricmp strcasecmp
#endif

#define DBPATH "bv2.db"


//--Macros--

//MasterServers table
#define MASTER_SCORE	i*nColumn+5
#define MASTER_ID		i*nColumn+6
#define MASTER_IP		i*nColumn+7
#define MASTER_LOCATION	i*nColumn+8
#define MASTER_PORT		i*nColumn+9


//States of the launcher
#define STATE_SCORING	0		//getting the score of every update servers
#define STATE_UPDATING	1		//currently being updated to a new version
#define STATE_LAUNCHING 2		//launching bv2!



//--Globals--
sqlite3* db=0;
char CurrentVersion[6];			//keep the current version of the game
char NewVersion[6];			//keep the version returned from the chosen server
unsigned short	CurrentDBVersion=0;	//keep the current db version

float	UpdateTimeout = 0;	//timout before the update process is aborted

int		State = 0;	//what are we doing, 0 connecting to an update server, 1 updating, 2 ready to boot bv2

FILE* 	CurrentFile=0;		//file we are currently receiving
long	FileSize=0;		//total size of the file currently being received

CServer* MasterServers=0;	//list of master server

CServer* ChosenServer=0;	//pointer on chosen master server

bool UpdatingDB = false;
bool Waiting4DBStatus = false;


//--Prototypes--
void	InitDB();
void	InitNetwork();
void	ReadSettings();		//read from table LauncherSettings in the database

void	CleanUp();
bool	Update(float elapsed);

void	RetreiveNetData();
void	AddMasterServer(CServer *newServer);
bool	UpdateNetwork(float elapsed);
void	CreateFolders( char * asFilename );

bool	StillScoring();
CServer *GetBestMaster();
void	DisconnectServers( CServer *exception );

void LaunchBV2(char* pExeName);

int main(int argc, char *argv[])
{

	printf("****************************\n");
	printf("*  BaboViolent 2 Launcher  *\n");
	printf("*  RndLabs inc. 2006-2007  *\n");
	printf("*  All rights reserved.    *\n");
	printf("****************************\n\n");

	//init sqlite3 and connect to database
	InitDB();

	//let's init babonet so people can connect to us
	InitNetwork();

	//init the timer
	dkcInit(30); //clocked at 30 fps

	//lets read the settings in the database (servers ip's and current game version)
	ReadSettings();


	//main program here
	bool isRunning = true;


	while(isRunning)
	{
		int nbFrameElapsed = dkcUpdateTimer();

		while (nbFrameElapsed)
		{
			//let's update our NetManager
			isRunning	=	Update(dkcGetElapsedf());

			if(!isRunning) break;

			nbFrameElapsed--;
		}
	}


	//cleanup
	CleanUp();

	//Launch bv2
	printf("\n\n >>> Launching Babo Violent 2 <<< \n");
	Sleep(500);

   char* bv2ExeName = "bv2.exe";
   if (argc > 1)
      {
      bv2ExeName = argv[1];
      }
	

	LaunchBV2(bv2ExeName);


	return 0;
}

void LaunchBV2(char* pExeName)
{
	STARTUPINFO          si = { sizeof(si) };
	PROCESS_INFORMATION  pi;

	//start teh bv2 process
	if (!CreateProcess(
		pExeName, //LPCTSTR lpApplicationName,
		NULL, //LPTSTR lpCommandLine,
		NULL, //LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL, //LPSECURITY_ATTRIBUTES lpThreadAttributes,
		NULL, //BOOL bInheritHandles,
		NORMAL_PRIORITY_CLASS, //DWORD dwCreationFlags,
		NULL, //LPVOID lpEnvironment,
		NULL, //LPCTSTR lpCurrentDirectory,
		&si, //LPSTARTUPINFO lpStartupInfo,
		&pi //LPPROCESS_INFORMATION lpProcessInformation
	))

//if( L(WinExec( "bv2.exe" , SW_SHOWNORMAL )) < 32 )
	{
		printf("\n\n >>> Problem launching %s, re-installing the game may solve this issue \n", pExeName);
		Sleep(5000);
	}

}

void InitDB()
{
	int rc = sqlite3_open(DBPATH,&db);

	if(rc)
	{
		printf(" !! Error opening database !!\nReinstall the game\n\n Aborting Launch");
		exit(0);
	}
	else
	{
		//printf("Opened database successfully\n");
	}
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
		//printf("BaboNet Loaded\n");
	}

}

void CleanUp()
{
	bb_shutdown();
}

bool Update(float elapsed)
{
	switch(State)
	{
		case STATE_SCORING:
		{
			//update our scoring servers
			CServer *S = 0;
			for( S = MasterServers; S ; S = S->Next )
			{
				S->Update(elapsed);
			}

			//are we finished scoring servers ?
			if( !StillScoring() )
			{
				State = STATE_UPDATING;

				ChosenServer = GetBestMaster();

				// no servers are up
				if( !ChosenServer )
				{
					printf( "\n > *** No servers for update ***\n ");
					return false;
				}

				//disconnect allservers except the best scoring one
				DisconnectServers( ChosenServer );
				
				//send a packet that contains our current game version
				stVersion v;
				sprintf( v.Version , CurrentVersion );
				bb_clientSend( ChosenServer->bbID , (char*)&v , sizeof(stVersion) , VERSION , NET_TCP );
			}



			break;
		}

		case STATE_UPDATING:
		{
			if ( (bb_clientUpdate( ChosenServer->bbID , elapsed )) > 0 )
			{
				printf(" > Remote server closed the connection... try again later\n");
				State = STATE_LAUNCHING;
			}

			RetreiveNetData();
			
			//update timout
			UpdateTimeout += elapsed;
			
			if( UpdateTimeout > 120.0f )
			{
				printf ( " > Update process timed-out... try again later \n");
				State = STATE_LAUNCHING;
			}
			
			break;
		}

		case STATE_LAUNCHING:
		{
			return false;
		}
	}
	
	return true;
}

void RetreiveNetData()
{
	
	char *buffer;
	int typeID;

	while(buffer = bb_clientReceive(ChosenServer->bbID,&typeID))
	{
		switch(typeID)
		{
			case STATUS:
			{
				stStatus st;
				memcpy(&st,buffer,sizeof(stStatus));

				if(st.Status == 0) //the player is up to date
				{
					if( !Waiting4DBStatus )
					{
						printf(" > Game Version is up to date\n");
					
						// send the current db version
						stDBVersion v;
						v.Version = CurrentDBVersion;
						bb_clientSend( ChosenServer->bbID , (char*)&v , sizeof(stDBVersion) , DB_VERSION , NET_TCP );
						Waiting4DBStatus = true;
					}
					else
					{
						printf(" > DB Version is up to date\n");
						bb_clientDisconnect(ChosenServer->bbID);
						State = STATE_LAUNCHING;
					}
				}
				else
				{
					if( !Waiting4DBStatus )
					{
						printf(" > Version is out of date : Update may take a few minutes\n > Thank you for your patience\n");
						UpdateTimeout = 0;
						UpdatingDB = false;
					}
					else
					{
						printf(" > DB Version is out of date : Update may take a few seconds\n > Thank you for your patience\n");
						UpdateTimeout = 0;
						UpdatingDB = true;
					}
				}
				break;
			}
			case FILE_HEADER:
			{
				stFileHeader fh;
				memcpy(&fh,buffer,sizeof(stFileHeader));

				if(!fh.Action) //adding a file
				{
					CreateFolders( fh.Filename );

					CurrentFile = fopen(fh.Filename,"wb");
					FileSize = fh.Filesize;

					printf(" > Receiving '%s' : %li bytes\n",fh.Filename,FileSize);
				}
				else //removing a file
				{
					printf(" > Deleting '%s' : No longer needed\n",fh.Filename);
					remove(fh.Filename);
				}
				
				UpdateTimeout = 0;

				//ack to the server that we got the file header
				bb_clientSend( ChosenServer->bbID , 0 , 0 , ACK , NET_TCP );
				break;
			}
			case FILE_CHUNK:
			{
				stFileChunk fc;
				memcpy(&fc.ChunkSize,buffer,2);
				memcpy(fc.ChunkData,buffer + 2,fc.ChunkSize);
				
				//write to the file
				fwrite(fc.ChunkData, 1, fc.ChunkSize, CurrentFile);
								
				FileSize -= fc.ChunkSize;
				printf("\r      > Remaining : %li   ",FileSize);


				if(FileSize < 1) //are we finished			
				{
					printf("  > OK\n");
					fclose(CurrentFile);
				}
				
				
				UpdateTimeout = 0;

				//ack to the server that we got the file chunk
				bb_clientSend( ChosenServer->bbID , 0 , 0 , ACK , NET_TCP );

				break;
			}
			case VERSION:
			{
				//we are receiving our new version number
				stVersion v;
				memcpy(&v,buffer,sizeof(stVersion));

				printf(" > Updated to version %s\n",v.Version);

				//let's write it to our database
				//reconnect to db first
				InitDB();

				char SQL[256];
				sprintf(SQL,"Update LauncherSettings set Value = '%s' where Name = 'Version';",v.Version);
				//sqlite3_exec(db,SQL,0,0,0);

				sqlite3_close( db );
				
				break;
			}
			case END_OF_UPDATE:
			{
				if( UpdatingDB )
				{
					printf(" > DB Updated!\n");
				}
				bb_clientDisconnect( ChosenServer->bbID );
				State = STATE_LAUNCHING;
				break;
			}
		}
	}
}

void DisconnectServers( CServer *exception )
{
	CServer *S = 0;
	for( S = MasterServers; S ; S = S->Next )
	{
		if( S->LoadScore > -1 && S != exception )
		{
			bb_clientDisconnect( S->bbID );
		}
	}
}

CServer * GetBestMaster()
{
	CServer *Best = 0;
	CServer *S = 0;
	for( S = MasterServers; S ; S = S->Next )
	{
		if(!Best && S->LoadScore != -1)
		{
			Best = S;
		}
		else
		{
			if( S->LoadScore != -1 && S->LoadScore < Best->LoadScore )
			{
				Best = S;
			}
		}
	}

	return Best;
}

bool StillScoring()
{
	//check to see if we have our score for every server
	for( CServer *S=MasterServers; S ; S = S->Next )
	{
		if(S->LoadScore == 0) return true;
	}

	return false;
}


void ReadSettings()
{
	//let's read settings from our LauncherSettings table first
	printf(" > Reading Settings\n");

	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int		nRow;			// nombre denregistrement retourner par la requete
	int		nColumn;		// nombre de column
	char	SQL[128];

	// Get the current version
	sprintf(SQL,"Select Value From LauncherSettings Where Name = 'Version';");
	int dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	if( !nRow )
	{
		sqlite3_free_table(azResult);
		sqlite3_close( db );
		return;
	}

	sprintf( CurrentVersion , "%s" ,azResult[1]);
	printf( "  > Current Version is %s\n",CurrentVersion);

	sqlite3_free_table(azResult);

	// Get list of servers
	sprintf(SQL,"Select * From MasterServers;");
	dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	for(int i=0;i<nRow;i++)
	{
		//create a new server with the infos we have
		AddMasterServer( new CServer( azResult[MASTER_IP], atoi(azResult[MASTER_PORT])) );
	}

	sqlite3_free_table(azResult);

	// get the current DB version
	sprintf(SQL,"Select Value From LauncherSettings Where Name = 'DBVersion';");
	dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	if( !nRow )
	{
		sqlite3_free_table(azResult);
		sqlite3_close( db );
		return;
	}

	CurrentDBVersion = (unsigned short)atoi(azResult[1]);
	printf( "  > Current DB Version is %i\n",CurrentDBVersion);

	sqlite3_close( db );
}

void AddMasterServer( CServer *newServer )
{

	if(MasterServers)
	{
		//go to the end of the list
		for(CServer *S = MasterServers; S->Next ; S=S->Next)
		{
		}

		S->Next = newServer;
		newServer->Previous = S;

	}
	else
	{
		MasterServers = newServer;
	}
}

void CreateFolders( char * asFilename )
{
	CString Folders[10];
	int		FolderIndex = 0;

	CString str = CString( asFilename );

	//get folder list ( last one will be the actual filename )
	while( FolderIndex < 10 )
	{
		Folders[FolderIndex++] = str.getFirstToken('/');
		if( Folders[FolderIndex-1] == "" )
		{
			break;
		}
	}

	//create recursively the folders until the next string
	//is empty( which means we arrived at the filename, not folder name)
	int i=0;
	CString folder = "";
	while( Folders[i + 1] != "" )
	{
		int z=0;
		while( z <= i )
		{
			if( z == 0 )
			{
				folder = Folders[z];
			}
			else
			{
				folder = folder + "/" + Folders[z];
			}
			z++;
		}

		//create the folder!
		CreateDirectory( folder.s , NULL );
		i++;
	}

	
}

