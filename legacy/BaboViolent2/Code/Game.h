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

#ifndef GAME_H
#define GAME_H


#include "Player.h"
#include "Map.h"
#include <vector>
#include <map>
#include "netPacket.h"
#include "MemIO.h"


class Client;


#define MAX_PLAYER 32
#ifndef DEDICATED_SERVER
#define MAX_FLOOR_MARK 500
#endif
#define GAME_TYPE_COUNT 4
#define GAME_TYPE_DM 0
#define GAME_TYPE_TDM 1
#define GAME_TYPE_CTF 2
#define GAME_TYPE_SND 3

#define SERVER_TYPE_NORMAL 0
#define SERVER_TYPE_PRO 1

#define SPAWN_TYPE_NORMAL 0
#define SPAWN_TYPE_LADDER 1

#define GAME_PLAYING -1
#define GAME_BLUE_WIN 0
#define GAME_RED_WIN 1
#define GAME_DRAW 2
#define GAME_DONT_SHOW 3
#define GAME_MAP_CHANGE 4

#define ROUND_BLUE_WIN 5
#define ROUND_RED_WIN 6

#define ITEM_LIFE_PACK 1
#define ITEM_WEAPON 2
#define ITEM_GRENADE 3


// Les sprites au sol! (le boute le fun)
#ifndef DEDICATED_SERVER
struct NukeFlash
{
public:
	CVector3f position;
	float life;
	float fadeSpeed;
	float density;
	float radius;
	NukeFlash()
	{
		life = 1;
		density = 2;
		fadeSpeed = .50f;
		radius = 16;
	}
	void update(float pdelay)
	{
		life -= pdelay * fadeSpeed;
	}
	void render()
	{
#ifndef _DX_
		glPushMatrix();
			glTranslatef(position[0], position[1], position[2]);
			glScalef(radius, radius, radius);
			glColor4f(1, 1, 1, density*life);
			glBegin(GL_QUADS);
				glTexCoord2i(0,1);
				glVertex2i(-1,1);
				glTexCoord2i(0,0);
				glVertex2i(-1,-1);
				glTexCoord2i(1,0);
				glVertex2i(1,-1);
				glTexCoord2i(1,1);
				glVertex2i(1,1);
			glEnd();
		glPopMatrix();
#endif
	}
};

struct Drip
{
	CVector3f position;
	float life;
	float size;
	float fadeSpeed;
	Drip()
	{
		life = 1;
		size = .15f;
		fadeSpeed = 2;
	}
	void update(float pdelay)
	{
		life -= pdelay * fadeSpeed;
	}
	void render()
	{
#ifndef _DX_
		glPushMatrix();
			glTranslatef(position[0], position[1], position[2]);
			float _size = (1 - life) * size;
			glScalef(_size, _size, _size);
			glColor4f(.25f, .7f, .3f, life*2);
			glBegin(GL_QUADS);
				glTexCoord2i(0,1);
				glVertex2i(-1,1);
				glTexCoord2i(0,0);
				glVertex2i(-1,-1);
				glTexCoord2i(1,0);
				glVertex2i(1,-1);
				glTexCoord2i(1,1);
				glVertex2i(1,1);
			glEnd();
		glPopMatrix();
#endif
	}
};

