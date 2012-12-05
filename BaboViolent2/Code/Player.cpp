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

#include "Player.h"
#include "GameVar.h"
#include "Console.h"
#include "Map.h"
#include "Game.h"
#include "Scene.h"
#include <limits>

extern Scene * scene;


PlayerStats::PlayerStats(const Player* player)
{
	name = player->name;//textColorLess(player->name);
	userID = player->userID;
	teamID = player->teamID;
	kills = player->kills;
	deaths = player->deaths;
	dmg = player->dmg;
	score = player->score;
	returns = player->returns;
	flagAttempts = player->flagAttempts;
	timePlayedCurGame = player->timePlayedCurGame;
}

void PlayerStats::MergeStats(PlayerStats* mergeWith)
{
	if (userID == 0 || userID != mergeWith->userID)
		return;
	name = textColorLess(mergeWith->name);
	kills += mergeWith->kills;
	deaths += mergeWith->deaths;
	dmg += mergeWith->dmg;
	score += mergeWith->score;
	returns += mergeWith->returns;
	flagAttempts += mergeWith->flagAttempts;
	timePlayedCurGame += mergeWith->timePlayedCurGame;
}

//
// Constructeur
//
Player::Player(char pPlayerID, Map * pMap, Game * pGame): pingLogInterval(0.05f),
	pingLogID(0)
{
	nextPingLogTime = pingLogInterval = 1.0f/30;
	rocketInAir = false;
	detonateRocket = false;

	for (int i = 0; i < PING_LOG_SIZE; i++)
		pingLog[i] = 0;
	// enough time has elapsed for any good to be able to shoot when creating the Player
	mfElapsedSinceLastShot = 9999.0f;

	miNbCoord = 0;
    mfCFTimer = 0.0f; 
	mfCumulativeVel = 0.0f; 
	shootShakeDis.set(0,0,0);

#ifdef _PRO_
   spawnSlot = -1;
#endif

	timeDead = 0.0f;
	timeAlive = 0.0f;
	timeIdle = 0.0f;
	timeInServer = 0.0f;

#ifdef _PRO_
	minibot = 0;
#endif

	userID = 0;

	incShot = 0;
	voted = false;
	protection = 0;
	isAdmin = false;
	playerIP[0] = '\0';
	frameSinceLast = 0;
	lastFrame = 0;
	currentFrame = 0;
	speedHackCount = 0;
	shotsPerSecond = 0;
	shotCount = 0;
	secondPassed = 0;
	meleeDelay = 0;
	fireFrameDelay = 0;
	game = pGame;
	map = pMap;
	name = "Unnamed Babo";
	babonetID = 0; // Retreive this from gknet_
	playerID = pPlayerID;
	teamID = PLAYER_TEAM_SPECTATOR; // -1 = spectator, 0 = bleu, 1 = rouge
	status = PLAYER_STATUS_LOADING;
	ping = -1;
	pingSum = 0;
	avgPing = 0;
	babySitTime = 5.0f;
	pingOverMax = 0.0f;
	memset(pingLog, 0, sizeof(int)*PING_LOG_SIZE);

	dmg = 0;
	kills = 0;
	deaths = 0;
	score = 0;
	returns = 0;
	damage = 0;
	flagAttempts = 0;
	
	timePlayedCurGame = 0.0f;
	waitForPong = false;
	sendPosFrame=0;
#ifndef CONSOLE
#ifndef _DX_
	qObj = gluNewQuadric();
#endif
	isThisPlayer = false;
#endif
	timeToSpawn = gameVar.sv_timeToSpawn;
	remoteEntity = true;
	cFProgression = 0;
	weapon = 0;
	meleeWeapon = 0;
#ifndef CONSOLE
	tex_baboShadow = dktCreateTextureFromFile("main/textures/BaboShadow.tga", DKT_FILTER_BILINEAR);
	tex_baboHalo = dktCreateTextureFromFile("main/textures/BaboHalo.tga", DKT_FILTER_BILINEAR);
#endif
	nextSpawnWeapon = gameVar.cl_primaryWeapon;//WEAPON_SMG;
	nextMeleeWeapon = gameVar.cl_secondaryWeapon + WEAPON_KNIVES;//WEAPON_KNIVES;
#ifndef CONSOLE
	initedMouseClic = false;
#endif
	firedShowDelay = 0;
	deadSince = 0;
#ifndef CONSOLE
	followingPlayer = 0;
#endif
	screenHit = 0;
	grenadeDelay = 0;
	nbGrenadeLeft = 2; // On commence toujours avec 2 grenade
	nbMolotovLeft = 1; // On commence toujours avec 1 molotov
	currentPingFrame = 0;
	connectionInterrupted = false;
	spawnRequested = false;
#ifndef CONSOLE
	scopeMode = false;
#endif

	//--- on load un skin par default
	skin = "skin10";
#ifndef CONSOLE
	tex_skinOriginal = dktCreateTextureFromFile((CString("main/skins/") + skin + ".tga").s, DKT_FILTER_BILINEAR);
	tex_skin = dktCreateEmptyTexture(64, 32, 3, DKT_FILTER_BILINEAR);
#endif
}



//
// Destructeur
//
Player::~Player()
{
#ifndef CONSOLE
#ifndef _DX_
	gluDeleteQuadric(qObj);
#endif
	ZEVEN_SAFE_DELETE(weapon);
	dktDeleteTexture(&tex_baboShadow);
	dktDeleteTexture(&tex_baboHalo);
	dktDeleteTexture(&tex_skinOriginal);
	dktDeleteTexture(&tex_skin);
#endif
#ifdef _PRO_
	if (minibot) delete minibot;
#endif
	//--- Est-ce qu'on est server et que ce player poc�e le flag???
	if (scene->server)
	{
		if (scene->server->game && !scene->server->needToShutDown)
		{
			if (scene->server->game->map)
			{
				for (int i=0;i<2;++i)
				{
					if (scene->server->game->map->flagState[i] == playerID)
					{
						scene->server->game->map->flagState[i] = -1; // Le server va nous communiquer la position du flag exacte
						scene->server->game->map->flagPos[i] = currentCF.position;
						scene->server->game->map->flagPos[i][2] = 0;
						if (scene->server->game->isServerGame)
						{
							// On envoit la new pos du flag aux autres
							net_svcl_drop_flag dropFlag;
							dropFlag.flagID = (char)i;
							dropFlag.position[0] = scene->server->game->map->flagPos[i][0];
							dropFlag.position[1] = scene->server->game->map->flagPos[i][1];
							dropFlag.position[2] = scene->server->game->map->flagPos[i][2];
							bb_serverSend((char*)&dropFlag, sizeof(net_svcl_drop_flag), NET_SVCL_DROP_FLAG, 0);
						}
					}
				}
			}
		}
	}
}



