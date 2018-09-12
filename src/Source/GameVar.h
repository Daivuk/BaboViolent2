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

#ifndef GAMEVAR_H
#define GAMEVAR_H


#include "Zeven.h"
#include "Weapon.h"


#define WEAPON_SMG 0
#define WEAPON_SHOTGUN 1
#define WEAPON_SNIPER 2
#define WEAPON_DUAL_MACHINE_GUN 3
#define WEAPON_CHAIN_GUN 4
#define WEAPON_BAZOOKA 5
#define WEAPON_PHOTON_RIFLE 6
#define WEAPON_FLAME_THROWER 7
#define WEAPON_GRENADE 8
#define WEAPON_COCKTAIL_MOLOTOV 9
#define WEAPON_KNIVES 10
#define WEAPON_NUCLEAR 11
#define WEAPON_SHIELD 12
#if defined(_PRO_) && defined(_MINIBOT_)
#define WEAPON_MINIBOT 13
#define WEAPON_MINIBOT_WEAPON 100
#endif

#define SOUND_GRENADE_REBOUND 1
#define SOUND_MOLOTOV 2
#define SOUND_OVERHEAT 3
#define SOUND_PHOTON_START 4

#if defined(_PRO_)
#define SUBGAMETYPE_NORMAL 0
#define SUBGAMETYPE_INSTAGIB 1
#define SUBGAMETYPE_RANDOMWEAPON 2

#endif


struct SLangText
{
	CString varName;
	CString * varPtr;
	SLangText(CString in_varName, CString * in_varPtr) :
		varName(in_varName), varPtr(in_varPtr) {}
};


//--- Visual props
/*
struct SVisual
{
	unsigned int dkoModel;
	float angle;
	SVisual(CString filename, float in_angle)
	{
		dkoModel = dkoLoadModel(filename.s);
		angle = in_angle;
	}
	~SVisual()
	{
		dkoDeleteModel(&dkoModel);
	}
};*/


class GameVar
{
public:
	bool sv_friendlyFire;
	bool sv_reflectedDamage;
	float sv_timeToSpawn;
	bool sv_topView;
	int sv_minSendInterval;
	bool sv_forceRespawn;
	bool sv_baboStats;
	float sv_roundTimeLimit;
	float sv_gameTimeLimit;
	int sv_scoreLimit;
	int sv_winLimit;
	int sv_gameType;
   int sv_serverType;
   int sv_spawnType;   
   int sv_subGameType;   
	float sv_bombTime;
	CString sv_gameName;
	int sv_port;
	int sv_maxPlayer;
   int sv_maxPlayerInGame;
   float sv_minTilesPerBabo;
   float sv_maxTilesPerBabo;
	CString sv_password;
	bool sv_enableSMG;
	bool sv_enableShotgun;
	bool sv_enableSniper;
	bool sv_enableDualMachineGun;
	bool sv_enableChainGun;
	bool sv_enableBazooka;
	bool sv_enableShotgunReload;
	bool sv_slideOnIce;
	bool sv_showEnemyTag;
	bool sv_enableSecondary;
	bool sv_enableKnives;
	bool sv_enableNuclear;
	bool sv_enableShield;
	#if defined(_PRO_)
		bool sv_enableMinibot;
	#endif
	float sv_shottyDropRadius;
	float sv_shottyRange;
	float sv_ftMaxRange;
	float sv_ftMinRange;
	float sv_photonDamageCoefficient;
	float sv_ftDamage;
	float sv_smgDamage;
	float sv_dmgDamage;
	float sv_cgDamage;
	float sv_sniperDamage;
	float sv_shottyDamage;
	float sv_zookaRadius;
	float sv_nukeRadius;
	float sv_nukeTimer;
	float sv_nukeReload;
	float sv_zookaDamage;
	float sv_ftExpirationTimer;
	bool sv_explodingFT;
	int sv_photonType;
	bool sv_zookaRemoteDet;
	bool sv_enableMolotov;
	float sv_photonDistMult;
	float sv_photonHorizontalShift;
	float sv_photonVerticalShift;
	bool sv_autoBalance;
	int sv_autoBalanceTime;
	bool sv_gamePublic;
	bool sv_enableVote;
	bool sv_enablePhotonRifle;
	bool sv_enableFlameThrower;
	float sv_maxUploadRate;
	bool sv_showKills;
	CString sv_matchcode;
	int sv_matchmode;
	bool sv_report;
	int sv_maxPing;
	bool sv_autoSpectateWhenIdle;
	int sv_autoSpectateIdleMaxTime;
	bool sv_beGoodServer;
	bool sv_validateWeapons;
	CString sv_joinMessage;
	bool sv_sendJoinMessage;

