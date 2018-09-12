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

#ifndef SERVER_H
#define SERVER_H


#include "Game.h"
#include <map>

#ifndef WIN32
	#include "platform_types.h"
    #include <string>
#endif
#if defined(_PRO_)
	#include "ChecksumQuery.h"
	#include <vector>
   #define GAME_VERSION_SV 21100
#else
   #define GAME_VERSION_SV 21000
#endif

#define GAME_UPDATE_DELAY 20


class CCurl;

struct cachedPlayer
{
	bool Valid;
	char NickName[32];
	char IP[16];
	char macAddr[20];
};

struct invalidChecksumEntity
{
	int id;
	char name[32];
	char playerIP[16];
};

class Server
{
public:
	// Le jeu controll�
	Game * game;

#ifndef DEDICATED_SERVER
	// La font
	unsigned int font;
#endif

	#if defined(_PRO_)
		std::vector<CChecksumQuery *> m_checksumQueries;
	#endif

	// Si le server run
	bool isRunning;

	// une erreur ou de quoi, on doit shutdowner
	bool needToShutDown;

	// Le temps restant avant le prochain ping
	float pingDelay;

	// Le delait avant de switcher de map
	float changeMapDelay;
	CString nextMap;

	// La liste des maps � jouer
	std::vector<CString> mapList;

	//A list of all maps + their sizes;
	std::vector<mapInfo> mapInfoList;

	// Ban list, name & IP
	std::vector< std::pair<CString,CString> > banList;

	// 50 Cached players in ram only , so we can ban them eventually
	int			 CachedIndex; // what index are we going to use for next client
	cachedPlayer CachedPlayers[50];

	// List of players downloading maps (playerID,map)
	struct SMapTransfer
	{
		unsigned long	uniqueClientID;
		CString	mapName;
		int		chunkNum;
	};
	std::vector<SMapTransfer> mapTransfers;

	// List of commands that can be used with vote
	std::vector<CString> voteList;

	// List of authorization requests
	std::vector<CCurl*>	authRequests;

	const float maxTimeOverMaxPing;

	//const float maxIdleTime;

	long frameID;

	float autoBalanceTimer;

	float infoSendDelay;

public:
	// Constructeur
	Server(Game * pGame);

	// Destructeur
	virtual ~Server();

	// Pour starter le server
	int host();

	// Pour l'updater
	void update(float delay);
	void updateNet(float delay, bool send);
	void updateCTF(float delay);
	void updateSnD(float delay);
	void sendServerInfo();

	// Pour changer la map
	void changeMap(CString & mapName);
	void addmap(CString & mapName);
	void removemap(CString & mapName);
	std::vector<CString> populateMapList(bool all = false);

#if defined(_PRO_)
	std::vector<invalidChecksumEntity> getInvalidChecksums(unsigned long bbnetID, int number, int offsetFromEnd);
	//void sendInvalidChecksums(unsigned long bbnetID, int number, int offsetFromEnd);
	void deleteInvalidChecksums();
	int getNumberOfInvalidChecksums();
#endif

	// Le voting validity
	bool validateVote(CString vote);

	void nukeAll();
	void nukePlayer(int);
	// Pour chatter
	void sayall(CString message);
#ifndef DEDICATED_SERVER
	// Pour le dessiner !?
	void render();
#endif

	// On a re�u un message y�� !
	void recvPacket(char * buffer, int typeID, unsigned long bbnetID);

	// Pour modifier une variable remotly
	void sendSVChange(CString varCom);

	// Send player list to a remote admin
	void SendPlayerList( long in_peerId );

	// Pour aller chercher la prochaine map � loader
	CString queryNextMap();

	// Auto balance
	void autoBalance();

	bool filterMapFromRotation(const mapInfo & map);

	std::vector<std::string> reportUploadURLs;

	typedef std::multimap<int, PlayerStats*> StatsCache;
	typedef std::pair<int, PlayerStats*> StatsCachePair;

	StatsCache getCachedStats()
	{
		return statsCache;
	}

private:
	void cacheStats(const Player* player);

	void cacheStats(const Player* player, int teamid);

	PlayerStats* getStatsFromCache(int userID);

	void removeStatsFromCache(int userID);

	void clearStatsCache();

	void updateStatsCache();

	// List of stats of disconnected players, cleared at the end of round
	StatsCache statsCache;

	std::vector<CCurl*> reportUploads;

	struct delayedKickStruct
	{
		delayedKickStruct(unsigned long _babonetID, int _playerID, float _timeToKick)
		{
			babonetID = _babonetID;
			playerID = _playerID;
			timeToKick = _timeToKick;
		}

		UINT4 babonetID;
		int playerID;
		float timeToKick;
	};

	// babonetID->info
	typedef std::map<unsigned long, delayedKickStruct> DelayedKicksMap;
	typedef std::pair<unsigned long, delayedKickStruct> DelayedKicksPair;
	DelayedKicksMap delayedKicks;

	void addDelayedKick(unsigned long _babonetID, int _playerID, float _timeToKick);
};


#endif