//
// Pour le forcer �crever (suposons quil change de team)
//
void Player::kill(bool silenceDeath)
{
#ifndef CONSOLE
	if (silenceDeath)
	{
#endif
		status = PLAYER_STATUS_DEAD;
		deadSince = 0;
#ifndef CONSOLE
	}
	else
	{
		status = PLAYER_STATUS_DEAD;
		dksPlay3DSound(gameVar.sfx_baboCreve[rand()%3], -1, 5, currentCF.position, 255);
		game->spawnBlood(currentCF.position, 1);
		deadSince = 0;

#ifdef _PRO_
		//--- Spawn bot blood
		if (minibot)
		{
			game->spawnBloodMinibot(minibot->currentCF.position, .5f);
		}
#endif
		
		//--- Spawn some gibs :D
	/*	for (int i=0;i<10;++i)
		{
			if (game) game->douilles.push_back(new Douille(currentCF.position, 
				rand(CVector3f(-2.5,-2.5,1),CVector3f(2.5,2.5,2.5)), 
				CVector3f(1,0,0), DOUILLE_TYPE_GIB));
		}*/
	}
#endif

#ifdef _PRO_
	//--- Delete minibot
	if (minibot) delete minibot; minibot = 0;
#endif

	// Si il avait le flag, on le laisse tomber
	for (int i=0;i<2;++i)
	{
		if (game->map->flagState[i] == playerID)
		{
			game->map->flagState[i] = -1; // Le server va nous communiquer la position du flag exacte
			game->map->flagPos[i] = currentCF.position;
			game->map->flagPos[i][2] = 0;
#ifndef CONSOLE
			game->map->flagAngle[i] = 0;
#endif
			if (game->isServerGame)
			{
				// On envoit la new pos du flag aux autres
				net_svcl_drop_flag dropFlag;
				dropFlag.flagID = (char)i;
				dropFlag.position[0] = game->map->flagPos[i][0];
				dropFlag.position[1] = game->map->flagPos[i][1];
				dropFlag.position[2] = game->map->flagPos[i][2];
				bb_serverSend((char*)&dropFlag, sizeof(net_svcl_drop_flag), NET_SVCL_DROP_FLAG, 0);
			}
		}
	}
	currentCF.position.set(-999,-999,0);
}



#ifdef _PRO_
//
//--- Spawn the little evil bot
//
void Player::SpawnMiniBotSV()
{
	if (minibot) return; // Can't, already present

	//--- Is there a place to spawn it in front of us?
	CVector3f dir = currentCF.mousePosOnMap - currentCF.position;
	dir[2] = 0;
	normalize(dir);
	CVector3f normal;
	CVector3f p2 = currentCF.position + dir;
	if (!game->map->rayTest(currentCF.position, p2, normal))
	{
		minibot = new CMiniBot();

		minibot->owner = this;
		minibot->game = game;

		CVector3f botPos = currentCF.position + dir;
		botPos[2] = .15f;
		minibot->currentCF.position = botPos;
		minibot->currentCF.mousePosOnMap = botPos + dir;
		minibot->currentCF.mousePosOnMap[2] = 0;
		minibot->lastCF = minibot->currentCF;
		minibot->netCF0 = minibot->currentCF;
		minibot->netCF1 = minibot->currentCF;
		minibot->destination = botPos;

		//--- Tell players
		net_svcl_create_minibot miniBot;
		miniBot.playerID = playerID;
		miniBot.position[0] = (short)(botPos[0] * 10.0f);
		miniBot.position[1] = (short)(botPos[1] * 10.0f);
		miniBot.position[2] = (short)(botPos[2] * 10.0f);
		miniBot.mousePos[0] = (short)(minibot->currentCF.mousePosOnMap[0] * 10.0f);
		miniBot.mousePos[1] = (short)(minibot->currentCF.mousePosOnMap[1] * 10.0f);
		miniBot.mousePos[2] = (short)(minibot->currentCF.mousePosOnMap[2] * 10.0f);
		bb_serverSend((char*)&miniBot, sizeof(net_svcl_create_minibot), NET_SVCL_CREATE_MINIBOT, 0);
	}
}
void Player::SpawnNukeBotSV()
{
	if (minibot) return; // Can't, already present
		minibot = new CMiniBot();

	minibot->owner = this;
	minibot->game = game;

	CVector3f botPos = currentCF.position;
	botPos[2] = .15f;
	minibot->MakeNukeBot();
	minibot->currentCF.position = botPos;
	minibot->currentCF.mousePosOnMap = botPos;
	minibot->currentCF.mousePosOnMap[2] = 0;
	minibot->lastCF = minibot->currentCF;
	minibot->netCF0 = minibot->currentCF;
	minibot->netCF1 = minibot->currentCF;
	minibot->destination = botPos;

	//--- Tell players
	net_svcl_create_minibot miniBot;
	miniBot.playerID = playerID;
	miniBot.position[0] = (short)(botPos[0] * 10.0f);
	miniBot.position[1] = (short)(botPos[1] * 10.0f);
	miniBot.position[2] = (short)(botPos[2] * 10.0f);
	miniBot.mousePos[0] = (short)(minibot->currentCF.mousePosOnMap[0] * 10.0f);
	miniBot.mousePos[1] = (short)(minibot->currentCF.mousePosOnMap[1] * 10.0f);
	miniBot.mousePos[2] = (short)(minibot->currentCF.mousePosOnMap[2] * 10.0f);
	bb_serverSend((char*)&miniBot, sizeof(net_svcl_create_minibot), NET_SVCL_CREATE_MINIBOT, 0);
}
void Player::SpawnMiniBot(const CVector3f & spawnPoint, const CVector3f & mousePos)
{
	if (minibot) delete minibot;
	minibot = new CMiniBot();

	CVector3f botPos = spawnPoint;
	if(meleeWeapon && meleeWeapon->weaponID == WEAPON_NUCLEAR)
	{
		minibot->MakeNukeBot();//fixes things up so client side rendering is correct
	}
	minibot->owner = this;
	minibot->game = game;

	minibot->currentCF.position = botPos;
	minibot->currentCF.mousePosOnMap = mousePos;

	minibot->lastCF = minibot->currentCF;
	minibot->netCF0 = minibot->currentCF;
	minibot->netCF1 = minibot->currentCF;
}
#endif


