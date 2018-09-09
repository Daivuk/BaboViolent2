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

#include "Game.h"
#include "netPacket.h"
#include "Console.h"
#include "GameVar.h"
#ifndef DEDICATED_SERVER
#include "Client.h"
#endif
#include "Server.h"
#include "Scene.h"
#include <time.h>
#include <algorithm>

extern Scene * scene;



//
// Constructeur
//
Game::Game(CString pMapName)
#ifndef DEDICATED_SERVER
: mapBuffer(8192)
#endif
{
//	uniqueProjectileID = 0;
	mapName = pMapName;
#ifndef DEDICATED_SERVER
	font = dkfCreateFont("main/fonts/babo.tga");
	tex_miniMapAllied = dktCreateTextureFromFile("main/textures/MiniMapAllied.tga", DKT_FILTER_LINEAR);
	tex_miniMapEnemy = dktCreateTextureFromFile("main/textures/MiniMapEnemy.tga", DKT_FILTER_LINEAR);;
	tex_baboShadow = dktCreateTextureFromFile("main/textures/BaboShadow.tga", DKT_FILTER_BILINEAR);
#endif

	players = new Player*[MAX_PLAYER];
	for (int i=0;i<MAX_PLAYER;++i) players[i] = 0;
#ifndef DEDICATED_SERVER
	// On ne cré pas notre player tout de suite, on attends confirmation du server
	thisPlayer = 0;

	// Pour afficher les stats multiplayers
	showStats = false;
#endif
	isServerGame = false;
	roundState = GAME_PLAYING;
	bombPlanted = false;
	bombTime = 0;

	map = 0;
#ifndef DEDICATED_SERVER
	dkpReset();
	dkpSetSorting(false);

	nextWriteFloorMark = 0;
	nextWriteDrip = 0;
	viewShake = 0;
	dotAnim = 0;
	mapBytesRecieved = 0;
#endif

	// Le type de jeu
	gameType = gameVar.sv_gameType;   
   spawnType = gameVar.sv_spawnType;
   subGameType = gameVar.sv_subGameType;

	blueScore = 0;
	redScore = 0;
	blueWin = 0;
	redWin = 0;
	gameTimeLeft = gameVar.sv_gameTimeLimit;
	roundTimeLeft = gameVar.sv_roundTimeLimit;

	teamApproveAll[PLAYER_TEAM_RED] = true;
	teamApproveAll[PLAYER_TEAM_BLUE] = true;

	// Ça ça va être utilise quand on va équilibrer les teams
	bluePing = 0;
	redPing = 0;
	ffaPing = 0;
	spectatorPing = 0;
#ifndef DEDICATED_SERVER
	// On load nos sons pour le mode CTF
	sfx_fcapture = dksCreateSoundFromFile("main/sounds/ftook.wav", false);
	sfx_ecapture = dksCreateSoundFromFile("main/sounds/etook.wav", false);
	sfx_return = dksCreateSoundFromFile("main/sounds/return.wav", false);
	sfx_win = dksCreateSoundFromFile("main/sounds/cheerRedTeam.wav", false);
	sfx_loose = dksCreateSoundFromFile("main/sounds/cheerBlueTeam.wav", false);
#endif

   UpdateProSettings();

}



//
// Destructeur
//
Game::~Game()
{
  int i;
	ZEVEN_SAFE_DELETE(map);
#ifndef DEDICATED_SERVER
	dkfDeleteFont(&font);
	dktDeleteTexture(&tex_baboShadow);
	dktDeleteTexture(&tex_miniMapAllied);
	dktDeleteTexture(&tex_miniMapEnemy);
#endif
	for (i=0;i<MAX_PLAYER;++i) ZEVEN_SAFE_DELETE(players[i]);
	ZEVEN_SAFE_DELETE_ARRAY(players);
	ZEVEN_DELETE_VECTOR(projectiles, i);
#ifndef DEDICATED_SERVER
	ZEVEN_DELETE_VECTOR(clientProjectiles, i);
	ZEVEN_DELETE_VECTOR(trails, i);
	ZEVEN_DELETE_VECTOR(douilles, i);
	dksDeleteSound(sfx_fcapture);
	dksDeleteSound(sfx_ecapture);
	dksDeleteSound(sfx_return);
	dksDeleteSound(sfx_win);
	dksDeleteSound(sfx_loose);
	ZEVEN_DELETE_VECTOR(nikeFlashes, i);
#endif
}





//
// Pour starter un new round
//
void Game::resetGameType(int pGameType)
{
	// Server ony
	gameType = pGameType;
   
   spawnType = gameVar.sv_spawnType;
   subGameType = gameVar.sv_subGameType;

	blueScore = 0;
	redScore = 0;
	blueWin = 0;
	redWin = 0;
	gameTimeLeft = gameVar.sv_gameTimeLimit;
	roundTimeLeft = gameVar.sv_roundTimeLimit;
	resetRound();

	// On dit aux autres d'en faire autant
	if (isServerGame)
	{
		net_svcl_change_game_type changeGameType;
		changeGameType.newGameType = pGameType;
		bb_serverSend((char*)&changeGameType, sizeof(net_svcl_change_game_type), NET_SVCL_CHANGE_GAME_TYPE, 0);
	}

	// On remet les score des players à 0
	for (int i=0;i<MAX_PLAYER;++i)
	{
		if (players[i]) 
		{
			players[i]->reinit();
		}
	}
   
}


void Game::UpdateProSettings()
{
// Adjust weapons depending on game var
	if (gameVar.sv_serverType == SERVER_TYPE_PRO)
	{
		gameVar.weapons[WEAPON_NUCLEAR]->fireDelay = 12;
		gameVar.weapons[WEAPON_SHIELD]->fireDelay = 2.5;
		gameVar.weapons[WEAPON_CHAIN_GUN]->reculVel = 1.0f;         
	}
	else
	{
		gameVar.weapons[WEAPON_NUCLEAR]->fireDelay = 12;
		gameVar.weapons[WEAPON_SHIELD]->fireDelay = 3.0;
		gameVar.weapons[WEAPON_CHAIN_GUN]->reculVel = 2.0f;
	}
}


//
// Pour starter un new round
//
void Game::resetRound()
{
	roundTimeLeft = gameVar.sv_roundTimeLimit;

	map->flagState[0] = -2;
	map->flagState[1] = -2;

	bombPlanted = false;
	bombTime = gameVar.sv_bombTime; // Seconds before kaboom

	// On clair les trails n Stuff
	int i;
	ZEVEN_DELETE_VECTOR(projectiles, i);
#ifndef DEDICATED_SERVER
	ZEVEN_DELETE_VECTOR(clientProjectiles, i);
	ZEVEN_DELETE_VECTOR(trails, i);
	ZEVEN_DELETE_VECTOR(douilles, i);
	ZEVEN_DELETE_VECTOR(nikeFlashes, i);
	for (i=0;i<MAX_FLOOR_MARK;floorMarks[i++].delay = 0);
	for (i=0;i<MAX_FLOOR_MARK;drips[i++].life = 0);

#endif

	// On respawn tout les players (le server va décider de tout ça)
	for (i=0;i<MAX_PLAYER;++i)
	{
		if (players[i]) 
		{
			players[i]->kill(true);
			players[i]->timeToSpawn = 0;
			players[i]->timePlayedCurGame = 0;
		}
	}
}



