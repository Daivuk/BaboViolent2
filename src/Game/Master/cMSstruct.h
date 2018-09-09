#ifndef _MASTER_STRUCT_H
#define	_MASTER_STRUCT_H



//type ID des message
#define		BV2_ROW			997		//ce que le master retourne pour BV2
#define		BV2_LIST		999		//un client nous demande la liste des game de BV2
//#define	STOP_QUERY		1000	//REMOVED, not usefull anymore
#define		KILL_SERV		1001	//un client nous dit que son serveur est dead, not used anymore
#define		MASTER_INFO		1002	//packet qui va contenir le nombre denregistrement retourner par la derniere requete
#define		PLAYER_RESULT	1003	//used to update stats for a player after a game
#define		PLAYER_REGISTER	1004	//used to registere a new BaboStats player
#define		REGISTER_RESULT	1005	//used to send the player if the registration was OK
#define		FINISH_RESULT	1006	//used to tell MasterServer that we have finished sending him baboStats(no packet, just a typeID)
#define		SERVER_REGISTER	1007	//used to try registering with master server

#define		IM_SERVER	1008	//used to tell master server, we are a server (no struct needed)
#define		IM_PLAYER	1009	//used to tell master server, we are a player (no struct needed)	


#define		CACHE_BAN	1010	// tell the master we are sending him a cache ban, also to send a server requestiong cacheban
#define		CACHE_BAN_LIST	1011	// tell the master we want the list of cached bans ( with a filter for name if any present )
#define		CACHE_UNBAN	1012	// tell the master we want to unban someone 
#define		CACHE_BANNED	1013	// ask the master if the player is banned
#define		CACHE_ANSWER	1014	// tell the server who ask for a banned player if it is actually banned

#define		ACCOUNT_URL	1015	// tell the accountUrl

#define		SURVEY_SUBMIT	1016	//--- Survey ID
#define		SURVEY_NEVER	1017	//--- The guy don't want to answer the survey, E.V.E.R
#define		SURVEY_CONFIRM	1018	//--- Master send confirmation that he received the survey

#define		CACHE_LIST_REMOTE	1019	// tell the master we want the list of cached list of remote server
#define		CACHE_LIST_PLAYER	1021	// received along with data of cached players
#define		CACHE_LIST_PLAYER_END	1022	// remote server -> master, no more players in cache
#define		CACHE_LIST_REQUEST_CANCEL	1023	// client -> master, cancel request

#define		CLIENT_HASH			1024

#define		CREATE_ACCOUNT		1025
#define		DELETE_ACCOUNT		1026
#define		UPDATE_ACCOUNT		1027
#define		CHANGE_PASSWORD		1028
#define		RECOVER_PASSWORD	1029
#define		LOGIN_ACCOUNT		1030
#define		LOGOUT_ACCOUNT		1031

#define		USER_STATUS_UPDATE	1032

#define		REGISTER_CLAN		1036
#define		REMOVE_CLAN			1037
#define		CHANGE_CLAN_PERMISSIONS	1038
#define		JOIN_CLAN_REQUEST	1039
#define		JOIN_CLAN_ACCEPT	1040
#define		LEAVE_CLAN			1041

#define		REQUEST_FRIEND		1046
#define		ACCEPT_FRIEND		1047
#define		REMOVE_FRIEND		1048
#define		MOVE_FRIEND			1049

#define		STATS_UPDATE		1050