#ifndef CONSOLE
//
// Render
//
void Player::render()
{
	if (status == PLAYER_STATUS_ALIVE)
	{
#ifndef _DX_
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
#endif
		//--- TEMP render path with his bot
#ifdef _PRO_
		if (minibot && gameVar.d_showPath)
		{
#ifndef _DX_
			glColor3f(1, 1, 0);
			glLineWidth(2);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glBegin(GL_LINE_STRIP);
				CPathNode * searchFrom = game->map->aStar->GetNodeAt((int)minibot->currentCF.position[0], (int)minibot->currentCF.position[1]);
				CPathNode * searchTo = game->map->aStar->GetNodeAt((int)currentCF.position[0], (int)currentCF.position[1]);
				CPathNode * pathList = game->map->aStar->FindPath(searchFrom, searchTo);
				for (CPathNode * p = pathList;p;p = p->parentNode)
				{
					if (p->parentNode) glVertex3f(p->m_centerX, p->m_centerY, .15f);
				}
				glVertex3fv(minibot->currentCF.position.s);
			glEnd();
#endif
		}
#endif
			// On render son shadow :)
			if (gameVar.r_playerShadow)
			{
#ifndef _DX_
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tex_baboShadow);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_LIGHTING);
				glColor4f(1,1,1,.75f);
				glDepthMask(GL_FALSE);
				glPushMatrix();
					glTranslatef(currentCF.position[0]+.1f, currentCF.position[1]-.1f, .025f);
					glBegin(GL_QUADS);
						glTexCoord2f(0,1);
						glVertex2f(-.5f, .5f);
						glTexCoord2f(0,0);
						glVertex2f(-.5f, -.5f);
						glTexCoord2f(1,0);
						glVertex2f(.5f, -.5f);
						glTexCoord2f(1,1);
						glVertex2f(.5f, .5f);
					glEnd();
				glPopMatrix();
#endif
#ifdef _PRO_
				//--- Mini bot shadow :D
				if (minibot)
				{
#ifndef _DX_
					glPushMatrix();
						glTranslatef(minibot->currentCF.position[0]+.06f, minibot->currentCF.position[1]-.06f, .025f);
						glBegin(GL_QUADS);
							glTexCoord2f(0,1);
							glVertex2f(-.25f, .25f);
							glTexCoord2f(0,0);
							glVertex2f(-.25f, -.25f);
							glTexCoord2f(1,0);
							glVertex2f(.25f, -.25f);
							glTexCoord2f(1,1);
							glVertex2f(.25f, .25f);
						glEnd();
					glPopMatrix();
#endif
				}
#endif
			}
			if( (game->gameType != GAME_TYPE_DM) &&	
				(game->gameType != GAME_TYPE_SND) && 
				(gameVar.cl_teamIndicatorType == 1 || (gameVar.cl_teamIndicatorType == 2 && teamID == game->thisPlayer->teamID) || (gameVar.cl_teamIndicatorType > 0 && game->thisPlayer->teamID == PLAYER_TEAM_SPECTATOR) ) )
			{
				//--- Get up & right vectors
				float modelview[16];
#ifndef _DX_
				glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
				CVector3f up( modelview[1],  modelview[5],  modelview[9]);
				CVector3f right( modelview[0],  modelview[4],  modelview[8]);

				float size = gameVar.cl_glowSize;
				CVector3f a,b,c,d;
				a =  (right + up) * -size;
				b =  (right - up) * size;
				c =  (right + up) * size;
				d =  (right - up) * -size;
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				if (game->gameType != GAME_TYPE_DM)
				{
					if(teamID == PLAYER_TEAM_RED)
					{
						glColor3f(1, 0, 0);
					}
					else if(teamID == PLAYER_TEAM_BLUE)
					{
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor3f(0, 0, 1);
					}
					else
					{
						glColor3f(1, 1, 1);
					}
				}
				else
				{
					glColor3f(1, 1, 1);
				}
	 
	 
				glPushMatrix();
				glTranslatef(currentCF.position[0], currentCF.position[1], currentCF.position[2]);
				glBindTexture(GL_TEXTURE_2D, tex_baboHalo);
				glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex3f(a[0], a[1], a[2]);
				glTexCoord2f(0,0);
				glVertex3f(b[0], b[1], b[2]);
				glTexCoord2f(1,0);
				glVertex3f(c[0], c[1], c[2]);
				glTexCoord2f(1,1);
				glVertex3f(d[0], d[1], d[2]);
				glEnd();
				glPopMatrix();
#ifdef _PRO_
				if(minibot)
				{
					a = a*0.5f;
					b = b*0.5f;
					c = c*0.5f;
					d = d*0.5f;
					glPushMatrix();
					glTranslatef(minibot->currentCF.position[0], minibot->currentCF.position[1], minibot->currentCF.position[2]);
					glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(a[0], a[1], a[2]);
					glTexCoord2f(0,0);
					glVertex3f(b[0], b[1], b[2]);
					glTexCoord2f(1,0);
					glVertex3f(c[0], c[1], c[2]);
					glTexCoord2f(1,1);
					glVertex3f(d[0], d[1], d[2]);
					glEnd();
					glPopMatrix();
				}
#endif
#endif
			}

			// La boule
#ifndef _DX_
			glDepthMask(GL_TRUE);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
			glDepthFunc(GL_LEQUAL);
			glPushMatrix();
				glTranslatef(currentCF.position[0], currentCF.position[1], currentCF.position[2]);
				matrix.MultOglMatrix();
				glEnable(GL_COLOR_MATERIAL);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tex_skin);
				glColor3f(1,1,1);
				glPolygonMode(GL_FRONT, GL_FILL);
				gluQuadricTexture(qObj, true);
				gluSphere(qObj, .25f, 16, 16);

				//--- On pogne la position sur l'�ran
				CVector3f screenPos = dkglProject(CVector3f(0,0,0));
				CVector2i res = dkwGetResolution();
				onScreenPos[0] = (int)screenPos[0];
				onScreenPos[1] = res[1] - (int)screenPos[1];
				if(gameVar.r_widescreen > 1) onScreenPos[0] = static_cast<int>(onScreenPos[0] - (res[0] - res[1]*1.333f)/2);
			glPopMatrix();

			glPushMatrix();
				// Draw the gun
				glPolygonMode(GL_FRONT, GL_FILL);
				glTranslatef(currentCF.position[0], currentCF.position[1], 0);
				glRotatef(currentCF.angle, 0, 0, 1);
				glScalef(0.005f, 0.005f, 0.005f);
				if (weapon) weapon->render();
				if (meleeWeapon)
				{
					if(meleeWeapon->weaponID == WEAPON_NUCLEAR)
						dkoRender(meleeWeapon->dkoModel);//this way it won't flash on the babo
					else
						meleeWeapon->render();
				}
				glRotatef(-90,0,0,1);

			glPopMatrix();



#ifdef _PRO_
			//--- Minibot !!!
			if (minibot)
			{
				glPushMatrix();
					glTranslatef(minibot->currentCF.position[0], minibot->currentCF.position[1], minibot->currentCF.position[2]);
					minibot->matrix.MultOglMatrix();
					glEnable(GL_COLOR_MATERIAL);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, tex_skin);
					glColor3f(1,1,1);
					glPolygonMode(GL_FRONT, GL_FILL);
					gluQuadricTexture(qObj, true);
					gluSphere(qObj, .15f, 8, 8);
				glPopMatrix();

				glPushMatrix();
					// Draw the gun (SMG) + little antena too
					glPolygonMode(GL_FRONT, GL_FILL);
					glTranslatef(minibot->currentCF.position[0], minibot->currentCF.position[1], 0);
					glRotatef(minibot->currentCF.angle, 0, 0, 1);
					glScalef(0.003f, 0.003f, 0.003f);
					if(minibot->nukeBot)
						meleeWeapon->render();//the nuke will flash on the nukebot
					else
						gameVar.weapons[WEAPON_SMG]->render();
					gameVar.weapons[WEAPON_MINIBOT]->render();

					glRotatef(-90,0,0,1);

				glPopMatrix();
			}
