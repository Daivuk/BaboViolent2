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

#ifndef PLAYER_H
#define PLAYER_H


#include "Zeven.h"
#include "netPacket.h"
#include "Helper.h"
#include "GameVar.h"

#ifdef _PRO_
#include <list>
#endif


#define PLAYER_STATUS_ALIVE 0
#define PLAYER_STATUS_DEAD 1
#define PLAYER_STATUS_LOADING 2

#define PLAYER_TEAM_SPECTATOR -1
#define PLAYER_TEAM_BLUE 0
#define PLAYER_TEAM_RED 1
#define PLAYER_TEAM_AUTO_ASSIGN 2


class Map;
class Game;

// Notre coordframe
struct CoordFrame
{
	CVector3f position;
	CVector3f vel;
	long frameID;
	float angle;
	CVector3f mousePosOnMap;
#ifdef _PRO_
	float camPosZ;
#endif

	CoordFrame()
	{
		reset();
	}

	void reset()
	{
		frameID = 0;
		angle = 0;
	}

	void operator=(const CoordFrame & coordFrame)
	{
		position = coordFrame.position;
		vel = coordFrame.vel;
		angle = coordFrame.angle;
		frameID = coordFrame.frameID;
		mousePosOnMap = coordFrame.mousePosOnMap;
	}

	void interpolate(long & cFProgression, CoordFrame & from, CoordFrame & to, float delay)
	{
		cFProgression++; // On incr�ent
		long size = to.frameID - from.frameID;
		if (cFProgression > size)
		{
			if (cFProgression < 15)
			{
				// On le fait avancer normalement avec sa velocity
				position += vel * delay;
			}
			else
			{
				position = to.position;
			}
		}
		else if (cFProgression >= 0)
		{
			// On pogne le temps t en float de 0 �1 de sa progression
			float t = (float)cFProgression / (float)size;
			float animTime = (float)size / (30.0f*3.0f);

			// Quadric motion, pas tr� beau
		/*	if (gameVar.cl_quadricMotion)
			{
				// Il faut scaller nos velocity d�endament du temps �parcourir
				CVector3f acc = to.vel - from.vel;

				// On interpolate sa position
				CVector3f coord1 = from.position;
				CVector3f coord2 = from.position + from.vel * animTime;
				CVector3f coord3 = to.position + to.vel * animTime + acc * .5f * (animTime * animTime);
				CVector3f coord4 = coord3 - (to.vel * animTime + acc * animTime);
				position = cubicSpline(coord1, coord2, coord3, coord4, t);
			}
			else
			{*/
				if (gameVar.cl_cubicMotion)
				{
					position = cubicSpline(
						from.position, 
						from.position + from.vel * animTime, 
						to.position - to.vel * animTime, 
						to.position, 
						t);
					mousePosOnMap = cubicSpline(
						from.mousePosOnMap, 
						from.mousePosOnMap + (to.mousePosOnMap-from.mousePosOnMap) * animTime, 
						to.mousePosOnMap - (from.mousePosOnMap-to.mousePosOnMap) * animTime, 
						to.mousePosOnMap, 
						t);
				}
				else
				{
					position = to.position;
					vel = to.vel;
				}
		//	}
		}
	}
};

// Structure for holding stats of disconnected players
struct PlayerStats
{
	PlayerStats(const Player* player);

	void MergeStats(PlayerStats* mergeWith);

	CString name;
	int userID;
	int teamID;
	int kills;
	int deaths;
	float dmg;
	int score;
	int	returns;
	int flagAttempts;
	float timePlayedCurGame;
};

#define PING_LOG_SIZE 60

#ifdef _PRO_

class Player;

#define MINIBOT_STATE_SEEKING 0

class CMiniBot
{
public:
	bool nukeBot;
	// Ses coord frames
	CoordFrame currentCF; // Celui qu'on affiche
	CoordFrame lastCF; // Le key frame de sauvegarde du frame courant
	CoordFrame netCF0; // L'avant dernier keyframe re� du net
	CoordFrame netCF1; // Le dernier keyframe re� du net

	// Sa progression sur la courbe
	long cFProgression;

	//--- His orientation matrix
	CMatrix3x3f matrix;

	Player * owner;
	Game * game;

	//--- His current state
	int m_state;

	//--- Destination
	CVector3f destination;
	std::list<CVector3f> path;

	//--- Seeking time
	float m_seekingTime;
	float m_checkAStarTimer;
	float m_fireRate;

public:
	CMiniBot(); 
	CMiniBot(bool nuker);
	virtual ~CMiniBot();

	//--- Control it
	void MakeNukeBot();
	void Think(float delay);
};
#endif

class Player
{
public:

	//--- Anti-cheat code
	long frameSinceLast;
	long lastFrame;
	long currentFrame;
	long speedHackCount;
	float shotsPerSecond;
	long shotCount;
	float secondPassed;

	CVector3f shootShakeDis;

#ifdef _PRO_

   int spawnSlot;

#endif

	// anti cheat for projectiles and shots
	float mfElapsedSinceLastShot;

	// anti cheat for teleport ack
	int	  miNbCoord; //number foc oord frame packets received
    float mfCFTimer; //timer that we use to check for speed average every 3 second
	float mfCumulativeVel; //keep the total of every max(vel) from received coordframes

	// time since the player is dead ( for last second nade / molotov )
	float timeDead;

	// time since player is alive , if < 3, collisions with other baboes are diabled
	float timeAlive;
	
	//time since player joined the server
	float timeInServer;

	// how long player has been idle
	float timeIdle;

	//--- Si ce joueur est admin.
	//    � veut dire quil peut envoyer des commandes au server.
	//    Et tout les messages server vont lui �re envoy�
	bool isAdmin;

	// Son nom
	CString name;