//
// Pour lui dire : ok, tu peux créer la map
//
void Game::createMap()
{
	ZEVEN_SAFE_DELETE(map);
	srand((unsigned int)time(0));//mapSeed); // Fuck le mapSeed, on l'utilise pus

	if(scene->server) {
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
	}

#ifdef DEDICATED_SERVER
	int font = 0;
#endif
	map = new Map(mapName, this, font);

	if (!map->isValid)
	{
		console->add("\x4> Invalid map");
		ZEVEN_SAFE_DELETE(map);
#ifndef DEDICATED_SERVER
		if (scene->client) 
		{
			scene->client->needToShutDown = true;
			scene->client->isRunning = false;
		}
#endif
		if (scene->server)
		{
			scene->server->needToShutDown = true;
			scene->server->isRunning = false;
		}
		return;
	}
	else
	{
#ifndef DEDICATED_SERVER
		if (thisPlayer) 
		{
			thisPlayer->map = map;
			dkpReset();
			if (gameVar.s_inGameMusic)
			{
			/*	if (rand()%2 == 0)
				{*/
					dksPlayMusic("main/sounds/Music.ogg", -1, 60);
			/*	}
				else
				{
					dksPlayMusic("main/sounds/InGame02.ogg", -1);
				}*/
			}
		}
#endif
	}
	dkcJumpToFrame(0);
}



//--- He oui, une fonction GLOBAL !!!!!!!!!!!!
int SelectToAvailableWeapon()
{
	if (gameVar.sv_enableSMG) return WEAPON_SMG;
	if (gameVar.sv_enableShotgun) return WEAPON_SHOTGUN;
	if (gameVar.sv_enableSniper) return WEAPON_SNIPER;
	if (gameVar.sv_enableDualMachineGun) return WEAPON_DUAL_MACHINE_GUN;
	if (gameVar.sv_enableChainGun) return WEAPON_CHAIN_GUN;
	if (gameVar.sv_enableBazooka) return WEAPON_BAZOOKA;
	if (gameVar.sv_enablePhotonRifle) return WEAPON_PHOTON_RIFLE;
	if (gameVar.sv_enableFlameThrower) return WEAPON_FLAME_THROWER;

	return WEAPON_SMG;
}



//--- He oui, une fonction GLOBAL !!!!!!!!!!!!
int SelectToAvailableMeleeWeapon()
{
	if (gameVar.sv_enableKnives)	return WEAPON_KNIVES;
	if (gameVar.sv_enableNuclear)	return WEAPON_NUCLEAR;
	if (gameVar.sv_enableShield)	return WEAPON_SHIELD;
#if defined(_PRO_)
	if (gameVar.sv_enableMinibot)	return WEAPON_MINIBOT;
#endif

	return WEAPON_KNIVES;
}