#endif

			// Le flag si c'est le cas
			if (game->gameType == GAME_TYPE_CTF)
			{
				if (game->map)
				{
					if (game->map->flagState[0] == playerID)
					{
						game->map->flagAngle[0] = currentCF.angle-90;
						//game->map->renderFlag(0);
						//dkoRender(game->map->dko_flag[0], game->map->flagAnim);
					}
					if (game->map->flagState[1] == playerID)
					{
						game->map->flagAngle[1] = currentCF.angle-90;
						//game->map->renderFlag(1);
						//dkoRender(game->map->dko_flag[1], game->map->flagAnim);
					}
				}
			}
		glPopAttrib();
#endif
	}
}

void Player::renderName()
{
	if (gameVar.sv_showEnemyTag && game->thisPlayer)
	{
		if (!isThisPlayer && teamID != PLAYER_TEAM_SPECTATOR && teamID != game->thisPlayer->teamID && game->gameType != GAME_TYPE_DM && game->gameType != GAME_TYPE_SND)
		{
			//--- We don't print it !!!!!!
			return;
		}
	}
	if (status == PLAYER_STATUS_ALIVE)
	{
		CVector2i res = dkwGetResolution();
		if (onScreenPos[0] > 0 && onScreenPos[0] < res[0] &&
			onScreenPos[1] > 0 && onScreenPos[1] < res[1])
		{
			CString showName;
			if (ping > 12 && scene->client->blink < .25f) showName.insert(CString(" \x5%s", gameVar.lang_laggerC.s).s, 0);
			if (gameVar.r_maxNameLenOverBabo > 0 && name.len() > gameVar.r_maxNameLenOverBabo)
			{
				CString sname(CString("%%.%is[...]", gameVar.r_maxNameLenOverBabo).s, name.s);
				showName.insert(sname.s, 0);
			}
			else
				showName.insert(name.s, 0);
			showName.insert("\x8",0);
			if (ping > 12 && scene->client->blink < .25f) showName.insert(CString("\x5%s ", gameVar.lang_laggerC.s).s, 0);
			printCenterText((float)onScreenPos[0], (float)onScreenPos[1]-28, 28,showName);
		}
	}

	//--- The life of this player
	if (game->thisPlayer && status == PLAYER_STATUS_ALIVE)
	{
		if ((!isThisPlayer && teamID == game->thisPlayer->teamID && game->gameType != GAME_TYPE_DM && game->gameType != GAME_TYPE_SND)  ||
			teamID == PLAYER_TEAM_SPECTATOR)
		{
#ifndef _DX_
			glColor3f(1,1,1);
			renderTexturedQuad(onScreenPos[0] - 15, onScreenPos[1] - 8, 30, 7, 0);
			glColor3f(0,0,0);
			renderTexturedQuad(onScreenPos[0] - 14, onScreenPos[1] - 7, 28, 5, 0);
			if (life > .25f || scene->client->blink < .25f)
			{
				glColor3f(1-life,life,0);
				renderTexturedQuad(onScreenPos[0] - 14, onScreenPos[1] - 7, (int)(life*28.0f), 5, 0);
			}
#endif
		}
	}
}
#endif



#ifndef CONSOLE
//
// Pour updater la skin texture
//
void Player::updateSkin()
{
	CColor3f redDecalT;
	CColor3f greenDecalT;
	CColor3f blueDecalT;
	CString skinT;

	//--- Ici c'est nowhere on update les couleurs lol
	//--- Si � chang�on update � au autres joueur!
	if (isThisPlayer)
	{
		redDecalT = gameVar.cl_redDecal;
		greenDecalT = gameVar.cl_greenDecal;
		blueDecalT = gameVar.cl_blueDecal;
		skinT = gameVar.cl_skin;
	}
	else
	{
		redDecalT = redDecal;
		greenDecalT = greenDecal;
		blueDecalT = blueDecal;
		skinT = skin;
	}

	//--- On reload le skin si � chang�
	if (lastSkin != skinT)
	{
		skin = skinT;
		dktDeleteTexture(&tex_skinOriginal);
		tex_skinOriginal = dktCreateTextureFromFile(CString("main/skins/%s.tga", skin.s).s, DKT_FILTER_BILINEAR);
	}

	redDecal = redDecalT;
	greenDecal = greenDecalT;
	blueDecal = blueDecalT;
	lastSkin = skin;

	//--- Hey oui, on recr�une texture ogl �chaque fois pour chaque babo qui spawn!!!!
	//--- On est en ogl, faq � kick ass MOUHOUHOUHAHAHA
	unsigned char imgData[64*32*3];
	dktGetTextureData(tex_skinOriginal, imgData);

	//--- Celon son team, on set la couleur du babo en cons�uence
#ifdef _PRO_
   if ((game->gameType != GAME_TYPE_DM) && (game->gameType != GAME_TYPE_SND) && gameVar.cl_teamIndicatorType == 0)
#else
	if (game->gameType != GAME_TYPE_DM)
#endif
	{
		switch (teamID)
		{
		case PLAYER_TEAM_RED:
			{
				redDecalT.set(1,.5f,.5f);
				greenDecalT.set(1,.0f,.0f);
				blueDecalT.set(.5f,0,0);
				break;
			}
		case PLAYER_TEAM_BLUE:
			{
				redDecalT.set(.5f,.5f,1);
				greenDecalT.set(0,0,1);
				blueDecalT.set(0,0,.5f);
				break;
			}
		}
	}
	else
	{
		//--- Sinon on prend les couleurs que le gars a mis
		redDecalT = redDecal;
		greenDecalT = greenDecal;
		blueDecalT = blueDecal;
	}

	int i,j,k;
	float r,g,b;
	CColor3f finalColor;
	for (j=0;j<32;++j)
	{
		for (i=0;i<64;++i)
		{
			k = ((j*64) + i) * 3;
			r = (float)imgData[k + 0] / 255.0f;
			g = (float)imgData[k + 1] / 255.0f;
			b = (float)imgData[k + 2] / 255.0f;
			finalColor = (redDecalT * r + greenDecalT * g + blueDecalT * b) / (r+g+b);
			imgData[k + 0] = (unsigned char)(finalColor[0] * 255.0f);
			imgData[k + 1] = (unsigned char)(finalColor[1] * 255.0f);
			imgData[k + 2] = (unsigned char)(finalColor[2] * 255.0f);
		}
	}

	// update
	dktCreateTextureFromBuffer(&tex_skin, imgData, 64, 32, 3, DKT_FILTER_BILINEAR);
}
#endif



