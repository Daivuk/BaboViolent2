#include "cMasterServer.h"


#ifdef WIN32
     #include <windows.h>


	void GetDate( char * dateOUT )
	{
		SYSTEMTIME st;
        	GetSystemTime(&st);

		sprintf( dateOUT , "%d/%d/%d", st.wDay , st.wMonth , st.wYear );
	}


#else
	#include <time.h>

	long GetDateLong()
	{
		time_t rawtime;
  		time ( &rawtime );

		return (long) rawtime;
	}

	void GetDateString( char * dateOUT , long dateLong)
	{

  		struct tm * timeinfo;
  		timeinfo = localtime ( &dateLong );
		
		sprintf( dateOUT , "%d/%d/%d", timeinfo->tm_mday , timeinfo->tm_mon , timeinfo->tm_year - 100 );
	}

	int GetDayDiff( time_t time1 , time_t time2 )
	{
		double diff = difftime ( time2 , time1 );
		
		return (int)( diff / 86400 );	
	}

#endif


#define DBPATH "master.db"

cMasterServer::cMasterServer()
{
	//Reset ClientList
	Clients =	0;
	
	NbGames	=	0;
	Games	=	0;

	//on va se connecter a la Master Database
 	int rc = sqlite3_open(DBPATH,&MasterDB);
 	
 	if(rc)
 	{
 		perror("Open Database");	
 	}
 	else
 	{
 		printf("Database opened successfully\n");
 	}

	TimeSinceBanCheck = 0.0f;
	
	//char date[20];

	////long dateLong = GetDateLong();
	//GetDateString( date , dateLong );

	//printf("time is %li\ndate is %s", dateLong , date);

	//on clear les game de bv2
	//sqlite3_exec(MasterDB,"Delete from Servers;",0,0,0);

	//on indique la version courrante de bv2
	sprintf(Version,"2.09");
}

void cMasterServer::SendBanList( unsigned long fromID , char * filter )
{
	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];


	if( !stricmp( filter , "" ) )
	{
		sprintf(SQL,"Select * From BanList order by ID;");
	}
	else
	{
		sprintf(SQL,"Select * From BanList Where Nick LIKE '%%%s%%' order by ID;", filter );
		//printf("the final string : %s\n",SQL);
	}

	
	int dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	
	

	// we have bans
	if(nRow)
	{
		for( int CurrentRow = 0; CurrentRow < nRow; CurrentRow ++ )
		{
			stCacheBan ban;

			long dateLong = atol( azResult[BV2_BAN_DATE] );
			GetDateString( ban.Date , dateLong );

			ban.Duration 	= (char)atoi( azResult[BV2_BAN_DURATION] );
			ban.ID 		= (short)atoi( azResult[BV2_BAN_ID] );

			sprintf( ban.IP , "%s" , azResult[BV2_BAN_IP] );
			sprintf( ban.MAC, "%s" , azResult[BV2_BAN_MAC] );
			sprintf( ban.Nick, "%s" , azResult[BV2_BAN_NICK] );

			bb_serverSend( (char*)&ban, sizeof(stCacheBan) , CACHE_BAN , fromID );
		
		}
	}
	//we dont
	
	sqlite3_free_table(azResult);

}

int cMasterServer::IsBanned( char * IP , char * MAC )
{
	
	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];

	sprintf(SQL,"Select * From BanList where IP = '%s' or MAC = '%s';", IP,MAC);

	int dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	sqlite3_free_table(azResult);

	if( nRow )
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void cMasterServer::AddBan( char * NickName , char * IP , char * MAC , int Duration )
{
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];

	//filter ip adress
	if( strstr( "192.168", IP ))
	{
		//canoot ban lan adress
		return;
	}

	if( strstr( "127.0.0.1", IP ))
	{
		//canoot ban local loopback adress
		return;
	}

	for( int g=0; g<strlen(NickName); g++ )
	{
		if( NickName[g] == '\0' )
		{
			break;
		}
		if( NickName[g] == '\'' )
		{
			NickName[g] = '`';
		}
	}

	sprintf(SQL,"Select * From BanList where IP = '%s' or MAC = '%s';", IP , MAC );
	int dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	//get current time
	time_t theTime;
	theTime = time( NULL );

	int CurrentRow = 0;

	if( nRow )
	{
		// just update current row
		sprintf(SQL,"Update BanList Set Date = %li , Nick = '%s',IP = '%s',MAC = '%s',Duration = %i Where ID = %i;", theTime , NickName , IP, MAC, Duration , atoi(azResult[BV2_BAN_ID]) );
		sqlite3_exec(MasterDB,SQL,0,0,0);

		printf("Updated ban with ID %i\n",atoi(azResult[BV2_BAN_ID]));

	}
	else
	{
		// add new ban
		sprintf(SQL,"Insert into BanList ( Date , Duration , IP , MAC , Nick ) values( %li , %i , '%s', '%s','%s');", theTime , Duration , IP, MAC, NickName );
		sqlite3_exec(MasterDB,SQL,0,0,0);

		printf("Added ban : IP=%s  MAC=%s  NickName=%s\n",IP,MAC,NickName);
	}

 	sqlite3_free_table(azResult);

}