//
// Update
//
void Game::update(float delay)
{
    int i;
#ifndef DEDICATED_SERVER
	dotAnim += delay * 720;
	while (dotAnim >= 360) dotAnim -= 360;
#endif
	if (voting.votingInProgress)
	{
		if (voting.update(delay))
		{
			if (isServerGame)
			{
				//--- Voting complete, check the result
				//    In order to the vote to pass, more than
				//    50% of ALL the players should have voted YES.
				/*voting.nbActivePlayers = 0;
				for (int i=0;i<MAX_PLAYER;++i)
				{
					if ( (players[i]) && (players[i]->teamID != PLAYER_TEAM_AUTO_ASSIGN) &&
						(players[i]->teamID != PLAYER_TEAM_AUTO_ASSIGN) )
						++voting.nbActivePlayers;
				}*/

				net_svcl_vote_result voteResult;
				voteResult.passed = (voting.votingResults[0] > (int)voting.activePlayersID.size() / 2);
				bb_serverSend((char*)(&voteResult), sizeof(net_svcl_vote_result), NET_SVCL_VOTE_RESULT);
				if (voteResult.passed)
				{
					//--- Vote passed!!!
					console->sendCommand(voting.votingWhat);
				}
				else
				{
					//--- Vote failed...
				}
			}
		}
	}

	if (roundState == GAME_PLAYING)
	{
		// On update les players
		for (int i=0;i<MAX_PLAYER;++i) 
		{
			if (players[i])
			{
				players[i]->update(delay);

				if (players[i]->incShot > 0)
				{
					players[i]->incShot--;
					if (players[i]->incShot%3 == 0)
					{
						// On test premièrement si on touche un autre joueur!
						Player * hitPlayer = 0;
						CVector3f p3 = players[i]->p2;
						for (int j=0;j<MAX_PLAYER;j++)
						{
							if (players[j])
							{
								if (j != i)
								{
#ifdef _PRO_
									if (players[j]->status == PLAYER_STATUS_ALIVE && (players[j]->teamID != players[i]->teamID || gameType == GAME_TYPE_DM || gameType == GAME_TYPE_SND || gameVar.sv_friendlyFire || gameVar.sv_reflectedDamage))
#else
									if (players[j]->status == PLAYER_STATUS_ALIVE && (players[j]->teamID != players[i]->teamID || gameType == GAME_TYPE_DM || gameVar.sv_friendlyFire))
#endif
									{
										// Ray to sphere test
										if (segmentToSphere(players[i]->p1, p3, players[j]->currentCF.position, .35f))
										{
											hitPlayer = players[j];
											p3 = players[i]->p2; // Full length

											// On décrémente sa vie
											hitPlayer->hitSV(gameVar.weapons[WEAPON_PHOTON_RIFLE], players[i], gameVar.weapons[WEAPON_PHOTON_RIFLE]->damage / 2.0f);
										}
									}
								}
							}
						}
					}
				}

				//--- Update les guns
				if (players[i]->weapon)
				{
					if (players[i]->weapon->weaponID == WEAPON_SMG)
					{
						if (!gameVar.sv_enableSMG)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_SHOTGUN)
					{
						if (!gameVar.sv_enableShotgun)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_SNIPER)
					{
						if (!gameVar.sv_enableSniper/* || map->size[0] * map->size[1] <= 512*/)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_DUAL_MACHINE_GUN)
					{
						if (!gameVar.sv_enableDualMachineGun)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_CHAIN_GUN)
					{
						if (!gameVar.sv_enableChainGun)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_BAZOOKA)
					{
						if (!gameVar.sv_enableBazooka)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_PHOTON_RIFLE)
					{
						if (!gameVar.sv_enablePhotonRifle)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
					if (players[i]->weapon->weaponID == WEAPON_FLAME_THROWER)
					{
						if (!gameVar.sv_enableFlameThrower)
						{
							players[i]->switchWeapon(SelectToAvailableWeapon(), true);
						}
					}
				}

				//--- Update les melee
				if (players[i]->meleeWeapon)
				{
					if (!gameVar.sv_enableSecondary)
					{
						players[i]->switchMeleeWeapon(SelectToAvailableMeleeWeapon(), true);
					}
					else
					{
						if (players[i]->meleeWeapon->weaponID == WEAPON_KNIVES)
						{
							if (!gameVar.sv_enableKnives)
							{
								players[i]->switchMeleeWeapon(SelectToAvailableMeleeWeapon(), true);
							}
						}
						if (players[i]->meleeWeapon->weaponID == WEAPON_NUCLEAR)
						{
							if (!gameVar.sv_enableNuclear)
							{
								players[i]->switchMeleeWeapon(SelectToAvailableMeleeWeapon(), true);
							}
						}
						if (players[i]->meleeWeapon->weaponID == WEAPON_SHIELD)
						{
							if (!gameVar.sv_enableShield)
							{
								players[i]->switchMeleeWeapon(SelectToAvailableMeleeWeapon(), true);
							}
						}
#ifdef _PRO_
						if (players[i]->meleeWeapon->weaponID == WEAPON_MINIBOT)
						{
							if (!gameVar.sv_enableMinibot)
							{
								players[i]->switchMeleeWeapon(SelectToAvailableMeleeWeapon(), true);
							}
						}
#endif
					}
				}
			}
		}
	}

#ifdef _PRO_
	//--- Perform bot collisions with walls
	if (isServerGame)
	{
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (players[i])
			{
				if (players[i]->status == PLAYER_STATUS_ALIVE)
				{
					if (players[i]->minibot)
					{
						if (map) map->performCollision(players[i]->minibot->lastCF, players[i]->minibot->currentCF, .15f);
						map->collisionClip(players[i]->minibot->currentCF, .15f);
					}
				}
			}
		}
	}
#endif

	// Si on tiens tab, on montre les stats
#ifndef DEDICATED_SERVER
	if (!console->isActive() && dkiGetState(gameVar.k_showScore) || roundState != GAME_PLAYING)
	{
		showStats = true;
	}
	else
	{
		showStats = false;
	}

	for (i=0;i<(int)nikeFlashes.size();++i)
	{
		nikeFlashes[i]->update(delay);
		if (nikeFlashes[i]->life <= 0)	 
		{
			delete nikeFlashes[i];
			nikeFlashes.erase(nikeFlashes.begin() + i);
			--i;
		}
	}

	if (thisPlayer && roundState == GAME_PLAYING)
	{
		if (thisPlayer->teamID == PLAYER_TEAM_SPECTATOR && !console->isActive() && !writting && !showMenu && !(menuManager.root && menuManager.root->visible))
		{
			// On est spectateur, alors on peut se déplacer comme on veut dans la map
			// Pour l'instant les flèches (a,s,w,d, pomal temp)
			if (dkiGetState(gameVar.k_moveRight))
			{
				map->camLookAt[0] += 10 * delay;
			}
			if (dkiGetState(gameVar.k_moveLeft))
			{
				map->camLookAt[0] -= 10 * delay;
			}
			if (dkiGetState(gameVar.k_moveUp))
			{
				map->camLookAt[1] += 10 * delay;
			}
			if (dkiGetState(gameVar.k_moveDown))
			{
				map->camLookAt[1] -= 10 * delay;
			}
		}

		// On performe les collisions sur notre joueur
		if (thisPlayer->status == PLAYER_STATUS_ALIVE) 
		{
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (players[i] && players[i] != thisPlayer)
				{
					if (players[i]->status == PLAYER_STATUS_ALIVE && players[i]->timeAlive > 3.0f && thisPlayer->timeAlive > 3.0f) // player msut have been on the field for more than 3 second before we check collisions with him
					{
						float disSq = distanceSquared(thisPlayer->currentCF.position, players[i]->currentCF.position);
						if (disSq <= .5f*.5f)
						{
							CVector3f dis = players[i]->currentCF.position - thisPlayer->currentCF.position;
							normalize(dis);
							thisPlayer->currentCF.position = players[i]->currentCF.position - dis * .51f;
							thisPlayer->currentCF.vel = -thisPlayer->currentCF.vel * BOUNCE_FACTOR;
							if (map) map->performCollision(thisPlayer->lastCF, thisPlayer->currentCF, .25f);
							map->collisionClip(thisPlayer->currentCF, .25f);
							thisPlayer->lastCF.position = thisPlayer->currentCF.position;
						}
					}
				}
			}

			if (map) map->performCollision(thisPlayer->lastCF, thisPlayer->currentCF, .25f);

			// Performing a final clip cibole de caliss
			map->collisionClip(thisPlayer->currentCF, .25f);

			//--- Est-ce qu'on est stuck dans un wall??? Oui? on respawn request
			int x = (int)thisPlayer->currentCF.position[0];
			int y = (int)thisPlayer->currentCF.position[1];
			if ((!map->dko_map && !map->cells[(y)*map->size[0]+(x)].passable) ||
				(map->dko_map && 
				(x < 0 || x > map->size[0] || y < 0 || y > map->size[1])))
			{
				// Respawn request!
				if (!thisPlayer->spawnRequested)
				{
					// Ici on le call juste une fois, isshh sinon ça sera pas trop bon...
					// On request to spawn
					thisPlayer->spawnRequested = true;
					net_clsv_spawn_request spawnRequest;
					spawnRequest.playerID = thisPlayer->playerID;
					spawnRequest.weaponID = thisPlayer->nextSpawnWeapon;
					spawnRequest.meleeID = thisPlayer->nextMeleeWeapon;
					memcpy(spawnRequest.skin, thisPlayer->skin.s, (thisPlayer->skin.len() <= 6)?thisPlayer->skin.len()+1:7);
					spawnRequest.blueDecal[0] = (unsigned char)(thisPlayer->blueDecal[0] * 255.0f);
					spawnRequest.blueDecal[1] = (unsigned char)(thisPlayer->blueDecal[1] * 255.0f);
					spawnRequest.blueDecal[2] = (unsigned char)(thisPlayer->blueDecal[2] * 255.0f);
					spawnRequest.greenDecal[0] = (unsigned char)(thisPlayer->greenDecal[0] * 255.0f);
					spawnRequest.greenDecal[1] = (unsigned char)(thisPlayer->greenDecal[1] * 255.0f);
					spawnRequest.greenDecal[2] = (unsigned char)(thisPlayer->greenDecal[2] * 255.0f);
					spawnRequest.redDecal[0] = (unsigned char)(thisPlayer->redDecal[0] * 255.0f);
					spawnRequest.redDecal[1] = (unsigned char)(thisPlayer->redDecal[1] * 255.0f);
					spawnRequest.redDecal[2] = (unsigned char)(thisPlayer->redDecal[2] * 255.0f);
					bb_clientSend(scene->client->uniqueClientID, (char*)&spawnRequest, sizeof(net_clsv_spawn_request), NET_CLSV_SPAWN_REQUEST);
				}
			}
		}
	}

	// On update la map
	if (map && thisPlayer) 
	{
		map->update(delay, thisPlayer);

		//--- Est-ce qu'il pleut?
		if (map->weather == WEATHER_RAIN)
		{
			for (int i=0;i<5;++i)
			{
				//--- Spawn da rain!
				int idrip = getNextDrip();
				drips[idrip].life = 1;
				drips[idrip].position = rand(map->camPos + CVector3f(-5,-5,0), map->camPos + CVector3f(5,5,0));
				drips[idrip].position[2] = 0;
				drips[idrip].size = .15f;
				drips[idrip].fadeSpeed = 2;
			}

			//--- Spawn des drip sous les players
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (players[i])
				{
					if (players[i]->status == PLAYER_STATUS_ALIVE)
					{
						if (map->cells[(int)(players[i]->currentCF.position[1] - .5f) * map->size[0] + (int)(players[i]->currentCF.position[0] - .5f)].splater[0] > .5f)
						{
							if (players[i]->currentCF.vel.length() >= 2.25f)
							{
								//--- Spawn da rain!
								int idrip = getNextDrip();
								drips[idrip].life = .5f;
								drips[idrip].position = players[i]->currentCF.position;
								drips[idrip].position[2] = 0;
								drips[idrip].size = .3f;
								drips[idrip].fadeSpeed = 1;
							}
						}
					}
				}
			}
		}

		//--- Si on roule dans la lave, on spawn de la fumé :D
		if (map->theme == THEME_LAVA)
		{
			//--- Spawn des drip sous les players
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (players[i])
				{
					if (players[i]->status == PLAYER_STATUS_ALIVE && rand()%50 == 5)
					{
						if (map->cells[(int)(players[i]->currentCF.position[1] - .5f) * map->size[0] + (int)(players[i]->currentCF.position[0] - .5f)].splater[0] > .5f)
						{
							//--- Spawn da smoke psssiiii
							for (int j=0;j<4;++j)
							{
								dkpCreateParticle(	players[i]->currentCF.position.s,//float *position,
													CVector3f(0,0,(float)j* .25f).s,//float *vel,
													CVector4f(.7f,.7f,.7f,1).s,//float *startColor,
													CVector4f(.7f,.7f,.7f,0).s,//float *endColor,
													.25f,//float startSize,
													.5f,//float endSize,
													2,//float duration,
													0,//float gravityInfluence,
													0,//float airResistanceInfluence,
													30,//float rotationSpeed,
													gameVar.tex_smoke1,//unsigned int texture,
													DKP_SRC_ALPHA,//unsigned int srcBlend,
													DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
													0);//int transitionFunc);
							}

							dksPlay3DSound(gameVar.sfx_lavaSteam, -1, 5, players[i]->currentCF.position,125);
						}
					}
				}
			}
		}

		// La view qui shake
		if (viewShake > 0)
		{
			if (viewShake > 2.5f) viewShake = 2.5f;

			CVector3f dir(1,0,0);
			dir = rotateAboutAxis(dir, rand(0.0f, 360.0f), CVector3f(0,0,1));
			dir *= viewShake * .10f;

			map->camPos += dir;
			viewShake -= delay*.75f;
			if (viewShake < 0) viewShake = 0;
		}


		//-- We check for all enable guns
		scene->client->btn_guns[WEAPON_SMG]->enable = gameVar.sv_enableSMG;
		scene->client->btn_guns[WEAPON_SHOTGUN]->enable = gameVar.sv_enableShotgun;
		scene->client->btn_guns[WEAPON_SNIPER]->enable = gameVar.sv_enableSniper;
		scene->client->btn_guns[WEAPON_DUAL_MACHINE_GUN]->enable = gameVar.sv_enableDualMachineGun;
		scene->client->btn_guns[WEAPON_CHAIN_GUN]->enable = gameVar.sv_enableChainGun;
		scene->client->btn_guns[WEAPON_BAZOOKA]->enable = gameVar.sv_enableBazooka;
		scene->client->btn_guns[WEAPON_PHOTON_RIFLE]->enable = gameVar.sv_enablePhotonRifle;
		scene->client->btn_guns[WEAPON_FLAME_THROWER]->enable = gameVar.sv_enableFlameThrower;
		
		if(gameVar.sv_enableSecondary)
		{
			scene->client->btn_meleeguns[WEAPON_KNIVES-WEAPON_KNIVES]->enable = gameVar.sv_enableKnives;
			scene->client->btn_meleeguns[WEAPON_NUCLEAR-WEAPON_KNIVES]->enable = gameVar.sv_enableNuclear;
			scene->client->btn_meleeguns[WEAPON_SHIELD-WEAPON_KNIVES]->enable = gameVar.sv_enableShield;
#if defined(_PRO_)
			scene->client->btn_meleeguns[WEAPON_MINIBOT-WEAPON_KNIVES]->enable = gameVar.sv_enableMinibot;
#endif
		}
		else
		{
			scene->client->btn_meleeguns[WEAPON_KNIVES-WEAPON_KNIVES]->enable = false;
			scene->client->btn_meleeguns[WEAPON_NUCLEAR-WEAPON_KNIVES]->enable = false;
			scene->client->btn_meleeguns[WEAPON_SHIELD-WEAPON_KNIVES]->enable = false;
#if defined(_PRO_)
			scene->client->btn_meleeguns[WEAPON_MINIBOT-WEAPON_KNIVES]->enable = false;
#endif
		}

	}

	// On update les trails
	for (int i=0;i<(int)trails.size();++i)
	{
		trails[i]->update(delay);
		if (trails[i]->delay >= 1)
		{
			delete trails[i];
			trails.erase(trails.begin()+i);
			i--;
		}
	}

	// On update les floor mark
	for (int i=0;i<MAX_FLOOR_MARK;++i)
	{
		if (floorMarks[i].delay > 0)
		{
			floorMarks[i].update(delay);
		}
		if (drips[i].life > 0)
		{
			drips[i].update(delay);
		}
	}