struct FloorMark
{
	CVector3f position;
	float angle;
	float size;
	float delay; // Sa vie restante
	float startDelay;
	unsigned int texture;
	CVector4f color;
	FloorMark()
	{
		delay = 0;
	}
	void set(CVector3f & pposition, float pangle, float psize, float pdelay, float pstartDelay, unsigned int ptexture, CVector4f pcolor)
	{
		position = pposition;
		angle = pangle;
		size = psize;
		delay = pdelay;
		startDelay = pstartDelay;
		texture = ptexture;
		color = pcolor;
	}
	void update(float pdelay)
	{
		if (startDelay > 0)
		{
			startDelay -= pdelay;
		}
		else
		{
			delay -= pdelay;
		}
	}
	void render()
	{
		if (startDelay <= 0)
		{
#ifndef _DX_
			glBindTexture(GL_TEXTURE_2D, texture);
			glPushMatrix();
				glTranslatef(position[0], position[1], position[2] + .025f);
				glRotatef(angle, 0, 0, 1);
				glScalef(size, size, size);
				if (delay < 10)
					glColor4f(color[0], color[1], color[2], color[3] * ((delay)*0.1f));
				else
					glColor4fv(color.s);
				glBegin(GL_QUADS);
					glTexCoord2i(0,1);
					glVertex2i(-1,1);
					glTexCoord2i(0,0);
					glVertex2i(-1,-1);
					glTexCoord2i(1,0);
					glVertex2i(1,-1);
					glTexCoord2i(1,1);
					glVertex2i(1,1);
				glEnd();
			glPopMatrix();
#endif
		}
	}
};

#define DOUILLE_TYPE_DOUILLE 0
#define DOUILLE_TYPE_GIB 1

// Nos douilles, ça on gère pas ça sur le net
struct Douille
{
	CVector3f position;
	CVector3f vel;
	float delay;
	bool soundPlayed;
	int type;
	Douille(CVector3f pPosition, CVector3f pDirection, CVector3f right, int in_type=DOUILLE_TYPE_DOUILLE)
	{
		type = in_type;
		vel = pDirection * 1.5f;
		if (type == DOUILLE_TYPE_DOUILLE)
		{
			delay = 2; // Ça dure 2sec ça, en masse
			vel = rotateAboutAxis(vel, rand(-30.0f, 30.0f), right);
			vel = rotateAboutAxis(vel, rand(0.0f, 360.0f), pDirection);
		}
		else if (type == DOUILLE_TYPE_GIB)
		{
			delay = 2; // 5 sec haha malade
		}
		position = pPosition;
		soundPlayed = false;
	}
	void update(float pDelay, Map * map);
	void render()
	{
#ifndef _DX_
		glPushMatrix();
			glTranslatef(position[0], position[1], position[2]);
			glRotatef(delay*90,vel[0], vel[1],0);
			glScalef(.005f,.005f,.005f);
			if (type == DOUILLE_TYPE_DOUILLE) dkoRender(gameVar.dko_douille);
			else if (type == DOUILLE_TYPE_GIB) dkoRender(gameVar.dko_gib);
		glPopMatrix();
#endif
	}
};
#endif



// Projectiles (rocket, grenade, etc)
struct Projectile
{
	// Le type (rocket, grenade, etc)
	int projectileType;

	// Son (ses) coordframes
	CoordFrame currentCF; // Celui qu'on affiche
	CoordFrame lastCF; // Le key frame de sauvegarde du frame courant
	CoordFrame netCF0; // L'avant dernier keyframe reçu du net
	CoordFrame netCF1; // Le dernier keyframe reçu du net

	// Sa progression sur la courbe
	long cFProgression;

	// Si c'est un entity controllé par le server
	bool remoteEntity;

	// On en a fini avec, on l'efface
	bool needToBeDeleted;

	// quick hack to give the molotov one more frame before it gets deleted ( to prevent invisi flame bug )
	bool reallyNeedToBeDeleted;

#ifndef DEDICATED_SERVER
	// La rocket tourne sur elle même
	float rotation;
	float rotateVel;

	bool isClientOnly;
#endif

	// Il a une duration limite
	float duration;

	// Hey, de qui ça vient ça?
	char fromID;
	bool movementLock;

	// Pour savoir quand shooter le data au client
	long whenToShoot;
#ifndef DEDICATED_SERVER
	int spawnParticleTime;
#endif
	int damageTime;

	int stickToPlayer;
	float stickFor;

	// determines how long the flame has been thrown, usefull so that when you are the thrower, you can only stick on flames after 1 second
	float timeSinceThrown;

	// Son ID dans le vector
	short projectileID;

	// Notre ID unique pour identifier nos projectiles
	long uniqueID;
	static long uniqueProjectileID;

