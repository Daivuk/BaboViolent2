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

#include "Server.h"
#include "Console.h"
#include "netPacket.h"
#include "RemoteAdminPackets.h"
#include "CCurl.h"
#include "ReportGen.h"
#include <time.h>
#include <fstream>
#include <algorithm>
#ifdef WIN32
	#include <direct.h>
#else
	#include <dirent.h>
#endif



//
// Constructeur
//
Server::Server(Game * pGame): maxTimeOverMaxPing(5.0f)//, maxIdleTime(180.0f)
{
	game = pGame;
	game->isServerGame = true;
#ifndef CONSOLE
	font = dkfCreateFont("main/fonts/babo.tga");
#endif
	needToShutDown = false;
	pingDelay = 0;
	changeMapDelay = 0;
	frameID = 0;
	autoBalanceTimer = 0;
	infoSendDelay = 15;

	// reset cached users
	CachedIndex = 0; // what index are we going to use for next client
	for( int i = 0; i < 50; i++ )
	{
		CachedPlayers[i].Valid = false;
		CachedPlayers[i].macAddr[0] = '\0';
		CachedPlayers[i].NickName[0] = '\0';
		CachedPlayers[i].IP[0] = '\0';
	}

	// Load banlist
	std::ifstream file("main/banlist", std::ios::binary);
	char name[32], ip[16];

	while(file.is_open() && !file.eof())
	{
		file.read(name, sizeof(char)*32);
		file.read(ip, sizeof(char)*16);

		// Will not hit eof until after first read
		if(file.eof()) break;

		// Add to ban list
		banList.push_back( std::pair<CString,CString>(name, ip) );

	}
	//reportUploadURLs.push_back("http://localhost/index.php");
}



#include "CMaster.h"
//
// Destructeur
//
Server::~Server()
{
#ifdef _PRO_

	for( unsigned int i=0; i<m_checksumQueries.size(); i++ )
	{
		delete m_checksumQueries[i];
	}
	m_checksumQueries.clear();

#endif


	if (master) master->RunningServer = 0;

    mapList.clear();
	banList.clear();
#ifndef CONSOLE
	dkfDeleteFont(&font);
#endif

	bb_serverShutdown();
	ZEVEN_SAFE_DELETE(game);

	stKillServ killServ;
	killServ.Port = gameVar.sv_port;
	if (master) master->sendPacket((char*)(&killServ), sizeof(stKillServ), KILL_SERV);
}



//
// Le voting validity
//
bool Server::validateVote(CString vote)
{
	CString com = vote;
	CString command = com.getFirstToken(' ');
	CString var = com.getFirstToken(' ');

	// If setting a var, remove the 'set' since it is not included
	// in the list.
	if (strnicmp("set sv_", vote.s, 7) == 0)
		command = var;

	// Not case sensitive
	command.toLower();

	// Compare against allowed commands
	for (std::vector<CString>::iterator i = voteList.begin(); i != voteList.end(); ++i)
		if (*i == command)
			return true;

	return false;
}



//
// Pour starter le server
//
int Server::host()
{
	//permet de spawner un serveur
	if (bb_serverCreate(false, MAX_PLAYER, gameVar.sv_port)) 
	{
		isRunning = false;
		return 0;
	}
	else 
	{
		srand((unsigned int)time(0));
		game->mapSeed = rand()%1000000; // Quin, 1000000 maps, c tu assez �?
		game->createMap();
		nextMap = game->mapName;
		mapList.push_back(game->mapName);
		if(!game->map)
		{
			console->add("\x4> Map not loaded", true);
			needToShutDown = true;
			isRunning = false;
			return 0;
		}
		if(!IsMapValid(*game->map, game->gameType))
		{
			console->add("\x4> Map is missing some entities, server can not be started", true);
			needToShutDown = true;
			isRunning = false;
			return 0;
		}
		isRunning = true;

		// La game roule!
		// On ouvre notre port UDP
		if (bb_peerBindPort(gameVar.sv_port ) == 1)
		{
			console->add(CString("\x4> Error binding port on %i.", gameVar.sv_port/* + 1*/), true);
		}

		//"register" the server with our master server ( for udp broadcast ... kind of an hack )
		master->RunningServer = this;

		return 1;
	}
}



//
// Pour changer la map
//
void Server::changeMap(CString & mapName)
{
	if (isRunning)
	{
		if (game)
		{
			// if map name is not given - get the next map from the servers map list
			mapName.trim(' ');
			if(mapName == "")
			{
				mapName = queryNextMap();
			}
			//--- Check first is that map exist.
			CString filename("main/maps/%s.bvm", mapName.s);
			FILE* fic = fopen(filename.s, "rb");
			if (!fic)
			{
				console->add(CString("\x9> Warning, map not found %s", mapName.s));
				return;
			}
			else
			{
				fclose(fic);
			}
			clearStatsCache();
			nextMap = mapName;
			changeMapDelay = 10;
			net_svcl_round_state roundState;
			game->roundState = GAME_MAP_CHANGE;
			roundState.newState = game->roundState;
			roundState.reInit = false;
			bb_serverSend((char*)&roundState, sizeof(net_svcl_round_state), NET_SVCL_GAME_STATE, 0);
		}
	}
}

void Server::addmap(CString & mapName)
{
	//--- Check first is that map exist.
	CString filename("main/maps/%s.bvm", mapName.s);
	FILE* fic = fopen(filename.s, "rb");
	if (!fic)
	{
		console->add(CString("\x9> Warning, map not found %s", mapName.s), true);
		return;
	}
	else
	{
		fclose(fic);
	}

	// Un map ne peut pas �re l�2 fois (c poche mais c hot)
	for (int i=0;i<(int)mapList.size();++i)
	{
		if (mapList[i] == mapName) return;
	}
	mapList.push_back(mapName);

	console->add(CString("\x9> %s added", mapName.s), true);
}

void Server::removemap(CString & mapName)
{
	if (mapName == game->mapName) return;
	for (int i=0;i<(int)mapList.size();++i)
	{
		if (mapList[i] == mapName)
		{
			console->add(CString("\x9> %s removed", mapName.s), true);
			mapList.erase(mapList.begin()+i);
			for (int i=0;i<(int)mapInfoList.size();++i)
			{//removes from the info list
				if (mapInfoList[i].mapName == mapName)
				{
					mapInfoList.erase(mapInfoList.begin()+i);
					return;
				}
			}
			return;
		}
	}
}

std::vector<CString> Server::populateMapList(bool all)
{
	std::vector<CString> maps;
	if (all == false)
		maps = mapList;
	else
	{
#ifdef WIN32
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		char DirSpec[MAX_PATH]; // directory specification
		DWORD dwError;
		char appPath[_MAX_PATH];

		// Chercher le path du "current working directory".
		_getcwd(appPath, _MAX_PATH);

		strncpy(DirSpec, appPath, strlen(appPath)+1);
		strncat(DirSpec, "\\main\\maps\\*.bvm", strlen("\\main\\maps\\*.bvm")+1);

		hFind = FindFirstFile(DirSpec, &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			// Si on ne trouve pas le rA©pertoire dA©sirA©.
		}
		else
		{
			CString filename = CString(FindFileData.cFileName);
			filename.resize(filename.len() - 4);
			maps.push_back(filename);

			while (FindNextFile(hFind, &FindFileData) != 0)
			{
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					CString filename = CString(FindFileData.cFileName);
					filename.resize(filename.len() - 4);
					maps.push_back(filename);
				}
			}

			dwError = GetLastError();
			FindClose(hFind);
			if (dwError != ERROR_NO_MORE_FILES)
			{
				// Si il y a une error en dA©tectant qu'il n'y a plus de fichiers.
			}
		}
#else
		dirent* file;
		DIR* hFile;
        char dirspec[256];
        getcwd(dirspec, sizeof(dirspec));
        strcat(dirspec, "/main/maps");
		hFile = opendir(dirspec);
		if(hFile)
		{
			while(file = readdir(hFile))
			{
				char *extension = strrchr(file->d_name, '.');
				if(extension)
				{
					if(!strcasecmp(extension, ".bvm"))
						maps.push_back(file->d_name);
				}
			}
			closedir(hFile);
		}

#endif
	}
	return maps;
}



