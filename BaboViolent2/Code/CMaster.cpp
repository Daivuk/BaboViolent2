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

#include "CMaster.h"
#include "Console.h"
#include "RemoteAdminPackets.h"
#include "GameVar.h"
#include "Scene.h"
#ifndef CONSOLE
#include "CLobby.h"
#endif
#ifdef LINUX64
#include <openssl/md5.h>
#endif

CMaster* master = 0;
bool surveyReceived = false;
extern Scene* scene;


CMaster::CMaster()
{
	s1 = -1; s2 = -1; s3 = -1; s4 = -1;
	uniqueClientID = 0;
	m_isConnected = false;
	m_nbGameFound = 0;
	m_ping = 0;
	RunningServer = 0;

	GetMasterInfos();

	for( int i=0; i<64; i++)
	{
		BannedAnswers[i].ID = -1;
		BannedAnswers[i].Answer = -1;
	}

	clearRemoteCacheList();
}


CMaster::~CMaster()
{
	ZEVEN_SAFE_DELETE(m_ping);
	int i;
	ZEVEN_DELETE_VECTOR(messageStack, i);
	disconnectMaster();
	eraseGames();
	m_peers.clear();
}

void CMaster::UpdateDB()
{
	sqlite3 *DB=0;
	sqlite3_open("bv2.db",&DB);
	char	SQL[300];
	
	sprintf(SQL,"Update LauncherSettings Set Value = '%i' Where Name = 'DBVersion';", gameVar.db_version);
	sqlite3_exec(DB,SQL,0,0,0);

	sprintf(SQL,"Update LauncherSettings Set Value = '%s' Where Name = 'AccountURL';", gameVar.db_accountServer.s);
	sqlite3_exec(DB,SQL,0,0,0);	

	sqlite3_close(DB);
}


//
//--- Connecting the master
//
void CMaster::connectToMaster(const char* in_IP, short in_port)
{
	uniqueClientID = bb_clientConnect(in_IP, in_port);
	m_isConnected = false;
}



//
//--- Kill it
//
void CMaster::disconnectMaster()
{
	if (uniqueClientID) bb_clientDisconnect(uniqueClientID);
	uniqueClientID = 0;
	m_isConnected = false;
	ZEVEN_SAFE_DELETE(m_ping);
}



//
//--- La row est valid� on a le ping pis toute
//
void CMaster::pingReceived(int ping)
{
	//--- We remove the ping object
	ZEVEN_SAFE_DELETE(m_ping);

	//--- Show that in console
	//if (gameVar.c_debug) console->add(CString("\x3> Ping received : %i", ping));

	if (m_games.size() > 0)
	{
		SBrowsableGame* browsableGame = new SBrowsableGame();
		//--- Add that BV2 to the GUIs
		browsableGame->bv2Row = new stBV2row();
		memcpy(browsableGame->bv2Row, m_games[0], sizeof(stBV2row));
		browsableGame->ping = ping;
		
#ifndef CONSOLE
		//--- Push it into the lobby
		if (lobby) lobby->pushGame(browsableGame);
#endif

		//--- Remove that row
		m_games.erase(m_games.begin());

		if( m_games.size() == 0 )
		{
			disconnectMaster();
		}
	}
}



//
//--- Updating the master client
//
void CMaster::update(float in_delay)
{
	//--- Update peer connection
	bool isNew;
	int result = bb_peerUpdate(in_delay, isNew);
	if (result == -1)
	{
		if (gameVar.c_debug) console->add(CString("\x4> Error in peerUpdate : %s",  bb_peerGetLastError()));
	}
	else if (isNew)
	{
		// a new peer!
		if( result )
		{
			m_peers.push_back( SPeer(result) );
		}
	}
	else
	{
		//--- Ping result
		if (m_ping)
		{
			int ping = m_ping->pingResult(result);
			if (ping != -1)
			{
				pingReceived(ping);
			}
		}

		//remove this peer
		for( unsigned int i=0; i<m_peers.size(); i++ )
		{
			if( m_peers[i].peerId == result )
			{
				m_peers.erase( m_peers.begin() + i );
				bb_peerDelete( result, true );
				break;
			}
		}
	}

	//receive packets from peers ( udp broadcast )
	ReceivePeersPacket();

	//--- Update le ping
	if (m_ping) 
	{
		int ping = m_ping->update(in_delay);

		if (ping != -1)
		{
			pingReceived(ping);
		}
	}

	// On update le client
	if (uniqueClientID > 0)
	{
		int result = bb_clientUpdate(uniqueClientID, in_delay, UPDATE_SEND_RECV);

		if (result == 1)
		{
			// Une erreur !!! On arr�e tout !!!
			if (gameVar.c_debug) console->add(CString("\x4> Error connection to master : %s", bb_clientGetLastError(uniqueClientID)));
			uniqueClientID = 0;
			m_isConnected = false;
			ZEVEN_SAFE_DELETE(m_ping);
		}
		else if (result == 2)
		{
			// Le server a foutu le CAMP !
			//if (gameVar.c_debug) console->add("\x9> Master server disconnected");
			uniqueClientID = 0;
			m_isConnected = false;
			ZEVEN_SAFE_DELETE(m_ping);
		}
		else if (result == 3)
		{
			//if (gameVar.c_debug) console->add("\x3> Master server connected");

			//--- We are connected.
			m_isConnected = true;
		}
	}


	// check if we have gam einfos to send to the master
	if( m_isConnected && uniqueClientID && messageStack.size() > 0 )
	{
		for (std::vector<SMasterMessage*>::iterator it = messageStack.begin(); 
			it != messageStack.end(); ++it)
		{
			bb_clientSend(uniqueClientID, (*it)->buffer, (*it)->size, (*it)->typeID);
			//if (gameVar.c_debug) console->add("\x3> Packet sent to master");
		}

		//--- Delete the stack
		int i;
		ZEVEN_DELETE_VECTOR(messageStack, i);
	}


	if (uniqueClientID > 0)
	{
		// On recv les messages
		char * buffer;
		int messageID;
		while (buffer = bb_clientReceive(uniqueClientID, &messageID))
		{
			// On g�e les messages re�
			recvPacket(buffer, messageID);
		}
	}

	//--- We check if we have a new server to ping
	if (!m_ping && m_games.size() > 0)
	{
		stBV2row * bv2Row = m_games[0];

		m_ping = new CPing();
		m_ping->ping(CString("%s", bv2Row->ip), bv2Row->port);
	}

	// peer timeouts
	for( unsigned int z=0; z<m_peers.size(); z++ )
	{
		m_peers[z].timeout -= in_delay;
		if( m_peers[z].timeout < 0.0f )
		{
			if( m_peers[z].remoteAdmin ) console->add( CString("Remote admin timeout ") + (int)m_peers[z].peerId );
			bb_peerDelete( m_peers[z].peerId, true );
			m_peers.erase( m_peers.begin() + z );
			z--;
		}
	}
}