//
// Pour spawner un joueur y�
//
void Player::spawn(const CVector3f & spawnPoint)
{
	status = PLAYER_STATUS_ALIVE;
	life = 1; // Full of life
	timeToSpawn = gameVar.sv_timeToSpawn;

	timeDead = 0.0f;
	timeAlive = 0.0f;
	timeIdle = 0.0f;

	matrix.LoadIdentity();

	spawnRequested = false;

	currentCF.position = spawnPoint;
	currentCF.vel.set(0,0,0);
	currentCF.angle = 0;

	lastCF = currentCF;
	netCF0 = currentCF;
	netCF1 = currentCF;
	netCF0.reset();
	netCF1.reset();
	cFProgression = 0;

	grenadeDelay = 0;
	meleeDelay = 0;
	nbGrenadeLeft = 2;
	nbMolotovLeft = 1;


	switchWeapon(nextSpawnWeapon);
	switchMeleeWeapon(nextMeleeWeapon);

	//--- Si c'est nous on force la camera dessus
#ifndef CONSOLE
	if (isThisPlayer) map->setCameraPos(spawnPoint);

	if (!game->isServerGame)
	{
		updateSkin();
	}
#endif
}



//
// Pour remettre ses stats �0
//
void Player::reinit()
{
	timeIdle = 0.0f;
	dmg = 0;
	kills = 0;
	deaths = 0;
	score = 0;
	returns = 0;
	damage = 0;
	ping = 0;
	pingSum = 0;
	avgPing = 0;
	babySitTime = 5.0f;
	pingOverMax = 0.0f;
	flagAttempts = 0;
	timePlayedCurGame = 0.0f;

#ifdef _PRO_
   spawnSlot = -1;
#endif

}



//
// Pour changer son gun!
//
void Player::switchWeapon(int newWeaponID, bool forceSwitch)
{
	if (weapon && forceSwitch)
	{
		if (weapon->weaponID == newWeaponID) return;
	}
	// Bon testing, on va lui refiler un gun
	ZEVEN_SAFE_DELETE(weapon);
	weapon = new Weapon(gameVar.weapons[newWeaponID]);
	weapon->currentFireDelay = 1; // On a une 1sec de delait quand on switch de gun
	weapon->m_owner = this;

	gameVar.cl_primaryWeapon = newWeaponID;

	// On entends �
#ifndef CONSOLE
	if (scene->client) dksPlay3DSound(gameVar.sfx_equip, -1, 1, currentCF.position,255);
#endif

	// Reset rapid-fire hack check
	shotCount = 0;
	shotsPerSecond = 0;
}



//
// Pour changer son melee gun!
//
void Player::switchMeleeWeapon(int newWeaponID, bool forceSwitch)
{
	if (meleeWeapon && forceSwitch)
	{
		if (meleeWeapon->weaponID == newWeaponID) return;
	}
	// Bon testing, on va lui refiler un gun
	ZEVEN_SAFE_DELETE(meleeWeapon);
	meleeWeapon = new Weapon(gameVar.weapons[newWeaponID]);
	meleeWeapon->currentFireDelay = 0;
	meleeWeapon->m_owner = this;

	gameVar.cl_secondaryWeapon = newWeaponID - WEAPON_KNIVES;
}


#ifndef CONSOLE
//
// Si on se fait toucher !
//
void Player::hit(Weapon * fromWeapon, Player * from, float damage)
{
	float cdamage = life - damage; // La diff�ence :) (boom headshot)
	if (damage == -1) cdamage = fromWeapon->damage; // C'est pus possible �

	if (status == PLAYER_STATUS_ALIVE)
	{
		// On check si c'est ff, ou reflect, etc
		if (from->teamID == teamID && game->gameType != GAME_TYPE_DM && game->gameType != GAME_TYPE_SND)
		{
			if (gameVar.sv_friendlyFire || from->playerID == playerID || game->gameType == GAME_TYPE_DM)
			{
				dksPlay3DSound(gameVar.sfx_hit[rand()%2], -1, 5, currentCF.position, 255);
				game->spawnBlood(currentCF.position, cdamage);
				if (from != this)
               {               
                  from->dmg += (cdamage<life)?cdamage:life;
               }
				life -= cdamage;
				screenHit += cdamage;
				if (screenHit > 1.0) screenHit = 1.0;
				if (cdamage > 1) screenHit = 0;
				if (from->isThisPlayer) 
				{
					scene->client->hitIndicator = 1;
					dksPlaySound(scene->client->sfxHit, -1, 250);
				}
			}
			if (gameVar.sv_reflectedDamage && from->playerID != playerID) 
			{
				from->hit(fromWeapon, from, damage);
			}

			// Oups, on cr�e?
			if (life <= std::numeric_limits<float>::epsilon())
			{
				if (scene->client)
				{
					CString message = /*textColorLess*/(name);
					switch (teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("{",0);break;
					case PLAYER_TEAM_RED:message.insert("}",0);break;
					}
					message.insert("-----> ", 0);
					message.insert(gameVar.lang_friendlyFire.s, 0);
					message.insert("\x8 -----", 0);
					message.insert(/*textColorLess*/(from->name).s, 0);
					switch (from->teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("{",0);break;
					case PLAYER_TEAM_RED:message.insert("}",0);break;
					}
					console->add(message);
					scene->client->eventMessages.push_back(TimedMessage(message));
				}
				kill(false);
				if(game->gameType == GAME_TYPE_TDM)
				{
					if (from->teamID == PLAYER_TEAM_BLUE)
						game->blueScore--;
					else if(from->teamID == PLAYER_TEAM_RED)
						game->redScore--;
				}//If we do a friendly fire kill, reduce score
				if ((game->gameType == GAME_TYPE_DM || game->gameType == GAME_TYPE_SND) && from != this) 
				{
					if (from->teamID == PLAYER_TEAM_BLUE)
					{
						game->blueScore++;
					}
					else if (from->teamID == PLAYER_TEAM_RED)
					{
						game->redScore++;
					}
					if(game->gameType != GAME_TYPE_CTF)
						from->score++;
					from->kills++;
					deaths++;
				}
				else 
				{
					from->deaths++;
					if(game->gameType != GAME_TYPE_CTF)
						from->score--;
				}
			}
		}
		else
		{
			dksPlay3DSound(gameVar.sfx_hit[rand()%2], -1, 5, currentCF.position, 255);
			game->spawnBlood(currentCF.position, cdamage);
			if (from != this)
				from->dmg += (cdamage<life)?cdamage:life;
			life -= cdamage;
			screenHit += cdamage;
			if (screenHit > 1.0) screenHit = 1.0;
			if (cdamage > 1) screenHit = 0;
			if (from->isThisPlayer) 
			{
				scene->client->hitIndicator = 1;
				dksPlaySound(scene->client->sfxHit, -1, 250);
			}

			// Oups, on cr�e?
			if (life <= std::numeric_limits<float>::epsilon())
			{
				if (scene->client)
				{
					CString message = /*textColorLess*/(name);
					switch (teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("{",0);break;
					case PLAYER_TEAM_RED:message.insert("}",0);break;
					}
					message.insert("-----> ", 0);
					message.insert(fromWeapon->weaponName.s, 0);
					message.insert("\x8 -----", 0);
					message.insert(/*textColorLess*/(from->name).s, 0);
					switch (from->teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("{",0);break;
					case PLAYER_TEAM_RED:message.insert("}",0);break;
					}
					console->add(message);
					scene->client->eventMessages.push_back(TimedMessage(message));
				}
				kill(false);
				if (from != this) 
				{
					if (from->teamID == PLAYER_TEAM_BLUE && game->gameType != GAME_TYPE_CTF)
					{
						game->blueScore++;
					}
					else if (from->teamID == PLAYER_TEAM_RED && game->gameType != GAME_TYPE_CTF)
					{
						game->redScore++;
					}
					if(game->gameType != GAME_TYPE_CTF)
						from->score++;
					from->kills++;
					deaths++;
				}
				else 
				{
					from->deaths++;
					if(game->gameType != GAME_TYPE_CTF)
					{
						from->kills--;
						from->score--;
					}
				}
			}
		}
	}
}
#endif