	// Constructeur
	Projectile(CVector3f & position, CVector3f & vel, char pFromID, int pProjectileType, bool pRemoteEntity, long pUniqueProjectileID);

	// Son update
	void update(float delay, Map * map);
#ifndef DEDICATED_SERVER
	// Pour l'afficher (client Only)
	void render();
	void renderShadow();
#endif
	// pour updater le coordFrame avec celui du server
	void setCoordFrame(net_svcl_projectile_coord_frame & projectileCoordFrame);
};


// Pour tenir nos explosion, ça c'est client OnLy
/*
struct Explosion
{
	CVector3f position;
	float delay;
	float size;
	CVector3f normal;
	// Ce qui arrive c'est qu'on spawn un paquet de particule
	// Ici c'est juste pour le shockwave :)
};
*/
#ifndef DEDICATED_SERVER
// Pour nos trail (smoke, rocket, etc)
struct Trail
{
	CVector3f p1;
	CVector3f p2;
	float dis;
	float delay;
	float size;
	float delaySpeed;
	float offset;
	int trailType;
	CVector4f color;
	CVector3f right;
	Trail(CVector3f & pP1, CVector3f & pP2, float pSize, CVector4f & pColor, float duration, int in_trailType=0)
	{
		trailType = in_trailType;
		dis = distance(pP1, pP2);
		delay = 0;
		delaySpeed = 1.0f / (duration);
		p1 = pP1;
		p2 = pP2;
		size = pSize;
		color = pColor;
		right = cross(p2 - p1, CVector3f(0,0,1));
		normalize(right);
		offset = rand(0.0f, 1.0f);
	}
	void render()
	{
#ifndef _DX_
		glColor4f(.7f, .7f, .7f, (1-delay)*.5f);
		if (trailType == 1) glColor4f(color[0], color[1], color[2],(1-delay));
		glBegin(GL_QUADS);
			glTexCoord2f(0,dis);
			glVertex3fv((p2-right*delay*size).s);
			glTexCoord2f(0,0);
			glVertex3fv((p1-right*delay*size).s);
			glTexCoord2f(1,0);
			glVertex3fv((p1+right*delay*size).s);
			glTexCoord2f(1,dis);
			glVertex3fv((p2+right*delay*size).s);
		glEnd();
#endif
	}
	void renderBullet()
	{
		float progress = ((delay/delaySpeed)*40 + offset*1) / dis;
		if (progress < 1)
		{
			CVector3f dir = p2 - p1;
			float x = p1[0]+dir[0]*progress;
			float y = p1[1]+dir[1]*progress;

#ifndef _DX_
				glColor4f(color[0], color[1], color[2],.1f);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(x-1.0f,y+1.0f,0);
					glTexCoord2f(0,0);
					glVertex3f(x-1.0f,y-1.0f,0);
					glTexCoord2f(1,0);
					glVertex3f(x+1.0f,y-1.0f,0);
					glTexCoord2f(1,1);
					glVertex3f(x+1.0f,y+1.0f,0);
				glEnd();

				glColor4f(color[0], color[1], color[2], 1);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3fv((p1+dir*progress+dir/dis-right*.05f).s);
					glTexCoord2f(0,0);
					glVertex3fv((p1+dir*progress-right*.05f).s);
					glTexCoord2f(1,0);
					glVertex3fv((p1+dir*progress+right*.05f).s);
					glTexCoord2f(1,1);
					glVertex3fv((p1+dir*progress+dir/dis+right*.05f).s);
				glEnd();
#endif
		}
	}
	void update(float pDelay)
	{
		if (delay > 0)
		{
			delay += pDelay * delaySpeed;
		}
		else
		{
			delay = .001f;
		}
	}
};
#endif


class Game
{
public:
	// Sa liste de player
	Player ** players;

#ifndef DEDICATED_SERVER
	// Notre player
	Player * thisPlayer;
	// La font
	unsigned int font;

	// Les textures pour dessiner sur la minimap
	unsigned int tex_miniMapAllied;
	unsigned int tex_miniMapEnemy;
	
	// Render stats
	bool showStats;