	//spawn immunity
	float sv_spawnImmunityTime;

	//--- For remote control
	CString zsv_adminUser;
	CString zsv_adminPass;

	CString db_accountServer;
	int db_version;

	CString cl_accountUsername;
	CString cl_accountPassword;
	CString cl_playerName;
	CString cl_mapAuthorName;
	bool cl_cubicMotion;
	CString cl_lastUsedIP;
	int cl_port;
	CString cl_password;
	CColor3f cl_redDecal;
	CColor3f cl_greenDecal;
	CColor3f cl_blueDecal;
	CString cl_skin;
	int cl_teamIndicatorType;
	float cl_glowSize;
	bool cl_grassTextureForAllMaps;
	bool cl_preciseCursor;
	bool cl_enableXBox360Controller;
	bool cl_enableVSync;
	int cl_affinityMode;
	int cl_primaryWeapon;
	//bool cl_weaponSideRight;
	int cl_secondaryWeapon;
#ifndef DISABLE_QUICK_MESSAGES
	// quick messages
	CString cl_qMsg01;
	CString cl_qMsg02;
	CString cl_qMsg03;
	CString cl_qMsg04;
	CString cl_qMsg05;
	CString cl_qMsg06;
	CString cl_qMsg07;
	CString cl_qMsg08;
	CString cl_qMsg09;
	CString cl_qMsg10;
#endif
	//--- Those are not included into the cfg file
		long scl_honor;
		long scl_xp;
		long scl_leftToNextLevel;
		long scl_totalKill;
		long scl_totalDeath;
		float scl_ratio;
		long scl_weaponOfChoice;
		long scl_killWeapon[20];

	bool r_showStats;
	bool r_fullScreen;
	CVector2i r_resolution;
	int r_bitdepth;
	int r_refreshRate;
	bool r_weatherEffects;
	bool r_reflection;
	int r_shadowQuality;
	bool r_playerShadow;
	bool r_projectileShadow;
	bool r_particleLighting;
	bool r_particleSort;
	bool r_terrainSplater;
	bool r_showCasing;
	bool r_showGroundMark;
	bool r_highDetailMenu;
	bool r_animatedMenu;
	bool r_showLatency;
	bool r_reducedParticles;
	bool r_simpleText;
	int r_widescreen;
	int r_maxNameLenOverBabo;

#if defined(_PRO_)	
   int r_chatTextSize;
   int r_eventTextSize;
   bool r_showEventText;
#endif

	int s_mixRate;
	int s_maxSoftwareChannels;
	float s_masterVolume;
	bool s_inGameMusic;

	int k_moveUp;
	int k_moveDown;
	int k_moveRight;
	int k_moveLeft;
	int k_shoot;
	int k_throwGrenade;
	int k_throwMolotov;
	int k_pickUp;
	int k_chatAll;
	int k_chatTeam;
	int k_showScore;
	int k_menuAccess;
	int k_melee;
#if defined(_PRO_)	
   int k_screenShot;
   int k_stats;
#endif
#ifndef DISABLE_QUICK_MESSAGES
	// quick messages
	int k_qMsg01;
	int k_qMsg02;
	int k_qMsg03;
	int k_qMsg04;
	int k_qMsg05;
	int k_qMsg06;
	int k_qMsg07;
	int k_qMsg08;
	int k_qMsg09;
	int k_qMsg10;
#endif

	int ro_nbParticle;
	CVector3f ro_hitPoint;

	// Si on est en debug !!! tr�s important
	bool c_debug;
	bool c_huge;
	bool d_showPath;
	bool d_showNodes;