#define		ACCOUNT_MGR_RESULT	1060

	// Ban structs

	//CACHE_BAN		1010
	struct stCacheBan
	{
		short	ID;
		char	Nick[32];
		char	IP[16];
		char	MAC[20];
		char	Duration;
		char	Date[9];
		char	Pass[8];
	};

	//CACHE BANLIST
	struct stCacheList
	{
		char Filter[32];	// 32 char max filter, can be '\0' for none
	};

	//CACHE_LIST_REMOTE
	typedef struct
	{
		unsigned long FromID;
		short ReqNum;		// used to number consecutive cache list requests
		char Filter[32];	// 32 char max filter, can be '\0' for none
		char ServerIP[16];
		char ServerPort[6];
	} stCacheListRemote;

	//CACHE_UNBAN		1012
	struct stCacheUnban
	{
		short	ID;
		char	Pass[10];
	};

	//CACHE_BANNED		1013
	struct stCacheBanned
	{
		short	ID;			// player ID, only used for the server
		char	IP[16];
		char	MAC[20];
	};

	//CACHE_ANSWER		1014
	struct stCacheAnswer
	{
		short	ID;
		char	Answer;	// 0 = no , 1 = yes
	};

	typedef struct
	{
		unsigned long FromID;
		short ReqNum;		// id of request this data belong to
		short ID;
		char NickName[32];
		char IP[16];
		char macAddr[20];
	} stCachePlayer;

	//CACHE_LIST_PLAYER_END
	struct stCacheListEnd
	{
		unsigned long requestFromID;
	};

	//CLIENT_HASH
	typedef struct
	{
		short s1;
		short s2;
		short s3;
		short s4;
	} stHashSeed;

	//CREATE_ACCOUNT, UPDATE_ACCOUNT
	typedef struct
	{
		char Login[33];
		char HashedPassMD5[17]; // md5 of password hashed with seed from stHashSeed
		char Nick[33];
		char Email[49];
	} stAccount;

	//DELETE_ACCOUNT
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedPassMD5[17];
	} stDeleteAccount;

	//CHANGE_PASSWORD
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedOldPassMD5[17];
		char HashedNewPassMD5[17];
	} stChangePassword;

	//RECOVER_PASSWORD
	typedef struct
	{
		char Login[33];
	} stRecoverPassword;

	//LOGIN_ACCOUNT, LOGOUT_ACCOUNT
	typedef struct
	{
		char Login[33];
		char HashedPassMD5[17];
	} stLoginLogout;

	//USER_STATUS_UPDATE
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedPassMD5[17];
		char ServerName[65]; // 63 + '\0'
		char IP[16];
		unsigned short Port;
		//char LastSeenDate[12]; // format HH:MM:SS, end with "\0"
		//char LastSeenTime[12]; // format YYYY-MM-DD, end with "\0"
	} stUserStatusUpdate;

	//REGISTER_CLAN
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedPassMD5[17];
		char Name[49];
		char Tag[9];
		char Email[49];
		char Website[49];
	} stRegisterClan;

	//REMOVE_CLAN
	//user can remove only the clan he is admin of
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedPassMD5[17]; // pass of user requesting this op
	} stRemoveClan;

	//CHANGE_CLAN_PERMISSIONS
	typedef struct
	{
		int UserID;	// user requesting this operation, have to be admin of clan
		char HashedPassMD5[17];
		int DestUserID; // user whose permissions we are about to change
		char Permissions; // 1 - make admin, 2 - remove admin
	} stChangeClanPermissions;

	//JOIN_CLAN_REQUEST
	typedef struct
	{
		int UserID;	// user requesting this operation, have to be admin of clan
		char HashedPassMD5[17];
		int DestUserID; // user that gonna be added to clan members
	} stJoinClanRequest;

	//JOIN_CLAN_ACCEPT
	typedef struct
	{
		int UserID;	// user that was invited to clan
		char HashedPassMD5[17];
		int UserIDFrom; // stJoinClanRequest.UserID
	} stJoinClanAccept;

	//LEAVE_CLAN
	typedef struct
	{
		int UserID;	// user requesting this operation, member of a clan ofc
		char HashedPassMD5[17];
	} stLeaveClan;

	//REQUEST_FRIEND, ACCEPT_FRIEND, REMOVE_FRIEND, MOVE_FRIEND
	typedef struct
	{
		int UserID;	// user requesting this operation
		char HashedPassMD5[17];
		int DestUserID; // requested friend or friend that gonna be added/removed
		char GroupName[50];
	} stFriendOp;

	//STATS_UPDATE
	typedef struct
	{
	} stStatsUpdate;

	//ACCOUNT_MGR_RESULT
	typedef struct
	{
		int MessageID;
		int Param;
		char Result; // 0 - ok, 1 - failed
		char Info[100];
	} stAccountMgrResult;