// get the broadcast/remote admin data
void CMaster::ReceivePeersPacket()
{
	char *buf=0;
	INT4 peerID;
	int typeID;

	while( buf = bb_peerReceive( &peerID, &typeID ))
	{		
		switch( typeID )
		{
			case NET_BROADCAST_GAME_INFO:
			{
				net_svcl_broadcast_game_info lanGame;

				memcpy( &lanGame , buf , sizeof(net_svcl_broadcast_game_info));
				
				if( !stricmp( lanGame.key , "3333113333" ) )
				{

					SBrowsableGame* browsableGame = new SBrowsableGame();
					//--- Add that BV2 to the GUIs
					browsableGame->bv2Row = new stBV2row();

					
					memcpy(browsableGame->bv2Row, &(lanGame.GameInfo) , sizeof(stBV2row));
					browsableGame->ping = 0;
			
					#ifndef CONSOLE
						//--- Push it into the lobby
						if (lobby) lobby->pushGame(browsableGame);
					#endif
				}

				break;
			}

			// we receive a lan udp broacast query
			case NET_BROADCAST_QUERY:
			{
				if( !RunningServer ) break;


				net_clsv_broadcast_query query;
				memcpy( &query , buf , sizeof(net_clsv_broadcast_query));

				if( !stricmp( query.key , "3333113333" ) )
				{
					//we receive a query, lets send a broadcast with our infos
					net_svcl_broadcast_game_info gameInfos;

					sprintf( gameInfos.key , "3333113333" );
					
					
					
					strncpy(gameInfos.GameInfo.map, RunningServer->game->mapName.s, 16);

					char srvName[63];
					sprintf( srvName , "{ LAN } %s", gameVar.sv_gameName.s );
					strncpy(gameInfos.GameInfo.serverName, srvName, 63);
					strncpy(gameInfos.GameInfo.password, gameVar.sv_password.s, 15);
					sprintf( gameInfos.GameInfo.ip , bb_getMyIP() );
					gameInfos.GameInfo.port = (unsigned short)gameVar.sv_port;
					int nbPlayer = 0;
					for (int i=0;i<MAX_PLAYER;++i)
					{
						if (RunningServer->game->players[i])
						{
							nbPlayer++;
						}
					}
					gameInfos.GameInfo.nbPlayer = nbPlayer;
					gameInfos.GameInfo.maxPlayer = gameVar.sv_maxPlayer;
					gameInfos.GameInfo.flags = 0;
					gameInfos.GameInfo.gameType = RunningServer->game->gameType;
					gameInfos.GameInfo.ServerID = 0;
					gameInfos.GameInfo.Priority = 0;

					//send the packet
					bb_peerSend( NET_BROADCAST , (char*) &gameInfos , NET_BROADCAST_GAME_INFO , sizeof(net_svcl_broadcast_game_info) , false );

				}

				break;
			}

			//--- The following packets are for Remote Admin application exclusively... they are in CMaster because we ar edelaing with all p2p stuff at
			// the same place... or else it will become a mess...more than it already is
			case RA_LOGIN:
			{
				// extract data
				net_ra_login loginInfos;
				memcpy( &loginInfos , buf , sizeof(net_ra_login));

				// check if we can login
				net_ra_login_reply reply;
				reply.result = 0;
				if( RunningServer && gameVar.zsv_adminUser.len() > 0 )
				{
					// check with variables
					CString user = CString("%s",loginInfos.login);
					CString pass = CString("%s",loginInfos.pass);

					if( gameVar.zsv_adminUser == user && pass == gameVar.zsv_adminPass )
					{
						reply.result = 1;
						char ip[16];
						unsigned short port;
						bb_peerGetIPport( peerID, ip, &port);

						CString log = CString("Remote admin logged in from IP %s",ip);
						console->add( log );
						
						for( unsigned int z=0; z<m_peers.size(); z++ )
						{
							if( m_peers[z].peerId == peerID)
							{
								m_peers[z].remoteAdmin = true;
								m_peers[z].timeout = 20.0f;
								break;
							}
						}
					}
					else
					{
						char ip[16];
						unsigned short port;
						bb_peerGetIPport( peerID, ip, &port);

						CString log = CString("Remote admin login attempt from IP %s",ip);
						console->add( log );
					}
				}
				// reply back
				bb_peerSend( peerID, (char*)&reply, RA_LOGIN_REPLY, sizeof(net_ra_login_reply), true );
				return;
			}
			case RA_VAR_LIST:
			{
				// this peer is asking for the var list, make sure he is remote admin authorized
				if(!IsRemoteAdmin( peerID )) return;

				// send him the var list
				gameVar.sendSVVar(peerID);

				// tell him its over
				bb_peerSend( peerID, 0, RA_VAR_END, 0, true );
				
				return;
			}
			case RA_DISCONNECT:
			{
				if(!IsRemoteAdmin( peerID )) return;
				for( unsigned int i=0; i<m_peers.size(); i++ )
				{
					if( m_peers[i].peerId == peerID )
					{
						m_peers.erase( m_peers.begin() + i );
						break;
					}
				}

				bb_peerDelete( peerID, true );
				return;
			}
			case RA_COMMAND:
			{
				// we received a command from the remote admin that we should execute immediately
				if(!IsRemoteAdmin( peerID )) return;

				// extract command
				net_ra_command command;
				memcpy( &command , buf , sizeof(net_ra_command));
				command.command[127] = '\0'; // makes sure remote admin doesnt send to big command ><

				char ip[16];
				unsigned short port;
				bb_peerGetIPport( peerID, ip, &port);

				CString log = CString("Remote admin command sent from IP %s",ip);
				console->add( log );
				console->sendCommand( CString(command.command), true );

				return;
			}
			case RA_PLAYER_LIST:
			{
				// remote admin requesting player list
				if(!IsRemoteAdmin( peerID )) return;

				if( RunningServer )
				{
					RunningServer->SendPlayerList( peerID );
				}
				return;
			}
			case RA_CONSOLE_BROADCAST_ACTIVATE:
			{
				if(!IsRemoteAdmin( peerID )) return;
				net_ra_console_broadcast_activate data;
				memcpy( &data , buf , sizeof(net_ra_console_broadcast_activate));

				for( unsigned int i=0; i<m_peers.size(); i++ )
				{
					if( m_peers[i].peerId == peerID )
					{
						m_peers[i].broadcastConsole = data.enabled == 1 ? true : false;
						return;
					}
				}
				return;
			}
			case RA_KEEP_ALIVE:
			{
				if(!IsRemoteAdmin( peerID )) return;
				for( unsigned int i=0; i<m_peers.size(); i++ )
				{
					if( m_peers[i].peerId == peerID )
					{
						m_peers[i].timeout = 10.0f;
						return;
					}
				}

				return;
			}
			case RA_MAP_LIST_REQUEST:
			{
				if(!IsRemoteAdmin( peerID )) return;

				// ask server for all maps on the server
				if( RunningServer )
				{
					std::vector<CString> maps = RunningServer->populateMapList(true);

					for( unsigned int i=0; i<maps.size(); i++ )
					{
						net_ra_map_entry map;
						sprintf(map.mapName,"%s",maps[i].s);

						// send the map entry
						bb_peerSend( peerID, (char*)&map, RA_MAP_LIST_ENTRY, sizeof(net_ra_map_entry), true);	
					}
				}
				return;
			}
			case RA_MAP_ROTATION_REQUEST:
			{
				if(!IsRemoteAdmin( peerID )) return;

				// ask server for maps in the rotation on the server
				if( RunningServer )
				{
					std::vector<CString> maps = RunningServer->populateMapList(false);

					for( unsigned int i=0; i<maps.size(); i++ )
					{
						net_ra_map_rotation_entry map;
						sprintf(map.mapName,"%s",maps[i].s);

						// send the map entry
						bb_peerSend( peerID, (char*)&map, RA_MAP_ROTATION_ENTRY, sizeof(net_ra_map_entry), true);	
					}
				}
				return;
			}
			case RA_MAP_FILE_HEADER:
			{
				if(!IsRemoteAdmin( peerID )) return;

				// a new file coming for us
				net_ra_map_file_header header;
				memcpy( &header , buf , sizeof(net_ra_map_file_header));
				
				// find our peer
				for( unsigned int i=0; i<m_peers.size(); i++ )
				{
					if( m_peers[i].peerId == peerID )
					{
						m_peers[i].m_sizeRemaining = header.fileSize;
						CString str = "main/maps/";
						str += header.fileName;
						m_peers[i].m_currentFile = fopen( str.s, "wb" );
						CString message = "Receiving map from remote admin : ";
						message += str;
						message += "(";
						message += (int)header.fileSize;
						message += ")";
						console->add( message );
						return;
					}
				}
				return;
			}
			case RA_MAP_FILE_CHUNK:
			{
				if(!IsRemoteAdmin( peerID )) return;

				// find our peer
				for( unsigned int i=0; i<m_peers.size(); i++ )
				{
					if( m_peers[i].peerId == peerID )
					{
						// read first 4 bytes of the buffer
						long size;
						memcpy( &size, buf, 4 );
						fwrite( buf+4, size,1, m_peers[i].m_currentFile );

						m_peers[i].m_sizeRemaining -= size;
						if( m_peers[i].m_sizeRemaining == 0 )
						{
							// finished with this file
							fclose( m_peers[i].m_currentFile );
							m_peers[i].m_currentFile = 0;
						}
						return;
					}
				}
				return;
			}
		}
	}

}


