/*
	Copyright 2012 bitHeads inc.

	This file is part of the BaboViolent 2 source code.

	The BaboViolent 2 source code is free software: you can redistribute it and/or 
	modify it under the terms of the GNU General Public License as published by the 
	Free Software Foundation, either version 3 of the License, or (at your option) 
	any later version.

	The BaboViolent 2 source code is distributed in the hope that it will be useful, 
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with the 
	BaboViolent 2 source code. If not, see http://www.gnu.org/licenses/.
*/

#ifndef CMASTER_H_INCLUDED
#define CMASTER_H_INCLUDED

#include "RemoteAdminPackets.h"


// some macros to help fetch data after an sql query
#define MASTER_SCORE		i*nColumn+5
#define MASTER_ID			i*nColumn+6
#define MASTER_IP			i*nColumn+7
#define MASTER_LOCATION		i*nColumn+8
#define MASTER_PORT			i*nColumn+9


#include "sqlite3.h"

#include "CPing.h"

#include <vector>

#include "Server.h"

#include "AccountManager.h"

extern bool surveyReceived;


struct SMasterMessage
{
	char* buffer;
	int typeID;
	int size;
	SMasterMessage(const char* in_buffer, int in_size, int in_typeID)
	{
		size = in_size;
		buffer = new char [in_size];
		memcpy(buffer, in_buffer, in_size);
		typeID = in_typeID;
	}
	~SMasterMessage()
	{
		delete[] buffer;
	}
};

struct SBrowsableGame
{
	int ping;
	stBV2row * bv2Row;
	~SBrowsableGame()
	{
		delete bv2Row;
	}
};

struct SPeer
{
public:
	long peerId;
	float timeout;
	bool remoteAdmin;
	bool broadcastConsole;

	// if the peer is currently sending us a new file, keep infos here
	FILE * m_currentFile;
	long	m_sizeRemaining;

	SPeer( long in_peerId )
	{
		peerId = in_peerId;
		timeout = 5.0f;
		remoteAdmin = false;
		broadcastConsole = false;
		m_currentFile = 0;
	}

	~SPeer()
	{
		if( m_currentFile ) fclose( m_currentFile );
	}
};

class AccountManagerClient : public AccountManager
{
private:
	AccountManagerClient()
	{}

	friend class CMaster;
public:
	bool createAccount(char* login, char* password, char* nick, char* email);
	bool deleteAccount(int userID, char* password);
	bool updateAccount(char* login, char* password, char* nick, char* email);
	bool changePassword(int userID, char* oldPass, char* newPass);
	bool recoverPassword(char* login);
	bool loginAccount(char* login, char* password);
	bool logoutAccount(char* login, char* password);
	bool userStatusUpdate(int userID, char* password, char* serverName, char* ip, short port);
	bool registerClan(int userID, char* password, char* name, char* tag, char* email, char* website);
	bool removeClan(int userID, char* password);
	bool changeClanPermissions(int userID, char* password, int userIDChanging, char permissions);
	bool joinClanRequest(int userID, char* password, int userIDDest);
	bool joinClanAccept(int userID, char* password, int userIDFrom);
	bool leaveClan(int userID, char* password);
	bool requestFriend(int userID, char* password, int userIDDest, char* groupName);
	bool acceptFriend(int userID, char* password, int userIDDest, char* groupName);
	bool removeFriend(int userID, char* password, int userIDDest);
	bool moveFriend(int userID, char* password, int userIDDest, char* groupName);
	bool statsUpdate();
};

class CMaster
{
private:
	// account stuff hash values (received from server)
	short s1,s2,s3,s4;

	// holds the master server IP
	char m_IP[16];

	// holds the master server Port
	unsigned short m_Port;

	// keep peers here...some might be trying to login in remotely
	std::vector<SPeer>	m_peers;

	// holds the current game version we have
	char m_CurrentVersion[5];

	unsigned long uniqueClientID;
	bool m_isConnected;
	std::vector<SMasterMessage*> messageStack;
	int m_nbGameFound;
	std::vector<stBV2row*> m_games;
	CPing *m_ping;

	// id of last remote cache list request
	short cacheListReqNum;

	//--- Connecting the master
	void connectToMaster(const char* in_IP, short in_port);

	//--- To handle the incomming packets
	void recvPacket(const char * buffer, int typeID);

	//--- Effacer toute les games pr�sente
	void eraseGames();

	//--- La row est valid�, on a le ping pis toute
	void pingReceived(int ping);

	// to get IP/port of current master server in the sqlite3 database
	void GetMasterInfos();

	void UpdateDB();

	bool cachePlayerRemoteServer(const stCachePlayer& player);

	void clearRemoteCacheList();

	void sendCacheListToMaster(const stCacheListRemote& info);

	// hlper function to know if the current peer is remote admin authorized
	bool IsRemoteAdmin( long in_peerId );

public:
	AccountManagerClient AccountManager;

	stCacheAnswer	BannedAnswers[64]; //we keep a max of 64 cached banned answers

	cachedPlayer CachedPlayersRemote[50];

	Server		*RunningServer; //the server currently running on the machine

	CMaster();
	virtual ~CMaster();

	//--- Kill it
	void disconnectMaster();

	//--- Updating the master client
	void update(float in_delay);

	//--- Send message to master (If not connected, it will try open a connection)
	void sendPacket(const char* in_data, int in_size, int in_ID, bool disconnectAfter=false);

	//--- Requester la game
	void requestGames();

	//--- Read data from the p2p module ( used for udp broadcast )
	void ReceivePeersPacket();

	//--- Shooter l'info de la game
	void sendGameInfo(Server* server);

	//--- Est-ce qu'on est pr�sentement connect� au master
	bool isConnected() const {return (uniqueClientID != 0);}

	void requestRemoteCacheList(CString& strFilter, CString& serverIP, CString& serverPort, unsigned long baboid);

	bool hasValidHashSeed();

	//pass max len is 16 + null
	void HashPass(char* pass);

	// tell remote admins about a new player/disconnected player
	void RA_NewPlayer( char * in_playerName, char * in_playerIp, long in_playerId );
	void RA_DisconnectedPlayer( char * in_playerName, char * in_playerIp, long in_playerId );
	void RA_Chat( char * in_chatString, int in_playerId );
	void RA_ConsoleBroadcast( char * in_message );
	void RA_PositionBroadcast( long in_playerId, short in_x, short in_y );
};


extern CMaster* master;


#endif