//
// Ici c'est pour �iter de faire des sons pis toute, vu qu'on est le server
//
void Player::hitSV(Weapon * fromWeapon, Player * from, float damage)
{
	float cdamage = damage;
	if (damage == -1) 
	{
		if(gameVar.sv_serverType == 1)
		{
			switch(fromWeapon->weaponID)
			{
			case WEAPON_SMG:
				cdamage = gameVar.sv_smgDamage;
				break;
			case WEAPON_SNIPER:
				cdamage = gameVar.sv_sniperDamage;
				break;
			case WEAPON_SHOTGUN:
				cdamage = gameVar.sv_shottyDamage;
				break;
			case WEAPON_DUAL_MACHINE_GUN:
				cdamage = gameVar.sv_dmgDamage;
				break;
			case WEAPON_CHAIN_GUN:
				cdamage = gameVar.sv_cgDamage;
				break;
			default:
				cdamage = fromWeapon->damage;
			};
		}
		else
		{
			cdamage = fromWeapon->damage;
		}
	}
	if(fromWeapon->weaponID == WEAPON_PHOTON_RIFLE && gameVar.sv_serverType == 1 && from != this)
	{
		CVector3f pos1 = from->weapon->shotFrom;
		CVector3f pos2 = currentCF.position;
		CVector3f dist = pos2 - pos1;
		float distance = dist.length();
		switch(gameVar.sv_photonType)
		{
		case 1://a+(pi/2-arctan(x-b)c)d
			cdamage = cdamage*(gameVar.sv_photonVerticalShift + gameVar.sv_photonDamageCoefficient*(PI/2 - atanf((distance - gameVar.sv_photonHorizontalShift)*gameVar.sv_photonDistMult)));
			break;
		case 2://(a-d/(x-b)c)
			if (distance == 0)
				distance = 0.0000000000000000000000000000000000000000000000000000000001f;
			cdamage = cdamage*(gameVar.sv_photonVerticalShift+gameVar.sv_photonDamageCoefficient/((distance-gameVar.sv_photonHorizontalShift)*gameVar.sv_photonDistMult));
			break;
		case 3:
			cdamage = cdamage*(gameVar.sv_photonVerticalShift + gameVar.sv_photonDamageCoefficient/(1 + pow((distance - gameVar.sv_photonHorizontalShift)*gameVar.sv_photonDistMult,2)));
			break;
		default://constant damage
			cdamage = cdamage*gameVar.sv_photonDamageCoefficient;
			break;
		};
	}
	if(fromWeapon->weaponID == WEAPON_FLAME_THROWER && gameVar.sv_serverType == 1 && from != this)
	{
		cdamage = gameVar.sv_ftDamage;
		CVector3f pos1 = from->weapon->shotFrom;
		CVector3f pos2 = currentCF.position;
		CVector3f dist = pos2 - pos1;
		float distance = dist.length();
		cdamage = (1 - distance/gameVar.sv_ftMaxRange)*cdamage;
	}

	//--- Shield protection!!!
	if (protection > .6f)
	{
		cdamage *= .50f; // Shield power !
	}

#ifdef _PRO_
   if ((gameVar.sv_subGameType == SUBGAMETYPE_INSTAGIB) && (fromWeapon->weaponID != WEAPON_GRENADE) &&  (fromWeapon->weaponID != WEAPON_KNIVES) && (fromWeapon->weaponID != WEAPON_COCKTAIL_MOLOTOV))
      {
      cdamage = life; 
      }
#endif

	if (status == PLAYER_STATUS_ALIVE)
	{
		// On check si c'est ff, ou reflect, etc
		if (from->teamID == teamID && game->gameType != GAME_TYPE_DM && game->gameType != GAME_TYPE_SND)
		{
			if (gameVar.sv_friendlyFire || from->playerID == playerID || game->gameType == GAME_TYPE_DM || game->gameType == GAME_TYPE_SND)
			{
				if (from != this)
					from->dmg += (cdamage<life)?cdamage:life;
				life -= cdamage;
				screenHit += cdamage;

				//--- On doit shooter � au clients
				net_svcl_player_hit playerHit;
				playerHit.damage = life;
				playerHit.playerID = playerID;
				playerHit.fromID = (char)from->playerID;
				playerHit.weaponID = fromWeapon->weaponID;
				playerHit.vel[0] = 0;
				playerHit.vel[1] = 0;
				playerHit.vel[2] = 1;
				bb_serverSend((char*)&playerHit,sizeof(net_svcl_player_hit),NET_SVCL_PLAYER_HIT,0);
			}
			if (gameVar.sv_reflectedDamage && from->playerID != playerID) 
			{
					from->hitSV(fromWeapon, from, cdamage);
			}

			// Oups, on cr�e?
			if (life <= std::numeric_limits<float>::epsilon())
			{
#ifndef CONSOLE
				if (scene->client || (scene->server && gameVar.sv_showKills))
#else
				if (scene->server && gameVar.sv_showKills)
#endif
				{
#ifndef CONSOLE
					CString message = name;
					switch (teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("\x1",0);break;
					case PLAYER_TEAM_RED:message.insert("\x4",0);break;
					}
					message.insert("-----> ", 0);
					message.insert(gameVar.lang_friendlyFire.s, 0);
					message.insert("\x8 -----", 0);
					message.insert(from->name.s, 0);
					switch (from->teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("\x1",0);break;
					case PLAYER_TEAM_RED:message.insert("\x4",0);break;
					}
#else
					CString message ("Player id:%d killed player id:%d with weapon id:%d", this->playerID, from->playerID, fromWeapon->weaponID);
#endif
					console->add(message);
				}
				if (game->isServerGame)
				{
					// On spawn un pack de vie
					net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.nuzzleID = 0;
					playerProjectile.playerID = playerID;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					CVector3f pVel(0,0,1);
					pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
					pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
					pVel += currentCF.vel * .25f;
					playerProjectile.vel[0] = (char)(pVel[0] * 10);
					playerProjectile.vel[1] = (char)(pVel[1] * 10);
					playerProjectile.vel[2] = (char)(pVel[2] * 10);
					playerProjectile.weaponID = 0;
					playerProjectile.projectileType = PROJECTILE_LIFE_PACK;
			//		playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
					game->spawnProjectile(playerProjectile, true);
					bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					playerProjectile.nuzzleID = 0;
					playerProjectile.playerID = playerID;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					pVel.set(0,0,1);
					pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
					pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
					pVel += currentCF.vel * .25f;
					playerProjectile.vel[0] = (char)(pVel[0] * 10);
					playerProjectile.vel[1] = (char)(pVel[1] * 10);
					playerProjectile.vel[2] = (char)(pVel[2] * 10);
					playerProjectile.weaponID = weapon->weaponID;
					playerProjectile.projectileType = PROJECTILE_DROPED_WEAPON;
				//	playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
					game->spawnProjectile(playerProjectile, true);
					bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					for (int i=0;i<this->nbGrenadeLeft;++i)
					{
						playerProjectile.nuzzleID = 0;
						playerProjectile.playerID = playerID;
						playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
						playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
						playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
						pVel.set(0,0,1);
						pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
						pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
						pVel += currentCF.vel * .25f;
						playerProjectile.vel[0] = (char)(pVel[0] * 10);
						playerProjectile.vel[1] = (char)(pVel[1] * 10);
						playerProjectile.vel[2] = (char)(pVel[2] * 10);
						playerProjectile.weaponID = 0;
						playerProjectile.projectileType = PROJECTILE_DROPED_GRENADE;
				//		playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
						game->spawnProjectile(playerProjectile, true);
						bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					}
				}
				kill(true);
				if (game->gameType == GAME_TYPE_DM && from != this) 
				{
					if (from->teamID == PLAYER_TEAM_BLUE)
					{
						game->blueScore++;
					}
					else if (from->teamID == PLAYER_TEAM_RED)
					{
						game->redScore++;
					}
					if(game->gameType != GAME_TYPE_CTF)
						from->score++;
					from->kills++;
					deaths++;
				}
				else 
				{
					if(game->gameType == GAME_TYPE_TDM)
					{
						if (from->teamID == PLAYER_TEAM_BLUE)
						{
							game->blueScore--;
						}
						else if (from->teamID == PLAYER_TEAM_RED)
						{
							game->redScore--;
						}
					}
					from->deaths++;
					if(game->gameType != GAME_TYPE_CTF)
						from->score--;
				}
			}
		}
		else
		{
			if (from != this)
				from->dmg += (cdamage<life)?cdamage:life;
			life -= cdamage;
			screenHit += cdamage;

			//--- On doit shooter � au clients
			net_svcl_player_hit playerHit;
			playerHit.damage = life;
			playerHit.playerID = playerID;
			playerHit.fromID = (char)from->playerID;
			playerHit.weaponID = fromWeapon->weaponID;
			playerHit.vel[0] = 0;
			playerHit.vel[1] = 0;
			playerHit.vel[2] = 1;
			bb_serverSend((char*)&playerHit,sizeof(net_svcl_player_hit),NET_SVCL_PLAYER_HIT,0);

			// Oups, on cr�e?
			if (life <= std::numeric_limits<float>::epsilon())    
			{
#ifndef CONSOLE
				if (scene->client || (scene->server && gameVar.sv_showKills))
#else
				if (scene->server && gameVar.sv_showKills)
#endif
				{
#ifndef CONSOLE
					CString message = name;
					switch (teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("\x1",0);break;
					case PLAYER_TEAM_RED:message.insert("\x4",0);break;
					}
					message.insert("-----> ", 0);
					message.insert(fromWeapon->weaponName.s, 0);
					message.insert("\x8 -----", 0);
					message.insert(from->name.s, 0);
					switch (from->teamID)
					{
					case PLAYER_TEAM_BLUE:message.insert("\x1",0);break;
					case PLAYER_TEAM_RED:message.insert("\x4",0);break;
					}
#else
					CString message ("Player id:%d killed player id:%d with weapon id:%d", this->playerID, from->playerID, fromWeapon->weaponID);
#endif
					console->add(message);
				}
				if (game->isServerGame)
				{
					// On spawn un pack de vie
					net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.nuzzleID = 0;
					playerProjectile.playerID = playerID;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					CVector3f pVel(0,0,1);
					pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
					pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
					pVel += currentCF.vel * .25f;
					playerProjectile.vel[0] = (char)(pVel[0] * 10);
					playerProjectile.vel[1] = (char)(pVel[1] * 10);
					playerProjectile.vel[2] = (char)(pVel[2] * 10);
					playerProjectile.weaponID = 0;
					playerProjectile.projectileType = PROJECTILE_LIFE_PACK;
			//		playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
					game->spawnProjectile(playerProjectile, true);
					bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					playerProjectile.nuzzleID = 0;
					playerProjectile.playerID = playerID;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					pVel.set(0,0,1);
					pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
					pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
					pVel += currentCF.vel * .25f;
					playerProjectile.vel[0] = (char)(pVel[0] * 10);
					playerProjectile.vel[1] = (char)(pVel[1] * 10);
					playerProjectile.vel[2] = (char)(pVel[2] * 10);
					playerProjectile.weaponID = weapon->weaponID;
					playerProjectile.projectileType = PROJECTILE_DROPED_WEAPON;
				//	playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
					game->spawnProjectile(playerProjectile, true);
					bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					for (int i=0;i<this->nbGrenadeLeft;++i)
					{
						playerProjectile.nuzzleID = 0;
						playerProjectile.playerID = playerID;
						playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
						playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
						playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
						pVel.set(0,0,1);
						pVel = rotateAboutAxis(pVel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
						pVel = rotateAboutAxis(pVel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
						pVel += currentCF.vel * .25f;
						playerProjectile.vel[0] = (char)(pVel[0] * 10);
						playerProjectile.vel[1] = (char)(pVel[1] * 10);
						playerProjectile.vel[2] = (char)(pVel[2] * 10);
						playerProjectile.weaponID = 0;
						playerProjectile.projectileType = PROJECTILE_DROPED_GRENADE;
				//		playerProjectile.uniqueProjectileID = ++(game->uniqueProjectileID);
						game->spawnProjectile(playerProjectile, true);
						bb_serverSend((char*)&playerProjectile, sizeof(net_clsv_svcl_player_projectile), NET_CLSV_SVCL_PLAYER_PROJECTILE, 0);
					}
				}
				kill(true);
				if (from != this) 
				{
					if (from->teamID == PLAYER_TEAM_BLUE && game->gameType != GAME_TYPE_CTF)
					{
						game->blueScore++;
					}
					else if (from->teamID == PLAYER_TEAM_RED && game->gameType != GAME_TYPE_CTF)
					{
						game->redScore++;
					}
					if(game->gameType != GAME_TYPE_CTF)
						from->score++;
					from->kills++;
					deaths++;
				}
				else 
				{
					from->deaths++;
					if(game->gameType != GAME_TYPE_CTF)
					{
						from->kills--;
						from->score--;
					}
				}
			}
		}
	}
}



//
// Pour lui donner les info de notre joueur
// � c'est pour lui donner mon nom ex : Daivuk, etc
//
void Player::setThisPlayerInfo()
{
	remoteEntity = false;
#ifndef CONSOLE
	isThisPlayer = true;
#endif
	name = gameVar.cl_playerName;
}



//
// Pour setter le coordframe du player
//
void Player::setCoordFrame(net_clsv_svcl_player_coord_frame & playerCoordFrame)
{
	if (playerID != playerCoordFrame.playerID) return; // Wtf c pas le bon player!? (Pas suposer arriver)

	// On check si ce n'est pas un out of order data
	if (netCF1.frameID > playerCoordFrame.frameID) return;

	// Notre dernier keyframe change pour celui qu'on est rendu
	netCF0 = currentCF;
	netCF0.frameID = netCF1.frameID; // On pogne le frameID de l'ancien packet par contre
	cFProgression = 0; // On commence au d�ut de la courbe ;)

	// On donne la nouvelle velocity �notre entity
	currentCF.vel[0] = (float)playerCoordFrame.vel[0] / 10.0f;
	currentCF.vel[1] = (float)playerCoordFrame.vel[1] / 10.0f;
	currentCF.vel[2] = (float)playerCoordFrame.vel[2] / 10.0f;

#ifdef _PRO_
	currentCF.camPosZ = (float)playerCoordFrame.camPosZ;
#endif

	// Son frame ID
	netCF1.frameID = playerCoordFrame.frameID;

	// Va faloir interpoler ici et pr�ire (job's done!)
	netCF1.position[0] = (float)playerCoordFrame.position[0] / 100.0f;
	netCF1.position[1] = (float)playerCoordFrame.position[1] / 100.0f;
	netCF1.position[2] = (float)playerCoordFrame.position[2] / 100.0f;

	// Sa velocity (� aussi va faloir l'interpoler jcr�ben
	netCF1.vel[0] = (char)playerCoordFrame.vel[0] / 10.0f;
	netCF1.vel[1] = (char)playerCoordFrame.vel[1] / 10.0f;
	netCF1.vel[2] = (char)playerCoordFrame.vel[2] / 10.0f;

	// La position de la mouse
	netCF1.mousePosOnMap[0] = (short)playerCoordFrame.mousePos[0] / 100.0f;
	netCF1.mousePosOnMap[1] = (short)playerCoordFrame.mousePos[1] / 100.0f;
	netCF1.mousePosOnMap[2] = (short)playerCoordFrame.mousePos[2] / 100.0f;

	// Si notre frameID �ait �0, on le copie direct
	if (netCF0.frameID == 0) 
	{
		netCF0 = netCF1;
	}
}

#ifdef _PRO_
void Player::setCoordFrameMinibot(net_svcl_minibot_coord_frame & minibotCoordFrame)
{
	if (playerID != minibotCoordFrame.playerID) return; // Wtf c pas le bon player!? (Pas suposer arriver)

	//--- Create the minibot if doesn't exist so we don't
	//	  have to send it in initial game state :)
	if (!minibot)
	{
		CVector3f pos;
		CVector3f mousePos;

		pos[0] = (float)minibotCoordFrame.position[0] / 100.0f;
		pos[1] = (float)minibotCoordFrame.position[1] / 100.0f;
		pos[2] = (float)minibotCoordFrame.position[2] / 100.0f;

		mousePos[0] = (short)minibotCoordFrame.mousePos[0] / 100.0f;
		mousePos[1] = (short)minibotCoordFrame.mousePos[1] / 100.0f;
		mousePos[2] = (short)minibotCoordFrame.mousePos[2] / 100.0f;
		
		SpawnMiniBot(pos, mousePos);
	}

	// On check si ce n'est pas un out of order data
	if (minibot->netCF1.frameID > minibotCoordFrame.frameID) return;

	// Notre dernier keyframe change pour celui qu'on est rendu
	minibot->netCF0 = minibot->currentCF;
	minibot->netCF0.frameID = minibot->netCF1.frameID; // On pogne le frameID de l'ancien packet par contre
	minibot->cFProgression = 0; // On commence au d�ut de la courbe ;)

	// On donne la nouvelle velocity �notre entity
	minibot->currentCF.vel[0] = (float)minibotCoordFrame.vel[0] / 10.0f;
	minibot->currentCF.vel[1] = (float)minibotCoordFrame.vel[1] / 10.0f;
	minibot->currentCF.vel[2] = (float)minibotCoordFrame.vel[2] / 10.0f;

	// Son frame ID
	minibot->netCF1.frameID = minibotCoordFrame.frameID;

	// Va faloir interpoler ici et pr�ire (job's done!)
	minibot->netCF1.position[0] = (float)minibotCoordFrame.position[0] / 100.0f;
	minibot->netCF1.position[1] = (float)minibotCoordFrame.position[1] / 100.0f;
	minibot->netCF1.position[2] = (float)minibotCoordFrame.position[2] / 100.0f;

	// Sa velocity (� aussi va faloir l'interpoler jcr�ben
	minibot->netCF1.vel[0] = (char)minibotCoordFrame.vel[0] / 10.0f;
	minibot->netCF1.vel[1] = (char)minibotCoordFrame.vel[1] / 10.0f;
	minibot->netCF1.vel[2] = (char)minibotCoordFrame.vel[2] / 10.0f;

	// La position de la mouse
	minibot->netCF1.mousePosOnMap[0] = (short)minibotCoordFrame.mousePos[0] / 100.0f;
	minibot->netCF1.mousePosOnMap[1] = (short)minibotCoordFrame.mousePos[1] / 100.0f;
	minibot->netCF1.mousePosOnMap[2] = (short)minibotCoordFrame.mousePos[2] / 100.0f;

	// Si notre frameID �ait �0, on le copie direct
	if (minibot->netCF0.frameID == 0) 
	{
		minibot->netCF0 = minibot->netCF1;
	}
}
#endif