bool CMaster::IsRemoteAdmin( long in_peerId )
{				
	for( unsigned int i=0; i<m_peers.size(); i++ )
	{
		if( m_peers[i].peerId == in_peerId)
		{
			return m_peers[i].remoteAdmin;
		}
	}
	return false;
}

//
//--- On efface les games
//
void CMaster::eraseGames()
{
	for (int i=0;i<(int)m_games.size();++i)
	{
		delete m_games[i];
	}
	m_games.clear();
}

// Remote admin specific commands
void CMaster::RA_NewPlayer( char * in_playerName, char * in_playerIp, long in_playerId )
{
	net_ra_new_player player;
	sprintf( player.name, "%s", in_playerName );
	sprintf( player.ip, "%s", in_playerIp );
	player.id = in_playerId;

	for( unsigned int i=0; i<m_peers.size();i++ )
	{
		if( m_peers[i].remoteAdmin )
		{
			bb_peerSend( m_peers[i].peerId, (char*)&player, RA_NEW_PLAYER, sizeof(net_ra_new_player), true );
		}
	}
}

void CMaster::RA_PositionBroadcast( long in_playerId, short in_x, short in_y )
{
	net_ra_player_position pos;
	pos.playerId = in_playerId;
	pos.x = in_x;
	pos.y = in_y;

	for( unsigned int i=0; i<m_peers.size();i++ )
	{
		if( m_peers[i].remoteAdmin )
		{
			bb_peerSend( m_peers[i].peerId, (char*)&pos, RA_PLAYER_POSITION, sizeof(net_ra_player_position), true );
		}
	}
}