	// Map buffer
	MemIO mapBuffer;
	unsigned int mapBytesRecieved;
#endif
	bool isServerGame;

	// Notre map
	Map * map;

	// Le seed
	long mapSeed;
#ifndef DEDICATED_SERVER
	// Notre liste de trail à afficher
	std::vector<Trail*> trails;
#endif
	// notre liste de projectile (très important de toujours les garder dans l'ordre
	std::vector<Projectile*> projectiles;

#ifndef DEDICATED_SERVER
	// La liste de projectile client
	std::vector<Projectile*> clientProjectiles;
	// Notre liste de douilles
	std::vector<Douille*> douilles;
	bool showMenu;

	// Son shadow
	unsigned int tex_baboShadow;

	// Nos marques sur le plancher (ça c chouette)
	FloorMark floorMarks[MAX_FLOOR_MARK];
	long nextWriteFloorMark;
	Drip drips[MAX_FLOOR_MARK];
	long nextWriteDrip;

	std::vector<NukeFlash*> nikeFlashes;

	// Une explosion nous fait shaker??
	float viewShake;

	float dotAnim;
#endif

	CString mapName;

	// Le type de jeu et les scores
	int gameType;
   int spawnType;
   int subGameType;
	int blueScore;
	int redScore;
	int blueWin;
	int redWin;
	float gameTimeLeft;
	float roundTimeLeft;
	bool bombPlanted;
	float bombTime;

	// key: teamid, value: list of userids
	typedef std::map<int, std::vector<int> > ApprovedPlayers;
	// list of approved players for each team
	ApprovedPlayers approvedPlayers;
	// flag for each team
	std::map<int, bool> teamApproveAll;

	// La moyenne des ping dans chaque team
	int bluePing;
	int redPing;
	int ffaPing;
	int spectatorPing;
#ifndef DEDICATED_SERVER
	// Les sons pour CTF
	FSOUND_SAMPLE * sfx_fcapture;
	FSOUND_SAMPLE * sfx_ecapture;
	FSOUND_SAMPLE * sfx_return;
	FSOUND_SAMPLE * sfx_win;
	FSOUND_SAMPLE * sfx_loose;
#endif
	// On est en mode fin de round
	int roundState;

	// Notre unique Projectile ID
//	long uniqueProjectileID;//

	//--- Voting
	struct SVoting
	{
		bool votingInProgress;
		CString votingFrom;
		CString votingWhat;
		int votingResults[2]; // Yes/No
		float remaining;
		bool voted;
		std::vector<int> activePlayersID;
		SVoting()
		{
			//nbActivePlayers = 0;
			voted = false;
			remaining = 0;
			votingInProgress = false;
			votingResults[0] = 0;
			votingResults[1] = 0;
		}
		void castVote(Game* game, const net_clsv_svcl_vote_request & voteRequest)
		{
			//nbActivePlayers = 0;
			activePlayersID.clear();
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (game->players[i])
				{
					if ( (game->players[i]->teamID != PLAYER_TEAM_AUTO_ASSIGN) &&
						(game->players[i]->teamID != PLAYER_TEAM_SPECTATOR) )
					{
						activePlayersID.push_back(game->players[i]->babonetID);
						game->players[i]->voted = false;
					}
					else
						game->players[i]->voted = true;
				}
			}
#ifndef DEDICATED_SERVER
			if ( game->thisPlayer && (game->thisPlayer->teamID != PLAYER_TEAM_AUTO_ASSIGN) &&
				(game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR) )
				voted = false;
			else
				voted = true;
#else
			voted = true;
#endif
			votingWhat = voteRequest.vote;
			votingResults[0] = 0;
			votingResults[1] = 0;
			remaining = 30; // 30 sec to vote
			votingInProgress = true;
		}
		bool update(float delay)
		{
			if (votingInProgress)
			{
				remaining -= delay;
				if (remaining <= 0)
				{
					remaining = 0;
					votingInProgress = false;
					return true;
				}

				if (votingResults[0] > (int)activePlayersID.size() / 2 ||
					votingResults[1] > (int)activePlayersID.size() / 2 ||
					votingResults[0] + votingResults[1] >= (int)activePlayersID.size())
				{
					votingInProgress = false;
					return true;
				}
			}
			return false;
		}

	} voting;