//
// Pour aller chercher la prochaine map �loader
//
CString Server::queryNextMap()
{
	CString currentName = game->mapName;
	int lengthDif = mapList.size() - mapInfoList.size();
	for(int i = mapList.size() - lengthDif; i < mapList.size(); i++)
	{//because generating the map info requires creating the map and we dont want to do this midgame with the addmap command for fear of bugs, this is kept separately in map list
		game->mapName = mapList[i];
		game->createMap();//we just create and add any maps in maplist that arent also in the info list
		int mapArea = 0;
		for (int j = 1; j < game->map->size[0] - 1; j++)
			for (int k = 1; k < game->map->size[1] - 1; k++)
			{//get the total area
				if(game->map->cells[k*game->map->size[0] + j].passable)
				{
					mapArea++;
				}
			}
		mapInfo mInfo = {game->mapName, mapArea, 1000000000};
		mapInfoList.push_back(mInfo);
	}
	if (lengthDif > 0)
	{
		game->mapName = currentName;
		game->createMap();//since we're done with the game's map and other functions might not be, we put it back the way we found it, who needs proper OOP when you've got manners anyways?
	}
	int indexOfMax = -1;
	for (int i = 0; i < (int)mapInfoList.size(); ++i)
	{
		if (mapInfoList[i].mapName == currentName)
		{
			mapInfoList[i].lastPlayed = 1;
		}
		else
		{
			mapInfoList[i].lastPlayed++;
		}
		bool suitable = filterMapFromRotation(mapInfoList[i]);
		if (suitable)//Only select indices of suitable maps
		{
			if(indexOfMax == -1)
			{
				indexOfMax = i;
			}
			else if(mapInfoList[i].lastPlayed > mapInfoList[indexOfMax].lastPlayed)
			{
				indexOfMax = i;
			}
		}
	}
	if(indexOfMax >= 0)
	{
		CString infoOutput = mapInfoList[indexOfMax].mapName;
		infoOutput+= ": ";
		infoOutput+= mapInfoList[indexOfMax].mapArea;
		console->add(infoOutput, true, true);
		mapInfoList[indexOfMax].lastPlayed = 0;
		return mapInfoList[indexOfMax].mapName;
	}
	else
	{
		int n = (int)mapInfoList.size() - 1;
		int index = rand(0,n);
		mapInfoList[index].lastPlayed = 0;
		return mapInfoList[index].mapName;
	}
	if (mapList.size() > 0)
	{
		return mapList[0];
	} 
	return "";
}



//
// Update de babonet
//
void Server::updateNet(float delay, bool send)
{
	// On update le server
	char IPDuGars[16];
	int clientID = bb_serverUpdate(delay, UPDATE_SEND_RECV, IPDuGars);//(send)?UPDATE_SEND:UPDATE_RECV);
	console->debugBBNET(false, true);
	int playerID = -1;
	if (clientID > 0)
	{
		// On a un nouveu client!
		console->add(CString("\x3> A client has connected. Client ID : %i", clientID), true);

		// Check against ban list
		for(std::size_t i = 0; i < banList.size(); ++i)
		{
			if(banList[i].second == IPDuGars)
			{
				bb_serverDisconnectClient(clientID);
				console->add(CString("\x3> Disconnecting banned client, %s. IP: %s",
								banList[i].first.s, banList[i].second.s), true);
				return;
			}
		}

		// On le cr�A
		playerID = game->createNewPlayerSV(clientID);
		if (playerID == -1)
		{
			// Oups!! Y a pus de place, on le canne
			bb_serverDisconnectClient(clientID);
			console->add("\x3> Disconnecting client, server is full", true);
		}
		else
			strcpy(game->players[playerID]->playerIP, IPDuGars);
		/*PlayerStats* ps = getStatsFromCache(game->players[playerID]->userID);
		if (ps != 0)
			ps->MergeStats(game->players[playerID]);*/
	}
	else if (clientID == BBNET_ERROR)
	{
		// Une erreur !!! On arr�e tout !!!
		console->add(CString("\x3> Error : %s", bb_serverGetLastError()), true);
		needToShutDown = true;
	}
	else if (clientID < 0)
	{
		// On client a disconnect�
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->babonetID == -clientID)
				{
					// Save stats to cache
					if (game->players[i]->timePlayedCurGame > EPSILON)
						cacheStats(game->players[i]);
					// On cancel le vote si on pensait le kicker!
					if (game->voting.votingInProgress)
					{
						CString com = game->voting.votingWhat;
						CString command = com.getFirstToken(' ');
						if (command == "kick" || command == "ban")
						{
							if (com == textColorLess(game->players[i]->name))
							{
								//--- CANCEL THE VOTE!
								net_svcl_vote_result voteResult;
								voteResult.passed = false;
								bb_serverSend((char*)(&voteResult), sizeof(net_svcl_vote_result), NET_SVCL_VOTE_RESULT);
							}
						}
						else if (command == "kickid" || command == "banid")
						{
							if (com.toInt() == i)
							{
								//--- CANCEL THE VOTE!
								net_svcl_vote_result voteResult;
								voteResult.passed = false;
								bb_serverSend((char*)(&voteResult), sizeof(net_svcl_vote_result), NET_SVCL_VOTE_RESULT);
							}
						}
					}
					// On le disconnect !!
					console->add(CString("\x3> Player disconnected : %s ID:%i", game->players[i]->name.s, i), true);
					// broadcast to potential remote admins
					if( master ) master->RA_DisconnectedPlayer( textColorLess(game->players[i]->name).s, game->players[i]->playerIP, (long)game->players[i]->playerID );
					ZEVEN_SAFE_DELETE(game->players[i]);
					net_svcl_player_disconnect playerDisconnect;
					playerDisconnect.playerID = (char)i;
					bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
					break;
				}
			}
		}
	}
}



#include "CMaster.h"
//
// On envoit les info au master
//
void Server::sendServerInfo()
{
	if (master && gameVar.sv_gamePublic)
		master->sendGameInfo(this);
}

//void Server::SendGlobalBan( CString nick , CString IP , int duration )
//{
//	// lets tell master server we want to send him a global ban
//	if(master)
//	{
//
//		// build ou packet
//
//		master->sendPacket(
//	}
//}