void CMaster::RA_DisconnectedPlayer( char * in_playerName, char * in_playerIp, long in_playerId )
{
	net_ra_player_disconnected player;
	player.id = in_playerId;

	for( unsigned int i=0; i<m_peers.size();i++ )
	{
		if( m_peers[i].remoteAdmin )
		{
			bb_peerSend( m_peers[i].peerId, (char*)&player, RA_PLAYER_DISCONNECTED, sizeof(net_ra_player_disconnected), true );
		}
	}
}

void CMaster::RA_Chat( char * in_chatString, int playerId )
{
	net_ra_chat chat;
	sprintf( chat.message, "%s", in_chatString );
	chat.message[127] = '\0';
	chat.id = playerId;

	for( unsigned int i=0; i<m_peers.size();i++ )
	{
		if( m_peers[i].remoteAdmin )
		{
			bb_peerSend( m_peers[i].peerId, (char*)&chat, RA_CHAT, sizeof(net_ra_chat), true );
		}
	}
}

void CMaster::RA_ConsoleBroadcast( char * in_message )
{
	net_ra_console_broadcast bc;
	sprintf( bc.message, "%s", in_message );
	bc.message[127] = '\0';

	for( unsigned int i=0; i<m_peers.size();i++ )
	{
		if( m_peers[i].remoteAdmin && m_peers[i].broadcastConsole)
		{
			bb_peerSend( m_peers[i].peerId, (char*)&bc, RA_CONSOLE_BROADCAST, sizeof(net_ra_console_broadcast), true );
		}
	}
}