#endif

	// On update les projectiles
	for (int i=0;i<(int)projectiles.size();++i)
	{
		Projectile * projectile = projectiles[i];
		projectile->update(delay, map);
		projectile->projectileID = (short)i; // On l'update toujours
		if ( projectile->needToBeDeleted )
		{
			if( !projectile->reallyNeedToBeDeleted )
			{
				projectile->reallyNeedToBeDeleted = true;
				continue;
			}
			projectiles.erase(projectiles.begin()+i);
			net_svcl_delete_projectile deleteProjectile;
			deleteProjectile.projectileID = projectile->uniqueID;
			bb_serverSend((char*)&deleteProjectile,sizeof(net_svcl_delete_projectile),NET_SVCL_DELETE_PROJECTILE,0);
			i--;
			delete projectile;
		}

	}

#ifndef DEDICATED_SERVER

	// On update les projectiles client
	for (int i=0;i<(int)clientProjectiles.size();++i)
	{
		Projectile * projectile = clientProjectiles[i];
		projectile->update(delay, map);
		projectile->projectileID = (short)i; // On l'update toujours
		if (projectile->needToBeDeleted)
		{
			clientProjectiles.erase(projectiles.begin()+i);
			i--;
			delete projectile;
		}
	}

	// On update les douilles
	for (int i=0;i<(int)douilles.size();++i)
	{
		Douille * douille = douilles[i];
		douille->update(delay, map);
		if (douille->delay<=0)
		{
			douilles.erase(douilles.begin()+i);
			i--;
			delete douille;
		}
	}
#endif	

	// Update les particules
//	gameVar.ro_nbParticle = dkpUpdate(delay);
}



