#ifndef _MASTERSERVER_H
#define	_MASTERSERVER_H

#ifdef WIN32
	#include "stdlib.h"
	#include "string.h"
#else
	#include "LinuxHeader.h"
#endif

#include "cMSstruct.h"
#include "baboNet.h"
#include "CClient.h"
#include "cBV2game.h"
#include "sqlite3.h"


	//Macros for field indexing 
	
	#define BV2_SERVERID		CurrentRow*nColumn+12
	#define BV2_VERSION			CurrentRow*nColumn+13
	#define BV2_ID				CurrentRow*nColumn+14
	#define BV2_FLAGS			CurrentRow*nColumn+15
	#define BV2_GAMETYPE		CurrentRow*nColumn+16
	#define BV2_IP				CurrentRow*nColumn+17
	#define BV2_MAP				CurrentRow*nColumn+18
	#define BV2_MAXPLAYERS		CurrentRow*nColumn+19
	#define BV2_NBPLAYERS		CurrentRow*nColumn+20
	#define BV2_PASSWORD		CurrentRow*nColumn+21
	#define BV2_PORT			CurrentRow*nColumn+22
	#define BV2_SERVERNAME		CurrentRow*nColumn+23

	#define BV2_BAN_DATE		CurrentRow*nColumn+6
	#define BV2_BAN_DURATION	CurrentRow*nColumn+7
	#define BV2_BAN_ID			CurrentRow*nColumn+8
	#define BV2_BAN_IP			CurrentRow*nColumn+9
	#define BV2_BAN_MAC			CurrentRow*nColumn+10
	#define BV2_BAN_NICK		CurrentRow*nColumn+11




// struct stServerLink
// {
// 	CServer *Next;
// 	CServer *Previous;
// 	CServer *Current;
// };
// 
// struct stPlayerLink
// {
// 	cPlayer *Next;
// 	cPlayer *Previous;
// 	cPlayer *Current;
// };


class cMasterServer
{
private:
public:


	char			Version[5];		//garde la version de BaboViolent 2 ici
	
	int			NbGames;		//number of games currently being played
	cBV2game		*Games;			//list of current games in lobby

	CClient			*Clients;		//list of currently connected clients(Players or Servers)
	//stPlayerLink		*Players;		//connected players that are in *Clients
	//stServerLink		*Servers;		//connected servers that are in *Clients

	sqlite3			*MasterDB;		//pointeur sur notre master database

	
	
	cMasterServer();
	~cMasterServer();


	


	int		Update(float elapsed);
	void		AddClient(long baboNetID,char *ip);
	void		RemoveClient(long baboNetID);

	CClient*	GetClientByID(unsigned long babonetID);
	cBV2game*	GetGameByIPport(char *ip,unsigned short port);								//va retourner la game qui est sur ce ip/port
	int		CreateGame(cBV2game* newGame);										//permet de creer une nouvelle game de bv2
	void		UpdateStats(const stPlayerResult & result);


	float		TimeSinceBanCheck;

	void		CheckBans();												// to see if a ban must stop
	void		AddBan( char * NickName , char * IP , char * MAC , int Duration );					// to add or update a ban
	void		Unban( int ID );											// unban someone by ID
	void		SendBanList( unsigned long fromID , char * filter );							//function to send a ban list to a client	
	int		IsBanned( char * IP , char * MAC );									// return 0 if the guy is not banned, return 1 if he is

	int		RegisterServer(const stServerRegister & reg,char *ip);							//authentification of a server, return 0 on failure, return the ServerID on success
	int		RegisterPlayer(const stPlayerRegister & reg);								//tries to register a player
	int		GetBV2List(CClient *client);										//va pogner la liste des games de bv2, et la donner au player qui l'attend
	int		UpdateGame(stBV2row *row,unsigned long fromID);								//met a jour un enregistrement dla DB
	void		GetLastBV2ID();												//get the last gameID
};


#endif