//
//--- To handle the incomming packets
//
void CMaster::recvPacket(const char * buffer, int typeID)
{
	switch (typeID)
	{
	case MASTER_INFO:
		{
#ifndef CONSOLE
			if (lobby) lobby->clearLobby();
#endif
			eraseGames();
			stMasterInfo masterInfo;
			memcpy(&masterInfo, buffer, sizeof(stMasterInfo));
			if (masterInfo.NbGames == 0)
			{
				m_nbGameFound = 0;
				disconnectMaster();
				console->add(CString("\x3> No games found"));
			}
			else if (masterInfo.NbGames == -1)
			{
				m_nbGameFound = 0;
				disconnectMaster();
				console->add(CString("\x4> Outdated version"));
			}
			else
			{
				m_nbGameFound = masterInfo.NbGames;
				console->add(CString("\x3> Games found : %i", m_nbGameFound));
			}
			break;
		}
	case BV2_ROW:
		{
			stBV2row* bv2Row = new stBV2row();
			memcpy(bv2Row, buffer, sizeof(stBV2row));
			m_games.push_back(bv2Row);

			//--- On check si on a pas fini
			//if ((int)m_games.size() >= m_nbGameFound)
			//{
			//	disconnectMaster();
			//}

			if (gameVar.c_debug) console->add(CString("\x9> Received game %i/%i", (int)m_games.size(), m_nbGameFound));
			break;
		}
	case CACHE_ANSWER:
		{	
			stCacheAnswer ca;
			memcpy(&ca,buffer,sizeof(stCacheAnswer));
	
			//add the answer to our list of cached answer for the server to check them later.
			for( int i=0; i<64; i++ )
			{
				if( BannedAnswers[i].ID == -1 )
				{
					BannedAnswers[i].ID			= ca.ID;
					BannedAnswers[i].Answer		= ca.Answer;
					return;
				}
			}

			break;
		}
	case CACHE_BAN:
		{
			stCacheBan cb;
			memcpy(&cb,buffer,sizeof(stCacheBan));

			console->add(CString("[%02i] %s - IP:%s  MAC:%s  Date:%s  Duration:%i", cb.ID,
						cb.Nick,
						cb.IP,
						cb.MAC,
						cb.Date,
						cb.Duration), true);

			break;
		}
	case CACHE_LIST_PLAYER:
		{
			stCachePlayer cp;
			memcpy(&cp,buffer,sizeof(stCachePlayer));

			if (cachePlayerRemoteServer(cp))
				console->add(CString("[%02i] %s \x8- %s  %s", cp.ID, cp.NickName, cp.IP, cp.macAddr), true);

			break;
		}
	case CACHE_LIST_REMOTE:
		{
			stCacheListRemote clr;
			memcpy(&clr,buffer,sizeof(stCacheListRemote));
			sendCacheListToMaster(clr);
			break;
		}
	case ACCOUNT_URL:
		{
			unsigned short version=0;
			memcpy(&version,buffer,2);
			gameVar.db_version = version;
			gameVar.db_accountServer = CString((char*)buffer + 2);

			// update the db
			UpdateDB();
			
			break;
		}
	case SURVEY_CONFIRM:
		{
			surveyReceived = true;

			//--- Write in db the 'surveySent' flag
			sqlite3 *DB=0;
			sqlite3_open("bv2.db",&DB);
			char	SQL[300];
			
			sprintf(SQL,"Update LauncherSettings Set Value = '1' Where Name = 'DidSurvey';");
			sqlite3_exec(DB,SQL,0,0,0);
			
			
			sqlite3_close(DB);
			break;
		}
	case CLIENT_HASH:
		{
			stHashSeed hs;
			memcpy(&hs,buffer,sizeof(stHashSeed));
			master->s1 = hs.s1;
			master->s2 = hs.s2;
			master->s3 = hs.s3;
			master->s4 = hs.s4;
			console->add(CString("hash seed: %d %d %d %d", hs.s1, hs.s2, hs.s3, hs.s4));
			return;
		}
	default:
		{
			console->add("Unhandled message by CMaster");
			break;
		}
	}
}



//
//--- Shooter l'info de la game
//
void CMaster::sendGameInfo(Server* server)
{
	if (server->game)
	{
		//clear la message stack first
		int i;

		ZEVEN_DELETE_VECTOR(messageStack, i);

		stBV2row bv2Row;

		strncpy(bv2Row.map, server->game->mapName.s, 16);
		strncpy(bv2Row.serverName, gameVar.sv_gameName.s, 63);
		strncpy(bv2Row.password, gameVar.sv_password.s, 15);
		bv2Row.ip[0] = '\0';
		bv2Row.port = (unsigned short)gameVar.sv_port;
		int nbPlayer = 0;
		for (i=0;i<MAX_PLAYER;++i)
		{
			if (server->game->players[i])
			{
				nbPlayer++;
			}
		}
		bv2Row.nbPlayer = nbPlayer;
		bv2Row.maxPlayer = gameVar.sv_maxPlayer;
		bv2Row.flags = 0;
		bv2Row.gameType = server->game->gameType;
		bv2Row.ServerID = 0;
		bv2Row.Priority = 0;
		bv2Row.DBVersion = (unsigned short)gameVar.db_version;

		sendPacket((char*)(&bv2Row), sizeof(stBV2row), BV2_ROW);
	}
}




//
//--- Requester la game
//
void CMaster::requestGames()
{
	#ifndef CONSOLE
		//clear lobby
		if( lobby ) lobby->clearLobby();
	#endif

	//clear the game stack
	int i;
	ZEVEN_DELETE_VECTOR(m_games, i);

	stBV2list bv2List;
	strcpy(bv2List.Version, m_CurrentVersion );
	sendPacket((char*)(&bv2List), sizeof(stBV2list), BV2_LIST);

	//send a udp broadcast
	net_clsv_broadcast_query query;
	sprintf( query.key , "3333113333");
	bb_peerSend( NET_BROADCAST , (char*) &query , NET_BROADCAST_QUERY , sizeof( net_clsv_broadcast_query ) , false );


}