//
// pour donner un team à un player
//
int Game::assignPlayerTeam(int playerID, char teamRequested, Client * client)
{
	if (players[playerID])
	{
#ifdef _PRO_
   players[playerID]->spawnSlot = -1;
#endif

		if (teamRequested == PLAYER_TEAM_AUTO_ASSIGN)
		{
			// On va équilibrer les team :)
			int blueCount = 0;
			int redCount = 0;
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (players[i] && i != playerID)
				{
					switch (players[i]->teamID)
					{
					case PLAYER_TEAM_BLUE:
						blueCount++;
						break;
					case PLAYER_TEAM_RED:
						redCount++;
						break;
					}
				}
			}

			// On le met dans le team qui a le moins de joueur
			if (redCount > blueCount)
			{
				teamRequested = PLAYER_TEAM_BLUE;
			}
			else if (redCount < blueCount)
			{
				teamRequested = PLAYER_TEAM_RED;
			}
			else 
			{
				// choose the losing team
				if(blueScore < redScore)
				{
					teamRequested = PLAYER_TEAM_BLUE;
				}
				else if(blueScore > redScore)
				{
					teamRequested = PLAYER_TEAM_RED;
				}
				else
				{
					// random is bad! but there is nothing else we could do...
					if (rand()%2 == 0) teamRequested = PLAYER_TEAM_BLUE;
					else teamRequested = PLAYER_TEAM_RED;
				}
			}
		}

		if (isApproved(players[playerID]->userID, teamRequested) == false)
			teamRequested = PLAYER_TEAM_SPECTATOR;

		if (players[playerID]->teamID != teamRequested)
		{
			// On le tue seuleument si on change de team
			players[playerID]->kill(true);
			players[playerID]->timeToSpawn = gameVar.sv_timeToSpawn;

			// On lui donne le nouveau team
			players[playerID]->teamID = teamRequested;

			// On print dans console
			switch (players[playerID]->teamID)
			{
			case PLAYER_TEAM_SPECTATOR:
				console->add(CString("%s\x8 goes spectator ID:%i",players[playerID]->name.s, playerID));
#ifndef DEDICATED_SERVER
				if (client) client->eventMessages.push_back(CString(gameVar.lang_goesSpectator.s,players[playerID]->name.s));
#endif
				break;
			case PLAYER_TEAM_BLUE:
				console->add(CString("%s\x1 joins blue team ID:%i",players[playerID]->name.s,playerID));
#ifndef DEDICATED_SERVER
				if (client) client->eventMessages.push_back(CString(gameVar.lang_joinBlueTeamP.s,players[playerID]->name.s));
#endif
				break;
			case PLAYER_TEAM_RED:
				console->add(CString("%s\x4 joins red team ID:%i",players[playerID]->name.s, playerID));
#ifndef DEDICATED_SERVER
				if (client) client->eventMessages.push_back(CString(gameVar.lang_joinRedTeamP.s,players[playerID]->name.s));
#endif
				break;
			}
		}

		// Just in case
		players[playerID]->teamID = teamRequested;

		return teamRequested;
	}
	else
	{
		return PLAYER_TEAM_SPECTATOR; // Pas suposer en arriver là!
	}
}

// let the player join selected team
bool Game::approvePlayer(int userID, char team)
{
	if (userID <= 0 || (team != PLAYER_TEAM_RED && team != PLAYER_TEAM_BLUE))
		return false;

	approvedPlayers[team].push_back(userID);
	return true;
}

void Game::rejectPlayer(int userID)
{
	for (int i = 0; i < (int)approvedPlayers.size(); i++)
	{
		std::vector<int>::iterator it = std::find(approvedPlayers[i].begin(),
			approvedPlayers[i].end(), userID);
		if (it != approvedPlayers[i].end())
		{
			approvedPlayers[i].erase(it);
			continue;
		}
	}
}

void Game::rejectPlayer(int userID, char team)
{
	if (team != PLAYER_TEAM_RED && team != PLAYER_TEAM_BLUE)
		return;
	std::vector<int>::iterator it = std::find(approvedPlayers[team].begin(),
		approvedPlayers[team].end(), userID);
	if (it != approvedPlayers[team].end())
		approvedPlayers[team].erase(it);
}

// approve all players to join selected team
void Game::approveAll(int team)
{
	if (team != PLAYER_TEAM_RED && team != PLAYER_TEAM_BLUE)
		return;
	approvedPlayers[team].clear();
	teamApproveAll[team] = true;
}

// approve all players to join
void Game::approveAll()
{
	ApprovedPlayers::iterator it = approvedPlayers.begin();
	for (; it != approvedPlayers.end(); it++)
	{
		it->second.clear();
		teamApproveAll[it->first] = true;
	}
}

bool Game::isApproved(int userID, int team)
{
	if (team == PLAYER_TEAM_SPECTATOR)
		return true;
	if (team != PLAYER_TEAM_RED && team != PLAYER_TEAM_BLUE)
		return false;

	std::vector<int>::iterator it = std::find(approvedPlayers[team].begin(),
		approvedPlayers[team].end(), userID);
	if (teamApproveAll[team] == true || it != approvedPlayers[team].end())
		return true;
	else
		return false;
}

void Game::rejectAllPlayers()
{
	approveAll();
	for (int i = 0; i < (int)teamApproveAll.size(); i++)
		teamApproveAll[i] = false;
}

#ifndef DEDICATED_SERVER
void Douille::update(float pDelay, Map * map)
{
	CVector3f lastPos = position;
	delay -= pDelay;
	if (vel.length() > .5f)
	{
		position += vel * pDelay;
		vel[2] -= 9.8f * pDelay;
		CVector3f p1 = lastPos;
		CVector3f p2 = position;
		CVector3f normal;
		if (map->rayTest(p1, p2, normal))
		{
			// On dit à tout le monde de jouer le son (pour l'instant juste server side)
			if (!soundPlayed) 
			{
				if (type == DOUILLE_TYPE_DOUILLE) dksPlay3DSound(gameVar.sfx_douille[rand()%3],-1,1,position,255);
				else if (type == DOUILLE_TYPE_GIB)
				{
					scene->client->game->spawnBlood(position, .1f);
				//	delay = 0;
				}
				soundPlayed = true;
			}
			position = p2 + normal*.1f;
			vel = reflect(vel, normal);
			vel *= .3f;
		}
	}
}