//
// Pour l'updater
//
void Server::update(float delay)
{
	if (game && isRunning)
	{
	/*	if (infoSendDelay == 0)
		{
			infoSendDelay = 0;
		}*/
		infoSendDelay += delay;
		if (infoSendDelay > 20)
		{
			sendServerInfo();
			infoSendDelay = 0;
		}

		// Check to see if we have auth request responses
		if( authRequests.size() > 0 )
		{
			// Since deleting from a vector invaldates the iterator, we'll add to another
			// vector and swap.
			std::vector<CCurl*> stillRunning;

			// Iterate through requests
			for(std::vector<CCurl*>::iterator i = authRequests.begin(); i != authRequests.end(); ++i)
			{
				CCurl* request = (*i);
				if(!request->isRunning())
				{
					console->add(CString("[Auth] Response (%i bytes) recieved",request->recieved()));

					// Get player id
					int id = *((int*)request->userData());
					delete (int*)request->userData();

					// Get user id
					if(request->recieved() > 0 && game->players[id] && request->recieved() < MAX_CARAC - 1)
					{
						CString temp = CString("%s", request->response().c_str() );
						int userID = temp.toInt();

						// Set player's id
						if(userID > 0)
						{
							game->players[id]->userID = userID;
							console->add(CString("[Auth] %s authenticated with id %i", game->players[id]->name.s, userID), true);
							/*PlayerStats* ps = getStatsFromCache(game->players[id]->userID);
							if (ps != 0)
							{
								ps->MergeStats(game->players[id]);

								net_svcl_player_update_stats playerStats;
								playerStats.playerID = (char)id;
								playerStats.kills = (short)game->players[id]->kills;
								playerStats.deaths = (short)game->players[id]->deaths;
								playerStats.score = (short)game->players[id]->score;
								playerStats.returns = (short)game->players[id]->returns;
								playerStats.flagAttempts = (short)game->players[id]->flagAttempts;
								playerStats.timePlayedCurGame = game->players[id]->timePlayedCurGame;

								for (int i=0;i<MAX_PLAYER;++i)
								{
									if (game->players[i])
										bb_serverSend((char*)&playerStats, sizeof(net_svcl_player_update_stats), NET_SVCL_PLAYER_UPDATE_STATS, game->players[i]->babonetID);
								}
							}*/
						}
						else
						{
							console->add(CString("[Auth] %s failed authentication", game->players[id]->name.s), true);
							//// If this is a match server, kick unauthorized players
							//bb_serverDisconnectClient(game->players[id]->babonetID);
							//ZEVEN_SAFE_DELETE(game->players[id]);
							//net_svcl_player_disconnect playerDisconnect;
							//playerDisconnect.playerID = (char)id;
							//bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
						}

					}

					// We're done, delete it
					delete *i;
				}
				else
					stillRunning.push_back(*i);
			}

			authRequests.swap(stillRunning);
		}

		if (reportUploads.size() > 0)
		{
			std::vector<CCurl*>::iterator it = reportUploads.begin();
			for (; it != reportUploads.end(); )
			{
				if ((*it)->isRunning() == false)
				{
					std::string response = (*it)->response();
					if (!response.empty() && response.size() <= 480)
						console->add(CString("\x2Report Sent: %s", response.c_str()), true);
					else
						console->add("\x2Report Failure: Sent, but no response recieved.", true);
					
					delete (*it);
					it = reportUploads.erase(it);
				}
				else
					it++;
			}
		}

		if( gameVar.sv_gamePublic && master )
		{
			//check to see if we have answers from our banned query
			for( int i=0; i<64; i++ )
			{
				if( master->BannedAnswers[i].ID != -1 )
				{
					//the guy is banned
					if( master->BannedAnswers[i].Answer )
					{
						// kick
						if( master ) master->RA_DisconnectedPlayer( textColorLess(game->players[master->BannedAnswers[i].ID]->name).s, game->players[master->BannedAnswers[i].ID]->playerIP, (long)game->players[master->BannedAnswers[i].ID]->playerID );
						bb_serverDisconnectClient(game->players[master->BannedAnswers[i].ID]->babonetID);
						ZEVEN_SAFE_DELETE(game->players[master->BannedAnswers[i].ID]);
						net_svcl_player_disconnect playerDisconnect;
						playerDisconnect.playerID = (char)master->BannedAnswers[i].ID;
						bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
					}
					master->BannedAnswers[i].ID = -1;
					master->BannedAnswers[i].Answer = -1;
				}
			}
		}

#ifdef _PRO_
		// update current checksum queries
		for( unsigned int nn = 0; nn < m_checksumQueries.size(); nn++ )
		{
			if( !m_checksumQueries[nn]->Update(delay))
			{
				// we need to get rid of the client associated with that query, because we didnt receive the checksum in the right amount of time
				// kick
				if( game->players[m_checksumQueries[nn]->GetID()] )
				{
					if( master ) master->RA_DisconnectedPlayer( textColorLess(game->players[m_checksumQueries[nn]->GetID()]->name).s, game->players[m_checksumQueries[nn]->GetID()]->playerIP, (long)game->players[m_checksumQueries[nn]->GetID()]->playerID );
					bb_serverDisconnectClient(game->players[m_checksumQueries[nn]->GetID()]->babonetID);
					ZEVEN_SAFE_DELETE(game->players[m_checksumQueries[nn]->GetID()]);
					net_svcl_player_disconnect playerDisconnect;
					playerDisconnect.playerID = (char)m_checksumQueries[nn]->GetID();
					bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
				}
				delete m_checksumQueries[nn];
				m_checksumQueries.erase( m_checksumQueries.begin() + nn );
				nn--;
			}
		}

		DelayedKicksMap::iterator it = delayedKicks.begin(), itTmp;
		for (; it != delayedKicks.end(); )
		{
			it->second.timeToKick -= delay;
			if (it->second.timeToKick < 0.0)
			{
				// kick
				if( game->players[it->second.playerID] &&
					game->players[it->second.playerID]->babonetID == it->second.babonetID)
				{
					if( master ) master->RA_DisconnectedPlayer( textColorLess(game->players[it->second.playerID]->name).s, game->players[it->second.playerID]->playerIP, (long)game->players[it->second.playerID]->playerID );
					bb_serverDisconnectClient(it->second.babonetID);
					ZEVEN_SAFE_DELETE(game->players[it->second.playerID]);
					net_svcl_player_disconnect playerDisconnect;
					playerDisconnect.playerID = (char)it->second.playerID;
					bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
				}
				itTmp = it;
				it++;
				delayedKicks.erase(itTmp);
			}
			else
				it++;
		}
#endif
		
		// On update le server
		updateNet(delay, false);

		// On check si on change pas de map
		if (changeMapDelay > 0)
		{
			changeMapDelay -= delay;
			if (changeMapDelay <= 0)
			{
				changeMapDelay = 0;
				game->resetGameType(gameVar.sv_gameType);
				// On load la new map
				CString lastMap = game->mapName;
				game->mapName = nextMap;
				game->createMap();
				while((!game->map || !IsMapValid(*game->map, game->gameType)) && mapList.size() > 0)
				{
					console->add("\x4> Map is missing some entities, removing it from the list");
					needToShutDown = false;
					isRunning = true;
					CString mapNameTemp("%s", game->mapName.s);
					game->mapName = "";
					removemap(mapNameTemp);
					game->mapName = mapNameTemp;
					game->mapName = queryNextMap();
					game->createMap();
				}
				if (!game->map) 
				{
					needToShutDown = true;
					isRunning = false;
					return;
				}
				else
				{
					// On le dit au autres
					net_svcl_map_change mapChange;
					memcpy(mapChange.mapName, game->map->mapName.s, strlen(game->map->mapName.s)+1);
					mapChange.gameType = gameVar.sv_gameType;
					bb_serverSend((char*)&mapChange, sizeof(net_svcl_map_change), NET_SVCL_MAP_CHANGE, 0);

					// On change le round �playing
					net_svcl_round_state roundState;
					game->roundState = GAME_PLAYING;
					roundState.newState = game->roundState;
					roundState.reInit = true; // On remets tout �0, les scores etc
					bb_serverSend((char*)&roundState, sizeof(net_svcl_round_state), NET_SVCL_GAME_STATE, 0);
				}
			}
		}

		// On change le type de game??
		if (gameVar.sv_gameType != game->gameType)
		{
			game->resetGameType(gameVar.sv_gameType);
		}

		// On update le timing de la game
		if (game->gameTimeLeft > 0) game->gameTimeLeft -= delay;
		if (game->roundTimeLeft > 0) game->roundTimeLeft -= delay;
		if (game->gameTimeLeft < 0)
		{
			game->gameTimeLeft = 0;

			// On a fini la partie! Next map
		}
		if (game->roundTimeLeft < 0)
		{
			game->roundTimeLeft = 0;

			// On a fini le round! Next round
		}

		// On check si on a pas fini un round ou une game
		if (game->roundState == GAME_PLAYING)
		{
			bool changeRoundState = false;

			// � c le plus simple
			if (game->gameType == GAME_TYPE_DM)
			{
				if (game->gameTimeLeft == 0 && gameVar.sv_gameTimeLimit > 0)
				{
					game->roundState = GAME_DONT_SHOW;
					changeRoundState = true;
				} // Sinon c'est temps illimit�

				// On check si un joueur atteint pas le score max
				for (int i=0;i<MAX_PLAYER;++i)
				{
					if (game->players[i])
					{
						if (game->players[i]->score >= gameVar.sv_scoreLimit && gameVar.sv_scoreLimit > 0)
						{
							game->roundState = GAME_DONT_SHOW;
							changeRoundState = true;
							break;
						}
					}
				}
			}

			// TDM
			if (game->gameType == GAME_TYPE_TDM)
			{
				// On check si on attein pas les max de score
				if (game->blueScore == game->redScore && game->redScore >= gameVar.sv_scoreLimit && gameVar.sv_scoreLimit > 0)
				{
					game->roundState = GAME_DRAW;
					changeRoundState = true;
				}
				else if (game->blueScore >= gameVar.sv_scoreLimit && gameVar.sv_scoreLimit > 0)
				{
					game->roundState = GAME_BLUE_WIN;
					changeRoundState = true;
				}
				else if (game->redScore >= gameVar.sv_scoreLimit && gameVar.sv_scoreLimit > 0)
				{
					game->roundState = GAME_RED_WIN;
					changeRoundState = true;
				}

				if (game->gameTimeLeft == 0 && gameVar.sv_gameTimeLimit > 0)
				{
					game->roundState = GAME_DONT_SHOW;
					changeRoundState = true;
				}
			}

			// Le mode CTF, assez simple aussi
			if (game->gameType == GAME_TYPE_CTF)
			{
				// On check si on attein pas les max de score
				if (game->blueWin == game->redWin && game->redWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_DRAW;
					changeRoundState = true;
				}
				else if (game->blueWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_BLUE_WIN;
					changeRoundState = true;
				}
				else if (game->redWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_RED_WIN;
					changeRoundState = true;
				}

				// On check si le temps est �oul�
				if (game->gameTimeLeft == 0 && gameVar.sv_gameTimeLimit > 0)
				{
					if (game->blueWin == game->redWin)
					{
						game->roundState = GAME_DRAW;
						changeRoundState = true;
					}
					else if (game->blueWin > game->redWin)
					{
						game->roundState = GAME_BLUE_WIN;
						changeRoundState = true;
					}
					else
					{
						game->roundState = GAME_RED_WIN;
						changeRoundState = true;
					}
				}
			}

			// Search and Destroy, le plus toff de tous (vu que les round sont entre coup�de petit round :S)
			if (game->gameType == GAME_TYPE_SND)
			{
				// On check si on attein pas les max de score
				if (game->blueWin == game->redWin && game->redWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_DRAW;
					changeRoundState = true;
				}
				else if (game->blueWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_BLUE_WIN;
					changeRoundState = true;
				}
				else if (game->redWin >= gameVar.sv_winLimit && gameVar.sv_winLimit > 0)
				{
					game->roundState = GAME_RED_WIN;
					changeRoundState = true;
				}

				// On check si le temps est �oul�(de la partie complete)
				if (game->gameTimeLeft == 0 && gameVar.sv_gameTimeLimit > 0)
				{
					if (game->blueWin == game->redWin)
					{
						game->roundState = GAME_DRAW;
						changeRoundState = true;
					}
					else if (game->blueWin > game->redWin)
					{
						game->roundState = GAME_BLUE_WIN;
						changeRoundState = true;
					}
					else
					{
						game->roundState = GAME_RED_WIN;
						changeRoundState = true;
					}
				}

				// On check si la bomb n'explose pas!

			}

			if (changeRoundState)
			{
				if (gameVar.sv_report && (game->roundState == GAME_RED_WIN ||
					game->roundState == GAME_BLUE_WIN || game->roundState == GAME_DRAW))
				{
					console->add("\x2Updating Stats Cache", true);
					updateStatsCache();

					ReportGen repGen;
					CUrlData data;
					data.add("action", "report");

					console->add("\x2Generating Report", true);
					data.add("report", (char*)repGen.genReport().c_str(), CUrlData::BASE64);

					for (size_t i = 0; i < reportUploadURLs.size(); i++)
					{
						console->add(CString("\x2Sending Report to: %s", reportUploadURLs[i].c_str()), true);
						CCurl* request = new CCurl((char*)reportUploadURLs[i].c_str(), data.get());
						reportUploads.push_back(request);
						request->start();
					}
				}
				clearStatsCache();

				changeMapDelay = 10; // Pour dire qu'on change de map apres le round
				nextMap = queryNextMap();
				net_svcl_round_state roundState;
				roundState.newState = game->roundState;
				roundState.reInit = false;
				bb_serverSend((char*)&roundState, sizeof(net_svcl_round_state), NET_SVCL_GAME_STATE, 0);
			}
		}



		// Est-ce que la babonet veut nous dire de quoi?
	/*	CString message = bb_serverGetLastMessage();
		if (!message.isNull())
		{
			console->add(CString("\x9> babonet : %s", message.s), true);
		}*/

		// On recv les messages
		char * buffer;
		int messageID;
		unsigned long babonetID;
		while (buffer = bb_serverReceive(babonetID, messageID))
		{
			// On g�e les messages re�the 
			recvPacket(buffer, messageID, babonetID);
		}

		int nbPlayers = 0;
		// On update et send les ping
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				nbPlayers++;

				if (!game->players[i]->waitForPong)
				{
					// On est pret �lui envoyer un ping?
					if (game->players[i]->currentPingFrame >= 30)
					{
						game->players[i]->currentPingFrame = 0;
						game->players[i]->waitForPong = true;
						game->players[i]->connectionInterrupted = false;

						// On lui send son pingdlidou
						net_svcl_ping ping;
						ping.playerID = char(i); // Ici on s'en occupe pas du ID
						bb_serverSend((char*)&ping,sizeof(net_svcl_ping),NET_SVCL_PING,game->players[i]->babonetID);
						continue;
					}
				}
				else
				{
					if (game->players[i]->currentPingFrame > game->players[i]->ping)
					{
						// On a d�ass�l'encient ping, on le met �jour (pour les updates l�
						game->players[i]->ping = game->players[i]->currentPingFrame;
					}
					if (game->players[i]->currentPingFrame > 30)
					{
						long saveFrame = game->players[i]->currentCF.frameID;
						game->players[i]->currentCF = game->players[i]->netCF1; // Pour les autres joueurs
						game->players[i]->currentCF.frameID = saveFrame;
						game->players[i]->connectionInterrupted = true;
						game->players[i]->sendPosFrame = 0; // On interrupt sont envoit de data non important (coordFrame), pour al�er le tout
					}

					// Est-ce que � fait trop longtemps qu'on attends l�??
					if (game->players[i]->currentPingFrame > 300) // ouf � fait 3sec l� on est assez tolerant:P
					{
						// Save stats to cache
						if (game->players[i]->timePlayedCurGame > EPSILON)
							cacheStats(game->players[i]);
						// Connection interrupted (on le disconnect, on est s��e)
						if( master ) master->RA_DisconnectedPlayer( textColorLess(game->players[i]->name).s, game->players[i]->playerIP, (long)game->players[i]->playerID);
						bb_serverDisconnectClient(game->players[i]->babonetID);
						console->add("\x3> Disconnecting client, no respond since 3sec", true);
						ZEVEN_SAFE_DELETE(game->players[i]);
						net_svcl_player_disconnect playerDisconnect;
						playerDisconnect.playerID = (char)i;
						bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
						continue;
//						connectionInterrupted = true;
//						game->players[i]->currentCF = game->players[i]->netCF1; // Pour les autres joueurs
					}
				}
				// On incr�ente son ping
				game->players[i]->currentPingFrame++;
			}
		}

		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->babySitTime <= EPSILON && game->players[i]->ping * 33 > gameVar.sv_maxPing && gameVar.sv_maxPing != 0)
				{
					game->players[i]->pingOverMax += delay;
					if (game->players[i]->pingOverMax > maxTimeOverMaxPing)
					{
						//console->add(CString("\x3> Client exceeded max ping for over %.0f seconds", maxTimeOverMaxPing), true);
						console->sendCommand(CString("sayid %d Maximum ping exceeded, moving to spectator", game->players[i]->playerID));
						console->sendCommand(CString("moveid %d %d", PLAYER_TEAM_SPECTATOR,
							game->players[i]->playerID));

						game->players[i]->pingOverMax = 0;
					}
				}
				else
					game->players[i]->pingOverMax = 0.0f;

				if (gameVar.sv_autoSpectateWhenIdle && game->players[i] &&
					(game->players[i]->timeIdle > gameVar.sv_autoSpectateIdleMaxTime) &&
					(game->players[i]->teamID == PLAYER_TEAM_RED ||
					game->players[i]->teamID == PLAYER_TEAM_BLUE))
				{
					console->add(CString("\x3> Too much idling, not enough playing"), true);
					//console->sendCommand(CString("kickid %d", game->players[i]->playerID));
					console->sendCommand(CString("moveid %d %d", PLAYER_TEAM_SPECTATOR,
						game->players[i]->playerID));
				}
				if(gameVar.sv_sendJoinMessage && game->players[i]->timeInServer > 1.0f && game->players[i]->timeInServer < 1.0f+delay)
				{
					CString sendPlayerJoinMessage("sayid %d ",game->players[i]->playerID);
					sendPlayerJoinMessage+=gameVar.sv_joinMessage;
					sendPlayerJoinMessage+="\0";
					console->sendCommand(sendPlayerJoinMessage);
				}
			}
		}

		// On update le jeu (� c'est autant client que server side)
		game->update(delay);

		// On check pour sender les coordframes des players au autres players s'il en ont le temps
		if (game->roundState == GAME_PLAYING)
		{
			net_clsv_svcl_player_coord_frame playerCoordFrame;
			net_svcl_minibot_coord_frame minibotCoordFrame;
			for (int i=0;i<MAX_PLAYER;i++)
			{
				if (game->players[i])
				{
					game->players[i]->sendPosFrame++;
					if (game->players[i]->sendPosFrame >= game->players[i]->avgPing && game->players[i]->sendPosFrame >= gameVar.sv_minSendInterval + nbPlayers/8)
					{
						game->players[i]->sendPosFrame = 0;

						// Lui il est ready �se faire envoyer les coordFrames
						for (int j=0;j<MAX_PLAYER;++j)
						{
							if (game->players[j])
							{
								if (j != i && game->players[j]->status == PLAYER_STATUS_ALIVE)
								{
									playerCoordFrame.playerID = (char)j;
									playerCoordFrame.babonetID = game->players[j]->babonetID;
									playerCoordFrame.frameID = game->players[j]->currentCF.frameID;
									playerCoordFrame.mousePos[0] = (short)(game->players[j]->currentCF.mousePosOnMap[0] * 100);
									playerCoordFrame.mousePos[1] = (short)(game->players[j]->currentCF.mousePosOnMap[1] * 100);
									playerCoordFrame.mousePos[2] = (short)(game->players[j]->currentCF.mousePosOnMap[2] * 100);
									playerCoordFrame.position[0] = (short)(game->players[j]->currentCF.position[0] * 100);
									playerCoordFrame.position[1] = (short)(game->players[j]->currentCF.position[1] * 100);
									playerCoordFrame.position[2] = (short)(game->players[j]->currentCF.position[2] * 100);
									playerCoordFrame.vel[0] = (char)(game->players[j]->currentCF.vel[0] * 10);
									playerCoordFrame.vel[1] = (char)(game->players[j]->currentCF.vel[1] * 10);
									playerCoordFrame.vel[2] = (char)(game->players[j]->currentCF.vel[2] * 10);
#ifdef _PRO_
									playerCoordFrame.camPosZ = 0;
#endif
									bb_serverSend((char*)&playerCoordFrame, sizeof(net_clsv_svcl_player_coord_frame), NET_CLSV_SVCL_PLAYER_COORD_FRAME, game->players[i]->babonetID, NET_UDP);
								}

#ifdef _PRO_
								if (game->players[j]->status == PLAYER_STATUS_ALIVE)
								{
									//--- Mini bot?
									if (game->players[j]->minibot)
									{
										minibotCoordFrame.playerID = (char)j;
										minibotCoordFrame.babonetID = game->players[j]->babonetID;
										minibotCoordFrame.frameID = game->players[j]->currentCF.frameID;
										minibotCoordFrame.mousePos[0] = (short)(game->players[j]->minibot->currentCF.mousePosOnMap[0] * 100);
										minibotCoordFrame.mousePos[1] = (short)(game->players[j]->minibot->currentCF.mousePosOnMap[1] * 100);
										minibotCoordFrame.mousePos[2] = (short)(game->players[j]->minibot->currentCF.mousePosOnMap[2] * 100);
										minibotCoordFrame.position[0] = (short)(game->players[j]->minibot->currentCF.position[0] * 100);
										minibotCoordFrame.position[1] = (short)(game->players[j]->minibot->currentCF.position[1] * 100);
										minibotCoordFrame.position[2] = (short)(game->players[j]->minibot->currentCF.position[2] * 100);
										minibotCoordFrame.vel[0] = (char)(game->players[j]->minibot->currentCF.vel[0] * 10);
										minibotCoordFrame.vel[1] = (char)(game->players[j]->minibot->currentCF.vel[1] * 10);
										minibotCoordFrame.vel[2] = (char)(game->players[j]->minibot->currentCF.vel[2] * 10);
										
										bb_serverSend((char*)&minibotCoordFrame, sizeof(net_svcl_minibot_coord_frame), NET_SVCL_MINIBOT_COORD_FRAME, game->players[i]->babonetID, NET_UDP);
									}
								}
#endif

								// On shoot aussi le ping de ce joueur
								net_svcl_player_ping playerPing;
								playerPing.playerID = (char)j;
								playerPing.ping = (short)game->players[j]->ping;
								bb_serverSend((char*)&playerPing,sizeof(net_svcl_player_ping),NET_SVCL_PLAYER_PING, game->players[i]->babonetID, NET_UDP);
							}
						}

				/*		for (j=0;j<(int)game->projectiles.size();++j)
						{
							Projectile * projectile = game->projectiles[j];

							if (projectile->projectileID >= (int)game->projectiles.size() || projectile->projectileID < 0)
							{
								console->add(CString("\x9> Server warning, trying to send a projectile with an out of range index : %i", j));
								continue;
							}

							net_svcl_projectile_coord_frame projectileCoordFrame;
							projectileCoordFrame.frameID = projectile->currentCF.frameID;
							projectileCoordFrame.projectileID = projectile->projectileID;
							projectileCoordFrame.position[0] = (short)(projectile->currentCF.position[0] * 100);
							projectileCoordFrame.position[1] = (short)(projectile->currentCF.position[1] * 100);
							projectileCoordFrame.position[2] = (short)(projectile->currentCF.position[2] * 100);
							projectileCoordFrame.vel[0] = (char)(projectile->currentCF.vel[0] * 10);
							projectileCoordFrame.vel[1] = (char)(projectile->currentCF.vel[1] * 10);
							projectileCoordFrame.vel[2] = (char)(projectile->currentCF.vel[2] * 10);
							projectileCoordFrame.uniqueID = projectile->uniqueID;
							bb_serverSend((char*)&projectileCoordFrame, sizeof(net_svcl_projectile_coord_frame), NET_SVCL_PROJECTILE_COORD_FRAME, game->players[i]->babonetID, NET_UDP);
						}*/

						// On shoot les info sur les horloges du server
						net_svcl_synchronize_timer synchronizeTimer;
						synchronizeTimer.frameID = frameID;
						synchronizeTimer.gameTimeLeft = game->gameTimeLeft;
						synchronizeTimer.roundTimeLeft = game->roundTimeLeft;
						bb_serverSend((char*)&synchronizeTimer, sizeof(net_svcl_synchronize_timer), NET_SVCL_SYNCHRONIZE_TIMER, game->players[i]->babonetID, NET_UDP);
					}
				}
			}

			// On check pour les flag
			if (game)
			{
				//--- Run the auto balance au 2mins
				if ((game->gameType == GAME_TYPE_TDM || game->gameType == GAME_TYPE_CTF) &&
					gameVar.sv_autoBalance)
				{
					if (autoBalanceTimer == 0)
					{
						//--- Count how much player in each team
						std::vector<Player*> reds;
						std::vector<Player*> blues;
						for (int i=0;i<MAX_PLAYER;++i)
						{
							if (game->players[i])
							{
								if (game->players[i]->teamID == PLAYER_TEAM_RED)
								{
									reds.push_back(game->players[i]);
								}
								else if (game->players[i]->teamID == PLAYER_TEAM_BLUE)
								{
									blues.push_back(game->players[i]);
								}
							}
						}

						//--- Check if they are even
						if ((int)reds.size() < (int)blues.size() - 1 ||
							(int)blues.size() < (int)reds.size() - 1)
						{
							//--- Send autobalance notification
							autoBalanceTimer = (float)gameVar.sv_autoBalanceTime;
							bb_serverSend(0,0,NET_SVCL_AUTOBALANCE,0);
						}
					}
					else
					{
						if (autoBalanceTimer > 0)
						{
							autoBalanceTimer -= delay;
							if (autoBalanceTimer < 0)
							{
								autoBalanceTimer = 0;
								autoBalance();
							}
						}
					}
				}
				else
				{
					autoBalanceTimer = 0;
				}

				//--- Run game type specific update
				if (game->map && game->gameType == GAME_TYPE_DM)
				{
					//...
				}
				else if (game->map && game->gameType == GAME_TYPE_TDM)
				{
					//...
				}
				else if (game->map && game->gameType == GAME_TYPE_CTF)
				{
					updateCTF(delay);
				}
				else if (game->map && game->gameType == GAME_TYPE_SND)
				{
#ifdef _PRO_
               // Every minute, new spawn-slots and respawn everyone
               if (game->roundTimeLeft == 0)
               {

                  game->roundTimeLeft = gameVar.sv_roundTimeLimit;

                  // Reset all spawn slots
                  for (int j=0;j<MAX_PLAYER;++j)
						{
                     if (game->players[j])
                        {
                        game->players[j]->spawnSlot = -1;
                        }
                  }

            // Reset all spawn slots
                  for (int j=0;j<MAX_PLAYER;++j)
						{
                     if ( (game->players[j]) && (game->players[j]->teamID == PLAYER_TEAM_BLUE || game->players[j]->teamID == PLAYER_TEAM_RED))
                        {
                        net_svcl_player_hit playerHit;
				            playerHit.damage = -100;
                        playerHit.playerID = game->players[j]->playerID;
				            playerHit.fromID = (char)game->players[j]->playerID;
				            playerHit.weaponID = 0;
				            playerHit.vel[0] = 0;
				            playerHit.vel[1] = 0;
				            playerHit.vel[2] = 1;
				            bb_serverSend((char*)&playerHit,sizeof(net_svcl_player_hit),NET_SVCL_PLAYER_HIT,0);
                        }                        
                  }
               }               
#else
               updateSnD(delay);
#endif
				}
			}
		}

		// On update le server
		updateNet(delay, true);
	}

	// Transfer maps
	int bytesSent = 0;
	int bytesPerFrame = int(gameVar.sv_maxUploadRate * 1024 / 30);
	std::vector<SMapTransfer> temp;

	for(std::size_t i = 0; i < mapTransfers.size(); ++i)
	{
		// Limit upload rate
		if(bytesSent > bytesPerFrame)
		{
			// If a player can't get a chunk this time, make sure
			// the transfer is kept
			temp.push_back(mapTransfers[i]);
			continue;
		}

		// Open map
		CString filename("main/maps/%s.bvm", mapTransfers[i].mapName.s);
		FILE* fic = fopen(filename.s, "rb");

		if (fic)
		{
			net_svcl_map_chunk chunk;

			// Read in a chunk
			fseek(fic, 250 * mapTransfers[i].chunkNum, SEEK_SET);
			chunk.size = (unsigned short)fread(chunk.data, 1, 250, fic);

			// Send chunk
			bb_serverSend((char*)&chunk, sizeof(net_svcl_map_chunk), NET_SVCL_MAP_CHUNK, mapTransfers[i].uniqueClientID);

			// Accumulate bytes sent
			bytesSent += 250;

			// If some data was sent, keep this transfer
			if(chunk.size != 0) 
			{
				mapTransfers[i].chunkNum++;
				temp.push_back(mapTransfers[i]);
			}
		}

		fclose(fic);
	}

	// Replace old transfer list
	mapTransfers.swap(temp);


	frameID++;
}