//
//--- Get Master Server IP / Port and our current game version from the database
//
void CMaster::GetMasterInfos()
{
	//connect to database
	sqlite3 *db = 0;
	
	int rc = sqlite3_open("bv2.db",&db);
	if(rc)
	{
		console->add("Game Database not found, please re-install the game");
		return ;
	}
	else
	{
		//printf("Opened database successfully\n");
	}

	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int		nRow;			// number of record
	int		nColumn;		// number of column
	char	SQL[256];		// the query

	// Get infos of master servers and choose the one with the lowest Score
	sprintf(SQL,"Select * From MasterServers;");
	sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	
	int i,best=9999,bestIndex=0;
	for(i=0;i<nRow;i++)
	{
		if( atoi(azResult[MASTER_SCORE]) < best )
		{
			best = atoi(azResult[MASTER_SCORE]);
			bestIndex = i;
		}
	}
	i = bestIndex;
	sprintf( m_IP , "%s", azResult[MASTER_IP]);
	m_Port = atoi(azResult[MASTER_PORT]) - 1000;
	sqlite3_free_table(azResult);

	// get our current game version
	sprintf(SQL,"Select Value From LauncherSettings Where Name = 'Version';");
	sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	sprintf( m_CurrentVersion , azResult[1] );
	
	sqlite3_free_table(azResult);
	sqlite3_close( db );
}


//
//--- Send message to master (If not connected, it will try open a connection)
//
void CMaster::sendPacket(const char* in_data, int in_size, int in_ID, bool disconnectAfter)
{

	//--- Are we connected?
	if (!uniqueClientID)
	{
		//--- Connect us first
		connectToMaster( m_IP , m_Port );
		console->add(" master needed to connect ");
	}

	// add the message to the queue
	if( messageStack.size() < 1 || (in_ID >= 1010 && in_ID <= 1013) || in_ID == 1016 || in_ID == 1017)
	{
		messageStack.push_back(new SMasterMessage(in_data, in_size, in_ID));
	}
}

bool CMaster::cachePlayerRemoteServer(const stCachePlayer& player)
{
	if (player.ID < 0 || player.ID >= 50 || player.ReqNum != cacheListReqNum)
		return false;

	CachedPlayersRemote[player.ID].Valid = true;
	sprintf( CachedPlayersRemote[player.ID].IP , "%s", player.IP );
	sprintf( CachedPlayersRemote[player.ID].macAddr , "%s", player.macAddr );
	sprintf( CachedPlayersRemote[player.ID].NickName , "%s", player.NickName );
	return true;
}

void CMaster::clearRemoteCacheList()
{
	for( int i = 0; i < 50; i++ )
	{
		CachedPlayersRemote[i].Valid = false;
		CachedPlayersRemote[i].macAddr[0] = '\0';
		CachedPlayersRemote[i].NickName[0] = '\0';
		CachedPlayersRemote[i].IP[0] = '\0';
	}
}

void CMaster::sendCacheListToMaster(const stCacheListRemote& info)
{
	console->add(" sending req list to master ");
	if (RunningServer)
	{
		CString filter("%s", info.Filter);
		bool sendThis;
		for (int i=0;i<50;i++)
		{
			if( RunningServer->CachedPlayers[i].Valid )
			{
				console->add(" > one player ?");
				sendThis = false;
				//if a parameter was entered after the list, onlyshow those who fits
				if( filter != "" )
				{
					filter.toLower();
					CString name( RunningServer->CachedPlayers[i].NickName );
					name.toLower();

					if( strstr( name.s , filter.s ) )
						sendThis = true;
					else if( strstr( RunningServer->CachedPlayers[i].IP , filter.s ) )
						sendThis = true;
				}
				else
					sendThis = true;
				if (sendThis == true)
				{
					console->add(" >              yes");
					stCachePlayer cp;
					memset(&cp, 0, sizeof(stCachePlayer));
					cp.FromID = info.FromID;
					cp.ID = i;
					memcpy(cp.IP, RunningServer->CachedPlayers[i].IP, 16);
					memcpy(cp.macAddr, RunningServer->CachedPlayers[i].macAddr, 20);
					memcpy(cp.NickName, RunningServer->CachedPlayers[i].NickName, 32);
					cp.ReqNum = info.ReqNum;
					sendPacket( (char*)&cp , sizeof(stCachePlayer) , CACHE_LIST_PLAYER);
				}
			}
		}
		console->add(" remote req end");
		stCacheListEnd cle;
		cle.requestFromID = info.FromID;
		sendPacket( (char*)&cle , sizeof(stCacheListEnd) , CACHE_LIST_PLAYER_END);
	}
}

void CMaster::requestRemoteCacheList(CString& strFilter, CString& serverIP, CString& serverPort, unsigned long baboid)
{
	if (serverIP.len() > 15)
	{
		console->add("\x3> Invalid IP address");
		return;
	}
	if (serverPort.len() > 5)
	{
		console->add("\x3> Invalid port");
		return;
	}
	cacheListReqNum++;
	clearRemoteCacheList();
	stCacheListRemote clr;
	memset(&clr, 0, sizeof(stCacheListRemote));
	clr.ReqNum = cacheListReqNum;
	clr.FromID = baboid;
	sprintf( clr.Filter , "%s", strFilter.s );
	sprintf( clr.ServerIP , "%s", serverIP.s );
	sprintf( clr.ServerPort , "%s", serverPort.s );
	
	sendPacket( (char*)&clr , sizeof(stCacheListRemote) , CACHE_LIST_REMOTE );
}