//
// Pour ajouter une trainer d'une shot
//
void Game::shoot(const CVector3f & position, const CVector3f & direction, float imp, float damage, Player * from, int projectileType)
{
	if (map)
	{
		CVector3f p2 = direction * 128; // Ça c'est le range, 128 c'est assez, grosseur max de map (c fucking big ça)
		if (projectileType == PROJECTILE_DIRECT && from->weapon->weaponID == WEAPON_FLAME_THROWER)
		{
			p2 = direction * 3;
		}
		p2 = rotateAboutAxis(p2, rand(-imp, imp), CVector3f(0,0,1));
		p2 = rotateAboutAxis(p2, rand(0.0f, 360.0f), direction);
		p2[2] *= .5f;
		p2 += position;

		if (projectileType == PROJECTILE_DIRECT && from->weapon)
		{
			net_clsv_player_shoot playerShoot;
			playerShoot.playerID = from->playerID;
			playerShoot.nuzzleID = (char)from->weapon->firingNuzzle;
			playerShoot.p1[0] = (short)(position[0] * 100);
			playerShoot.p1[1] = (short)(position[1] * 100);
			playerShoot.p1[2] = (short)(position[2] * 100);
#ifdef _PRO_
			playerShoot.p2[0] = (short)(direction[0] * 100);
			playerShoot.p2[1] = (short)(direction[1] * 100);
			playerShoot.p2[2] = (short)(direction[2] * 100);
#else
			playerShoot.p2[0] = (short)(p2[0] * 100);
			playerShoot.p2[1] = (short)(p2[1] * 100);
			playerShoot.p2[2] = (short)(p2[2] * 100);
#endif
			playerShoot.weaponID = from->weapon->weaponID;
			bb_clientSend(scene->client->uniqueClientID, (char*)&playerShoot,sizeof(net_clsv_player_shoot),NET_CLSV_PLAYER_SHOOT);
		}
		else if (projectileType == PROJECTILE_ROCKET && from->weapon)
		{
			// On demande au server de créer une instance d'une rocket
			net_clsv_svcl_player_projectile playerProjectile;
			playerProjectile.playerID = from->playerID;
			playerProjectile.nuzzleID = (char)from->weapon->firingNuzzle;
			playerProjectile.projectileType = (char)projectileType;
			playerProjectile.weaponID = from->weapon->weaponID;
			playerProjectile.position[0] = (short)(position[0] * 100.0f);
			playerProjectile.position[1] = (short)(position[1] * 100.0f);
			playerProjectile.position[2] = (short)(position[2] * 100.0f);
		//	CVector3f dir = from->currentCF.mousePosOnMap - position; // Pas une bonne idée ça, trop facile
		//	normalize(dir);
			playerProjectile.vel[0] = (char)(direction[0] * 10.0f);
			playerProjectile.vel[1] = (char)(direction[1] * 10.0f);
			playerProjectile.vel[2] = (char)(direction[2] * 10.0f);
			bb_clientSend(scene->client->uniqueClientID, (char*)&playerProjectile,sizeof(net_clsv_svcl_player_projectile),NET_CLSV_SVCL_PLAYER_PROJECTILE);

			// duplicate rocket was for hacking test only
			//playerProjectile.vel[1] = (char)(direction[0] * 10.0f);
			//playerProjectile.vel[2] = (char)(direction[1] * 10.0f);
			//playerProjectile.vel[0] = (char)(direction[2] * 10.0f);

			//bb_clientSend(scene->client->uniqueClientID, (char*)&playerProjectile,sizeof(net_clsv_svcl_player_projectile),NET_CLSV_SVCL_PLAYER_PROJECTILE);
		}
		else if (projectileType == PROJECTILE_GRENADE)
		{
		//	for (int i=0;i<20;++i)
		//	{
				// On demande au server de créer une instance d'une grenade
				net_clsv_svcl_player_projectile playerProjectile;
				playerProjectile.playerID = from->playerID;
				playerProjectile.nuzzleID = (char)from->weapon->firingNuzzle;
				playerProjectile.projectileType = (char)projectileType;
				playerProjectile.weaponID = WEAPON_GRENADE;
				playerProjectile.position[0] = (short)(position[0] * 100.0f);
				playerProjectile.position[1] = (short)(position[1] * 100.0f);
				playerProjectile.position[2] = (short)(position[2] * 100.0f);
			//	CVector3f dir = from->currentCF.mousePosOnMap - position; // Pas une bonne idée ça, trop facile
			//	normalize(dir);
				playerProjectile.vel[0] = (char)(direction[0] * 10.0f);
				playerProjectile.vel[1] = (char)(direction[1] * 10.0f);
				playerProjectile.vel[2] = (char)(direction[2] * 10.0f);
				bb_clientSend(scene->client->uniqueClientID, (char*)&playerProjectile,sizeof(net_clsv_svcl_player_projectile),NET_CLSV_SVCL_PLAYER_PROJECTILE);
		//	}
		}
		else if (projectileType == PROJECTILE_COCKTAIL_MOLOTOV)
		{
			// On demande au server de créer une instance d'une grenade
			net_clsv_svcl_player_projectile playerProjectile;
			playerProjectile.playerID = from->playerID;
			playerProjectile.nuzzleID = (char)from->weapon->firingNuzzle;
			playerProjectile.projectileType = (char)projectileType;
			playerProjectile.weaponID = WEAPON_COCKTAIL_MOLOTOV;
			playerProjectile.position[0] = (short)(position[0] * 100.0f);
			playerProjectile.position[1] = (short)(position[1] * 100.0f);
			playerProjectile.position[2] = (short)(position[2] * 100.0f);
		//	CVector3f dir = from->currentCF.mousePosOnMap - position; // Pas une bonne idée ça, trop facile
		//	normalize(dir);
			playerProjectile.vel[0] = (char)(direction[0] * 10.0f);
			playerProjectile.vel[1] = (char)(direction[1] * 10.0f);
			playerProjectile.vel[2] = (char)(direction[2] * 10.0f);
			bb_clientSend(scene->client->uniqueClientID, (char*)&playerProjectile,sizeof(net_clsv_svcl_player_projectile),NET_CLSV_SVCL_PLAYER_PROJECTILE);
		}
	}
}
#endif


//
// Pour savoir s'il y a un joueur dans le radius
//
Player * Game::playerInRadius(CVector3f position, float radius, int ignore )
{
	for (int i=0;i<MAX_PLAYER;++i)
	{
		if (players[i])
		{
			if (players[i]->status == PLAYER_STATUS_ALIVE && i != ignore)
			{
				if (distanceSquared(position, players[i]->currentCF.position) <= (radius+.25f)*(radius+.25f)) return players[i];
			}
		}
	}
	return 0;
}


//
// Quand un client shot, mais que le server le vérifie puis le shoot aux autres joueurs
//
void Game::shootSV(net_clsv_player_shoot & playerShoot)
{
	Player* player = players[playerShoot.playerID];
	if (player == 0)
		return;

	CVector3f p1;/*(playerShoot.p1);*/
	CVector3f p2;/*(playerShoot.p2);*/
	p1[0] = (float)playerShoot.p1[0] / 100.0f;
	p1[1] = (float)playerShoot.p1[1] / 100.0f;
	p1[2] = (float)playerShoot.p1[2] / 100.0f;
	p2[0] = (float)playerShoot.p2[0] / 100.0f;
	p2[1] = (float)playerShoot.p2[1] / 100.0f;
	p2[2] = (float)playerShoot.p2[2] / 100.0f;
	player->weapon->shotFrom = p1;

#ifdef _PRO_
	if (player->weapon->weaponID == WEAPON_SHOTGUN)
	{
		// ves's suggestion
		const float directionAngles[5] = {-10.0f, -5.0f, 0.0f, 5.0f, 10.0f};
		const float deviationAngles[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};//for the moment, these simply identify which shots are which, the actual deviation is fixed in the next call to shootSV
		// adder's suggestion
		//const float directionAngles[7] = {0.0f, 7.0f, -7.0f, 3.5f, -3.5f, 10.5f, -10.5f};
		//const float deviationAngles[7] = {3.5f, 3.5f, 3.5f, 3.5f, 3.5f, 3.5f, 3.5f};
		CVector3f newP2;
		for (int i=0;i<player->weapon->nbShot;++i)
		{
			newP2 = rotateAboutAxis(p2, directionAngles[i], CVector3f(0.0f, 0.0f, 1.0f));
			shootSV(playerShoot.playerID, playerShoot.nuzzleID, deviationAngles[i], p1, newP2);
		}
	}
	else if (player->weapon->weaponID == WEAPON_SNIPER)
	{
		for (int i=0;i<player->weapon->nbShot;++i)
			shootSV(playerShoot.playerID, playerShoot.nuzzleID, 0, p1, p2);
	}
	else if (player->weapon->weaponID == WEAPON_CHAIN_GUN)
   {
		player->weapon->currentImp += 3;
		if (player->weapon->currentImp > player->weapon->impressision)
			player->weapon->currentImp = player->weapon->impressision;

      float imp = player->weapon->currentImp;      
      
      if (gameVar.sv_serverType == SERVER_TYPE_PRO)
         {
         if (player->currentCF.vel.length() < 1.15f)
            {
            imp /= 2.70f;
            }
         }


		for (int i=0;i<player->weapon->nbShot;++i)
			shootSV(playerShoot.playerID, playerShoot.nuzzleID, imp, p1, p2);
   }
   else
#endif
	{
		player->weapon->currentImp += 3;
		if (player->weapon->currentImp > player->weapon->impressision)
			player->weapon->currentImp = player->weapon->impressision;
		for (int i=0;i<player->weapon->nbShot;++i)
			shootSV(playerShoot.playerID, playerShoot.nuzzleID, player->weapon->currentImp, p1, p2);
	}
}