	CString languageFile;


#ifndef DEDICATED_SERVER
	// On va toute pr�-loader nos models icite
	unsigned int dko_rocket;
	unsigned int dko_grenade;
	unsigned int dko_douille;
	unsigned int dko_lifePack;
	unsigned int dko_cocktailMolotov;
	unsigned int dko_gib;
#endif
	// Les gun
	Weapon * weapons[20];
#ifndef DEDICATED_SERVER
	// Textures
	unsigned int tex_nuzzleFlash;
	unsigned int tex_smoke1;
	unsigned int tex_shotGlow;
	unsigned int tex_smokeTrail;
	unsigned int tex_blood[10];
	unsigned int tex_explosionMark;
	unsigned int tex_sniperScope;
	unsigned int tex_medals[32];
	unsigned int tex_drip;
	unsigned int tex_sky;
	unsigned int tex_glowTrail;

	// Particles presets
	dkp_preset dkpp_firingSmoke;
	dkp_preset dkpp_bloodHit;

	// Les sons
	FSOUND_SAMPLE * sfx_ric[5];
	FSOUND_SAMPLE * sfx_hit[2];
	FSOUND_SAMPLE * sfx_baboCreve[3];
	FSOUND_SAMPLE * sfx_explosion[1];
	FSOUND_SAMPLE * sfx_grenadeRebond;
	FSOUND_SAMPLE * sfx_douille[3];
	FSOUND_SAMPLE * sfx_equip;
	FSOUND_SAMPLE * sfx_lifePack;
	FSOUND_SAMPLE * sfx_cocktailMolotov;
	FSOUND_SAMPLE * sfx_lavaSteam;
	FSOUND_SAMPLE * sfx_overHeat;
	FSOUND_SAMPLE * sfx_photonStart;

	// LES STRING DIFF�RENT POUR CHAQUE LANGUE
	CString lang_gameName;

	CString lang_dualMachineGun;
	CString lang_subMachineGun;
	CString lang_changGun;
	CString lang_shotgun;
	CString lang_sniper;
	CString lang_bazooka;
	CString lang_grenade;
	CString lang_flame;

	CString lang_autoAssignTeam;
	CString lang_joinBlueTeam;
	CString lang_joinRedTeam;
	CString lang_spectator;
	CString lang_mainMenu;
	CString lang_disconnect;

	CString lang_serverDisconnected;

	CString lang_dead;
	CString lang_team;

	CString lang_joinedTheGame;
	CString lang_playerDisconnected;
	CString lang_player;
	CString lang_playerChangedHisNameFor;
	CString lang_returnBlueFlag;
	CString lang_returnRedFlag;
	CString lang_tookBlueFlag;
	CString lang_tookRedFlag;
	CString lang_redScore;
	CString lang_blueScore;

	CString lang_reloading;

	CString lang_spawnIn;
	CString lang_pressToRespawn;

	CString lang_blueTeamWin;
	CString lang_redTeamWin;
	CString lang_roundDraw;
	CString lang_changingMap;

	CString lang_deathmatchC;
	CString lang_deathmatchD;
	CString lang_teamDeathmatchC;
	CString lang_teamDeathmatchD;
	CString lang_captureTheFlagC;
	CString lang_captureTheFlagD;
	CString lang_counterBaboristC;
	CString lang_counterBaboristD;
#if defined(_PRO_)
	CString lang_championC;
	CString lang_championD;
#endif

	CString lang_connectingC;
	CString lang_pressF10ToCancel;

	CString lang_goesSpectator;
	CString lang_joinBlueTeamP;
	CString lang_joinRedTeamP;

	CString lang_blueTeamC;
	CString lang_redTeamC;
	CString lang_freeForAllC;
	CString lang_spectatorC;

	CString lang_playerNameC;
	CString lang_scoreC;
	CString lang_pingC;

	CString lang_laggerC;

	CString lang_friendlyFire;

	CString lang_serverVersion;
	CString lang_clientVersion;

	CString lang_connectionFailed;
	CString lang_back;

	CString lang_launch;
	CString lang_dedicate;