	char playerIP[16];

	// La babonetID
	UINT4 babonetID;

	// Le ID dans le jeu
	char playerID;

	// Son team
	char teamID;

	// Notre status (mort, spectateur)
	char status;

	// Account ID
	int userID;

	float protection;

	// Son ping
	int ping;
	int pingSum;
	int avgPing;
	float pingOverMax;
	int currentPingFrame;

	float pingLogInterval;
	float nextPingLogTime;
	int pingLogID;
	int pingLog[PING_LOG_SIZE];
	float babySitTime;


	// Sa vie
	float life;

    // Stats
	float dmg;
	int kills;
	int deaths;
	int score;
	int	returns;
	int damage;
	int flagAttempts;
	bool rocketInAir;
	bool detonateRocket;

	// Time played(total time alive), reset on round start
	float timePlayedCurGame;

	// Son socket ID (en string) (� y a juste le server qui le sait)
	CString socketNumber;

	// Ses coord frames
	CoordFrame currentCF; // Celui qu'on affiche
	CoordFrame lastCF; // Le key frame de sauvegarde du frame courant
	CoordFrame netCF0; // L'avant dernier keyframe re� du net
	CoordFrame netCF1; // Le dernier keyframe re� du net

	// Sa progression sur la courbe
	long cFProgression;

	// sa matrice d'orientation (� c'est client side only)
	CMatrix3x3f matrix;

	// Server only �
	bool waitForPong;

	// ON attends de spawner
	bool spawnRequested;

	// To send the position at each x frame
	int sendPosFrame;
#ifndef CONSOLE
#ifndef _DX_
	// Pour dessiner notre sphere
	GLUquadricObj* qObj;
#endif

	// Si on est le joueur controll�
	bool isThisPlayer;
#endif

	// On attends 10 sec avant de spawner
	float timeToSpawn;

	float immuneTime;

	// Si c'est un entity controll�par le server
	bool remoteEntity;

	// Le gun avec lequel je vais spawner
	int nextSpawnWeapon;
	int nextMeleeWeapon;

	// Son gun
	Weapon * weapon;

	// Son melee gun
	Weapon * meleeWeapon;
#ifndef CONSOLE
	// Son shadow
	unsigned int tex_baboShadow;
	unsigned int tex_baboHalo;
#endif
	// On va garder un pointeur sur la map
	Map * map;
	
	// Notre game
	Game * game;
#ifndef CONSOLE
	// Pour savoir si on a initialis�le mouse down ici, et non dans le menu
	bool initedMouseClic;
#endif
	// Il vient de tirer, on le voit pendant 2sec sur la minimap
	float firedShowDelay;
	float secondsFired;

	// On compteur qui dit � fait combient de temps que je suis dead
	float deadSince;
#ifndef CONSOLE
	// Le player qu'on suis
	int followingPlayer;
#endif

	// Un timer qui dit que le player vient de se faire toucher
	float screenHit;

#ifndef CONSOLE
	// La position du babo on screen
	CVector2i onScreenPos;
#endif

	// Le delait pour shooter les grenade (2sec)
	float grenadeDelay;
	float meleeDelay;
	int nbGrenadeLeft;
	int nbMolotovLeft;

	// Il est en connection interrupted, il a besoin de recevoir les game state
	bool connectionInterrupted;

#ifndef CONSOLE
	// Est-ce qu'on est en mode scope FPS ou pas?
	bool scopeMode;
#endif

	long fireFrameDelay; // This is for the shoty bug

	bool lastShootWasNade;

	//--- Notre skin
#ifndef CONSOLE
	unsigned int tex_skin;
	unsigned int tex_skinOriginal;
#endif
	CString skin;
	CString lastSkin;

	//--- Les couleurs custom du babo
	CColor3f redDecal;
	CColor3f greenDecal;
	CColor3f blueDecal;

	//--- Player has already voted
	bool voted;

	//--- This player has shot a photon rifle
	unsigned int incShot;
	CVector3f p1;
	CVector3f p2;

#ifdef _PRO_
	//--- His bot
	CMiniBot * minibot;
#endif

public:
	// Constructeur
	Player(char pPlayerID, Map * pMap, Game * pGame);

	// Destructeur
	virtual ~Player();

	// Pour l'updater
	void update(float delay);

	void updatePing(float delay);

#ifndef CONSOLE
	// Pour le controller
	void controlIt(float delay);

	// Pour l'afficher
	void render();
	void renderName();
#endif

	// Pour remettre ses stats �0
	void reinit();

	// Pour lui donner les info de notre joueur
	void setThisPlayerInfo();

	// Pour le forcer �crever (suposons quil change de team)
	void kill(bool silenceDeath);

	// Pour spawner un joueur y�
	void spawn(const CVector3f & spawnPoint);

	// Pour setter le coordframe du player
	void setCoordFrame(net_clsv_svcl_player_coord_frame & playerCoordFrame);
#ifdef _PRO_
	void setCoordFrameMinibot(net_svcl_minibot_coord_frame & minibotCoordFrame);
#endif

	// Si on se fait toucher !
#ifndef CONSOLE
	void hit(Weapon * fromWeapon, Player * from, float damage=-1);
#endif
	void hitSV(Weapon * fromWeapon, Player * from, float damage=-1);

	// Pour changer son gun!
	void switchWeapon(int newWeaponID, bool forceSwitch=false);
	void switchMeleeWeapon(int newWeaponID, bool forceSwitch=false);

#ifndef CONSOLE
	// Pour updater la skin texture
	void updateSkin();
#endif

	#ifdef _PRO_
		void SpawnMiniBotSV();
		void SpawnNukeBotSV();
		void SpawnMiniBot(const CVector3f & spawnPoint, const CVector3f & mousePos);
	#endif
};


#endif


