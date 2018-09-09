#ifndef REMOTEADMINPACKETS_H_
#define REMOTEADMINPACKETS_H_


#define	RA_DISCONNECT 21000
#define RA_LOGIN	20000
struct net_ra_login
{
	char login[32];
	char pass[32];
};

#define RA_LOGIN_REPLY 20001
struct net_ra_login_reply
{
	long result;	// 0 = failure, 1 = success
};

#define RA_VAR_LIST		20002
#define RA_VAR			20003
struct net_ra_var
{
	char var[80];
};
#define RA_VAR_END		20004
#define RA_COMMAND		20005
struct net_ra_command
{
	char command[128];
};

#define RA_PLAYER_LIST	20006
#define RA_PLAYER_ENTRY	20007
struct net_ra_player_entry
{
	long id;
	char name[52];
	char ip[16];
};

#define RA_NEW_PLAYER	20008
struct net_ra_new_player
{
	long id;
	char name[52];
	char ip[16];
};

#define RA_PLAYER_DISCONNECTED	20009
struct net_ra_player_disconnected
{
	long id;
};

#define RA_CHAT	20010
struct net_ra_chat
{
	long id;
	char message[128];
};

#define RA_CONSOLE_BROADCAST 20011
struct net_ra_console_broadcast
{
	char message[128];
};

#define RA_CONSOLE_BROADCAST_ACTIVATE 20012
struct net_ra_console_broadcast_activate
{
	long	enabled;
};

#define RA_KEEP_ALIVE 20013

#define RA_MAP_LIST_REQUEST 20014
#define RA_MAP_LIST_ENTRY 20015
struct net_ra_map_entry
{
	char mapName[64];
};


#define RA_MAP_FILE_HEADER 20016
struct net_ra_map_file_header
{
	long fileSize;
	char fileName[64];
};

#define RA_MAP_FILE_CHUNK 20017
struct net_ra_map_file_chunk
{
	long chunkSize;
	char * chunkData;

	net_ra_map_file_chunk()
	{
		chunkData = 0;
	}

	~net_ra_map_file_chunk()
	{
		if( chunkData ) delete [] chunkData;
	}
};

#define RA_MAP_ROTATION_REQUEST 20018
#define RA_MAP_ROTATION_ENTRY 20019
struct net_ra_map_rotation_entry
{
	char mapName[64];
};

#define RA_PLAYER_POSITION 20020
struct net_ra_player_position
{
	long playerId;
	short x;
	short y;
};

#endif