#ifdef _PRO_
void Game::shootMinibotSV(CMiniBot * minibot, float imp, CVector3f p1, CVector3f p2)
{
	CVector3f normal;

	//--- Impresition
	CVector3f dir = p2 - p1;
	normalize(dir);
	p2 -= p1;
	p2 = rotateAboutAxis(p2, rand(-imp, imp), CVector3f(0,0,1));
	p2 = rotateAboutAxis(p2, rand(0.0f, 360.0f), dir);
	p2[2] *= .5f;
	p2 += p1;

	// On test s'il y a une collision
	map->rayTest(p1, p2, normal);

	// On test premièrement si on touche un autre joueur!
	Player * hitPlayer = 0;
	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (players[i])
		{
			if (i != minibot->owner->playerID)
			{
				if (players[i]->status == PLAYER_STATUS_ALIVE)
           //    if (players[i]->status == PLAYER_STATUS_ALIVE && (players[i]->teamID != player->teamID || gameType == GAME_TYPE_DM || gameType == GAME_TYPE_SND || gameVar.sv_friendlyFire))
				{
					// Ray to sphere test
					if (segmentToSphere(p1, p2, players[i]->currentCF.position, .25f))
					{
						hitPlayer = players[i];
						normal = p2 - p1;
						normalize(normal);
					}
				}
			}
		}
	}

	// On envoit le résultat à TOUT les joueurs y compris celui qui l'a tiré
	net_svcl_player_shoot playerShootSV;
	if (hitPlayer)
	{
		playerShootSV.hitPlayerID = hitPlayer->playerID;
		playerShootSV.weaponID = WEAPON_MINIBOT_WEAPON;

		// On décrémente sa vie
		hitPlayer->hitSV(gameVar.weapons[WEAPON_MINIBOT], players[minibot->owner->playerID]);
	}
	else
	{
		playerShootSV.hitPlayerID = -1;
	}
	playerShootSV.playerID = minibot->owner->playerID;
	playerShootSV.nuzzleID = 0;
	playerShootSV.p1[0] = (short)(p1[0] * 100);
	playerShootSV.p1[1] = (short)(p1[1] * 100);
	playerShootSV.p1[2] = (short)(p1[2] * 100);
	playerShootSV.p2[0] = (short)(p2[0] * 100);
	playerShootSV.p2[1] = (short)(p2[1] * 100);
	playerShootSV.p2[2] = (short)(p2[2] * 100);
	playerShootSV.normal[0] = (char)(normal[0] * 120);
	playerShootSV.normal[1] = (char)(normal[1] * 120);
	playerShootSV.normal[2] = (char)(normal[2] * 120);
	playerShootSV.weaponID = WEAPON_MINIBOT_WEAPON;
	bb_serverSend((char*)&playerShootSV,sizeof(net_svcl_player_shoot),NET_SVCL_PLAYER_SHOOT,0);
}
#endif

void Game::shootSV(int playerID, int nuzzleID, float imp, CVector3f p1, CVector3f p2)
{
	Player* player = players[playerID];
	if (player == 0)
		return;
	int ident = (int)imp;
	CVector3f oldP2;
	if(player->weapon->weaponID == WEAPON_SHOTGUN)
	{
		switch(ident)
		{
		case 1:
			oldP2 = rotateAboutAxis(p2, 10.0f, CVector3f(0.0f, 0.0f, 1.0f));
			break;
		case 2:
			oldP2 = rotateAboutAxis(p2, 5.0f, CVector3f(0.0f, 0.0f, 1.0f));
			break;
		case 3:
			oldP2 = rotateAboutAxis(p2, 0.0f, CVector3f(0.0f, 0.0f, 1.0f));
			break;
		case 4:
			oldP2 = rotateAboutAxis(p2, -5.0f, CVector3f(0.0f, 0.0f, 1.0f));
			break;
		case 5:
			oldP2 = rotateAboutAxis(p2, -10.0f, CVector3f(0.0f, 0.0f, 1.0f));
			break;
		};	
		normalize(oldP2);
	}	
	if(player->weapon->weaponID == WEAPON_SHOTGUN)
	{
		imp = 3.5f;
	}
#ifdef _PRO_
	CVector3f dir = p2;
    
	if (player->weapon->projectileType == PROJECTILE_DIRECT && player->weapon->weaponID == WEAPON_FLAME_THROWER)
	{
		if(gameVar.sv_ftExpirationTimer > 0)
		{
			float mult = (1.0f-player->secondsFired/gameVar.sv_ftExpirationTimer) * gameVar.sv_ftMaxRange;
			if (mult < gameVar.sv_ftMinRange)
				mult = gameVar.sv_ftMinRange;
			p2 = p2 * mult;//nuvem's awesome idea, ft range decreases the longer it's fired
			if(gameVar.sv_explodingFT && player->secondsFired/gameVar.sv_ftExpirationTimer > 1)
			{
				scene->server->nukePlayer(playerID);
			}
		}
		else
			p2 = p2 * gameVar.sv_ftMaxRange;
	}
	else
		p2 = p2 * 128;

	p2 = rotateAboutAxis(p2, rand(-imp, imp), CVector3f(0,0,1));
	p2 = rotateAboutAxis(p2, rand(0.0f, 360.0f), dir);
	p2[2] *= .5f;
	p2 += p1;
#endif

	CVector3f normal;

	   if (player->weapon->weaponID == WEAPON_SHOTGUN)
	   {
		   //--- Clamp shot
		   CVector3f dir = p2 - p1;
		   normalize(dir);
		   
         float clampShot;
		 float variation = 0.01f;
         if(gameVar.sv_serverType == SERVER_TYPE_PRO) 
		 {	//clampShot = gameVar.sv_shottyRange;
			 CVector3f sinThetaVector = cross(dir,oldP2);
			 float sinTheta = sinThetaVector.length();
			 clampShot = gameVar.sv_shottyDropRadius/sinTheta;
		 }
		 else 
		 {
	 		 switch(ident)
	 		 {
	 		 case 1:
	 			 clampShot = gameVar.sv_shottyRange*(0.333f+rand(-variation, variation));
				 break;
			 case 2:
				 clampShot = gameVar.sv_shottyRange*(0.667f+rand(-variation, variation));
				 break;
			 case 3:
				 clampShot = gameVar.sv_shottyRange;
				 break;
			 case 4:
				 clampShot = gameVar.sv_shottyRange*(0.667f+rand(-variation, variation));
				 break;
			 case 5:
				 clampShot = gameVar.sv_shottyRange*(0.333f+rand(-variation, variation));
				 break;
			 };
		 }
         
         p2 = p1 + dir * clampShot;
	   }


	bool isCollision = false;

	if (map->rayTest(player->currentCF.position, p1, normal))
	{
		p1 += normal * .01f;
	}

	// On test s'il y a une collision, sinon, fuck it on envoit pas ça
	if (map->rayTest(p1, p2, normal))
	{
		isCollision = true;
	}

	if (player->weapon->weaponID == WEAPON_PHOTON_RIFLE || player->weapon->weaponID == WEAPON_FLAME_THROWER)
	{
		if (player->weapon->weaponID == WEAPON_PHOTON_RIFLE)
		{
			player->p1 = p1;
			player->p2 = p2;
			player->incShot = 30;
		}
		CVector3f p3 = p2;
		// On test premièrement si on touche un autre joueur!
		Player * hitPlayer = 0;
		for (int i=0;i<MAX_PLAYER;i++)
		{
			if (players[i])
			{
				if (i != player->playerID)
				{
#ifdef _PRO_
               if (players[i]->status == PLAYER_STATUS_ALIVE && (players[i]->teamID != player->teamID || gameType == GAME_TYPE_DM || gameType == GAME_TYPE_SND || gameVar.sv_friendlyFire || gameVar.sv_reflectedDamage))
#else
               if (players[i]->status == PLAYER_STATUS_ALIVE && (players[i]->teamID != player->teamID || gameType == GAME_TYPE_DM || gameVar.sv_friendlyFire))
#endif					
					{
						// Ray to sphere test
						if (segmentToSphere(p1, p3, players[i]->currentCF.position, (player->weapon->weaponID == WEAPON_FLAME_THROWER)?.50f:.25f))
						{
							isCollision = true;
							hitPlayer = players[i];
							normal = p3 - p1;
							p3 = p2; // Full length
							normalize(normal);

							// On décrémente sa vie
							hitPlayer->hitSV(gameVar.weapons[player->weapon->weaponID], player, gameVar.weapons[player->weapon->weaponID]->damage);
						}
					}
				}
			}
		}

		// On envoit le résultat à TOUT les joueurs y compris celui qui l'a tiré
		net_svcl_player_shoot playerShootSV;
		playerShootSV.hitPlayerID = -1;
		playerShootSV.playerID = player->playerID;
		playerShootSV.nuzzleID = nuzzleID;
		playerShootSV.p1[0] = (short)(p1[0] * 100);
		playerShootSV.p1[1] = (short)(p1[1] * 100);
		playerShootSV.p1[2] = (short)(p1[2] * 100);
		playerShootSV.p2[0] = (short)(p2[0] * 100);
		playerShootSV.p2[1] = (short)(p2[1] * 100);
		playerShootSV.p2[2] = (short)(p2[2] * 100);
		playerShootSV.normal[0] = (char)(normal[0] * 120);
		playerShootSV.normal[1] = (char)(normal[1] * 120);
		playerShootSV.normal[2] = (char)(normal[2] * 120);
		playerShootSV.weaponID = player->weapon->weaponID;
		bb_serverSend((char*)&playerShootSV,sizeof(net_svcl_player_shoot),NET_SVCL_PLAYER_SHOOT,0);
	}
	else
	{
		// On test premièrement si on touche un autre joueur!
		Player * hitPlayer = 0;
		for (int i=0;i<MAX_PLAYER;i++)
		{
			if (players[i])
			{
				if (i != player->playerID)
				{
					if (players[i]->status == PLAYER_STATUS_ALIVE)
					{
						// Ray to sphere test
						if (segmentToSphere(p1, p2, players[i]->currentCF.position, .25f))
						{
							isCollision = true;
							hitPlayer = players[i];
							normal = p2 - p1;
							normalize(normal);
						}
					}
				}
			}
		}

		// On envoit le résultat à TOUT les joueurs y compris celui qui l'a tiré
		net_svcl_player_shoot playerShootSV;
		if (hitPlayer)
		{
			playerShootSV.hitPlayerID = hitPlayer->playerID;
			playerShootSV.weaponID = player->weapon->weaponID;

			// On décrémente sa vie
			hitPlayer->hitSV(gameVar.weapons[playerShootSV.weaponID], players[player->playerID]);
		}
		else
		{
			playerShootSV.hitPlayerID = -1;
		}
		playerShootSV.playerID = player->playerID;
		playerShootSV.nuzzleID = nuzzleID;
		playerShootSV.p1[0] = (short)(p1[0] * 100);
		playerShootSV.p1[1] = (short)(p1[1] * 100);
		playerShootSV.p1[2] = (short)(p1[2] * 100);
		playerShootSV.p2[0] = (short)(p2[0] * 100);
		playerShootSV.p2[1] = (short)(p2[1] * 100);
		playerShootSV.p2[2] = (short)(p2[2] * 100);
		playerShootSV.normal[0] = (char)(normal[0] * 120);
		playerShootSV.normal[1] = (char)(normal[1] * 120);
		playerShootSV.normal[2] = (char)(normal[2] * 120);
		playerShootSV.weaponID = player->weapon->weaponID;
		bb_serverSend((char*)&playerShootSV,sizeof(net_svcl_player_shoot),NET_SVCL_PLAYER_SHOOT,0);
	}
}