void cMasterServer::Unban( int ID )
{
	//let's delete the specified ID from the DB
	char SQL[300];

	sprintf(SQL,"Delete From BanList Where ID = %i", ID );
	sqlite3_exec(MasterDB,SQL,0,0,0);


	printf("Ban with ID %i has been removed\n");
}

void cMasterServer::CheckBans()
{
	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];
	
	sprintf(SQL,"Select * From BanList order by ID;");
	
	int dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	
	
	//get current time
	time_t theTime;
	theTime = time( NULL );

	
	// we have bans
	if(nRow)
	{
		for( int CurrentRow = 0; CurrentRow < nRow; CurrentRow ++ )
		{
	
			long dateLong = atol( azResult[BV2_BAN_DATE] );
			int Duration 	= atoi( azResult[BV2_BAN_DURATION] );

			// unlimited ban
			if( !Duration )
			{
				continue;
			}			

			int nbDays = GetDayDiff( dateLong , theTime );

			// we can remove the ban
			if( nbDays >= Duration )
			{
				char SQL[512];
				sprintf(SQL,"Delete From BanList Where ID = %i;",azResult[BV2_BAN_ID]);
				sqlite3_exec(MasterDB,SQL,0,0,0);
			}


		}
	}
	//we dont
	
	sqlite3_free_table(azResult);

}

int cMasterServer::Update(float elapsed)
{
	TimeSinceBanCheck += elapsed;

	char	IP[16];
	bool	isNew=false;
	long r = bb_serverUpdate(elapsed,UPDATE_SEND_RECV,IP);

	//un nouveau client s'est connecter
	if(r > 0)
	{
		//printf("new client ID %li\n",r);
		AddClient(r,IP);	//adding a client
		printf("new client\n");
	}
	else if(r==BBNET_ERROR) //une erreur c produite
	{
		printf("error on bb_serverUpdate\n");
		return 1;
	}
	else if(r<0)
	{
		printf("client ID %li disconnected\n",r*-1);
		RemoveClient(r*-1);
	}

	//on va updater les query des clients
	for(CClient *C=Clients;C;C=C->Next)
	{
		if(C->Update( elapsed ))
		{
			//client timed out
			bb_serverDisconnectClient( C->BabonetID );
			RemoveClient(C->BabonetID);
		
			break;
		}
	}

	for(cBV2game *G=Games;G;G=G->Next)
	{
		if(G->Update( elapsed ))
		{
			//game timed out, remove it form the list
			
			if(G->Next)	G->Next->Previous = G->Previous;
			if(G->Previous)	G->Previous->Next = G->Next;

			if(G == Games) Games = G->Next;
			delete G;
			NbGames--;
			break;
		}
	}


	// we check 2 times a day
	if(TimeSinceBanCheck >= 43200)
	{
		CheckBans();
		TimeSinceBanCheck = 0;
	}

	return 0;
}

void cMasterServer::AddClient(long baboNetID,char *IP)
{

	//on va se rendre a la fin des clients presentement connecter
	CClient *C	=	Clients;

	if(C)
	{
		while(C->Next)
		{
			C = C->Next;
		}

		C->Next			=	new CClient((unsigned long)baboNetID,IP);
		C->Next->Previous	=	C;
		//C			=	C->Next;
	}
	else
	{
		Clients =	new CClient((unsigned long)baboNetID,IP);
		//C	=	Clients;
	}
}

int cMasterServer::GetBV2List(CClient *client)
{

	if(!client) return 0;

	if(client->nbGames > 0) 
	{
		printf("Player %i is already querying master\n",client->BabonetID);
		return 0;
	}

	//on va sneder les info du master au player
	stMasterInfo info;
	info.NbGames	=	NbGames;
	
	bb_serverSend((char*)&info,sizeof(stMasterInfo),MASTER_INFO,client->BabonetID);
	
	//y a til des enregistrement retourne ?
	if(NbGames)
	{
		client->nbGames = NbGames;
		client->CurrentGame = 0;

		int i=0;
		//on va copier les game a notre client
		for(cBV2game *G=Games;G;G=G->Next)
		{
			memcpy(&(client->CurrentGames[i]),&(G->GameInfos),sizeof(stBV2row));

			i++;
			if(i>=100) return 0;
		}
	}
	
	

	return 0;
}