//Les struct Baboviolent2

	//this is the struct used to query the current game list
	//only client version is needed
	//struct typeID 999
	struct stBV2list
	{
		char	Version[5]; //exemple :  2.02  + '\0'
	};

	


	
	//struct received by master server after a query
	//if NbGames == -1 : Wrong version  ... else : Number of games
	//struct typeID 1002
	struct stMasterInfo
	{
		short		NbGames;
	};





	//Current Game Infos
	//struct typeID 997		//total bytes : 132
	struct stBV2row
	{
		char			map[17]; // 16 + '\0'				//TEXT
		char			serverName[64]; // 63 + '\0'			//TEXT
		char			password[16]; // 15 + '\0'			//TEXT
		char			ip[16];						//TEXT		lors d'un update vers le master, IP peut etre vide, le master va catcher le IP anyway en pognant le packet
		unsigned short		port;						//NUMERIC	
		char			nbPlayer;					//NUMERIC
		char			maxPlayer;					//NUMERIC
		unsigned short		flags;						//NUMERIC
		char			gameType;					//NUMERIC
		unsigned short		ServerID;					//if server is REGISTERED, put ID here, else, leave to 0
		char			Version[5];					//version of server
		char			Priority;					//master server usage only, leave at 0
		unsigned short		DBVersion;
		unsigned short		Padding;
	};

	



	//After a BaboStats enabled match
	//use this struct to update the database with each player
	//only the server uses this
	//Struct typeID 1003
	struct stPlayerResult
	{
		unsigned short	ServerID;		//UniqueID of the current server (only registered servers have this, 0 otherwise)
		unsigned long	BaboID;			//Unique ID of the bv2 player (received after the player has connected to the server)
        	unsigned short	Deaths;			//Number of deaths in this match
		unsigned short	Frags;			//Number of Frags
		unsigned long	Medals;			//The player has win any medals ?
		short		XP;			//Increment or Decrement of current player's XP
		unsigned short	Molotov;		//number of molotov kills
		unsigned short	Grenades;		//number of grenade kills
		unsigned short	Bazooka;		//number of bazooka kills
		unsigned short	SMG;			//number of SMG kills
		unsigned short	Shotgun;		//number of shotgun kills
		unsigned short	Sniper;			//number of sniper kills
		unsigned short	DMG;			//number of DualMachinegun kills
		unsigned short	Chaingun;		//number of chaingun kills
		unsigned short	Photon;			//number of photon rifle kills
		unsigned short	Nuke;			//number of Nuke kills
		unsigned short	Knife;			//number of Knife kills
	};






	//IF a server is BaboStats enabled, player must register in the option menu, or enter his current infos for stats tracking
	//use this struct to send what the user entered
	//Struct typeID 1004
	struct stPlayerRegister
	{
		char	Action;		//Action, 0 = User wants to create new account. 1 = User wants to authentify(account already created)
		char	Username[16];	//username entered (what will be shown on website)
		char	Password[16];	//password the user entered
		//char	cdkey[33];	//not yet
	};


	//If a server wants to be BaboStats Enabled,
	//it must first query master with username/password on creation
	//to get unique ID from Master, that he enclosed with every player stats sent
	//struct type 1007
	struct stServerRegister
	{
		char	Username[16];	//username that we give to official server
		char	Password[16];	//password the server must enter
	};






	//when a player or server registers, the result of Succes of creation/authentification
	//or failure is returned here
	//on success, ID is != 0
	//Struct typeID 1005
	
	/* :Result: can be one of these:

		On creation :

		0 :		Evrything is fine, new player creation successfull
		1 :		Failed, Username already exists

		On re-input (when i'm on another computer, but i entered Username/Password to keep my stats)

		2:		Evrything is fine, username/password exists and are valid
		3:		Username exists, but password is wrong
		4:		Username doesn't exist


		FOR SERVERS THAT WANTS TO AUTHENTICATE:

		0:		Failure
		1:		Success

	*/
	struct stRegisterResult
	{
		char	Result;
		long	ID;			
	};





	//tell the master server your server has closed, only server sends this
	//struct typeID 1001
	struct stKillServ
	{
		unsigned short Port;
	};


	//--- All information for the survey
	struct SSurvey
	{
		char gender;
		char age;
		char continent;
		char country[80];
		char gameGenre;
		char specifiedGenre[80];
		char hourPlayerGames;
		char magazine[80];
		char console;

		char hourPlayingBV2;
		char kindOfServer;
		char howYouLearnAboutBabo[80];
		char whoDoYouPlayWith; //--- bitMask
		char whatDoYouLike; //--- bitMask
		char whatDoYouLikeOther[80];

		char whatYouWouldLikeForBV3; //--- bitMask
		char whatYouWouldLikeForBV3Other[80];
		char adsAndFree;
		char payNoAds;

		char email[80];
	};



#endif