bool CMaster::hasValidHashSeed()
{
	if (s1 == -1 || s2 == -1 || s3 == -1 || s4 == -1)
		return false;
	else
		return true;
}

void CMaster::HashPass(char* pass)
{
	int* tmp = (int*)pass;
	tmp[0] = tmp[0] ^ (int)s1;
	tmp[1] = tmp[1] ^ (int)s2;
	tmp[2] = tmp[2] ^ (int)s3;
	tmp[3] = tmp[3] ^ (int)s4;
}

bool AccountManagerClient::createAccount(char* login, char* password, char* nick, char* email)
{
	if ((short)strlen(login) > loginMaxLength || (short)strlen(password) > passMaxLength ||
		(short)strlen(nick) > nickMaxLength || (short)strlen(email) > emailMaxLength || !master->hasValidHashSeed())
		return false;

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	stAccount ac;
	memset(&ac, 0, sizeof(stAccount));
	sprintf( ac.Login , "%s", login );
	strncpy( ac.HashedPassMD5, (char*)output, 16 );
	sprintf( ac.Nick , "%s", nick );
	sprintf( ac.Email , "%s", email );
	master->sendPacket( (char*)&ac , sizeof(stAccount) , CREATE_ACCOUNT );

	return true;
}

bool AccountManagerClient::deleteAccount(int userID, char* password)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	stDeleteAccount da;
	memset(&da, 0, sizeof(stDeleteAccount));
	da.UserID = userID;
	strncpy( da.HashedPassMD5, (char*)output, 16 );
	master->sendPacket( (char*)&da , sizeof(stDeleteAccount) , DELETE_ACCOUNT );
	return true;
}

bool AccountManagerClient::updateAccount(char* login, char* password, char* nick, char* email)
{
	if ((short)strlen(login) > loginMaxLength || (short)strlen(password) > passMaxLength ||
		(short)strlen(nick) > nickMaxLength || (short)strlen(email) > emailMaxLength || !master->hasValidHashSeed())
		return false;

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	stAccount ac;
	memset(&ac, 0, sizeof(stAccount));
	sprintf( ac.Login , "%s", login );
	strncpy( ac.HashedPassMD5, (char*)output, 16 );
	sprintf( ac.Nick , "%s", nick );
	sprintf( ac.Email , "%s", email );
	master->sendPacket( (char*)&ac , sizeof(stAccount) , UPDATE_ACCOUNT );
	return true;
}