int cMasterServer::UpdateGame(stBV2row *row,unsigned long fromID)
{
	// check for racial slurs in the server name
	if( (strstr( row->serverName, "Nigger" )) || (strstr( row->serverName, "nigger" )) || (strstr( row->serverName, "Niger" )) || (strstr( row->serverName, "niger" )))
	{
		return 0;
	}

	//on va creater la game, le id est retourner peu importe si elle existe ou pas

	CClient *c = GetClientByID(fromID);
	if(!c) return 0;

	// ban some IPs
	if( !stricmp( c->IP, "82.0.179.212" ))
	{
		return 0;
	}

	c->isServer = true;
	c->Timeout = 0;

	// let's check if the database version is good, if not, we'll send the new version/url
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	**azResult;		// Contient le resultat de la requete
	int	nRow;			// nombre denregistrement retourner par la requete
	int	nColumn;		// nombre de column
	char	SQL[300];
	sprintf(SQL,"Select Value From Settings Where Name = 'DBVersion';");

	int dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	unsigned short version = (unsigned short)atoi(azResult[1]);
	sqlite3_free_table(azResult);

	if( row->DBVersion != version )
	{
		sprintf(SQL,"Select Value From Settings Where Name = 'AccountURL';");
		dbResult = sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
		
		// send url!
		char *buf = new char[512];
		memcpy(buf,&version,2);
		sprintf(buf + 2,azResult[1]);
		bb_serverSend( buf,512,ACCOUNT_URL,fromID);
		delete [] buf;
	}

	
	int gameID = CreateGame(new cBV2game(c->IP,*row));
	return gameID;
}

// int cMasterServer::RegisterPlayer(const stPlayerRegister & reg)
// {
// 	//let's check wich action the user wants
// 	//Quelques infos sur la table qu'on va loader
// 	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
// 	char	**azResult;		// Contient le resultat de la requete
// 	int		nRow;			// nombre denregistrement retourner par la requete
// 	int		nColumn;		// nombre de column
// 	char	SQL[300];
// 
// 
// 	if(reg.Action == 0) //user wants to create a new account, let's see if the username already exists
// 	{
// 		sprintf(SQL,"Select Username from Players where Username = '%s';",reg.Username);
// 		sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
// 
// 		if(nRow > 0)//user already exists
// 		{
// 			sqlite3_free_table(azResult);
// 			return 1;
// 		}
// 		sqlite3_free_table(azResult);
// 		
// 		//let's get the last emitted baboID
// 		sprintf(SQL,"Select Max(BaboID) from Players;");
// 		sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
// 
// 		unsigned long id = 0;
// 		if(nRow > 0)//we have a max
// 		{
// 			id = atoi(azResult[1]) + 1;
// 			sqlite3_free_table(azResult);
// 		}
// 		else
// 		{
// 			id = 1;
// 			sqlite3_free_table(azResult);
// 		}
// 
// 		sprintf(SQL,"Insert into Players Values(%i,'%s',%i,'%s');",id + 1,reg.Password,0,reg.Username);
// 		sqlite3_exec(MasterDB,SQL,0,0,0);
// 
// 		return 0;
// 	}
// 	else
// 	{	//we want to authenticate an existing account
// 
// 		sprintf(SQL,"Select * from Players where Username = '%s';",reg.Username);
// 		sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
// 
// 		if(nRow > 0)//user exists!
// 		{
// 			//check the password
// 			if(!stricmp(azResult[6],reg.Password))
// 			{
// 				//password is good
// 				sqlite3_free_table(azResult);
// 				return 2;
// 			}
// 			else
// 			{
// 				//good user, bad password
// 				sqlite3_free_table(azResult);
// 				return 3;
// 			}
// 		}
// 		else
// 		{
// 			sqlite3_free_table(azResult);
// 			return 4;
// 		}
// 	}
// 
// }