bool Server::filterMapFromRotation(const mapInfo & map)
{
	int nbPlayer = 0;
	for (int i = 0; i <MAX_PLAYER; ++i)
	{
		if ((game->players[i]) && 
			((game->players[i]->teamID == PLAYER_TEAM_BLUE)||(game->players[i]->teamID == PLAYER_TEAM_RED)))
		{
			nbPlayer++;
		}
	}
	if(nbPlayer < 2)
		nbPlayer = 2;
	float tilesPerBabo = map.mapArea/(float)nbPlayer;
	if (tilesPerBabo < gameVar.sv_minTilesPerBabo || (tilesPerBabo > gameVar.sv_maxTilesPerBabo && gameVar.sv_maxTilesPerBabo != 0))
		return false;
	return true;
}

//
// Auto balance
//
void Server::autoBalance()
{
	//--- Voil� on autobalance les teams
	std::vector<Player*> reds;
	std::vector<Player*> blues;
	for (int i=0;i<MAX_PLAYER;++i)
	{
		if (game->players[i])
		{
			if (game->players[i]->teamID == PLAYER_TEAM_RED)
			{
				reds.push_back(game->players[i]);
			}
			else if (game->players[i]->teamID == PLAYER_TEAM_BLUE)
			{
				blues.push_back(game->players[i]);
			}
		}
	}

	//--- Check if they are uneven
	if ((int)reds.size() < (int)blues.size() - 1)
	{
		//--- On balance!
		int nbToSwitch = (int)blues.size() - 1 - (int)reds.size();

		//--- Bon... comment on choisi les candidats, c'est simple, 
		//    on switch les 2 plus poche en bas de la liste ^^
		while (nbToSwitch > 0)
		{
			int switchID = (int)blues.size() - 1;
			if(game->map->flagState[1] == blues[switchID]->playerID)
				switchID--;
			for(int i = switchID - 1; i >= 0; i--)
			{
				if(blues[switchID]->timePlayedCurGame > blues[i]->timePlayedCurGame && game->map->flagState[1] != blues[i]->playerID)
					switchID = i;
			}
			blues[switchID]->teamID = game->assignPlayerTeam(blues[switchID]->playerID, PLAYER_TEAM_RED);
			//--- Switch him team
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = blues[switchID]->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_RED;
			// On l'envoit �tout le monde, (si � chang�
			bb_serverSend((char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST, 0);
			nbToSwitch--;
		}
	}
	else if ((int)blues.size() < (int)reds.size() - 1)
	{
		//--- On balance
		int nbToSwitch = (int)reds.size() - 1 - (int)blues.size();

		//--- Bon... comment on choisi les candidats, c'est simple, 
		//    on switch les 2 plus poche en bas de la liste ^^
		while (nbToSwitch > 0)
		{
			int switchID = (int)reds.size() - 1;
			if(game->map->flagState[0] == reds[switchID]->playerID)
				switchID--;
			for(int i = switchID - 1; i >= 0; i--)
			{
				if(reds[switchID]->timePlayedCurGame > reds[i]->timePlayedCurGame && game->map->flagState[0] != reds[i]->playerID)
					switchID = i;
			}
			reds[switchID]->teamID = game->assignPlayerTeam(reds[switchID]->playerID, PLAYER_TEAM_BLUE);
			//--- Switch him team
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = reds[switchID]->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_BLUE;
			// On l'envoit �tout le monde, (si � chang�
			bb_serverSend((char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST, 0);
			nbToSwitch--;
		}
	}
	else
	{
		//--- Ha ok, les teams �aient balanc�
	}
}



//
// Pour modifier une variable remotly
//
void Server::sendSVChange(CString varCom)
{
	if(varCom == "sv_nukeReload")
	{
		gameVar.weapons[WEAPON_NUCLEAR]->fireDelay = gameVar.sv_nukeReload;
	}
	if (varCom.len() > 255) varCom.resize(255);
	net_svcl_sv_change svChange;
	memcpy(svChange.svChange, varCom.s, varCom.len()+1);
	bb_serverSend((char*)&svChange,sizeof(net_svcl_sv_change),NET_SVCL_SV_CHANGE,0);
}

void Server::nukeAll()
{
	for (int i = 0; i < MAX_PLAYER; i ++)
	{
		if(game->players[i] && (game->players[i]->teamID == PLAYER_TEAM_RED || game->players[i]->teamID == PLAYER_TEAM_BLUE))
		{
			net_svcl_explosion explosion;
			explosion.position[0] = game->players[i]->currentCF.position[0];
			explosion.position[1] = game->players[i]->currentCF.position[1];
			explosion.position[2] = game->players[i]->currentCF.position[2];
			explosion.normal[0] = 0;
			explosion.normal[1] = 0;
			explosion.normal[2] = 1;
			explosion.radius = gameVar.sv_nukeRadius;
			bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
			game->radiusHit(game->players[i]->currentCF.position, gameVar.sv_nukeRadius, game->players[i]->playerID, WEAPON_NUCLEAR);
		}
	}
}

void Server::nukePlayer(int i)
{
	if(game->players[i] && (game->players[i]->teamID == PLAYER_TEAM_RED || game->players[i]->teamID == PLAYER_TEAM_BLUE))
	{
		net_svcl_explosion explosion;
		explosion.position[0] = game->players[i]->currentCF.position[0];
		explosion.position[1] = game->players[i]->currentCF.position[1];
		explosion.position[2] = game->players[i]->currentCF.position[2];
		explosion.normal[0] = 0;
		explosion.normal[1] = 0;
		explosion.normal[2] = 1;
		explosion.radius = gameVar.sv_nukeRadius;
		bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
		game->radiusHit(game->players[i]->currentCF.position, gameVar.sv_nukeRadius, game->players[i]->playerID, WEAPON_NUCLEAR);
	}
}

//
// Pour chatter
//
void Server::sayall(CString message)
{
	if (message.isNull()) return;
	if (game && isRunning)
	{
		// On send � sur la network oui messieur
		net_clsv_svcl_chat chat_message;

	//	chat_message.fromID = game->thisPlayer->playerID;
		chat_message.teamID = PLAYER_TEAM_SPECTATOR - 1; // All player!

		// On insert la couleur d�endament du team 
		// (une fois apres le nom du joueur, parce que ce dernier a surement 
		// mis plein de caract�es de couleurs)
		message.insert(" : \x8", 0);

		// On insert le nom du joueur
		message.insert("console", 0);


		// Si le message est trop grand, on le resize
		if (message.len() > 49+80) message.resize(49+80);

		// Voil� on copie le finale
		memcpy(chat_message.message, message.s, sizeof(char) * (message.len() + 1));

		// Voil� on send � sur le network!
		bb_serverSend((char*)&chat_message, sizeof(net_clsv_svcl_chat), NET_CLSV_SVCL_CHAT, 0);
	}
}

#ifdef _PRO_

std::vector<invalidChecksumEntity> Server::getInvalidChecksums(unsigned long bbnetID, int number, int offsetFromEnd)
{
	sqlite3 *DB=0;
	sqlite3_open("./bv2.db",&DB);

	std::vector<invalidChecksumEntity> list;
	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int	nRow;			// number of record
	int	nColumn;		// number of column
	char	SQL[256];		// the query
	int maxRows = 50;

	if (number > maxRows)
		number = maxRows;
	//sprintf(SQL, CString("Select IP, Name From BadChecksum limit %i", maxRows).s);
	sprintf(SQL, CString("Select IP, Name From BadChecksum limit %i offset (select count(*) from BadChecksum) - %i",
		number, offsetFromEnd).s);
	sqlite3_get_table(DB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	{
		for (int i = 0; i < nRow; i++)
		{
			invalidChecksumEntity tmp;
			char* ip = azResult[(i + 1) * nColumn];
			char* name = azResult[(i + 1) * nColumn + 1];
			int minLen = static_cast<int>( (strlen(name) < 31) ? strlen(name) : 31 );
			tmp.id = i + 1;
			strncpy(tmp.name, name, minLen);
			strncpy(tmp.playerIP, ip, 16);
			list.push_back(tmp);
		}
		sqlite3_free_table(azResult);
	}
	sqlite3_close( DB );
	return list;
}

/*void Server::sendInvalidChecksums(unsigned long bbnetID, int number, int offsetFromEnd)
{
	sqlite3 *DB=0;
	sqlite3_open("./bv2.db",&DB);

	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int	nRow;			// number of record
	int	nColumn;		// number of column
	char	SQL[256];		// the query
	int maxRows = 50;

	if (number > maxRows)
		number = maxRows;
	//sprintf(SQL, CString("Select IP, Name From BadChecksum limit %i", maxRows).s);
	sprintf(SQL, CString("Select IP, Name From BadChecksum limit %i offset (select count(*) from BadChecksum) - %i",
		number, offsetFromEnd).s);
	sqlite3_get_table(DB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	{
		for (int i = 0; i < nRow; i++)
		{
			net_svcl_bad_checksum_entity bce;
			memset(&bce, 0, sizeof(net_svcl_bad_checksum_entity));
			char* ip = azResult[(i + 1) * nColumn];
			char* name = azResult[(i + 1) * nColumn + 1];
			int minLen = (strlen(name) < 31) ? strlen(name) : 31;
			strncpy(bce.name, name, minLen);
			strncpy(bce.playerIP, ip, 16);
			bce.id = i + 1;
			bb_serverSend((char*)&bce, sizeof(net_svcl_bad_checksum_entity), NET_SVCL_BAD_CHECKSUM_ENTITY, bbnetID);
		}
		sqlite3_free_table(azResult);
	}
	sqlite3_close( DB );
}*/

void Server::deleteInvalidChecksums()
{
	sqlite3 *DB=0;
	sqlite3_open("./bv2.db",&DB);
	
	sqlite3_exec(DB,"delete from BadChecksum",0,0,0);
	
	sqlite3_close(DB);
}

int Server::getNumberOfInvalidChecksums()
{
	sqlite3 *DB=0;
	sqlite3_open("./bv2.db",&DB);

	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int	nRow;			// number of record
	int	nColumn;		// number of column
	char	SQL[256];		// the query
	int num = 0;

	sprintf(SQL, "select count(*) as Number from BadChecksum");
	sqlite3_get_table(DB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);
	if (nRow == 1)
		num = atoi(azResult[1]);
	sqlite3_free_table(azResult);
	sqlite3_close( DB );
	return num;
}

#endif

void Server::cacheStats(const Player* player)
{
	if (player->userID == 0) // caching is only for logged users
		return;

	cacheStats(player, player->teamID);
}

void Server::cacheStats(const Player* player, int teamid)
{
	if (player->userID == 0) // caching is only for logged users
		return;

	//removeStatsFromCache(player->userID);
	PlayerStats* ps = new PlayerStats(player);
	ps->teamID = teamid;
	statsCache.insert(StatsCachePair(player->userID, ps));
}

PlayerStats* Server::getStatsFromCache(int userID)
{
	StatsCache::iterator it;
	if ((it = statsCache.find(userID)) != statsCache.end())
		return it->second;
	return 0;
}

void Server::removeStatsFromCache(int userID)
{
	StatsCache::iterator it;
	if ((it = statsCache.find(userID)) != statsCache.end())
	{
		delete it->second;
		statsCache.erase(it);
	}
}

void Server::clearStatsCache()
{
	StatsCache::iterator it;
	for (it = statsCache.begin(); it != statsCache.end(); it++)
		delete it->second;
	statsCache.clear();
}

void Server::updateStatsCache()
{
	//original cache may store few entries for player if he disconnected few times

	//cache that will store one cache element per team for each player
	StatsCache newCache;
	StatsCache::iterator it, itFound;

	std::pair<StatsCache::iterator, StatsCache::iterator> ret;

	for (it = statsCache.begin(); it != statsCache.end(); )
	{
		bool found = false;
		//check if player is in new cache
		//if yes, find cache element with same teamid and merge stats
		ret = newCache.equal_range(it->first);
		for (itFound = ret.first; itFound != ret.second; itFound++)
		{
			if (itFound->second->teamID == it->second->teamID)
			{
				itFound->second->MergeStats(it->second);
				found = true;
				break;
			}
		}

		//if not found, insert new
		if (found == false)
			newCache.insert(StatsCachePair(it->first, it->second));
		else
			delete it->second;	
		statsCache.erase(it++);
	}

	statsCache = newCache;
	newCache.clear();

	//do the same but with active players
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (game->players[i] == 0 || game->players[i]->timePlayedCurGame < EPSILON)/* ||
			(game->players[i]->teamID != PLAYER_TEAM_BLUE &&
			game->players[i]->teamID != PLAYER_TEAM_RED))*/
			continue;
		bool found = false;
		PlayerStats* ps = new PlayerStats(game->players[i]);
		if (ps->userID == 0)
		{
			delete ps;
			continue;
		}
		//check if player is in new cache
		//if yes, find cache element with same teamid and merge stats
		ret = statsCache.equal_range(game->players[i]->userID);
		for (itFound = ret.first; itFound != ret.second; itFound++)
		{
			if (itFound->second->teamID == game->players[i]->teamID)
			{
				itFound->second->MergeStats(ps);
				found = true;
				break;
			}
		}
		//if not found, insert new
		if (found == false)
			statsCache.insert(StatsCachePair(game->players[i]->userID, ps));
		else
			delete ps;
	}
}

void Server::addDelayedKick(unsigned long _babonetID, int _playerID, float _timeToKick)
{
	// add to list
	if (delayedKicks.find(_babonetID) == delayedKicks.end())
	{
		delayedKickStruct dks(_babonetID, _playerID, _timeToKick);
		delayedKicks.insert(DelayedKicksPair(_babonetID, dks));
	}
}

void Server::SendPlayerList( long in_peerId )
{
	if(!game) return;

	for( int i=0; i<MAX_PLAYER; i++ )
	{
		if( game->players[i] )
		{
			net_ra_player_entry entry;
			CString name = textColorLess(game->players[i]->name);
			sprintf( entry.name, "%s", name.s );
			sprintf( entry.ip, "%s", game->players[i]->playerIP );
			entry.id = (long)game->players[i]->playerID;

			bb_peerSend( in_peerId, (char*)&entry, RA_PLAYER_ENTRY, sizeof(net_ra_player_entry), true );
		}
	}

}

#ifndef CONSOLE
//
// Pour le dessiner !?
//
void Server::render()
{
}
#endif