//
// Pour toucher les joueurs dans un rayon
//
void Game::radiusHit(CVector3f & pos, float radius, char fromID, char weaponID, bool sameDmg)
{
	CVector3f position;
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];//to keep the nuke explosion from moving when the nuker dies
	// Est-ce que ce joueur existe toujours?
	if (players[fromID])
	{
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (fromID == i)
			{
				if (weaponID == WEAPON_KNIVES) continue;
			}
			Player * player = players[i];
			if (player)
			{
				// Sa distance de l'impact
				float dis = distance(player->currentCF.position, position);
				if (dis < radius)
				{
					CVector3f p1 = position;
					CVector3f p2 = player->currentCF.position;
					CVector3f normal;
					// On le touche si y a pas un mur qui intercept!
					if (!map->rayTest(p1, p2, normal))
					{
						CVector3f dir = player->currentCF.position - position;
						normalize(dir);
					/*	net_svcl_player_hit playerHit;
						playerHit.damage = (1 - (dis / radius)) * gameVar.weapons[weaponID]->damage;
						playerHit.playerID = (char)i;
						playerHit.fromID = fromID;
						playerHit.weaponID = weaponID;
						playerHit.vel[0] = (char)((dir[0] * playerHit.damage * 10) / 10.0f);
						playerHit.vel[1] = (char)((dir[1] * playerHit.damage * 10) / 10.0f);
						playerHit.vel[2] = (char)((dir[2] * playerHit.damage * 10) / 10.0f);
						bb_serverSend((char*)&playerHit,sizeof(net_svcl_player_hit),NET_SVCL_PLAYER_HIT,0);*/
						player->hitSV(gameVar.weapons[weaponID], players[fromID], ((sameDmg)?1:(1 - (dis / radius))) * gameVar.weapons[weaponID]->damage);
					}
				}
			}
		}
	}
}



//
// Pour ajouter un nouveau joueur
//
int Game::createNewPlayerSV(int babonetID)
{
	// On lui trouve une nouvelle place
	for (int i=0;i<gameVar.sv_maxPlayer;++i)
	{
		if (!players[i])
		{
			players[i] = new Player((char)i, map, this);
			players[i]->babonetID = babonetID;

			// On envoit l'info à tout les clients (y compris lui)
			net_svcl_newplayer newPlayer;
			newPlayer.newPlayerID = players[i]->playerID;
			newPlayer.baboNetID = babonetID;
			bb_serverSend((char*)&newPlayer, sizeof(net_svcl_newplayer), NET_SVCL_NEWPLAYER, 0);

			// On lui envoi premièrement la version du jeu
			net_svcl_gameversion gameVersion;
			gameVersion.gameVersion = GAME_VERSION_SV;
			bb_serverSend((char*)&gameVersion, sizeof(net_svcl_gameversion), NET_SVCL_GAMEVERSION, babonetID);

			//--- Est-ce que c'est le seul joueur? ou il y a 2 joueur? On restart le server.


			return i;
		}
	}

	return -1; // Pus de place
}

int Game::numPlayers(void)
{
	int nbPlayer = 0;
	for (int i=0;i<MAX_PLAYER;++i)
	{
		if (players[i])
		{
			nbPlayer++;
		}
	}

   return nbPlayer;
}

#ifndef DEDICATED_SERVER
void Game::createNewPlayerCL(int playerID, long babonetID)
{
	if (playerID < 0 || playerID >= MAX_PLAYER)
	{
		console->add(CString("\x4> Error : invalid playerID : %i", playerID));
	}
	else
	{
		// On l'efface au cas quil existe déjà (très pas bon ça)
		ZEVEN_SAFE_DELETE(players[playerID]);
		players[playerID] = new Player(playerID, map, this);
		players[playerID]->babonetID = babonetID;
	}
}
#endif