// int cMasterServer::RegisterServer(const stServerRegister & reg,char *IP)
// {
// 	//Quelques infos sur la table qu'on va loader
// 	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
// 	char	**azResult;		// Contient le resultat de la requete
// 	int		nRow;			// nombre denregistrement retourner par la requete
// 	int		nColumn;		// nombre de column
// 	char	SQL[300];
// 
// 
// 		sprintf(SQL,"Select * from RegisteredServers where Username = '%s';",reg.Username);
// 		sqlite3_get_table(MasterDB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
// 
// 		if(nRow > 0)//server exists
// 		{
// 			//let's check password
// 			if(!stricmp(reg.Password,azResult[13]))
// 			{
// 				//password ye bon
// 				//on check si ca vient du bon IP
// 				if(!stricmp(IP,azResult[10]))
// 				{
// 					//le IP est bon, on retourne le bon ID
// 					int id = atoi(azResult[15]);
// 					sqlite3_free_table(azResult);
// 					return id;
// 				}
// 			}
// 			sqlite3_free_table(azResult);
// 			return 0;
// 		}
// 
//         sqlite3_free_table(azResult);
// 		return 0;
// 
// }

int cMasterServer::CreateGame(cBV2game* newGame)
{
	cBV2game *lastHighPriority=0; //keep the last high priority game encountered


	if(Games)
	{
		//on va a la fin dla liste
		cBV2game *G = 0;
		for(G = Games;G->Next;G=G->Next)
		{
			if(G->GameInfos.Priority == 0) lastHighPriority = G;

			//on check si la game existe deja
			if(!stricmp(G->GameInfos.ip,newGame->GameInfos.ip))
			{
				if(G->GameInfos.port==newGame->GameInfos.port)
				{
					//printf("UPDATED Game with ID: %i IP : %s port : %i",G->ID,newGame->IP,newGame->Port);
					G->LastCheck = 0;
					//la game existe deja on update,delete et on sort
					memcpy(&(G->GameInfos),&(newGame->GameInfos),sizeof(stBV2row));

					delete newGame;
					return 0;
				}
			}
		}
		//on check si la game existe deja
		if(!stricmp(G->GameInfos.ip,newGame->GameInfos.ip))
		{
			if(G->GameInfos.Priority == 0) lastHighPriority = G;

			if(G->GameInfos.port==newGame->GameInfos.port)
			{
				//printf("UPDATED Game with ID: %i IP : %s port : %i",G->ID,newGame->IP,newGame->Port);

				//la game existe deja on update, delete et on sort
				G->LastCheck = 0;
				memcpy(&(G->GameInfos),&(newGame->GameInfos),sizeof(stBV2row));

				delete newGame;
				return 0;
			}
		}

		if(newGame->GameInfos.Priority == 0 && lastHighPriority)
		{
			if(lastHighPriority->Next)	lastHighPriority->Next->Previous = newGame;
			newGame->Next = lastHighPriority->Next;
			newGame->Previous = lastHighPriority;
			lastHighPriority->Next = newGame;
		}
		else
		{
			G->Next = newGame;
			newGame->Previous = G;
		}

		//on va attribuer un nouveau ID
		//newGame->ID = ++LastBV2id;

	}
	else
	{
		Games	= newGame;
		//newGame->ID = ++LastBV2id;
	}

	//printf("CREATED Game with IP : %s port : %i\n",newGame->GameInfos.ip ,newGame->GameInfos.port);
	//*exist = false;

	NbGames++;

	return 0;
}

cBV2game* cMasterServer::GetGameByIPport(char *ip,unsigned short port)
{
	for(cBV2game* G = Games; G; G=G->Next)
	{
		if(!stricmp(G->GameInfos.ip,ip))
		{
			if(G->GameInfos.port == port)
			{
				return G;
			}
		}
	}
	return 0;
}


void cMasterServer::RemoveClient(long babonetID)
{
	for(CClient *C=Clients;C;C=C->Next)
	{
		if(C->BabonetID	==	babonetID)
		{
			if(C->Next)		C->Next->Previous	=	C->Previous;
			if(C->Previous)		C->Previous->Next	=	C->Next;

				
			if(C==Clients) Clients = Clients->Next;
			delete C;
			return;
		}
	}
}	

CClient* cMasterServer::GetClientByID(unsigned long babonetID)
{	
	for(CClient *c=Clients;c;c=c->Next)
	{
		if(c->BabonetID == babonetID) return c;
	}

	return 0;
}


cMasterServer::~cMasterServer()
{
	//sqlite3_close(MasterDB);

	//on va delter toute les players
	CClient *toKill=0;
	for(CClient *C = Clients;C;delete toKill)
	{
		toKill 	= 	C;
		C 	= 	C->Next;
	}

	//on va deleter les games de bv2
	cBV2game *toK = 0;
	for(cBV2game *G = Games;G;delete toK)
	{
		toK	=	G;
		G	=	G->Next;
	}
	
}