public:
	// Constructeur
	Game(CString pMapName="");

	// Destructeur
	virtual ~Game();

	// Pour l'updater
	void update(float delay);
#ifndef DEDICATED_SERVER
	// Pour l'afficher
	void render();

	// Afficher les stats
	void renderStats();
	void renderBlueTeam(std::vector<Player*> & teamOrder, int & vPos);
	void renderRedTeam(std::vector<Player*> & teamOrder, int & vPos);
	void renderFFA(std::vector<Player*> & teamOrder, int & vPos);
	void renderSpectator(std::vector<Player*> & teamOrder, int & vPos);
#endif
   
   void UpdateProSettings();

   // Create map
	void createMap();

	// pour donner un team à un player
	int assignPlayerTeam(int playerID, char teamRequested, Client * client = 0);

	// let the player join selected team
	bool approvePlayer(int userID, char team);

	// approve all players to join selected team
	void approveAll(int team);

	// approve all players to join
	void approveAll();

	bool isApproved(int userID, int team);

	// removes player from the list of approved players of all teams
	// only works if player was approved to some team before
	void rejectPlayer(int userID);

	// removes player from the list of approved players of selected team
	// only works if player was approved to some team before
	void rejectPlayer(int userID, char team);

	void rejectAllPlayers();

	// Spawner un player
	bool spawnPlayer(int playerID);

	// Pour ajouter un nouveau joueur
	int createNewPlayerSV(int babonetID); // Ça c'est côté server
#ifndef DEDICATED_SERVER
	void createNewPlayerCL(int playerID, long babonetID); // Ça c'est côté client

	// Pour quand un client shot
	void shoot(const CVector3f & position, const CVector3f & direction, float imp, float damage, Player * from, int projectileType);
#endif

	// Quand un client shot, mais que le server le vérifie puis le shoot aux autres joueurs
	void shootSV(net_clsv_player_shoot & playerShoot);
#ifndef DEDICATED_SERVER
	// Pour spawner des particules sur le murs l'hors d'un impact
	void spawnImpact(CVector3f & p1, CVector3f & p2, CVector3f & normal, Weapon*weapon, float damage, int team);
	void spawnBlood(CVector3f & position, float damage);
#ifdef _PRO_
	void spawnBloodMinibot(CVector3f & position, float damage);
#endif
	void spawnExplosion(CVector3f & position, CVector3f & normal, float size);

	// Pour afficher la minimap (ouff, je mélange pomal les affaires, tk)
	void renderMiniMap();

	// Pour pogner le prochain markFloor
	long getNextFloorMark() {nextWriteFloorMark++;if(nextWriteFloorMark>=MAX_FLOOR_MARK)nextWriteFloorMark=0;return nextWriteFloorMark;}
	long getNextDrip() {nextWriteDrip++;if(nextWriteDrip>=MAX_FLOOR_MARK)nextWriteDrip=0;return nextWriteDrip;}
#endif

	// On spawn un projectile!
	bool spawnProjectile(net_clsv_svcl_player_projectile & playerProjectile, bool imServer);

	// Pour toucher les joueurs dans un rayon
	void radiusHit(CVector3f & position, float radius, char fromID, char weaponID, bool sameDmg=false);

	// Pour savoir s'il y a un joueur dans le radius, last parameter used to ignore a specific player ( -1 = not ignoring anyone )
	Player * playerInRadius(CVector3f position, float radius, int ignore = -1 );

	// Pour starter un nouveau type de game
	void resetGameType(int pGameType);

	// Pour starter un new round
	void resetRound();

   // Number of players

   int numPlayers();

	void shootMinibotSV(CMiniBot * minibot, float imp, CVector3f p1, CVector3f p2);
private:
	void shootSV(int playerID, int nuzzleID, float imp, CVector3f p1, CVector3f p2);
};


#endif


