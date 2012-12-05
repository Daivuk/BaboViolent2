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

#ifndef CONSOLE
#include "Client.h"
#include "netPacket.h"
#include "Console.h"
#include "Scene.h"
#include "md5.h"
#include "CStatus.h"

#ifdef _PRO_
	#include "md5_2.h"
#endif

extern Scene * scene;

//
// On a reçu un message yéé !
//
void Client::recvPacket(char * buffer, int typeID)
{
#ifdef _PRO_
   
	if( typeID == NET_SVCL_HASH_SEED )
	{
		// we receive an hash seed, we need to send a response back
		net_svcl_hash_seed hashseed;
		memcpy(&hashseed, buffer, sizeof(net_svcl_hash_seed));

		int output[4];
      // Client side, lets just grab the local executable name
#ifdef WIN32
      char pFile[512+1];
      GetModuleFileName(NULL, pFile, 512);
		int result = md5_file(pFile, (unsigned char*)&output);		
#else 
		int result = md5_file("./bv2.exe", (unsigned char*)&output);
#endif

		//console->add(CString("\x03> client MD5 Output1 : %i",output[0]));
		//console->add(CString("\x03> client MD5 Output2 : %i",output[1]));
		//console->add(CString("\x03> client MD5 Output3 : %i",output[2]));
		//console->add(CString("\x03> client MD5 Output4 : %i",output[3]));
		
		int hashedOutput[4];
		hashedOutput[0] = output[0] ^ (int)hashseed.s1;
		hashedOutput[1] = output[1] ^ (int)hashseed.s2;
		hashedOutput[2] = output[2] ^ (int)hashseed.s3;
		hashedOutput[3] = output[3] ^ (int)hashseed.s4;

		//console->add(CString("\x03> client MD5 HashedOutput1 : %i",hashedOutput[0]));
		//console->add(CString("\x03> client MD5 HashedOutput2 : %i",hashedOutput[1]));
		//console->add(CString("\x03> client MD5 HashedOutput3 : %i",hashedOutput[2]));
		//console->add(CString("\x03> client MD5 HashedOutput4 : %i",hashedOutput[3]));

		// answer back with our result
		net_svcl_hash_seed newHash;
		newHash.s1 = (short)hashedOutput[0];
		newHash.s2 = (short)hashedOutput[1];
		newHash.s3 = (short)hashedOutput[2];
		newHash.s4 = (short)hashedOutput[3];

		bb_clientSend(uniqueClientID, (char*)&newHash, sizeof(net_svcl_hash_seed), NET_SVCL_HASH_SEED_REPLY);
	}
#endif


	if (!gotGameState || !isConnected)
	{
		if (typeID != NET_SVCL_NEWPLAYER &&
			typeID != NET_SVCL_GAMEVERSION &&
			typeID != NET_SVCL_SERVER_INFO &&
			typeID != NET_SVCL_PING &&
			typeID != NET_SVCL_MAP_CHUNK)
		{
			return;
		}
	}

	switch (typeID)
	{
	case NET_SVCL_MAP_CHUNK:
		{
			net_svcl_map_chunk chunk;
			memcpy(&chunk, buffer, sizeof(net_svcl_map_chunk));

			if(isDownloadingMap && game && game->thisPlayer)
			{
				// Map has been recieved when last chunk is 0
				if(chunk.size == 0)
				{
					isDownloadingMap = false;
					game->mapBytesRecieved = game->mapBuffer.getPos();
					game->mapBuffer.reset();
					game->createMap();
					if (!game->map)
					{
						needToShutDown = true;
						break;
					}
					
					// Re-query server in case state has changed
					net_clsv_gameversion_accepted gameVersionAccepted;
					gameVersionAccepted.playerID = game->thisPlayer->playerID;
					strcpy(gameVersionAccepted.password, m_password.s);
					bb_clientSend(uniqueClientID, (char*)&gameVersionAccepted, sizeof(net_clsv_gameversion_accepted), NET_CLSV_GAMEVERSION_ACCEPTED);

				}
				else
				{
					game->mapBuffer.put(chunk.data, (int)chunk.size);
				}
			}
			break;
		}
	case NET_SVCL_VOTE_RESULT:
		{
			net_svcl_vote_result voteResult;
			memcpy(&voteResult, buffer, sizeof(net_svcl_vote_result));

			//--- Print on screen
			if (game)
			{
				game->voting.votingInProgress = false;
				if (voteResult.passed)
				{
					eventMessages.push_back(CString("\x9Vote passed"));
					console->add(CString("\x9Vote passed"));
				}
				else
				{
					eventMessages.push_back(CString("\x9Vote failed"));
					console->add(CString("\x9Vote failed"));
				}
			}
			break;
		}
	case NET_SVCL_UPDATE_VOTE:
		{
			net_svcl_update_vote updateVote;
			memcpy(&updateVote, buffer, sizeof(net_svcl_update_vote));

			if (game)
			{
				game->voting.votingResults[0] = (int)updateVote.nbYes;
				game->voting.votingResults[1] = (int)updateVote.nbNo;
			}
			break;
		}
	case NET_SVCL_MSG:
		{
			net_svcl_msg msg;
			memcpy(&msg, buffer, sizeof(net_svcl_msg));
			if (game->thisPlayer)
			{
				if ((msg.teamID == PLAYER_TEAM_SPECTATOR - 1 || msg.teamID == game->thisPlayer->teamID))
				{
					CString msgText("%s", "\x4 - Server: ");
					msgText += msg.message;
					console->add(msgText);
					if (msg.msgDest == 0)
					{
						chatMessages.push_back(TimedMessage(msgText));
						dksPlaySound(m_sfxChat, -1, 200);
					}
				}
			}
			break;
		}
	case NET_SVCL_PLAYER_UPDATE_STATS:
		{
			net_svcl_player_update_stats playerStats;
			memcpy(&playerStats, buffer, sizeof(net_svcl_player_update_stats));
			game->players[playerStats.playerID]->kills = (int)playerStats.kills;
			game->players[playerStats.playerID]->deaths = (int)playerStats.deaths;
			game->players[playerStats.playerID]->score = (int)playerStats.score;
			game->players[playerStats.playerID]->returns = (int)playerStats.returns;
			game->players[playerStats.playerID]->flagAttempts = (int)playerStats.flagAttempts;
			game->players[playerStats.playerID]->timePlayedCurGame = playerStats.timePlayedCurGame;
			break;
		}
	case NET_CLSV_SVCL_VOTE_REQUEST:
		{
			net_clsv_svcl_vote_request voteRequest;
			memcpy(&voteRequest, buffer, sizeof(net_clsv_svcl_vote_request));
			if (game)
			{
				if (game->players[voteRequest.playerID])
				{
					//--- Start the voting
					game->voting.castVote(game, voteRequest);
					game->voting.votingFrom = textColorLess(game->players[voteRequest.playerID]->name);

					/*for (int i=0;i<MAX_PLAYER;++i)
					{
						if ( (game->players[i]) && (game->players[i]->teamID != PLAYER_TEAM_AUTO_ASSIGN) &&
							(game->players[i]->teamID != PLAYER_TEAM_AUTO_ASSIGN) )
							++game->voting.nbActivePlayers;
					}*/
				}
			}
			break;
		}
	case NET_CLSV_SVCL_PLAYER_SHOOT_MELEE:
		{
			net_clsv_svcl_player_shoot_melee playerShootMelee;
			memcpy(&playerShootMelee, buffer, sizeof(net_clsv_svcl_player_shoot_melee));
			if (game->players[playerShootMelee.playerID])
			{
				if (game->players[playerShootMelee.playerID]->meleeWeapon)
				{
					game->players[playerShootMelee.playerID]->meleeWeapon->shootMelee(game->players[playerShootMelee.playerID]);
				}
			}
			break;
		}
	case NET_SVCL_ADMIN_ACCEPTED:
		{
			console->add("NOW ADMIN -\x2 ACCEPTED");
			isAdmin = true;
			break;
		}
	case NET_SVCL_NEWPLAYER:
		{
			net_svcl_newplayer newPlayer;
			memcpy(&newPlayer, buffer, sizeof(net_svcl_newplayer));
			game->createNewPlayerCL(newPlayer.newPlayerID, newPlayer.baboNetID);

			if (!game->thisPlayer)
			{
				game->thisPlayer = game->players[newPlayer.newPlayerID];
				game->thisPlayer->setThisPlayerInfo();
			}
			break;
		}
	case NET_SVCL_GAMEVERSION:
		{
			net_svcl_gameversion gameVersion;
			memcpy(&gameVersion, buffer, sizeof(net_svcl_gameversion));
			if (gameVersion.gameVersion == GAME_VERSION_CL && game->thisPlayer)
			{
				net_clsv_svcl_player_info playerInfo;

				// on pogne notre mac adress
				unsigned char mac[8];		// unsigned here is very important
				bb_getMyMAC( mac );
				sprintf( playerInfo.macAddr , "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x" , (int)mac[0], (int)mac[1],(int)mac[2],(int)mac[3],(int)mac[4],(int)mac[5] );
				
				// On send le playerInfo
				playerInfo.playerID = game->thisPlayer->playerID;
				memcpy(playerInfo.playerName, game->thisPlayer->name.s, game->thisPlayer->name.len() + 1);

				gameVar.cl_accountUsername.resize(20);
				memcpy(playerInfo.username, gameVar.cl_accountUsername.s, gameVar.cl_accountUsername.len() + 1);

				RSA::MD5 pw((unsigned char*)gameVar.cl_accountPassword.s);
				char* hex_digest = pw.hex_digest();
				memcpy(playerInfo.password, hex_digest, 32);
				delete[] hex_digest;

				bb_clientSend(uniqueClientID, (char*)&playerInfo, sizeof(net_clsv_svcl_player_info), NET_CLSV_SVCL_PLAYER_INFO);

				console->add(CString("\x3> Same game version. Server is : %01i.%02i.%02i", (int)gameVersion.gameVersion/10000, (int)(gameVersion.gameVersion%10000)/100, ((int)gameVersion.gameVersion%100)));
				net_clsv_gameversion_accepted gameVersionAccepted;
				gameVersionAccepted.playerID = game->thisPlayer->playerID;
				if (scene->server) //--- We are also server, retreive password
				{
					strcpy(gameVersionAccepted.password, gameVar.sv_password.s);
				}
				else
				{
					strcpy(gameVersionAccepted.password, m_password.s);
				}
				bb_clientSend(uniqueClientID, (char*)&gameVersionAccepted, sizeof(net_clsv_gameversion_accepted), NET_CLSV_GAMEVERSION_ACCEPTED);
			}
			else
			{
				// On disconnect
				console->add(CString("\x4> Wrong game version. Server is : %01i.%02i.%02i", (int)gameVersion.gameVersion/10000, (int)(gameVersion.gameVersion%10000)/100, ((int)gameVersion.gameVersion%100)));
				this->needToShutDown = true;
				wrongVersionReason = true;
			}
			break;
		}
	case NET_CLSV_SVCL_PLAYER_INFO:
		{
			net_clsv_svcl_player_info playerInfo;
			memcpy(&playerInfo, buffer, sizeof(net_clsv_svcl_player_info));
			playerInfo.playerName[31] = '\0';
			game->players[playerInfo.playerID]->name = playerInfo.playerName;
			memcpy(game->players[playerInfo.playerID]->playerIP, playerInfo.playerIP, 16);
			// On a son nom! on le print
			console->add(CString("\x3> %s joined the game", playerInfo.playerName));
			eventMessages.push_back(CString(gameVar.lang_joinedTheGame.s, playerInfo.playerName));
			break;
		}
	case NET_CLSV_SVCL_CHAT:
		{
			net_clsv_svcl_chat chat;
			memcpy(&chat, buffer, sizeof(net_clsv_svcl_chat));
			// On print dans console
			if (game->thisPlayer)
			{
				if ((chat.teamID < PLAYER_TEAM_SPECTATOR || chat.teamID == game->thisPlayer->teamID))
				{
					console->add(chat.message, false, false);
					chatMessages.push_back(TimedMessage(chat.message));
					dksPlaySound(m_sfxChat, -1, 200);
				}
			}
			break;
		}
	case NET_SVCL_PLAYER_DISCONNECT:
		{
			net_svcl_player_disconnect playerDisconnect;
			memcpy(&playerDisconnect, buffer, sizeof(net_svcl_player_disconnect));
			if (game->players[playerDisconnect.playerID])
			{
				game->players[playerDisconnect.playerID]->kill(true);
				console->add(CString("\x3> Player disconnected : %s ID:%i", game->players[playerDisconnect.playerID]->name.s, playerDisconnect.playerID));
				eventMessages.push_back(CString(gameVar.lang_playerDisconnected.s, game->players[playerDisconnect.playerID]->name.s));
				ZEVEN_SAFE_DELETE(game->players[playerDisconnect.playerID]);
			}
			break;
		}
	case NET_SVCL_SERVER_INFO:
		{
			isConnected = true;
			gotGameState = true; // C beau, on est IN !!
			net_svcl_server_info serverInfo;
			memcpy(&serverInfo, buffer, sizeof(net_svcl_server_info));
			game->mapSeed = serverInfo.mapSeed;
			game->mapName = serverInfo.mapName;
			game->blueScore = serverInfo.blueScore;
			game->redScore = serverInfo.redScore;
			game->blueWin = serverInfo.blueWin;
			game->redWin = serverInfo.redWin;
			// On cré sa map ici
			if(isServer)
				game->createMap();
			game->gameType = serverInfo.gameType;
			// On a fini de loader, on change notre status
			if (game->thisPlayer)
			{
				// On n'est plus en mode loading (y a tu vraiment un mode loader? lol)
				game->thisPlayer->status = PLAYER_STATUS_DEAD;
			}
			// If no map created, send request
			if (!game->map && isServer)
				this->needToShutDown = true;
			else if(!game->map)
			{
				gotGameState = false;
				isConnected = false;
				isDownloadingMap = true;

				net_clsv_map_request request;
				strcpy(request.mapName, serverInfo.mapName);
				request.uniqueClientID = uniqueClientID;
                bb_clientSend(uniqueClientID,(char*)&request,sizeof(net_clsv_map_request),NET_CLSV_MAP_REQUEST);
				
				break;
			}
			else if (!game->map->isValid)
				this->needToShutDown = true;

			//--- On start ça ste tune là !
	/*		if (gameVar.s_inGameMusic)
			{
				if (rand()%2 == 0)
				{
					dksPlayMusic("main/sounds/InGame01.ogg", -1);
				}
				else
				{
					dksPlayMusic("main/sounds/InGame02.ogg", -1);
				}
			}*/
			break;
		}
	case NET_SVCL_PLAYER_ENUM_STATE:
		{
			net_svcl_player_enum_state playerState;
			memcpy(&playerState, buffer, sizeof(net_svcl_player_enum_state));
			game->createNewPlayerCL(playerState.playerID, playerState.babonetID);
			game->players[playerState.playerID]->name = playerState.playerName;
			memcpy(game->players[playerState.playerID]->playerIP, playerState.playerIP, 16);
			game->players[playerState.playerID]->kills = (int)playerState.kills;
			game->players[playerState.playerID]->deaths = (int)playerState.deaths;
			game->players[playerState.playerID]->score = (int)playerState.score;
			game->players[playerState.playerID]->returns = (int)playerState.returns;
			game->players[playerState.playerID]->flagAttempts = (int)playerState.flagAttempts;
			game->players[playerState.playerID]->damage = (int)playerState.damage;
			game->players[playerState.playerID]->status = playerState.status;
			game->players[playerState.playerID]->teamID = playerState.teamID;
			game->players[playerState.playerID]->switchWeapon(playerState.weaponID);
			game->players[playerState.playerID]->life = playerState.life;
			game->players[playerState.playerID]->dmg = playerState.dmg;
			playerState.skin[6] = '\0';
			game->players[playerState.playerID]->skin = playerState.skin;
			game->players[playerState.playerID]->blueDecal.set(
				((float)playerState.blueDecal[0]) / 255.0f,
				((float)playerState.blueDecal[1]) / 255.0f,
				((float)playerState.blueDecal[2]) / 255.0f);
			game->players[playerState.playerID]->greenDecal.set(
				((float)playerState.greenDecal[0]) / 255.0f,
				((float)playerState.greenDecal[1]) / 255.0f,
				((float)playerState.greenDecal[2]) / 255.0f);
			game->players[playerState.playerID]->redDecal.set(
				((float)playerState.redDecal[0]) / 255.0f,
				((float)playerState.redDecal[1]) / 255.0f,
				((float)playerState.redDecal[2]) / 255.0f);
			game->players[playerState.playerID]->updateSkin();
			console->add(CString("\x3> Adding player %s", game->players[playerState.playerID]->name.s));
			break;
		}
	case NET_CLSV_SVCL_TEAM_REQUEST:
		{
			net_clsv_svcl_team_request teamRequest;
			memcpy(&teamRequest, buffer, sizeof(net_clsv_svcl_team_request));

			// Est-ce que ce player existe
			if (game->players[teamRequest.playerID])
			{
				game->assignPlayerTeam(teamRequest.playerID, teamRequest.teamRequested, this);
			}
			break;
		}
	case NET_SVCL_PLAYER_PING:
		{
			net_svcl_player_ping playerPing;
			memcpy(&playerPing, buffer, sizeof(net_svcl_player_ping));
			if (game->players[playerPing.playerID])
			{
				game->players[playerPing.playerID]->ping = (int)playerPing.ping;
			}
			break;
		}
	case NET_SVCL_PING:
		{
			if (game->thisPlayer)
			{
				net_clsv_pong pong;
				pong.playerID = game->thisPlayer->playerID;
				bb_clientSend(uniqueClientID, (char*)&pong, sizeof(net_clsv_pong), NET_CLSV_PONG);
			}
			break;
		}
	case NET_SVCL_PLAYER_SPAWN:
		{
			net_svcl_player_spawn playerSpawn;
			memcpy(&playerSpawn, buffer, sizeof(net_svcl_player_spawn));
			if (game->players[playerSpawn.playerID])
			{
				playerSpawn.skin[6] = '\0'; // Au cas qu'un hacker s'amuse
				game->players[playerSpawn.playerID]->skin = playerSpawn.skin;
				game->players[playerSpawn.playerID]->blueDecal.set(
					((float)playerSpawn.blueDecal[0]) / 255.0f,
					((float)playerSpawn.blueDecal[1]) / 255.0f,
					((float)playerSpawn.blueDecal[2]) / 255.0f);
				game->players[playerSpawn.playerID]->greenDecal.set(
					((float)playerSpawn.greenDecal[0]) / 255.0f,
					((float)playerSpawn.greenDecal[1]) / 255.0f,
					((float)playerSpawn.greenDecal[2]) / 255.0f);
				game->players[playerSpawn.playerID]->redDecal.set(
					((float)playerSpawn.redDecal[0]) / 255.0f,
					((float)playerSpawn.redDecal[1]) / 255.0f,
					((float)playerSpawn.redDecal[2]) / 255.0f);
				game->players[playerSpawn.playerID]->nextSpawnWeapon = playerSpawn.weaponID;
				game->players[playerSpawn.playerID]->nextMeleeWeapon = playerSpawn.meleeID;
				game->players[playerSpawn.playerID]->spawn(CVector3f((float)playerSpawn.position[0]/10.0f, (float)playerSpawn.position[1]/10.0f, (float)playerSpawn.position[2]/10.0f));
			}
			break;
		}
	case NET_CLSV_SVCL_PLAYER_COORD_FRAME:
		{
			net_clsv_svcl_player_coord_frame playerCoordFrame;
			memcpy(&playerCoordFrame, buffer, sizeof(net_clsv_svcl_player_coord_frame));
			if (game->players[playerCoordFrame.playerID])
			{
				game->players[playerCoordFrame.playerID]->setCoordFrame(playerCoordFrame);
			}
			break;
		}
#ifdef _PRO_
	case NET_SVCL_MINIBOT_COORD_FRAME:
		{
			net_svcl_minibot_coord_frame minibotCoordFrame;
			memcpy(&minibotCoordFrame, buffer, sizeof(net_svcl_minibot_coord_frame));
			if (game->players[minibotCoordFrame.playerID])
			{
				game->players[minibotCoordFrame.playerID]->setCoordFrameMinibot(minibotCoordFrame);
			}
			break;
		}
	case NET_SVCL_CREATE_MINIBOT:
		{
			net_svcl_create_minibot miniBot;
			memcpy(&miniBot, buffer, sizeof(net_svcl_create_minibot));
			if (game->players[miniBot.playerID])
			{
				CVector3f pos;
				pos[0] = (float)miniBot.position[0] / 10.0f;
				pos[1] = (float)miniBot.position[1] / 10.0f;
				pos[2] = (float)miniBot.position[2] / 10.0f;
				CVector3f mousePos;
				mousePos[0] = (float)miniBot.mousePos[0] / 10.0f;
				mousePos[1] = (float)miniBot.mousePos[1] / 10.0f;
				mousePos[2] = (float)miniBot.mousePos[2] / 10.0f;
				game->players[miniBot.playerID]->SpawnMiniBot(pos, mousePos);
			}
			break;
		}
#endif
	case NET_SVCL_PROJECTILE_COORD_FRAME:
		{
			net_svcl_projectile_coord_frame projectileCoordFrame;
			memcpy(&projectileCoordFrame, buffer, sizeof(net_svcl_projectile_coord_frame));
		//	console->add(CString("Client recved projectile Coord Frame %i", (int)projectileCoordFrame.frameID));
			// Est-ce que notre player pocède des projectiles au moins? Sinon il n'est peut etre pas créé encore (bug)
		//	if ((int)game->projectiles.size() > projectileCoordFrame.projectileID && projectileCoordFrame.projectileID >= 0)
		//	{
		//		if (game->projectiles[projectileCoordFrame.projectileID]->uniqueID != projectileCoordFrame.uniqueID)
		//		{
					// On doit searcher pour le bon, ils ont peut etre été décallé
					for (int i=0;i<(int)game->projectiles.size();++i)
					{
						Projectile * projectile = game->projectiles[i];
						if (game->projectiles[i]->uniqueID == projectileCoordFrame.uniqueID)
						{
							projectileCoordFrame.projectileID = (short)i;
							game->projectiles[projectileCoordFrame.projectileID]->setCoordFrame(projectileCoordFrame);
							break;
						}
					}
		//		}
		//		game->projectiles[projectileCoordFrame.projectileID]->setCoordFrame(projectileCoordFrame);
		//	}
		/*	else
			{
				// Warning en sale ici
				console->add(CString("\x9> Client warning, out of range projectile index : %i", (int)projectileCoordFrame.projectileID));
			}*/
			break;
		}
	case NET_SVCL_SV_CHANGE:
		{
			net_svcl_sv_change svChange;
			memcpy(&svChange, buffer, sizeof(net_svcl_sv_change));
			console->svChange(svChange.svChange);

			CString sv = CString("%s", svChange.svChange);
			sv.getFirstToken(' ');
         CString sValue = sv.getFirstToken(' ');
			if(sValue == "sv_gamePublic")
			{
				status->set(CStatus::INGAME, gameVar.sv_gameName, server_ip, gameVar.sv_port);
			}
			if(sValue == "sv_nukeReload")
			{
				gameVar.weapons[WEAPON_NUCLEAR]->fireDelay = gameVar.sv_nukeReload;
			}         
			if(sValue == "sv_serverType")
			{
				game->UpdateProSettings();
			}

			break;
		}
	case NET_CLSV_SVCL_PLAYER_CHANGE_NAME:
		{
			net_clsv_svcl_player_change_name playerChangeName;
			memcpy(&playerChangeName, buffer, sizeof(net_clsv_svcl_player_change_name));
			if (game->players[playerChangeName.playerID])
			{
				console->add(CString("\x3> Player %s changed his name for %s", game->players[playerChangeName.playerID]->name.s, playerChangeName.playerName));
				eventMessages.push_back(CString(gameVar.lang_playerChangedHisNameFor.s, game->players[playerChangeName.playerID]->name.s, playerChangeName.playerName));
				game->players[playerChangeName.playerID]->name = playerChangeName.playerName;
			}
			break;
		}
	case NET_SVCL_PLAYER_SHOOT:
		{
			net_svcl_player_shoot playerShoot;
			memcpy(&playerShoot, buffer, sizeof(net_svcl_player_shoot));
			if (game->players[playerShoot.playerID])
			{
				if (playerShoot.weaponID == WEAPON_MINIBOT_WEAPON)
				{
					Player * player = game->players[playerShoot.playerID];
					if (player->minibot)
					{
						CVector3f p1;
						CVector3f p2;
						CVector3f normal;
						p1[0] = (float)playerShoot.p1[0] / 100.0f;
						p1[1] = (float)playerShoot.p1[1] / 100.0f;
						p1[2] = (float)playerShoot.p1[2] / 100.0f;
						p2[0] = (float)playerShoot.p2[0] / 100.0f;
						p2[1] = (float)playerShoot.p2[1] / 100.0f;
						p2[2] = (float)playerShoot.p2[2] / 100.0f;
						normal[0] = (float)playerShoot.normal[0] / 120.0f;
						normal[1] = (float)playerShoot.normal[1] / 120.0f;
						normal[2] = (float)playerShoot.normal[2] / 120.0f;

						// Si c'est nous qui se fait toucher, on subit un recul!
						CVector3f direction = p2 - p1;
						normalize(direction);
						if (playerShoot.hitPlayerID == game->thisPlayer->playerID)
						{
							game->thisPlayer->currentCF.vel += direction * .08f * 2;
						}

						// On spawn la trail
						game->spawnImpact(p1, p2, normal, gameVar.weapons[WEAPON_SMG], .08f, game->players[playerShoot.playerID]->teamID);

						//--- Play a sound
						dksPlay3DSound(gameVar.sfx_ric[2], -1, 5, 
							player->minibot->currentCF.position,255);
					}
					return;
				}
				bool itsMine = false;
				// Si c'est le notre, on se spawn une trail
				if (game->thisPlayer)
				{
					CVector3f p1;
					CVector3f p2;
					CVector3f normal;
					p1[0] = (float)playerShoot.p1[0] / 100.0f;
					p1[1] = (float)playerShoot.p1[1] / 100.0f;
					p1[2] = (float)playerShoot.p1[2] / 100.0f;
					p2[0] = (float)playerShoot.p2[0] / 100.0f;
					p2[1] = (float)playerShoot.p2[1] / 100.0f;
					p2[2] = (float)playerShoot.p2[2] / 100.0f;
					normal[0] = (float)playerShoot.normal[0] / 120.0f;
					normal[1] = (float)playerShoot.normal[1] / 120.0f;
					normal[2] = (float)playerShoot.normal[2] / 120.0f;


					// Si c'est nous qui se fait toucher, on subit un recul!
					CVector3f direction = p2 - p1;
					normalize(direction);
					if (playerShoot.hitPlayerID == game->thisPlayer->playerID)
					{
						game->thisPlayer->currentCF.vel += direction * gameVar.weapons[playerShoot.weaponID]->damage * 2;
					}

					if (playerShoot.playerID == game->thisPlayer->playerID)
					{
						itsMine = true;
						// On spawn la trail
						if (game->players[playerShoot.playerID]->weapon->weaponID != WEAPON_FLAME_THROWER) game->spawnImpact(p1, p2, normal, game->players[playerShoot.playerID]->weapon, game->players[playerShoot.playerID]->weapon->damage, game->players[playerShoot.playerID]->teamID);
						else if (game->players[playerShoot.playerID]->weapon->weaponID == WEAPON_FLAME_THROWER)
						{
							//--- Spawn fire
							CVector3f firePos;
							for (float i=0;i<=1;i+=.05f)
							{
								firePos = p1 + (p2 - p1) * i;
								dkpCreateParticle(	(firePos+rand(CVector3f(-i*.3f,-i*.3f,0),CVector3f(i*.3f,i*.3f,0))).s,//float *position,
													(CVector3f(0,0,1) + normal).s,//float *vel,
													rand(CVector4f(i,0,1-i,0.0f),CVector4f(i,i*.75f,1-i,0.0f)).s,//float *startColor,
													CVector4f(i,i*.75f,1-i,1.0f).s,//float *endColor,
													.6f * (i*.5f + .5f),//float startSize,
													.0f,//float endSize,
													i/*.5f*/,//float duration,
													0,//float gravityInfluence,
													0,//float airResistanceInfluence,
													rand(0.0f, 30.0f),//float rotationSpeed,
													gameVar.tex_smoke1,//unsigned int texture,
													DKP_SRC_ALPHA,//unsigned int srcBlend,
													DKP_ONE,//unsigned int dstBlend,
													0);//int transitionFunc);
								dkpCreateParticle(	(firePos+rand(CVector3f(-i*.3f,-i*.3f,0),CVector3f(i*.3f,i*.3f,0))).s,//float *position,
													(CVector3f(0,0,1) + normal+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *vel,
													rand(CVector4f(i,0,1-i,1.0f),CVector4f(i,i*.75f,1-i,1.0f)).s,//float *startColor,
													CVector4f(i,i*.75f,1-i,0.0f).s,//float *endColor,
													.0f,//float startSize,
													.6f * (i*.5f + .5f),//float endSize,
													i/*.5f*/,//float duration,
													0,//float gravityInfluence,
													0,//float airResistanceInfluence,
													rand(0.0f, 30.0f),//float rotationSpeed,
													gameVar.tex_smoke1,//unsigned int texture,
													DKP_SRC_ALPHA,//unsigned int srcBlend,
													DKP_ONE,//unsigned int dstBlend,
													0);//int transitionFunc);
							}
						}
					}
				}

				// Si on touche un joueur, on spawn du SANG :D:D:D si c'est ff à on ou off pis que c un ennemy :(
				if (playerShoot.hitPlayerID >= 0)
				{
					// On décrémente sa vie
				//	game->players[playerShoot.hitPlayerID]->hit(game->players[playerShoot.playerID]->weapon, game->players[playerShoot.playerID]);
				}
				
				// Sinon on c'est un autre player, on spawn la trail et le feu de son gun
				if (!itsMine) 
				{
					game->players[playerShoot.playerID]->firedShowDelay = 2;
					game->players[playerShoot.playerID]->weapon->shoot(playerShoot, game->players[playerShoot.playerID]);
				}
			}
			break;
		}
	case NET_CLSV_SVCL_PLAYER_PROJECTILE:
		{
			net_clsv_svcl_player_projectile playerProjectile;
			memcpy(&playerProjectile, buffer, sizeof(net_clsv_svcl_player_projectile));
			bool itsMine = false;
			if (game->players[playerProjectile.playerID])
			{
				if (game->thisPlayer)
				{ 
					if (playerProjectile.playerID == game->thisPlayer->playerID)
					{
						itsMine = true;
						if(playerProjectile.projectileType == PROJECTILE_ROCKET)
						{
							//We just launched our rocket ship!
							game->thisPlayer->rocketInAir = true;
						}
					}
				}
				if (!itsMine && playerProjectile.projectileType != PROJECTILE_LIFE_PACK && playerProjectile.projectileType != PROJECTILE_DROPED_WEAPON) 
				{
					game->players[playerProjectile.playerID]->firedShowDelay = 2;
					net_svcl_player_shoot playerShoot;
					playerShoot.playerID = playerProjectile.playerID;
					playerShoot.nuzzleID = playerProjectile.nuzzleID;
					playerShoot.weaponID = playerProjectile.weaponID;
					playerShoot.p1[0] = (short)(playerProjectile.position[0] * 100);
					playerShoot.p1[1] = (short)(playerProjectile.position[1] * 100);
					playerShoot.p1[2] = (short)(playerProjectile.position[2] * 100);
					if (game->players[playerShoot.playerID]->weapon->weaponID == playerShoot.weaponID)
					{
						game->players[playerShoot.playerID]->weapon->shoot(playerShoot, game->players[playerShoot.playerID]);
					}
					else
					{
						if (playerShoot.weaponID != -1)
						{
							// on va juste emettre du son debords
							// On entends ça
							if (playerShoot.weaponID == -2)
							{
								// Play flame sound
								playerShoot.weaponID = WEAPON_COCKTAIL_MOLOTOV;
							}
							else
							{
								CVector3f p1;
								p1[0] = (float)playerShoot.p1[0] / 100.0f;
								p1[1] = (float)playerShoot.p1[1] / 100.0f;
								p1[2] = (float)playerShoot.p1[2] / 100.0f;
								dksPlay3DSound(gameVar.weapons[playerShoot.weaponID]->sfx_sound, -1, 5, p1, 255);
							}
						}
						else
						{
							playerShoot.weaponID = 0;
						}
					}
				}
			//	playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
				game->spawnProjectile(playerProjectile, false);
			}
			break;
		}
	case NET_SVCL_DELETE_PROJECTILE:
		{
			// Ça ça ne devrait pas fucker, car on envoit dans l'ordre tout le temps (viva el TCP)
			net_svcl_delete_projectile deleteProjectile;
			memcpy(&deleteProjectile, buffer, sizeof(net_svcl_delete_projectile));
			// On check que le projectile existe au moins, sinon peut etre que le player n'est pas encore actif
	/*		if ((int)game->projectiles.size() > deleteProjectile.projectileID)
			{
				game->projectiles.erase(game->projectiles.begin() + (int)deleteProjectile.projectileID);
			}*/

			for (int i=0;i<(int)game->projectiles.size();++i)
			{
				Projectile * projectile = game->projectiles[i];
				if (game->projectiles[i]->uniqueID == deleteProjectile.projectileID)
				{
					delete game->projectiles[i];
					game->projectiles.erase(game->projectiles.begin() + i);
					break;
				}
			}

			break;
		}
	case NET_SVCL_FLAME_STICK_TO_PLAYER:
		{
			// Ça ça ne devrait pas fucker, car on envoit dans l'ordre tout le temps (viva el TCP)
			net_svcl_flame_stick_to_player flameStickToPlayer;
			memcpy(&flameStickToPlayer, buffer, sizeof(net_svcl_flame_stick_to_player));
			// On check que le projectile existe au moins, sinon peut etre que le player n'est pas encore actif
			if ((int)game->projectiles.size() > flameStickToPlayer.projectileID)
			{
				game->projectiles[flameStickToPlayer.projectileID]->stickToPlayer = flameStickToPlayer.playerID;

				if (game->projectiles[flameStickToPlayer.projectileID]->stickToPlayer == -1)
				{
					game->projectiles[flameStickToPlayer.projectileID]->movementLock = false;
				}
				else
				{
					game->projectiles[flameStickToPlayer.projectileID]->movementLock = true;
				}
			}
			break;
		}
	case NET_SVCL_EXPLOSION:
		{
			net_svcl_explosion explosion;
			memcpy(&explosion, buffer, sizeof(net_svcl_explosion));
			if(explosion.playerID == game->thisPlayer->playerID)
			{
				game->thisPlayer->rocketInAir = false;
				if(game->thisPlayer->meleeWeapon->weaponID == WEAPON_NUCLEAR && 
					game->thisPlayer->minibot && explosion.radius >= 4.0f)
				{
					dksPlay3DSound(dksCreateSoundFromFile("main/sounds/BaboCreve3.wav", false), -1, 5, game->thisPlayer->minibot->currentCF.position,64);
					game->spawnBloodMinibot(game->thisPlayer->minibot->currentCF.position, .5f);
					delete game->thisPlayer->minibot;
					game->thisPlayer->minibot = 0;
				}
			}
			game->spawnExplosion(CVector3f(explosion.position), CVector3f(explosion.normal), explosion.radius);
			break;
		}
	case NET_SVCL_PLAYER_HIT:
		{
			net_svcl_player_hit playerHit;
			memcpy(&playerHit, buffer, sizeof(net_svcl_player_hit));
			if (game->players[playerHit.playerID] && game->players[playerHit.fromID])
			{
				if (game->thisPlayer)
				{
					if (playerHit.playerID == game->thisPlayer->playerID)
					{
						// On recul avec la vel donné
						CVector3f vel;
						vel[0] = (float)playerHit.vel[0] / 10.0f;
						vel[1] = (float)playerHit.vel[1] / 10.0f;
						vel[2] = (float)playerHit.vel[2] / 10.0f;
						game->thisPlayer->currentCF.vel += vel;
						// moved here from GameSpawn.cpp
						if((playerHit.weaponID == WEAPON_BAZOOKA) ||
							(playerHit.weaponID == WEAPON_GRENADE) ||
							(playerHit.weaponID == WEAPON_NUCLEAR))
						{
							float realDamage = gameVar.weapons[playerHit.weaponID]->damage;
							float viewShakeAmount = 2 - (playerHit.damage / realDamage);
							game->viewShake += viewShakeAmount;
						}
					}
				}
				game->players[playerHit.playerID]->hit(gameVar.weapons[playerHit.weaponID], game->players[playerHit.fromID], playerHit.damage);
			}
			break;
		}
	case NET_SVCL_AUTOBALANCE:
		{
			autoBalanceTimer = (float)gameVar.sv_autoBalanceTime;
			break;
		}
	case NET_SVCL_PLAY_SOUND:
		{
			net_svcl_play_sound playSound;
			memcpy(&playSound, buffer, sizeof(net_svcl_play_sound));
			CVector3f position;
			position[0] = (float)playSound.position[0];
			position[1] = (float)playSound.position[1];
			position[2] = (float)playSound.position[2];
			switch (playSound.soundID)
			{
			case SOUND_GRENADE_REBOUND:
				dksPlay3DSound(gameVar.sfx_grenadeRebond, -1, playSound.range, position, playSound.volume);
				break;
			case SOUND_MOLOTOV:
				dksPlay3DSound(gameVar.sfx_cocktailMolotov, -1, playSound.range, position, playSound.volume);
				break;
			case SOUND_OVERHEAT:
				dksPlay3DSound(gameVar.sfx_overHeat, -1, playSound.range, position, playSound.volume);
				break;
			case SOUND_PHOTON_START:
				dksPlay3DSound(gameVar.sfx_photonStart, -1, playSound.range, position, playSound.volume);
				break;
			}
			break;
		}
	case NET_SVCL_CONSOLE:
		{
			//--- Pas plus compliqué que ça !
			console->add(buffer);
			break;
		}
	case NET_SVCL_SYNCHRONIZE_TIMER:
		{
			net_svcl_synchronize_timer synchronizeTimer;
			memcpy(&synchronizeTimer, buffer, sizeof(net_svcl_synchronize_timer));
			if (synchronizeTimer.frameID > serverFrameID)
			{
				serverFrameID = synchronizeTimer.frameID;
				game->gameTimeLeft = synchronizeTimer.gameTimeLeft;
				game->roundTimeLeft = synchronizeTimer.roundTimeLeft;
			}
			break;
		}
	case NET_SVCL_CHANGE_FLAG_STATE:
		{
			net_svcl_change_flag_state flagState;
			memcpy(&flagState, buffer, sizeof(net_svcl_change_flag_state));
			if (game->map)
			{
				if (game->map->flagState[flagState.flagID] != flagState.newFlagState)
				{
					if (game->map->flagState[flagState.flagID] == -1 &&
						flagState.newFlagState == -2 &&
						game->players[flagState.playerID])
					{
						// Ce joueur a sauvé le flag !!
						game->players[flagState.playerID]->returns++;

						if (game->thisPlayer)
						{
							if (game->thisPlayer->teamID == game->players[flagState.playerID]->teamID)
							{
								dksPlaySound(game->sfx_return, -1, 255);
							}
						}

						if (flagState.flagID == 0)
						{
							//console->add(CString("\x3> \x1%s\x8 returned the blue flag", textColorLess(game->players[flagState.playerID]->name).s));
							//eventMessages.push_back(CString(gameVar.lang_returnBlueFlag.s, textColorLess(game->players[flagState.playerID]->name).s));
							CString message("\x1%s\x8 returned the blue flag", game->players[flagState.playerID]->name.s);
							eventMessages.push_back(message);
							console->add(CString("\x3> %s", message.s));
						}
						else
						{
							//console->add(CString("\x3> \x4%s\x8 returned the red flag", textColorLess(game->players[flagState.playerID]->name).s));
							//eventMessages.push_back(CString(gameVar.lang_returnRedFlag.s, textColorLess(game->players[flagState.playerID]->name).s));
							CString message("\x4%s\x8 returned the red flag", game->players[flagState.playerID]->name.s);
							eventMessages.push_back(message);
							console->add(CString("\x3> %s", message.s));
						}
					}
					else if ((game->map->flagState[flagState.flagID] == -1 ||
						game->map->flagState[flagState.flagID] == -2) &&
						flagState.newFlagState >= 0 &&
						game->players[flagState.playerID])
					{
						game->players[flagState.playerID]->flagAttempts++;

						if (game->thisPlayer)
						{
							if (game->thisPlayer->teamID == game->players[flagState.playerID]->teamID)
							{
								dksPlaySound(game->sfx_fcapture, -1, 255);
							}
							else
							{
								dksPlaySound(game->sfx_ecapture, -1, 255);
							}
						}

						if (flagState.flagID == 0)
						{
							//console->add(CString("\x3> \x4%s\x8 took the blue flag", textColorLess(game->players[flagState.playerID]->name).s));
							//eventMessages.push_back(CString(gameVar.lang_tookRedFlag.s, textColorLess(game->players[flagState.playerID]->name).s));
							CString message("\x4%s\x8 took the blue flag", game->players[flagState.playerID]->name.s);
							eventMessages.push_back(message);
							console->add(CString("\x3> %s", message.s));
						}
						else
						{
							//console->add(CString("\x3> \x1%s\x8 took the red flag", textColorLess(game->players[flagState.playerID]->name).s));
							//eventMessages.push_back(CString(gameVar.lang_tookRedFlag.s, textColorLess(game->players[flagState.playerID]->name).s));
							CString message("\x1%s\x8 took the red flag", game->players[flagState.playerID]->name.s);
							eventMessages.push_back(message);
							console->add(CString("\x3> %s", message.s));
						}
					}
					else if (flagState.newFlagState == -2)
					{
						if (game->thisPlayer)
						{
							if (/*game->thisPlayer->teamID*/game->players[flagState.playerID]->teamID == PLAYER_TEAM_RED/*game->players[flagState.playerID]->teamID*/)
							{
								dksPlaySound(game->sfx_win, -1, 255);
							}
							else
							{
								dksPlaySound(game->sfx_loose, -1, 255);
							}
						}

						if (flagState.flagID == 0)
						{
							CString message("\x04%s \x08scores for the Red team!", game->players[flagState.playerID]->name.s);
							//console->add(CString("\x3> \x4Red team scores!"));
							//eventMessages.push_back(gameVar.lang_redScore);
							eventMessages.push_back(message);
							console->add(CString("\x03> %s", message.s));
							game->redWin++;
							game->redScore++;
						}
						else
						{
							CString message("\x01%s \x08scores for the Blue team!", game->players[flagState.playerID]->name.s);
							//console->add(CString("\x3>\x1 Blue team scores!"));
							//eventMessages.push_back(gameVar.lang_blueScore);
							eventMessages.push_back(message);
							console->add(CString("\x03> %s", message.s));
							game->blueWin++;
							game->blueScore++;
						}
						game->players[flagState.playerID]->score++;
					}

					// On emet le bon avertissement
					game->map->flagState[flagState.flagID] = flagState.newFlagState;
				}
			}
			break;
		}
	case NET_SVCL_DROP_FLAG:
		{
			net_svcl_drop_flag dropFlag;
			memcpy(&dropFlag, buffer, sizeof(net_svcl_drop_flag));
			if (game->map)
			{
				game->map->flagState[dropFlag.flagID] = -1;
				game->map->flagPos[dropFlag.flagID].set(dropFlag.position);
				game->map->flagAngle[dropFlag.flagID] = 0;
			}
			break;
		}
	case NET_SVCL_FLAG_ENUM:
		{
			net_svcl_flag_enum flagEnum;
			memcpy(&flagEnum, buffer, sizeof(net_svcl_flag_enum));
			if (game->map)
			{
				game->map->flagState[0] = flagEnum.flagState[0];
				game->map->flagState[1] = flagEnum.flagState[1];
				game->map->flagPos[0].set(flagEnum.positionBlue);
				game->map->flagPos[1].set(flagEnum.positionRed);
			}
			break;
		}
	case NET_SVCL_GAME_STATE:
		{
			net_svcl_round_state roundState;
			memcpy(&roundState, buffer, sizeof(net_svcl_round_state));

			// Do not load round state if still downloading
			if(isDownloadingMap)
				break;

			game->roundState = roundState.newState;

			if (roundState.reInit)
			{
				// Ouch, on fout toute à 0 (score, etc)
			}

			// On switch sur ça et on emet le son appropriée
			switch (game->roundState)
			{
			case GAME_PLAYING: break;
			case GAME_BLUE_WIN:
				showMenu = false;
				break;
			case GAME_RED_WIN:
				showMenu = false;
				break;
			case GAME_DRAW:
				showMenu = false;
				break;
			}
			break;
		}
	case NET_SVCL_CHANGE_GAME_TYPE:
		{
			net_svcl_change_game_type changeGameType;
			memcpy(&changeGameType, buffer, sizeof(net_svcl_change_game_type));
			game->resetGameType(changeGameType.newGameType);
			break;
		}
	case NET_SVCL_MAP_CHANGE:
		{
			net_svcl_map_change mapChange;
			memcpy(&mapChange, buffer, sizeof(net_svcl_map_change));
			game->mapName = mapChange.mapName;
			game->gameType = mapChange.gameType;

			if(isServer) 
			{
				game->createMap();
				if (!game->map)
				{
					needToShutDown = true;
				}
			}
			else
			{
				// Reset map buffer to start
				game->mapBuffer.reset();

				// Send map request
				isDownloadingMap = true;
				net_clsv_map_request request;
				strcpy(request.mapName, mapChange.mapName);
				request.uniqueClientID = uniqueClientID;
				bb_clientSend(uniqueClientID,(char*)&request,sizeof(net_clsv_map_request),NET_CLSV_MAP_REQUEST);
			}
			break;
		}
	case NET_SVCL_PICKUP_ITEM:
		{
			net_svcl_pickup_item pickupItem;
			memcpy(&pickupItem, buffer, sizeof(net_svcl_pickup_item));
			if (game->players[pickupItem.playerID])
			{
				switch (pickupItem.itemType)
				{
				case ITEM_LIFE_PACK:
					game->players[pickupItem.playerID]->life += .5f;
					if (game->players[pickupItem.playerID]->life > 1) game->players[pickupItem.playerID]->life = 1;
					if (game->players[pickupItem.playerID] == game->thisPlayer)
					{
						dksPlaySound(gameVar.sfx_lifePack, -1, 255);
					}
					break;
				case ITEM_WEAPON:
					game->players[pickupItem.playerID]->switchWeapon(pickupItem.itemFlag);
					if (game->players[pickupItem.playerID] == game->thisPlayer)
					{
						dksPlaySound(gameVar.sfx_equip, -1, 255);
					}
					break;
				case ITEM_GRENADE:
					game->players[pickupItem.playerID]->nbGrenadeLeft++;
					if (game->players[pickupItem.playerID]->nbGrenadeLeft > 3) game->players[pickupItem.playerID]->nbGrenadeLeft = 3;
					if (game->players[pickupItem.playerID] == game->thisPlayer)
					{
						dksPlaySound(gameVar.sfx_equip, -1, 255);
					}
					break;
				}
			}
			break;
		}
	case NET_SVCL_MAP_LIST:
		{
			net_svcl_map_list mapl;
			memcpy(&mapl, buffer, sizeof(net_svcl_map_list));
			console->add(mapl.mapName);
			break;
		}
	case NET_CLSV_SVCL_PLAYER_UPDATE_SKIN:
		{
			net_clsv_svcl_player_update_skin updateSkin;
			memcpy(&updateSkin, buffer, sizeof(net_clsv_svcl_player_update_skin));

			if (game->players[updateSkin.playerID] && game->players[updateSkin.playerID] != game->thisPlayer)
			{
				updateSkin.skin[6] = '\0';
				game->players[updateSkin.playerID]->skin = updateSkin.skin;
				game->players[updateSkin.playerID]->blueDecal.set(
					((float)updateSkin.blueDecal[0]) / 255.0f,
					((float)updateSkin.blueDecal[1]) / 255.0f,
					((float)updateSkin.blueDecal[2]) / 255.0f);
				game->players[updateSkin.playerID]->greenDecal.set(
					((float)updateSkin.greenDecal[0]) / 255.0f,
					((float)updateSkin.greenDecal[1]) / 255.0f,
					((float)updateSkin.greenDecal[2]) / 255.0f);
				game->players[updateSkin.playerID]->redDecal.set(
					((float)updateSkin.redDecal[0]) / 255.0f,
					((float)updateSkin.redDecal[1]) / 255.0f,
					((float)updateSkin.redDecal[2]) / 255.0f);
				game->players[updateSkin.playerID]->updateSkin();
			}
		
			break;
		}

	case NET_SVCL_BAD_CHECKSUM_ENTITY:
		{
			net_svcl_bad_checksum_entity bce;
			memcpy(&bce, buffer, sizeof(net_svcl_bad_checksum_entity));
			console->add(CString("%i) %s, IP: %s", bce.id, bce.name, bce.playerIP));
		}
		break;

	case NET_SVCL_BAD_CHECKSUM_INFO:
		{
			net_svcl_bad_checksum_info bci;
			memcpy(&bci, buffer, sizeof(net_svcl_bad_checksum_info));
			console->add(CString(">> %i", bci.number));
		}
		break;

	}
}
#endif