	CString lang_generalGameOption;
	CString lang_gameNameS;
	CString lang_gameType;
   CString lang_serverType;
#if defined(_PRO_)   
   CString lang_spawnType;
   CString lang_subGameType;
#endif
	CString lang_freeForAll;
	CString lang_teamDeathmatch;
	CString lang_captureTheFlag;
	CString lang_TCPnUDPport;
	CString lang_gameLimits;
	CString lang_scoreLimit;
	CString lang_teamWinLimit;
	CString lang_timeLimit;
	CString lang_maxPlayer;
	CString lang_spawning;
	CString lang_timeToSpawn;
	CString lang_forceRespawn;
	CString lang_no;
	CString lang_yes;
	CString lang_reflectDamage;
	CString lang_maps;
	CString lang_includedMaps;

	CString lang_RndLabsTeam;
	CString lang_gameDesigner;
	CString lang_programmers;
	CString lang_soundDesigners;
	CString lang_musics;
	CString lang_thanksTo;
	CString lang_betaTesters;
	CString lang_buyAlbum;

	CString lang_internet;
	CString lang_localNetwork;
	CString lang_refreshList;
	CString lang_players;
	CString lang_mapName;
	CString lang_ping;
	CString lang_join;
	CString lang_IPAdress;
	CString lang_canRefreshIn;
	CString lang_nbGamesRefreshed;
	CString lang_noGameFound;
	CString lang_problemConnectingToMaster;

	CString lang_resumeCurrentGame;
	CString lang_joinGame;
	CString lang_createServer;
	CString lang_options;
	CString lang_credits;
	CString lang_quit;

	CString lang_noGameRunning;

	CString lang_selectAtLeastOneMap;

	CString lang_apply;
	CString lang_cancel;
	CString lang_someOptionRequireRestart;
	CString lang_clientOptions;
	CString lang_playerName;
	CString lang_inGameMusic;
	CString lang_renderingOptions;
	CString lang_showStats;
	CString lang_fullscreen;
	CString lang_screenResolution;
	CString lang_colorDepth;
	CString lang_bits;
	CString lang_wallShadowQuality;
	CString lang_noShadows;
	CString lang_hardEdges;
	CString lang_softEdges;
	CString lang_baboShadow;
	CString lang_projectileShadow;
	CString lang_particleLighting;
	CString lang_particleSorting;
	CString lang_terrainSplatter;
	CString lang_showCasing;
	CString lang_groundMarkNBlood;
	CString lang_soundOptions;
	CString lang_mixRate;
	CString lang_hertz;
	CString lang_maxSoftwareChannels;
	CString lang_masterVolume;
	CString lang_keyOptions;
	CString lang_moveUp;
	CString lang_moveDown;
	CString lang_moveRight;
	CString lang_moveLeft;
	CString lang_shoot;
	CString lang_throwGrenade;
	CString lang_throwCocktailMolotov;
	CString lang_use;
	CString lang_chatToAll;
	CString lang_chatToTeam;
	CString lang_showScoreBoard;
	CString lang_toggleMenu;

	CString lang_areYouSureYouWantToQuit;
	CString lang_yesQ;
	CString lang_noQ;

	CString lang_serverHasClosedTheConnection;

	CString lang_language;
	CString lang_incorrectName;

	// List des variables lang_ pour loader rapidement d'un fichier .lang
	std::vector<SLangText> langs;
#endif
	//--- Les visual components
/*	std::vector<SVisual*> hats;
	std::vector<SVisual*> hats;
	std::vector<SVisual*> hats;*/

public:
	// Constructeur
	GameVar();

	// Destructeur
	virtual ~GameVar();

	// on load les models pour le jeu
	void loadModels();

	// pour effacer les models du jeu
	void deleteModels();

	// Pour envoyer les variables servers au autres tayouin
	void sendSVVar(UINT4 babonetID);
	// send server var to a peer(remote admin)
	void sendSVVar(INT4 peerId);
	void sendOne(char * varName, UINT4 babonetID);
	// send one var to a peer( remote admin )
	void sendOne(char * varName, INT4 peerId);
#ifndef DEDICATED_SERVER
	bool languageLoaded;
	// Pour loader les lang_ var
	bool loadLanguage(char * filename);
	bool isLanguageLoaded();
#endif
};

extern GameVar gameVar;


#endif