bool AccountManagerClient::changePassword(int userID, char* oldPass, char* newPass)
{
	if ((short)strlen(oldPass) > passMaxLength || (short)strlen(newPass) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stChangePassword cp;
	memset(&cp, 0, sizeof(stChangePassword));

	unsigned char outputOld[32];
	MD5((unsigned char*)oldPass, strlen(oldPass), outputOld);
	master->HashPass((char*)outputOld);
	unsigned char outputNew[32];
	MD5((unsigned char*)newPass, strlen(newPass), outputNew);
	master->HashPass((char*)outputNew);

	cp.UserID = userID;
	strncpy( cp.HashedOldPassMD5, (char*)outputOld, 16 );
	strncpy( cp.HashedNewPassMD5, (char*)outputNew, 16 );
	master->sendPacket( (char*)&cp , sizeof(stChangePassword) , CHANGE_PASSWORD );
	return true;
}

bool AccountManagerClient::recoverPassword(char* login)
{
	if ((short)strlen(login) > loginMaxLength)
		return false;

	stRecoverPassword rp;
	memset(&rp, 0, sizeof(stRecoverPassword));
	sprintf( rp.Login , "%s", login );
	master->sendPacket( (char*)&rp , sizeof(stRecoverPassword) , RECOVER_PASSWORD );
	return true;
}

bool AccountManagerClient::loginAccount(char* login, char* password)
{
	if ((short)strlen(login) > loginMaxLength || (short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	stLoginLogout da;
	memset(&da, 0, sizeof(stLoginLogout));
	sprintf( da.Login , "%s", login );
	strncpy( da.HashedPassMD5, (char*)output, 16 );
	master->sendPacket( (char*)&da , sizeof(stLoginLogout) , LOGIN_ACCOUNT );
	return true;
}

bool AccountManagerClient::logoutAccount(char* login, char* password)
{
	if ((short)strlen(login) > loginMaxLength || (short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	stLoginLogout da;
	memset(&da, 0, sizeof(stLoginLogout));
	sprintf( da.Login , "%s", login );
	strncpy( da.HashedPassMD5, (char*)output, 16 );
	master->sendPacket( (char*)&da , sizeof(stLoginLogout) , LOGOUT_ACCOUNT );
	return true;
}

bool AccountManagerClient::userStatusUpdate(int userID, char* password, char* serverName, char* ip, short port)
{
	if ((short)strlen(password) > passMaxLength || (short)strlen(serverName) > 64 || strlen(ip) > 16 || !master->hasValidHashSeed())
		return false;

	stUserStatusUpdate su;
	memset(&su, 0, sizeof(stUserStatusUpdate));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	su.UserID = userID;
	strncpy( su.HashedPassMD5, (char*)output, 16 );
	sprintf( su.ServerName , "%s", serverName );
	sprintf( su.IP , "%s", ip );
	su.Port = port;
	master->sendPacket( (char*)&su , sizeof(stUserStatusUpdate) , USER_STATUS_UPDATE );
	return true;
}

bool AccountManagerClient::registerClan(int userID, char* password, char* name, char* tag, char* email, char* website)
{
	if ((short)strlen(password) > passMaxLength || (short)strlen(name) > clanNameMaxLength ||
		(short)strlen(tag) > clanTagMaxLength || (short)strlen(website) > websiteMaxLength || !master->hasValidHashSeed())
		return false;

	stRegisterClan data;
	memset(&data, 0, sizeof(stRegisterClan));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	sprintf( data.Name , "%s", name );
	sprintf( data.Tag , "%s", tag );
	sprintf( data.Email , "%s", email );
	sprintf( data.Website , "%s", website );

	master->sendPacket( (char*)&data , sizeof(stRegisterClan) , REGISTER_CLAN );
	return true;
}

bool AccountManagerClient::removeClan(int userID, char* password)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stRemoveClan data;
	memset(&data, 0, sizeof(stRemoveClan));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	master->sendPacket( (char*)&data , sizeof(stRemoveClan) , REMOVE_CLAN );
	return true;
}

bool AccountManagerClient::changeClanPermissions(int userID, char* password, int userIDChanging, char permissions)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stChangeClanPermissions data;
	memset(&data, 0, sizeof(stChangeClanPermissions));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDChanging;
	data.Permissions = permissions;
	master->sendPacket( (char*)&data , sizeof(stChangeClanPermissions) , CHANGE_CLAN_PERMISSIONS );
	return true;
}

bool AccountManagerClient::joinClanRequest(int userID, char* password, int userIDDest)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stJoinClanRequest data;
	memset(&data, 0, sizeof(stJoinClanRequest));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDDest;
	master->sendPacket( (char*)&data , sizeof(stJoinClanRequest) , JOIN_CLAN_REQUEST );
	return true;
}

bool AccountManagerClient::joinClanAccept(int userID, char* password, int userIDFrom)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stJoinClanAccept data;
	memset(&data, 0, sizeof(stJoinClanAccept));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.UserIDFrom = userIDFrom;
	master->sendPacket( (char*)&data , sizeof(stJoinClanAccept) , JOIN_CLAN_ACCEPT );
	return true;
}

bool AccountManagerClient::leaveClan(int userID, char* password)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stLeaveClan data;
	memset(&data, 0, sizeof(stLeaveClan));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	master->sendPacket( (char*)&data , sizeof(stLeaveClan) , LEAVE_CLAN );
	return true;
}

bool AccountManagerClient::requestFriend(int userID, char* password, int userIDDest, char* groupName)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stFriendOp data;
	memset(&data, 0, sizeof(stFriendOp));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDDest;
	strncpy( data.GroupName, groupName, 49 );
	master->sendPacket( (char*)&data , sizeof(stFriendOp) , REQUEST_FRIEND );
	return true;
}

bool AccountManagerClient::acceptFriend(int userID, char* password, int userIDDest, char* groupName)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stFriendOp data;
	memset(&data, 0, sizeof(stFriendOp));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDDest;
	strncpy( data.GroupName, groupName, 49 );
	master->sendPacket( (char*)&data , sizeof(stFriendOp) , ACCEPT_FRIEND );
	return true;
}

bool AccountManagerClient::removeFriend(int userID, char* password, int userIDDest)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stFriendOp data;
	memset(&data, 0, sizeof(stFriendOp));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDDest;
	master->sendPacket( (char*)&data , sizeof(stFriendOp) , REMOVE_FRIEND );
	return true;
}

bool AccountManagerClient::moveFriend(int userID, char* password, int userIDDest, char* groupName)
{
	if ((short)strlen(password) > passMaxLength || !master->hasValidHashSeed())
		return false;

	stFriendOp data;
	memset(&data, 0, sizeof(stFriendOp));

	unsigned char output[16];
	MD5((unsigned char*)password, strlen(password), output);
	master->HashPass((char*)output);

	data.UserID = userID;
	strncpy( data.HashedPassMD5, (char*)output, 16 );
	data.DestUserID = userIDDest;
	strncpy( data.GroupName, groupName, 49 );
	master->sendPacket( (char*)&data , sizeof(stFriendOp) , MOVE_FRIEND );
	return true;
}

bool AccountManagerClient::statsUpdate()
{
	stStatsUpdate data;
	memset(&data, 0, sizeof(stStatsUpdate));
	master->sendPacket( (char*)&data , sizeof(stStatsUpdate) , STATS_UPDATE );
	return true;
}

