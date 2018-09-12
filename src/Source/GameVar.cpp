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

#include "GameVar.h"
#include "netPacket.h"
#include "RemoteAdminPackets.h"
#ifndef DEDICATED_SERVER
#include "KeyManager.h"
#endif
#include "sqlite3.h"

GameVar gameVar;


// function to fetch db infos, version and web url
void FetchDBInfos()
{
	sqlite3* db=0;
	sqlite3_open("./bv2.db",&db);

	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int	nRow;			// number of record
	int	nColumn;		// number of column
	char	SQL[256];		// the query

	sprintf(SQL,"Select Value From LauncherSettings Where Name = 'DBVersion';");
	sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	gameVar.db_version = atoi(azResult[1]);
	sqlite3_free_table(azResult);

	sprintf(SQL,"Select Value From LauncherSettings Where Name = 'AccountURL';");
	sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	gameVar.db_accountServer = CString( azResult[1] );

	sqlite3_free_table(azResult);
	sqlite3_close(db);
}



//
// Constructeur
//
GameVar::GameVar()
{
#ifndef DEDICATED_SERVER
	languageLoaded = false;
	// LES STRING DIFF�RENT POUR CHAQUE LANGUE
	langs.push_back(SLangText("lang_gameName", &lang_gameName));

	langs.push_back(SLangText("lang_dualMachineGun", &lang_dualMachineGun));
	langs.push_back(SLangText("lang_subMachineGun", &lang_subMachineGun));
	langs.push_back(SLangText("lang_changGun", &lang_changGun));
	langs.push_back(SLangText("lang_shotgun", &lang_shotgun));
	langs.push_back(SLangText("lang_sniper", &lang_sniper));
	langs.push_back(SLangText("lang_bazooka", &lang_bazooka));
	langs.push_back(SLangText("lang_grenade", &lang_grenade));
	langs.push_back(SLangText("lang_flame", &lang_flame));

	langs.push_back(SLangText("lang_autoAssignTeam", &lang_autoAssignTeam));
	langs.push_back(SLangText("lang_joinBlueTeam", &lang_joinBlueTeam));
	langs.push_back(SLangText("lang_joinRedTeam", &lang_joinRedTeam));
	langs.push_back(SLangText("lang_spectator", &lang_spectator));
	langs.push_back(SLangText("lang_mainMenu", &lang_mainMenu));
	langs.push_back(SLangText("lang_disconnect", &lang_disconnect));

	langs.push_back(SLangText("lang_serverDisconnected", &lang_serverDisconnected));

	langs.push_back(SLangText("lang_dead", &lang_dead));
	langs.push_back(SLangText("lang_team", &lang_team));

	langs.push_back(SLangText("lang_joinedTheGame", &lang_joinedTheGame));
	langs.push_back(SLangText("lang_playerDisconnected", &lang_playerDisconnected));
	langs.push_back(SLangText("lang_player", &lang_player));
	langs.push_back(SLangText("lang_playerChangedHisNameFor", &lang_playerChangedHisNameFor));
	langs.push_back(SLangText("lang_returnBlueFlag", &lang_returnBlueFlag));
	langs.push_back(SLangText("lang_returnRedFlag", &lang_returnRedFlag));
	langs.push_back(SLangText("lang_tookBlueFlag", &lang_tookBlueFlag));
	langs.push_back(SLangText("lang_tookRedFlag", &lang_tookRedFlag));
	langs.push_back(SLangText("lang_redScore", &lang_redScore));
	langs.push_back(SLangText("lang_blueScore", &lang_blueScore));

	langs.push_back(SLangText("lang_reloading", &lang_reloading));

	langs.push_back(SLangText("lang_spawnIn", &lang_spawnIn));
	langs.push_back(SLangText("lang_pressToRespawn", &lang_pressToRespawn));

	langs.push_back(SLangText("lang_blueTeamWin", &lang_blueTeamWin));
	langs.push_back(SLangText("lang_redTeamWin", &lang_redTeamWin));
	langs.push_back(SLangText("lang_roundDraw", &lang_roundDraw));
	langs.push_back(SLangText("lang_changingMap", &lang_changingMap));

	langs.push_back(SLangText("lang_deathmatchC", &lang_deathmatchC));
	langs.push_back(SLangText("lang_deathmatchD", &lang_deathmatchD));
	langs.push_back(SLangText("lang_teamDeathmatchC", &lang_teamDeathmatchC));
	langs.push_back(SLangText("lang_teamDeathmatchD", &lang_teamDeathmatchD));
	langs.push_back(SLangText("lang_captureTheFlagC", &lang_captureTheFlagC));
	langs.push_back(SLangText("lang_captureTheFlagD", &lang_captureTheFlagD));
	langs.push_back(SLangText("lang_counterBaboristC", &lang_counterBaboristC));
	langs.push_back(SLangText("lang_counterBaboristD", &lang_counterBaboristD));
#ifdef _PRO
	langs.push_back(SLangText("lang_championC", &lang_championC));
	langs.push_back(SLangText("lang_championD", &lang_championD));
#endif

	langs.push_back(SLangText("lang_connectingC", &lang_connectingC));
	langs.push_back(SLangText("lang_pressF10ToCancel", &lang_pressF10ToCancel));

	langs.push_back(SLangText("lang_goesSpectator", &lang_goesSpectator));
	langs.push_back(SLangText("lang_joinBlueTeamP", &lang_joinBlueTeamP));
	langs.push_back(SLangText("lang_joinRedTeamP", &lang_joinRedTeamP));

	langs.push_back(SLangText("lang_blueTeamC", &lang_blueTeamC));
	langs.push_back(SLangText("lang_redTeamC", &lang_redTeamC));
	langs.push_back(SLangText("lang_freeForAllC", &lang_freeForAllC));
	langs.push_back(SLangText("lang_spectatorC", &lang_spectatorC));

	langs.push_back(SLangText("lang_playerNameC", &lang_playerNameC));
	langs.push_back(SLangText("lang_scoreC", &lang_scoreC));
	langs.push_back(SLangText("lang_pingC", &lang_pingC));

	langs.push_back(SLangText("lang_laggerC", &lang_laggerC));

	langs.push_back(SLangText("lang_friendlyFire", &lang_friendlyFire));

	langs.push_back(SLangText("lang_serverVersion", &lang_serverVersion));
	langs.push_back(SLangText("lang_clientVersion", &lang_clientVersion));

	langs.push_back(SLangText("lang_connectionFailed", &lang_connectionFailed));
	langs.push_back(SLangText("lang_back", &lang_back));

	langs.push_back(SLangText("lang_launch", &lang_launch));
	langs.push_back(SLangText("lang_dedicate", &lang_dedicate));

	langs.push_back(SLangText("lang_generalGameOption", &lang_generalGameOption));
	langs.push_back(SLangText("lang_gameNameS", &lang_gameNameS));
	langs.push_back(SLangText("lang_gameType", &lang_gameType));

   langs.push_back(SLangText("lang_serverType", &lang_serverType));
#if defined(_PRO_)
   langs.push_back(SLangText("lang_spawnType", &lang_spawnType));
   langs.push_back(SLangText("lang_subGameType", &lang_subGameType));
#endif
	
   langs.push_back(SLangText("lang_freeForAll", &lang_freeForAll));
	langs.push_back(SLangText("lang_teamDeathmatch", &lang_teamDeathmatch));
	langs.push_back(SLangText("lang_captureTheFlag", &lang_captureTheFlag));
	langs.push_back(SLangText("lang_TCPnUDPport", &lang_TCPnUDPport));
	langs.push_back(SLangText("lang_gameLimits", &lang_gameLimits));
	langs.push_back(SLangText("lang_scoreLimit", &lang_scoreLimit));
	langs.push_back(SLangText("lang_teamWinLimit", &lang_teamWinLimit));
	langs.push_back(SLangText("lang_timeLimit", &lang_timeLimit));
	langs.push_back(SLangText("lang_maxPlayer", &lang_maxPlayer));
	langs.push_back(SLangText("lang_spawning", &lang_spawning));
	langs.push_back(SLangText("lang_timeToSpawn", &lang_timeToSpawn));
	langs.push_back(SLangText("lang_forceRespawn", &lang_forceRespawn));
	langs.push_back(SLangText("lang_no", &lang_no));
	langs.push_back(SLangText("lang_yes", &lang_yes));
	langs.push_back(SLangText("lang_reflectDamage", &lang_reflectDamage));
	langs.push_back(SLangText("lang_maps", &lang_maps));
	langs.push_back(SLangText("lang_includedMaps", &lang_includedMaps));

	langs.push_back(SLangText("lang_RndLabsTeam", &lang_RndLabsTeam));
	langs.push_back(SLangText("lang_gameDesigner", &lang_gameDesigner));
	langs.push_back(SLangText("lang_programmers", &lang_programmers));
	langs.push_back(SLangText("lang_soundDesigners", &lang_soundDesigners));
	langs.push_back(SLangText("lang_musics", &lang_musics));
	langs.push_back(SLangText("lang_thanksTo", &lang_thanksTo));
	langs.push_back(SLangText("lang_betaTesters", &lang_betaTesters));
	langs.push_back(SLangText("lang_buyAlbum", &lang_buyAlbum));

	langs.push_back(SLangText("lang_internet", &lang_internet));
	langs.push_back(SLangText("lang_localNetwork", &lang_localNetwork));
	langs.push_back(SLangText("lang_refreshList", &lang_refreshList));
	langs.push_back(SLangText("lang_players", &lang_players));
	langs.push_back(SLangText("lang_mapName", &lang_mapName));
//	langs.push_back(SLangText("lang_ping", &lang_ping));
	langs.push_back(SLangText("lang_join", &lang_join));
	langs.push_back(SLangText("lang_IPAdress", &lang_IPAdress));
	langs.push_back(SLangText("lang_canRefreshIn", &lang_canRefreshIn));
	langs.push_back(SLangText("lang_nbGamesRefreshed", &lang_nbGamesRefreshed));
	langs.push_back(SLangText("lang_noGameFound", &lang_noGameFound));
	langs.push_back(SLangText("lang_problemConnectingToMaster", &lang_problemConnectingToMaster));

	langs.push_back(SLangText("lang_resumeCurrentGame", &lang_resumeCurrentGame));
	langs.push_back(SLangText("lang_joinGame", &lang_joinGame));
	langs.push_back(SLangText("lang_createServer", &lang_createServer));
	langs.push_back(SLangText("lang_options", &lang_options));
	langs.push_back(SLangText("lang_credits", &lang_credits));
	langs.push_back(SLangText("lang_quit", &lang_quit));

	langs.push_back(SLangText("lang_noGameRunning", &lang_noGameRunning));

	langs.push_back(SLangText("lang_selectAtLeastOneMap", &lang_selectAtLeastOneMap));

	langs.push_back(SLangText("lang_apply", &lang_apply));
	langs.push_back(SLangText("lang_cancel", &lang_cancel));
	langs.push_back(SLangText("lang_someOptionRequireRestart", &lang_someOptionRequireRestart));
	langs.push_back(SLangText("lang_clientOptions", &lang_clientOptions));
	langs.push_back(SLangText("lang_playerName", &lang_playerName));
	langs.push_back(SLangText("lang_inGameMusic", &lang_inGameMusic));
	langs.push_back(SLangText("lang_renderingOptions", &lang_renderingOptions));
	langs.push_back(SLangText("lang_showStats", &lang_showStats));
	langs.push_back(SLangText("lang_fullscreen", &lang_fullscreen));
	langs.push_back(SLangText("lang_screenResolution", &lang_screenResolution));
	langs.push_back(SLangText("lang_colorDepth", &lang_colorDepth));
	langs.push_back(SLangText("lang_bits", &lang_bits));
	langs.push_back(SLangText("lang_wallShadowQuality", &lang_wallShadowQuality));
	langs.push_back(SLangText("lang_noShadows", &lang_noShadows));
	langs.push_back(SLangText("lang_hardEdges", &lang_hardEdges));
	langs.push_back(SLangText("lang_softEdges", &lang_softEdges));
	langs.push_back(SLangText("lang_baboShadow", &lang_baboShadow));
	langs.push_back(SLangText("lang_projectileShadow", &lang_projectileShadow));
	langs.push_back(SLangText("lang_particleLighting", &lang_particleLighting));
	langs.push_back(SLangText("lang_particleSorting", &lang_particleSorting));
	langs.push_back(SLangText("lang_terrainSplatter", &lang_terrainSplatter));
	langs.push_back(SLangText("lang_showCasing", &lang_showCasing));
	langs.push_back(SLangText("lang_groundMarkNBlood", &lang_groundMarkNBlood));
	langs.push_back(SLangText("lang_soundOptions", &lang_soundOptions));
	langs.push_back(SLangText("lang_mixRate", &lang_mixRate));
	langs.push_back(SLangText("lang_hertz", &lang_hertz));
	langs.push_back(SLangText("lang_maxSoftwareChannels", &lang_maxSoftwareChannels));
	langs.push_back(SLangText("lang_masterVolume", &lang_masterVolume));
	langs.push_back(SLangText("lang_keyOptions", &lang_keyOptions));
	langs.push_back(SLangText("lang_moveUp", &lang_moveUp));
	langs.push_back(SLangText("lang_moveDown", &lang_moveDown));
	langs.push_back(SLangText("lang_moveRight", &lang_moveRight));
	langs.push_back(SLangText("lang_moveLeft", &lang_moveLeft));
	langs.push_back(SLangText("lang_shoot", &lang_shoot));
	langs.push_back(SLangText("lang_throwGrenade", &lang_throwGrenade));
	langs.push_back(SLangText("lang_throwCocktailMolotov", &lang_throwCocktailMolotov));
	langs.push_back(SLangText("lang_use", &lang_use));
	langs.push_back(SLangText("lang_chatToAll", &lang_chatToAll));
	langs.push_back(SLangText("lang_chatToTeam", &lang_chatToTeam));
	langs.push_back(SLangText("lang_showScoreBoard", &lang_showScoreBoard));
	langs.push_back(SLangText("lang_toggleMenu", &lang_toggleMenu));

	langs.push_back(SLangText("lang_areYouSureYouWantToQuit", &lang_areYouSureYouWantToQuit));
	langs.push_back(SLangText("lang_yesQ", &lang_yesQ));
	langs.push_back(SLangText("lang_noQ", &lang_noQ));

	langs.push_back(SLangText("lang_serverHasClosedTheConnection", &lang_serverHasClosedTheConnection));

	langs.push_back(SLangText("lang_language", &lang_language));
	langs.push_back(SLangText("lang_incorrectName", &lang_incorrectName));
#endif

	languageFile = "main/languages/en.lang";
	dksvarRegister(CString("languageFile [string : \"\"]"), &languageFile, true);

#ifndef DEDICATED_SERVER
	loadLanguage(languageFile.s);
	weapons[WEAPON_DUAL_MACHINE_GUN] = new Weapon("main/models/DualMachineGun.DKO", "main/sounds/DualMachineGun.wav", .1f, gameVar.lang_dualMachineGun.s, 
		.13f, 10, 1, .8f, 2, WEAPON_DUAL_MACHINE_GUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SMG] = new Weapon("main/models/SMG.DKO", "main/sounds/SMG.wav", .1f, gameVar.lang_subMachineGun.s,
		.1f, 8, 1, .5f, 1, WEAPON_SMG, PROJECTILE_DIRECT);
   weapons[WEAPON_CHAIN_GUN] = new Weapon("main/models/ChainGun.DKO", "main/sounds/ChainGun.wav", .1f, gameVar.lang_changGun.s,
      .19f, 15, 1, 2.00f, 5, WEAPON_CHAIN_GUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SHOTGUN] = new Weapon("main/models/ShotGun.DKO", "main/sounds/Shotgun.wav", 0.85f, gameVar.lang_shotgun.s,
		.21f, 20, 5, 3.0f, 12, WEAPON_SHOTGUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SNIPER] = new Weapon("main/models/Sniper.DKO", "main/sounds/Sniper.wav", 2.0f, gameVar.lang_sniper.s,
		.30f, 0, 1, 3.0f, 0, WEAPON_SNIPER, PROJECTILE_DIRECT);
	weapons[WEAPON_BAZOOKA] = new Weapon("main/models/Bazooka.DKO", "main/sounds/Bazooka.wav", 1.75f, lang_bazooka,
		.75f, 0, 1, 3.0f, 0, WEAPON_BAZOOKA, PROJECTILE_ROCKET);
	weapons[WEAPON_GRENADE] = new Weapon("main/models/Hand.DKO", "main/sounds/Grenade.wav", 1.0f, gameVar.lang_grenade.s,
		1.5f, 0, 1, -1.0f, 0, WEAPON_GRENADE, PROJECTILE_GRENADE);
	weapons[WEAPON_COCKTAIL_MOLOTOV] = new Weapon("main/models/Hand.DKO", "main/sounds/Grenade.wav", 1.0f, "Flame",
		0.15f, 0, 1, -1.0f, 0, WEAPON_COCKTAIL_MOLOTOV, PROJECTILE_COCKTAIL_MOLOTOV);
	weapons[WEAPON_KNIVES] = new Weapon("main/models/Knifes.DKO", "main/sounds/knifes.wav", 1.0f, "Popup Knives",
		0.60f, 0, 1, 0, 0, WEAPON_KNIVES, PROJECTILE_NONE);
	weapons[WEAPON_NUCLEAR] = new Weapon("main/models/Nuclear.DKO", "main/sounds/Siren.WAV", 12.0f, "Nuke Bot",
		8.0f, 0, 1, 0, 0, WEAPON_NUCLEAR, PROJECTILE_NONE);
	weapons[WEAPON_PHOTON_RIFLE] = new Weapon("main/models/PhotonRifle.DKO", "main/sounds/PhotonRifle.wav", 1.5f, "Photon Rifle",
		0.24f, 0, 1, 5.0f, 0, WEAPON_PHOTON_RIFLE, PROJECTILE_DIRECT);
	weapons[WEAPON_FLAME_THROWER] = new Weapon("main/models/FlameThrower.DKO", "main/sounds/FlameThrower.wav", .1f, "Flame Thrower",
		.08f, 10, 1, 0, 10, WEAPON_FLAME_THROWER, PROJECTILE_DIRECT);
	weapons[WEAPON_SHIELD] = new Weapon("main/models/Shield.DKO", "main/sounds/shield.wav", 3.0f, "Instant Shield",
		0, 0, 1, 0, 0, WEAPON_SHIELD, PROJECTILE_NONE);
	#if defined(_PRO_) && defined(_MINIBOT_)
		weapons[WEAPON_MINIBOT] = new Weapon("main/models/Antena.DKO", "main/sounds/equip.wav", 1.0f, "Mini Bot",
			.05f, 0, 1, 0, 0, WEAPON_MINIBOT, PROJECTILE_NONE);
	#endif
#else
	weapons[WEAPON_DUAL_MACHINE_GUN] = new Weapon("", "", .1f, "Dual Machine Gun",
		.13f, 10, 1, .8f, 2, WEAPON_DUAL_MACHINE_GUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SMG] = new Weapon("", "", .1f, "SMG",
		.1f, 8, 1, .5f, 1, WEAPON_SMG, PROJECTILE_DIRECT);
	weapons[WEAPON_CHAIN_GUN] = new Weapon("", "", .1f, "ChainGun",
		.19f, 15, 1, 2.00f, 5, WEAPON_CHAIN_GUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SHOTGUN] = new Weapon("", "", 0.85f, "Shotgun",
		.21f, 20, 5, 3.0f, 12, WEAPON_SHOTGUN, PROJECTILE_DIRECT);
	weapons[WEAPON_SNIPER] = new Weapon("", "", 2.0f,"Sniper Rifle",
		.30f, 0, 1, 3.0f, 0, WEAPON_SNIPER, PROJECTILE_DIRECT);
	weapons[WEAPON_BAZOOKA] = new Weapon("", "", 1.75f, "Bazooka",
		.75f, 0, 1, 3.0f, 0, WEAPON_BAZOOKA, PROJECTILE_ROCKET);
	weapons[WEAPON_GRENADE] = new Weapon("", "", 1.0f, "Grenade",
		1.5f, 0, 1, -1.0f, 0, WEAPON_GRENADE, PROJECTILE_GRENADE);
	weapons[WEAPON_COCKTAIL_MOLOTOV] = new Weapon("", "", 1.0f, "Flame",
		0.15f, 0, 1, -1.0f, 0, WEAPON_COCKTAIL_MOLOTOV, PROJECTILE_COCKTAIL_MOLOTOV);
	weapons[WEAPON_KNIVES] = new Weapon("", "", 1.0f, "Popup Knives",
		0.60f, 0, 1, 0, 0, WEAPON_KNIVES, PROJECTILE_NONE);
	weapons[WEAPON_NUCLEAR] = new Weapon("", "", 12.0f, "Nuke Bot",
		8.0f, 0, 1, 0, 0, WEAPON_NUCLEAR, PROJECTILE_NONE);
	weapons[WEAPON_PHOTON_RIFLE] = new Weapon("", "", 1.5f, "Photon Rifle",
		0.24f, 0, 1, 5.0f, 0, WEAPON_PHOTON_RIFLE, PROJECTILE_DIRECT);
	weapons[WEAPON_FLAME_THROWER] = new Weapon("", "", .1f, "Flame Thrower",
		.08f, 6, 1, 0, 6, WEAPON_FLAME_THROWER, PROJECTILE_DIRECT);
	weapons[WEAPON_SHIELD] = new Weapon("", "", 3.0f, "Instant Shield",
		0, 0, 1, 0, 0, WEAPON_SHIELD, PROJECTILE_NONE);
	#if defined(_PRO_) && defined(_MINIBOT_)
		weapons[WEAPON_MINIBOT] = new Weapon("", "", 1.0f, "Mini Bot",
			.05f, 0, 1, 0, 0, WEAPON_MINIBOT, PROJECTILE_NONE);
	#endif
#endif


	sv_timeToSpawn = 5;
	dksvarRegister(CString("sv_timeToSpawn [float : (default 5)]"), &sv_timeToSpawn, 0, 60,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_friendlyFire = false;
	dksvarRegister(CString("sv_friendlyFire [bool : (default false)]"), &sv_friendlyFire, true);
	sv_reflectedDamage = false;
	dksvarRegister(CString("sv_reflectedDamage [bool : (default false)]"), &sv_reflectedDamage, true);
	sv_topView = true;
	dksvarRegister(CString("sv_topView [bool : (default true)]"), &sv_topView, true);
	sv_minSendInterval = 2;
	dksvarRegister(CString("sv_minSendInterval [int : (default 2)]"), &sv_minSendInterval, 0, 5,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_forceRespawn = false;
	dksvarRegister(CString("sv_forceRespawn [bool : (default false)]"), &sv_forceRespawn, true);
	sv_baboStats = false;
	dksvarRegister(CString("sv_baboStats [bool : (default false)]"), &sv_baboStats, true);
	sv_roundTimeLimit = 3*60;
	dksvarRegister(CString("sv_roundTimeLimit [float : 0 = unlimited (default 180)]"), &sv_roundTimeLimit,
		0, 0, LIMIT_MIN, true);
	sv_gameTimeLimit = 30*60;
	dksvarRegister(CString("sv_gameTimeLimit [float : 0 = unlimited (default 1800)]"), &sv_gameTimeLimit,
		0, 0, LIMIT_MIN, true);
	sv_scoreLimit = 50;
	dksvarRegister(CString("sv_scoreLimit [int : 0 = unlimited (default 50)]"), &sv_scoreLimit,
		0, 0, LIMIT_MIN, true);
	sv_winLimit = 7;
	dksvarRegister(CString("sv_winLimit [int : 0 = unlimited (default 7)]"), &sv_winLimit,
		0, 0, LIMIT_MIN, true);

   sv_serverType = 0;
	dksvarRegister(CString("sv_serverType [int : 0=Normal, 1=Pro]"),
		&sv_serverType, 0, 1, LIMIT_MIN | LIMIT_MAX, true);

#if defined(_PRO_)
   sv_spawnType = 0;
	dksvarRegister(CString("sv_spawnType [int : 0=Normal, 1=Ladder]"),
		&sv_spawnType, 0, 1, LIMIT_MIN | LIMIT_MAX, true);

	sv_gameType = 1;
	dksvarRegister(CString("sv_gameType [int : 0=Deathmatch, 1=Team Deathmatch, 2=Capture The Flag, 3=Champion]"),
		&sv_gameType, 0, 3, LIMIT_MIN | LIMIT_MAX, true);

	sv_subGameType = 0;
	dksvarRegister(CString("sv_subGameType [int : 0=Normal, 1=Instagib, 2=RandomWeapon]"),
		&sv_subGameType, 0, 2, LIMIT_MIN | LIMIT_MAX, true);
#else
	sv_gameType = 1;
	dksvarRegister(CString("sv_gameType [int : 0=Deathmatch, 1=Team Deathmatch, 2=Capture The Flag]"),
		&sv_gameType, 0, 3, LIMIT_MIN | LIMIT_MAX, true);
#endif

	sv_bombTime = 60;
	dksvarRegister(CString("sv_bombTime [int : (default 60)]"), &sv_bombTime, 10, 0, LIMIT_MIN, true);
	sv_gameName = "Babo Violent 2 - Server";
	dksvarRegister(CString("sv_gameName [string : \"\"]"), &sv_gameName, true);
	sv_port = 3333;
	dksvarRegister(CString("sv_port [int : valid port (default 3333)]"), &sv_port, 1024, 65536,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_maxPlayer = 16; // Max player in the game
	dksvarRegister(CString("sv_maxPlayer [int : 1 to 32 (default 16)]"), &sv_maxPlayer, 1, 32,
		LIMIT_MIN | LIMIT_MAX, true);	   
   sv_maxPlayerInGame = 0; // Max player in the game
   dksvarRegister(CString("sv_maxPlayerInGame [int : 0 to 32 (default 0, no limit)]"), &sv_maxPlayerInGame, 0, 32,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_password = "";
	dksvarRegister(CString("sv_password [string : \"\""), &sv_password, true);
	sv_enableSMG = true;
	dksvarRegister(CString("sv_enableSMG [bool : true | false (default true)]"), &sv_enableSMG, true);
	sv_enableShotgun = true;
	dksvarRegister(CString("sv_enableShotgun [bool : true | false (default true)]"), &sv_enableShotgun, true);
	sv_enableSniper = true;
	dksvarRegister(CString("sv_enableSniper [bool : true | false (default true)]"), &sv_enableSniper, true);
	sv_enableDualMachineGun = true;
	dksvarRegister(CString("sv_enableDualMachineGun [bool : true | false (default true)]"), &sv_enableDualMachineGun, true);
	sv_enableChainGun = true;
	dksvarRegister(CString("sv_enableChainGun [bool : true | false (default true)]"), &sv_enableChainGun, true);
	sv_enableBazooka = true;
	dksvarRegister(CString("sv_enableBazooka [bool : true | false (default true)]"), &sv_enableBazooka, true);
	sv_enablePhotonRifle = true;
	dksvarRegister(CString("sv_enablePhotonRifle [bool : true | false (default true)]"), &sv_enablePhotonRifle, true);
	sv_enableFlameThrower = true;
	dksvarRegister(CString("sv_enableFlameThrower [bool : true | false (default true)]"), &sv_enableFlameThrower, true);
	sv_enableShotgunReload = true;
	dksvarRegister(CString("sv_enableShotgunReload [bool : true | false (default true)]"), &sv_enableShotgunReload, true);
	sv_slideOnIce = false;
	dksvarRegister(CString("sv_slideOnIce [bool : true | false (default false)]"), &sv_slideOnIce, true);
	sv_showEnemyTag = false;
	dksvarRegister(CString("sv_showEnemyTag [bool : true | false (default false)]"), &sv_showEnemyTag, true);
	zsv_adminUser = "";
	dksvarRegister(CString("zsv_adminUser [string : \"\""), &zsv_adminUser, true);
	zsv_adminPass = "";
	dksvarRegister(CString("zsv_adminPass [string : \"\""), &zsv_adminPass, true);
	sv_enableSecondary = true;
	dksvarRegister(CString("sv_enableSecondary [bool : true | false (default true)]"), &sv_enableSecondary, true);
	sv_enableKnives = true;
	dksvarRegister(CString("sv_enableKnives [bool : true | false (default true)]"), &sv_enableKnives, true);
	sv_enableNuclear = true;
	dksvarRegister(CString("sv_enableNuclear [bool : true | false (default true)]"), &sv_enableNuclear, true);
	sv_enableShield = true;
	dksvarRegister(CString("sv_enableShield [bool : true | false (default true)]"), &sv_enableShield, true);
	#if defined(_PRO_)
		sv_enableMinibot = true;
		dksvarRegister(CString("sv_enableMinibot [bool : true | false (default true)]"), &sv_enableMinibot, true);
	#endif
	sv_autoBalance = true;
	dksvarRegister(CString("sv_autoBalance [bool : true | false (default true)]"), &sv_autoBalance, true);
	sv_autoBalanceTime = 4;
	dksvarRegister(CString("sv_autoBalanceTime [int : 1 to 15 (default 4)]"), &sv_autoBalanceTime,
		1, 15, LIMIT_MIN | LIMIT_MAX, true);
	sv_enableVote = true;
	dksvarRegister(CString("sv_enableVote [bool : true | false (default true)]"), &sv_enableVote, true);
	sv_gamePublic = true;
	dksvarRegister(CString("sv_gamePublic [bool : true | false (default true)]"), &sv_gamePublic, true);
	sv_maxUploadRate = 8.0f;
	dksvarRegister(CString("sv_maxUploadRate [float : 0 = unlimited (default 8.0)]"), &sv_maxUploadRate,
		0, 0, LIMIT_MIN, true);
	sv_showKills = false;
	dksvarRegister(CString("sv_showKills [bool : true | false (default false)]"), &sv_showKills, true);
	sv_matchcode = "";
	dksvarRegister(CString("sv_matchcode [string : \"\" (default \"\")]"), &sv_matchcode, true);
	sv_matchmode = 0;
	dksvarRegister(CString("sv_matchmode [int : 0 = unlimited (default 0)]"), &sv_matchmode, 0, 0, LIMIT_MIN, true);
	sv_report = false;
	dksvarRegister(CString("sv_report [bool : true | false (default false)]"), &sv_report, true);
	sv_maxPing = 1000;
	dksvarRegister(CString("sv_maxPing [int : 0 to 1000 (default 1000)]"), &sv_maxPing, 0, 1000,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_autoSpectateWhenIdle = true;
	dksvarRegister(CString("sv_autoSpectateWhenIdle [bool : true | false (default true)]"), &sv_autoSpectateWhenIdle, true);
	sv_autoSpectateIdleMaxTime = 180;
	dksvarRegister(CString("sv_autoSpectateIdleMaxTime [int : 60 to unlimited (default 180)]"), &sv_autoSpectateIdleMaxTime, 60, 0,
		LIMIT_MIN, true);
	sv_beGoodServer = true;
	dksvarRegister(CString("sv_beGoodServer [bool : true | false (default true)]"), &sv_beGoodServer, true);
	sv_validateWeapons = true;
	dksvarRegister(CString("sv_validateWeapons [bool : true | false (default true)]"), &sv_validateWeapons, true);
	sv_minTilesPerBabo = 55.0;
	dksvarRegister(CString("sv_minTilesPerBabo [float : 0.0 to 250.0 (default 55.0)]"), &sv_minTilesPerBabo, 0, 250,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_maxTilesPerBabo = 80.0;
	dksvarRegister(CString("sv_maxTilesPerBabo [float : 0.0 to 500.0 (default 80.0)]"), &sv_maxTilesPerBabo, 0, 500,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_zookaRemoteDet = true;
	dksvarRegister(CString("sv_zookaRemoteDet [bool : true | false (default true)]"), &sv_zookaRemoteDet, true);
	sv_explodingFT = false;
	dksvarRegister(CString("sv_explodingFT [bool : true | false (default false)]"), &sv_explodingFT, true);
	sv_sendJoinMessage = true;
	dksvarRegister(CString("sv_sendJoinMessage [bool : true | false (default true)]"), &sv_sendJoinMessage,true);
	sv_enableMolotov = true;
	dksvarRegister(CString("sv_enableMolotov [bool : true | false (default true)]"), &sv_enableMolotov,true);
	sv_joinMessage = "Welcome to the server!\0";
	dksvarRegister(CString("sv_joinMessage [string : \"\" ]"), &sv_joinMessage, true);
	sv_shottyDropRadius = 0.40f;
	dksvarRegister(CString("sv_shottyDropRadius [float : 0 to 2 (default 0.40)]"), &sv_shottyDropRadius, 0, 2,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_shottyRange = 6.75f;
	dksvarRegister(CString("sv_shottyRange [float : 1 to 24 (default 6.75)]"), &sv_shottyRange, 1, 24,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_ftMaxRange = 8.0f;
	dksvarRegister(CString("sv_ftMaxRange [float : 1 to 24  (default 8.0)]"), &sv_ftMaxRange, 1, 24,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_ftMinRange = 1.0f;
	dksvarRegister(CString("sv_ftMinRange [float : 0 to 24  (default 1.0)]"), &sv_ftMinRange, 0, 24,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_ftExpirationTimer = 1.5f;
	dksvarRegister(CString("sv_ftExpirationTimer [float : 0 to 30  (default 1.5)]"), &sv_ftExpirationTimer, 0, 30,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_photonDamageCoefficient = 0.5f;
	dksvarRegister(CString("sv_photonDamageCoefficient [float : -100 to 100 (default 0.5)]"), &sv_photonDamageCoefficient, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_photonVerticalShift = 0.325f;
	dksvarRegister(CString("sv_photonVerticalShift [float : -100 to 100 (default 0.325)]"), &sv_photonVerticalShift, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_photonDistMult = 0.25;
	dksvarRegister(CString("sv_photonDistMult [float : -100 to 100 (default 0.25)]"), &sv_photonDistMult, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_photonHorizontalShift = 5.0;
	dksvarRegister(CString("sv_photonHorizontalShift [float : -100 to 100 (default 5.00)]"), &sv_photonHorizontalShift, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_ftDamage = 0.12f;
	dksvarRegister(CString("sv_ftDamage [float : -100 to 100 (actual damage 100 times this, default 0.12)]"), &sv_ftDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_smgDamage = 0.1f;
	dksvarRegister(CString("sv_smgDamage [float : -100 to 100 (actual damage 100 times this, default 0.10)]"), &sv_smgDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_dmgDamage = 0.14f;
	dksvarRegister(CString("sv_dmgDamage [float : -100 to 100 (actual damage 100 times this, default 0.14)]"), &sv_dmgDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_cgDamage = 0.16f;
	dksvarRegister(CString("sv_cgDamage [float : -100 to 100 (actual damage 100 times this, default 0.16)]"), &sv_cgDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_sniperDamage = 0.2f;
	dksvarRegister(CString("sv_sniperDamage [float : -100 to 100 (actual damage 100 times this, default 0.20)]"), &sv_sniperDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_shottyDamage = 0.21f;
	dksvarRegister(CString("sv_shottyDamage [float : -100 to 100 (actual damage 100 times this, default 0.21)]"), &sv_shottyDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_zookaDamage = 0.85f;
	dksvarRegister(CString("sv_zookaDamage [float : -100 to 100 (actual damage 100 times this, default 0.85)]"), &sv_zookaDamage, -100, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_zookaRadius = 2.0f;
	dksvarRegister(CString("sv_zookaRadius [float : 1 to 8 (default 2.0)]"), &sv_zookaRadius, 1, 8,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_nukeRadius = 6.0f;
	dksvarRegister(CString("sv_nukeRadius [float : 4 to 16 (default 6.0)]"), &sv_nukeRadius, 4, 16,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_nukeTimer = 3.0f;
	dksvarRegister(CString("sv_nukeTimer [float : 0 to 12 (default 3.0)]"), &sv_nukeTimer, 0, 12,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_nukeReload = 12.0f;
	dksvarRegister(CString("sv_nukeReload [float : 0 to 48 (default 12.0)]"), &sv_nukeReload, 0, 48,
		LIMIT_MIN | LIMIT_MAX, true);
	sv_photonType = 1;
	dksvarRegister(CString("sv_photonType [int : 0 to 3 (default 1)"), &sv_photonType, 0, 3,
		LIMIT_MIN | LIMIT_MAX, true);

	sv_spawnImmunityTime = 2.0f;
	dksvarRegister(CString("sv_spawnImmunityTime [float : 0 to 3 (default 2.0)]"), &sv_spawnImmunityTime, 0, 3, LIMIT_MIN | LIMIT_MAX, true);
	
	db_accountServer = "http://ladder.rndlabs.ca/bv2link.php";
	db_version = 0;
	FetchDBInfos();

//	dksvarRegister(CString("cl_accountServer [string : \"URL\"]", cl_accountServer.s), &cl_accountServer, true);

	cl_accountUsername = "";
	dksvarRegister(CString("cl_accountUsername [string : \"\" (default \"\")]"), &cl_accountUsername, true);
	cl_accountPassword = "";
	dksvarRegister(CString("cl_accountPassword [string : \"\" (default \"\")]"), &cl_accountPassword, true);
	cl_playerName = "Unnamed Babo";
	dksvarRegister(CString("cl_playerName [string : \"\" (default \"Unnamed Babo\")]"), &cl_playerName, true);
	cl_mapAuthorName = "";
	dksvarRegister(CString("cl_mapAuthorName [string : \"\" (default \"\", max 24 characters)]"), &cl_mapAuthorName, true);
	cl_cubicMotion = true;
	dksvarRegister(CString("cl_cubicMotion [bool : true | false (default true)]"), &cl_cubicMotion, true);
	cl_lastUsedIP = "0.0.0.0";
	dksvarRegister(CString("cl_lastUsedIP [string : \"\"]"), &cl_lastUsedIP, true);
	cl_port = 3333;
	dksvarRegister(CString("cl_port [int : (default 3333)"), &cl_port, 1024, 65536,
		LIMIT_MIN | LIMIT_MAX, true);
	cl_password = "";
	dksvarRegister(CString("cl_password [string : \"\"]"), &cl_password, true);
	cl_redDecal.set(.5f,.5f,1);
	dksvarRegister(CString("cl_redDecal [vector3f : (default .5 .5 1)]"), &cl_redDecal, true);
	cl_greenDecal.set(0,0,1);
	dksvarRegister(CString("cl_greenDecal [vector3f : (default 0 0 1)]"), &cl_greenDecal, true);
	cl_blueDecal.set(0,0,.5f);
	dksvarRegister(CString("cl_blueDecal [vector3f : (default 0 0 .5)]"), &cl_blueDecal, true);
	cl_skin = "skin10";
	dksvarRegister(CString("cl_skin [string : \"skin10\"]"), &cl_skin, true);
	cl_teamIndicatorType = 0;
	dksvarRegister(CString("cl_teamIndicatorType [int : 0=Colourize, 1=Halos 2=TeamHalosOnly(default 0)]"),
		&cl_teamIndicatorType,0,2, LIMIT_MIN | LIMIT_MAX, true);
	cl_glowSize = 0.5f;
	dksvarRegister(CString("cl_glowSize [float : 0.25 to 0.5 (default 0.5)]"),&cl_glowSize,0.25,0.5, LIMIT_MIN | LIMIT_MAX,true);
	cl_preciseCursor = true;
	dksvarRegister(CString("cl_preciseCursor [bool : true | false (default true)]"),&cl_preciseCursor,true);
	cl_enableXBox360Controller = false;
	dksvarRegister(CString("cl_enableXBox360Controller [bool : true | false (default false)]"), &cl_enableXBox360Controller, true);
	cl_grassTextureForAllMaps = false;
	dksvarRegister(CString("cl_grassTextureForAllMaps [bool : true | false (default false)]"),&cl_grassTextureForAllMaps,true);
	cl_enableVSync = true;
	dksvarRegister(CString("cl_enableVSync [bool : true | false (default true)]"), &cl_enableVSync, true);
	cl_affinityMode = 0;
	dksvarRegister(CString("cl_affinityMode [int : 0=Default, 1=PartialBias, 2=FullBias]"),
		&cl_affinityMode, 0, 2, LIMIT_MIN | LIMIT_MAX, true);
	cl_primaryWeapon = 0;
	dksvarRegister(CString("cl_primaryWeapon [int : 0=SMG, 1=Shotgun, 2=Sniper, 3=DMG, 4=Chaingun, 5=Bazooka, 6=Photon, 7=Flamethrower]"),
		&cl_primaryWeapon, 0, 7, LIMIT_MIN | LIMIT_MAX, true);
	//cl_weaponSideRight = true;
	//dksvarRegister(CString("cl_weaponSideRight [bool : true | false (default true)]"),&cl_weaponSideRight,true);
	cl_secondaryWeapon = 0;
	dksvarRegister(CString("cl_secondaryWeapon [int : 0=Knives, 1=Nuclear, 2=Shield, 3=Minibot]"),
		&cl_secondaryWeapon, 0, 3, LIMIT_MIN | LIMIT_MAX, true);
#ifndef DISABLE_QUICK_MESSAGES
	// quick messages
	cl_qMsg01 = "aHello!";
	dksvarRegister(CString("cl_qMsg01 [string]"), &cl_qMsg01, true);
	cl_qMsg02 = "aHelp!";
	dksvarRegister(CString("cl_qMsg02 [string]"), &cl_qMsg02, true);
	cl_qMsg03 = "aRun!";
	dksvarRegister(CString("cl_qMsg03 [string]"), &cl_qMsg03, true);
	cl_qMsg04 = "aHaha!!!";
	dksvarRegister(CString("cl_qMsg04 [string]"), &cl_qMsg04, true);
	cl_qMsg05 = "aArgh...";
	dksvarRegister(CString("cl_qMsg05 [string]"), &cl_qMsg05, true);
	cl_qMsg06 = "tI\'m on defence";
	dksvarRegister(CString("cl_qMsg06 [string]"), &cl_qMsg06, true);
	cl_qMsg07 = "tI\'m on offence";
	dksvarRegister(CString("cl_qMsg07 [string]"), &cl_qMsg07, true);
	cl_qMsg08 = "tStay here!";
	dksvarRegister(CString("cl_qMsg08 [string]"), &cl_qMsg08, true);
	cl_qMsg09 = "tWait for me!";
	dksvarRegister(CString("cl_qMsg09 [string]"), &cl_qMsg09, true);
	cl_qMsg10 = "tGO! GO! GO!";
	dksvarRegister(CString("cl_qMsg10 [string]"), &cl_qMsg10, true);
#endif

		scl_honor = 10;
		scl_xp = 25;
		scl_leftToNextLevel = 75;
		scl_totalKill = 75;
		scl_totalDeath = 50;
		scl_ratio = 75.0f / 50.0f;
		scl_weaponOfChoice = 0;
		for (int i=0;i<20;scl_killWeapon[i++] = 0);

	r_showStats = false;
	dksvarRegister(CString("r_showStats [bool : true | false (default false)]"), &r_showStats, true);
#ifdef _DEBUG
	r_fullScreen = false;
#else
	r_fullScreen = true;
#endif
	dksvarRegister(CString("r_fullScreen [bool : true | false (default true)]"), &r_fullScreen, true);
	r_resolution.set(800,600);
	dksvarRegister(CString("r_resolution [vector2i : (default 800 600)]"), &r_resolution, true);
	r_bitdepth = 32;
	dksvarRegister(CString("r_bitdepth [int : 16 | 32 (default 32)]"), &r_bitdepth, 16, 32,
		LIMIT_MIN | LIMIT_MAX, true);
	r_refreshRate = -1;
	dksvarRegister(CString("r_refreshRate [int : -1 | 60 | 70 | 72 | 75 | 85 (default -1)]"), &r_refreshRate, 60, 85,
		LIMIT_MIN | LIMIT_MAX, true);
	r_weatherEffects = false;
	dksvarRegister(CString("r_weatherEffects [bool : true | false (default false)]"), &r_weatherEffects, true);
	r_reflection = false;
	dksvarRegister(CString("r_reflection [bool : true | false (default false)]"), &r_reflection, true);
	r_shadowQuality = 2;
	dksvarRegister(CString("r_shadowQuality [int : 0=none, 1=hard, 2=soft (default 2)]"), &r_shadowQuality,
		0, 2, LIMIT_MIN | LIMIT_MAX, true);
	r_playerShadow = true;
	dksvarRegister(CString("r_playerShadow [bool : true | false (default true)]"), &r_playerShadow, true);
	r_projectileShadow = true;
	dksvarRegister(CString("r_projectileShadow [bool : true | false (default true)]"), &r_projectileShadow, true);
	r_particleLighting = true;
	dksvarRegister(CString("r_particleLighting [bool : true | false (default true)]"), &r_particleLighting, true);
	r_particleSort = false;
	dksvarRegister(CString("r_particleSort [bool : true | false (default false)]"), &r_particleSort, true);
	r_terrainSplater = true;
	dksvarRegister(CString("r_terrainSplater [bool : true | false (default true)]"), &r_terrainSplater, true);
	r_showCasing = true;
	dksvarRegister(CString("r_showCasing [bool : true | false (default true)]"), &r_showCasing, true);
	r_showGroundMark = true;
	dksvarRegister(CString("r_showGroundMark [bool : true | false (default true)]"), &r_showGroundMark, true);
	r_highDetailMenu = true;
	dksvarRegister(CString("r_highDetailMenu [bool : true | false (default true)]"), &r_highDetailMenu, true);
	r_animatedMenu = false;
	dksvarRegister(CString("r_animatedMenu [bool : true | false (default false)]"), &r_animatedMenu, true);
	r_showLatency = false;
	dksvarRegister(CString("r_showLatency [bool : true | false (default false)]"), &r_showLatency, true);
    r_reducedParticles = false;
	dksvarRegister(CString("r_reducedParticles [bool : true | false (default false)]"), &r_reducedParticles, true);
    r_simpleText = false;
	dksvarRegister(CString("r_simpleText [bool : true | false (default false)]"), &r_simpleText, true);
	r_widescreen = 0;
	dksvarRegister(CString("r_widescreen [int : 0=disabled, 1=stretch, 2=bars (default 0)]"), &r_widescreen, 0, 2, LIMIT_MIN | LIMIT_MAX, true);
	r_maxNameLenOverBabo = 0;
	dksvarRegister(CString("r_maxNameLenOverBabo [int : (default 0=no limit)]"), &r_maxNameLenOverBabo, 0, 0, LIMIT_MIN, true);

#if defined(_PRO_)
	r_chatTextSize = 28;   
	dksvarRegister(CString("r_chatTextSize [int : (default 28)]"), &r_chatTextSize,
		8, 28, LIMIT_MIN | LIMIT_MAX, true);
	r_eventTextSize = 28;   
	dksvarRegister(CString("r_eventTextSize [int : (default 28)]"), &r_eventTextSize,
		8, 28, LIMIT_MIN | LIMIT_MAX, true);
   r_showEventText = true;
	dksvarRegister(CString("r_showEventText [bool : true | false (default true)]"), &r_showEventText, true);
#endif

	s_mixRate = 22050;
	dksvarRegister(CString("s_mixRate [int : 22050 | 44100 (default 22050)]"), &s_mixRate, 22050, 44100,
		LIMIT_MIN | LIMIT_MAX, true);
	s_maxSoftwareChannels = 16;
	dksvarRegister(CString("s_maxSoftwareChannels [int : (default 16)]"), &s_maxSoftwareChannels,
		4, 64, LIMIT_MIN | LIMIT_MAX, true);
	s_masterVolume = 1;
	dksvarRegister(CString("s_masterVolume [float : 0-1 (default 1)]"), &s_masterVolume, 0, 100,
		LIMIT_MIN | LIMIT_MAX, true);
	s_inGameMusic = true;
	dksvarRegister(CString("s_inGameMusic [bool : true | false (default true)]"), &s_inGameMusic, true);

#ifndef DEDICATED_SERVER
	k_moveUp = keyManager.getKeyByName("W");
	dksvarRegister(CString("k_moveUp [int]"), &k_moveUp, 0, 0, 0, true);
	k_moveDown = keyManager.getKeyByName("S");
	dksvarRegister(CString("k_moveDown [int]"), &k_moveDown, 0, 0, 0, true);
	k_moveRight = keyManager.getKeyByName("D");
	dksvarRegister(CString("k_moveRight [int]"), &k_moveRight, 0, 0, 0, true);
	k_moveLeft = keyManager.getKeyByName("A");
	dksvarRegister(CString("k_moveLeft [int]"), &k_moveLeft, 0, 0, 0, true);
	k_shoot = keyManager.getKeyByName("Mouse1");
	dksvarRegister(CString("k_shoot [int]"), &k_shoot, 0, 0, 0, true);
	k_throwGrenade = keyManager.getKeyByName("Mouse2");
	dksvarRegister(CString("k_throwGrenade [int]"), &k_throwGrenade, 0, 0, 0, true);
	k_throwMolotov = keyManager.getKeyByName("Mouse3");
	dksvarRegister(CString("k_throwMolotov [int]"), &k_throwMolotov, 0, 0, 0, true);
	k_pickUp = keyManager.getKeyByName("F");
	dksvarRegister(CString("k_pickUp [int]"), &k_pickUp, 0, 0, 0, true);
	k_chatAll = keyManager.getKeyByName("T");
	dksvarRegister(CString("k_chatAll [int]"), &k_chatAll, 0, 0, 0, true);
	k_chatTeam = keyManager.getKeyByName("Y");
	dksvarRegister(CString("k_chatTeam [int]"), &k_chatTeam, 0, 0, 0, true);
	k_showScore = keyManager.getKeyByName("TAB");
	dksvarRegister(CString("k_showScore [int]"), &k_showScore, 0, 0, 0, true);
	k_menuAccess = keyManager.getKeyByName("Escape");
	dksvarRegister(CString("k_menuAccess [int]"), &k_menuAccess, 0, 0, 0, true);
	k_melee = keyManager.getKeyByName("Space");
	dksvarRegister(CString("k_melee [int]"), &k_melee, 0, 0, 0, true);
#if defined(_PRO_)
   k_screenShot = keyManager.getKeyByName("P");
	dksvarRegister(CString("k_screenShot [int]"), &k_screenShot, 0, 0, 0, true);

   k_stats = keyManager.getKeyByName("L");
	dksvarRegister(CString("k_stats [int]"), &k_stats, 0, 0, 0, true);
#endif

#ifndef DISABLE_QUICK_MESSAGES
	k_qMsg01 = keyManager.getKeyByName("1");
	dksvarRegister(CString("k_qMsg01 [int]"), &k_qMsg01, 0, 0, 0, true);
	k_qMsg02 = keyManager.getKeyByName("2");
	dksvarRegister(CString("k_qMsg02 [int]"), &k_qMsg02, 0, 0, 0, true);
	k_qMsg03 = keyManager.getKeyByName("3");
	dksvarRegister(CString("k_qMsg03 [int]"), &k_qMsg03, 0, 0, 0, true);
	k_qMsg04 = keyManager.getKeyByName("4");
	dksvarRegister(CString("k_qMsg04 [int]"), &k_qMsg04, 0, 0, 0, true);
	k_qMsg05 = keyManager.getKeyByName("5");
	dksvarRegister(CString("k_qMsg05 [int]"), &k_qMsg05, 0, 0, 0, true);
	k_qMsg06 = keyManager.getKeyByName("6");
	dksvarRegister(CString("k_qMsg06 [int]"), &k_qMsg06, 0, 0, 0, true);
	k_qMsg07 = keyManager.getKeyByName("7");
	dksvarRegister(CString("k_qMsg07 [int]"), &k_qMsg07, 0, 0, 0, true);
	k_qMsg08 = keyManager.getKeyByName("8");
	dksvarRegister(CString("k_qMsg08 [int]"), &k_qMsg08, 0, 0, 0, true);
	k_qMsg09 = keyManager.getKeyByName("9");
	dksvarRegister(CString("k_qMsg09 [int]"), &k_qMsg09, 0, 0, 0, true);
	k_qMsg10 = keyManager.getKeyByName("0");
	dksvarRegister(CString("k_qMsg10 [int]"), &k_qMsg10, 0, 0, 0, true);
#endif
#endif

	c_debug = false; // Default
	dksvarRegister(CString("c_debug [bool : true | false (default false)]"), &c_debug, true);

	c_huge = false;
	dksvarRegister(CString("c_huge [bool : true | false (default false)]"), &c_huge, true);
	d_showPath = false;
	d_showNodes = false;
	dksvarRegister(CString("d_showPath [bool : true | false (default false)]"), &d_showPath, false);
	dksvarRegister(CString("d_showNodes [bool : true | false (default false)]"), &d_showNodes, false);

#ifndef DEDICATED_SERVER
	ro_nbParticle = 0;
	ro_hitPoint.set(0,0,0);
#endif

}



//
// Destructeur
//
GameVar::~GameVar()
{
#ifndef DEDICATED_SERVER
	langs.clear();
#endif
}



//
// Pour envoyer les variables servers au autres tayouin
//
void GameVar::sendSVVar(UINT4 babonetID)
{
	sendOne("sv_friendlyFire", babonetID);
	sendOne("sv_reflectedDamage", babonetID);
	sendOne("sv_timeToSpawn", babonetID);
	sendOne("sv_topView", babonetID);
	sendOne("sv_minSendInterval", babonetID);
	sendOne("sv_forceRespawn", babonetID);
	sendOne("sv_baboStats", babonetID);
	sendOne("sv_roundTimeLimit", babonetID);
	sendOne("sv_gameTimeLimit", babonetID);
	sendOne("sv_scoreLimit", babonetID);
	sendOne("sv_winLimit", babonetID);
	sendOne("sv_gameType", babonetID);
   sendOne("sv_serverType", babonetID);
#if defined(_PRO_)
   sendOne("sv_spawnType", babonetID);
   sendOne("sv_subGameType", babonetID);
#endif
	sendOne("sv_bombTime", babonetID);
	sendOne("sv_gameName", babonetID);
	sendOne("sv_port", babonetID);
	sendOne("sv_maxPlayer", babonetID);
   sendOne("sv_maxPlayerInGame", babonetID);
	sendOne("sv_password", babonetID);
	sendOne("sv_enableSMG", babonetID);
	sendOne("sv_enableShotgun", babonetID);
	sendOne("sv_enableSniper", babonetID);
	sendOne("sv_enableDualMachineGun", babonetID);
	sendOne("sv_enableChainGun", babonetID);
	sendOne("sv_enableBazooka", babonetID);
	sendOne("sv_enablePhotonRifle", babonetID);
	sendOne("sv_enableFlameThrower", babonetID);
	sendOne("sv_enableShotgunReload", babonetID);
	sendOne("sv_slideOnIce", babonetID);
	sendOne("sv_showEnemyTag", babonetID);
	sendOne("sv_enableSecondary", babonetID);
	sendOne("sv_enableKnives", babonetID);
	sendOne("sv_enableNuclear", babonetID);
	sendOne("sv_enableShield", babonetID);
#if defined(_PRO_)
	sendOne("sv_enableMinibot", babonetID);
#endif
	sendOne("sv_autoBalance", babonetID);
	sendOne("sv_autoBalanceTime", babonetID);
	sendOne("sv_gamePublic", babonetID);
	sendOne("sv_matchcode", babonetID);
	sendOne("sv_matchmode", babonetID);
	sendOne("sv_maxPing", babonetID);
	sendOne("sv_shottyDropRadius", babonetID);
	sendOne("sv_shottyRange", babonetID);
	sendOne("sv_enableMolotov", babonetID);
	sendOne("sv_ftMaxRange", babonetID);
	sendOne("sv_ftMinRange", babonetID);
	sendOne("sv_photonDamageCoefficient", babonetID);
	sendOne("sv_zookaRemoteDet", babonetID);
	sendOne("sv_smgDamage", babonetID);
	sendOne("sv_ftDamage", babonetID);
	sendOne("sv_dmgDamage", babonetID);
	sendOne("sv_cgDamage", babonetID);
	sendOne("sv_shottyDamage", babonetID);
	sendOne("sv_sniperDamage", babonetID);
	sendOne("sv_zookaDamage", babonetID);
	sendOne("sv_photonType", babonetID);
	sendOne("sv_zookaRadius", babonetID);
	sendOne("sv_nukeRadius", babonetID);
	sendOne("sv_nukeTimer", babonetID);
	sendOne("sv_nukeReload", babonetID);
	sendOne("sv_minTilesPerBabo", babonetID);
	sendOne("sv_maxTilesPerBabo", babonetID);
	sendOne("sv_photonDistMult", babonetID);
	sendOne("sv_photonVerticalShift", babonetID);
	sendOne("sv_photonHorizontalShift", babonetID);
	sendOne("sv_joinMessage", babonetID);
	sendOne("sv_sendJoinMessage", babonetID);
	sendOne("sv_ftExpirationTimer", babonetID);
	sendOne("sv_explodingFT", babonetID);
	sendOne("sv_enableVote", babonetID);
}

void GameVar::sendSVVar(INT4 peerId)
{
	sendOne("sv_friendlyFire", peerId);
	sendOne("sv_reflectedDamage", peerId);
	sendOne("sv_timeToSpawn", peerId);
	sendOne("sv_topView", peerId);
	sendOne("sv_minSendInterval", peerId);
	sendOne("sv_forceRespawn", peerId);
	sendOne("sv_baboStats", peerId);
	sendOne("sv_roundTimeLimit", peerId);
	sendOne("sv_gameTimeLimit", peerId);
	sendOne("sv_scoreLimit", peerId);
	sendOne("sv_winLimit", peerId);
	sendOne("sv_gameType", peerId);
   sendOne("sv_serverType", peerId);
#if defined(_PRO_)
   sendOne("sv_spawnType", peerId);
   sendOne("sv_subGameType", peerId);
#endif
	sendOne("sv_bombTime", peerId);
	sendOne("sv_gameName", peerId);
	sendOne("sv_port", peerId);
	sendOne("sv_maxPlayer", peerId);
   sendOne("sv_maxPlayerInGame", peerId);
	sendOne("sv_password", peerId);
	sendOne("sv_enableSMG", peerId);
	sendOne("sv_enableShotgun", peerId);
	sendOne("sv_enableSniper", peerId);
	sendOne("sv_enableDualMachineGun", peerId);
	sendOne("sv_enableChainGun", peerId);
	sendOne("sv_enableBazooka", peerId);
	sendOne("sv_enablePhotonRifle", peerId);
	sendOne("sv_enableFlameThrower", peerId);
	sendOne("sv_enableShotgunReload", peerId);
	sendOne("sv_slideOnIce", peerId);
	sendOne("sv_showEnemyTag", peerId);
	sendOne("sv_enableSecondary", peerId);
	sendOne("sv_enableKnives", peerId);
	sendOne("sv_enableNuclear", peerId);
	sendOne("sv_enableShield", peerId);
#if defined(_PRO_)
	sendOne("sv_enableMinibot", peerId);
	sendOne("sv_showKills", peerId);
	sendOne("sv_beGoodServer", peerId);
	sendOne("sv_validateWeapons", peerId);
#endif
	sendOne("sv_autoBalance", peerId);
	sendOne("sv_autoBalanceTime", peerId);
	sendOne("sv_gamePublic", peerId);
	sendOne("sv_matchcode", peerId);
	sendOne("sv_matchmode", peerId);
	sendOne("sv_maxPing", peerId);
	sendOne("sv_shottyDropRadius", peerId);
	sendOne("sv_shottyRange", peerId);
	sendOne("sv_enableMolotov", peerId);
	sendOne("sv_ftMaxRange", peerId);
	sendOne("sv_ftMinRange", peerId);
	sendOne("sv_photonDamageCoefficient", peerId);
	sendOne("sv_zookaRemoteDet", peerId);
	sendOne("sv_smgDamage", peerId);
	sendOne("sv_ftDamage", peerId);
	sendOne("sv_dmgDamage", peerId);
	sendOne("sv_cgDamage", peerId);
	sendOne("sv_shottyDamage", peerId);
	sendOne("sv_sniperDamage", peerId);
	sendOne("sv_zookaDamage", peerId);
	sendOne("sv_photonType", peerId);
	sendOne("sv_zookaRadius", peerId);
	sendOne("sv_nukeRadius", peerId);
	sendOne("sv_nukeTimer", peerId);
	sendOne("sv_nukeReload", peerId);
	sendOne("sv_minTilesPerBabo",peerId);
	sendOne("sv_maxTilesPerBabo",peerId);
	sendOne("sv_photonDistMult", peerId);
	sendOne("sv_photonVerticalShift", peerId);
	sendOne("sv_photonHorizontalShift", peerId);
	sendOne("sv_joinMessage", peerId);
	sendOne("sv_sendJoinMessage", peerId);
	sendOne("sv_ftExpirationTimer", peerId);
	sendOne("sv_explodingFT", peerId);
	sendOne("sv_enableVote", peerId);
}

void GameVar::sendOne(char * varName, INT4 peerId)
{
	CString varCom;
	dksvarGetFormatedVar(varName, &varCom);
	//varCom.insert("set ", 0);

	if (varCom.len() > 79) varCom.resize(79);
	net_ra_var ra_var;
	memcpy(ra_var.var, varCom.s, varCom.len()+1);
	bb_peerSend(peerId,(char*)&ra_var,RA_VAR,sizeof(net_ra_var),true);
}

void GameVar::sendOne(char * varName, UINT4 babonetID)
{
	CString varCom;
	dksvarGetFormatedVar(varName, &varCom);
	varCom.insert("set ", 0);

	if(varName == "sv_nukeReload")
	{
		weapons[WEAPON_NUCLEAR]->fireDelay = sv_nukeReload;
	}

	if (varCom.len() > 79) varCom.resize(79);
	net_svcl_sv_change svChange;
	memcpy(svChange.svChange, varCom.s, varCom.len()+1);
	bb_serverSend((char*)&svChange,sizeof(net_svcl_sv_change),NET_SVCL_SV_CHANGE,babonetID);
}



//
// on load les models pour le jeu
//
void GameVar::loadModels()
{
#ifndef DEDICATED_SERVER	
	dko_rocket = dkoLoadFile("main/models/Rocket.DKO");
	dko_grenade = dkoLoadFile("main/models/Grenade.DKO");
	dko_douille = dkoLoadFile("main/models/Douille.DKO");
	dko_lifePack = dkoLoadFile("main/models/LifePack.DKO");
	dko_cocktailMolotov = dkoLoadFile("main/models/CocktailMolotov.DKO");
	dko_gib = dkoLoadFile("main/models/Gib.DKO");

	weapons[WEAPON_DUAL_MACHINE_GUN]->loadModels();
	weapons[WEAPON_SMG]->loadModels();
	weapons[WEAPON_CHAIN_GUN]->loadModels();
	weapons[WEAPON_SHOTGUN]->loadModels();
	weapons[WEAPON_SNIPER]->loadModels();
	weapons[WEAPON_BAZOOKA]->loadModels();
	weapons[WEAPON_GRENADE]->loadModels();
	weapons[WEAPON_COCKTAIL_MOLOTOV] ->loadModels();
	weapons[WEAPON_KNIVES]->loadModels();
	weapons[WEAPON_NUCLEAR]->loadModels();
	weapons[WEAPON_PHOTON_RIFLE]->loadModels();
	weapons[WEAPON_FLAME_THROWER]->loadModels();
	weapons[WEAPON_SHIELD]->loadModels();
#if defined(_PRO_) && defined(_MINIBOT_)
	weapons[WEAPON_MINIBOT]->loadModels();
#endif

	tex_nuzzleFlash = dktCreateTextureFromFile("main/textures/nuzzleFlash.tga", DKT_FILTER_LINEAR);
	tex_smoke1 = dktCreateTextureFromFile("main/textures/Smoke1.tga", DKT_FILTER_LINEAR);
	tex_shotGlow = dktCreateTextureFromFile("main/textures/shotGlow.tga", DKT_FILTER_LINEAR);
	tex_smokeTrail = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_LINEAR);
	tex_blood[0] = dktCreateTextureFromFile("main/textures/blood01.tga", DKT_FILTER_LINEAR);
	tex_blood[1] = dktCreateTextureFromFile("main/textures/blood02.tga", DKT_FILTER_LINEAR);
	tex_blood[2] = dktCreateTextureFromFile("main/textures/blood03.tga", DKT_FILTER_LINEAR);
	tex_blood[3] = dktCreateTextureFromFile("main/textures/blood04.tga", DKT_FILTER_LINEAR);
	tex_blood[4] = dktCreateTextureFromFile("main/textures/blood05.tga", DKT_FILTER_LINEAR);
	tex_blood[5] = dktCreateTextureFromFile("main/textures/blood06.tga", DKT_FILTER_LINEAR);
	tex_blood[6] = dktCreateTextureFromFile("main/textures/blood07.tga", DKT_FILTER_LINEAR);
	tex_blood[7] = dktCreateTextureFromFile("main/textures/blood08.tga", DKT_FILTER_LINEAR);
	tex_blood[8] = dktCreateTextureFromFile("main/textures/blood09.tga", DKT_FILTER_LINEAR);
	tex_blood[9] = dktCreateTextureFromFile("main/textures/blood10.tga", DKT_FILTER_LINEAR);
	tex_explosionMark = dktCreateTextureFromFile("main/textures/ExplosionMark.tga", DKT_FILTER_LINEAR);

	//tex_sniperScope = dktCreateTextureFromFile("main/textures/sniperScope.tga", DKT_FILTER_LINEAR);
	tex_sniperScope = dktCreateEmptyTexture(256, 256, 4, DKT_FILTER_BILINEAR);
	{
		const int w = 256;
		const int h = 256;
		const int w_2 = int(256/2.0f);
		const int h_2 = int(256/2.0f);
		int centerx = 0;//int(w / 2.0f);
		int centery = 0;//int(h / 2.0f);
		int x, y;
		unsigned char imgData[w*h*4];
		memset(imgData, 0, w*h*4);

		for (int i = 0; i < w*h; i++)
		{
			x = (i % w);
			y = int(i / (float)w);
			int distx = centerx-(x-w_2);
			int disty = centery-(y-h_2);
			float len = float(sqrt(double(distx*distx+disty*disty)));
			if (len < 118 && x != w_2 && y != h_2)
				imgData[i * 4 + 3] = 0;
			else
				imgData[i * 4 + 3] = 255;
		}

		// update
		dktCreateTextureFromBuffer(&tex_sniperScope, imgData, 256, 256, 4, DKT_FILTER_LINEAR);
	}

	tex_medals[0] = dktCreateTextureFromFile("main/textures/medals/Medal01.tga", DKT_FILTER_LINEAR);
	tex_medals[1] = dktCreateTextureFromFile("main/textures/medals/Medal02.tga", DKT_FILTER_LINEAR);
	tex_medals[2] = dktCreateTextureFromFile("main/textures/medals/Medal03.tga", DKT_FILTER_LINEAR);
	tex_medals[3] = dktCreateTextureFromFile("main/textures/medals/Medal04.tga", DKT_FILTER_LINEAR);
	tex_medals[4] = dktCreateTextureFromFile("main/textures/medals/Medal05.tga", DKT_FILTER_LINEAR);
	tex_medals[5] = dktCreateTextureFromFile("main/textures/medals/Medal06.tga", DKT_FILTER_LINEAR);
	tex_medals[6] = dktCreateTextureFromFile("main/textures/medals/Medal07.tga", DKT_FILTER_LINEAR);
	tex_medals[7] = dktCreateTextureFromFile("main/textures/medals/Medal08.tga", DKT_FILTER_LINEAR);
	tex_medals[8] = dktCreateTextureFromFile("main/textures/medals/Medal09.tga", DKT_FILTER_LINEAR);
	tex_medals[9] = dktCreateTextureFromFile("main/textures/medals/Medal10.tga", DKT_FILTER_LINEAR);
	tex_medals[10] = dktCreateTextureFromFile("main/textures/medals/Medal11.tga", DKT_FILTER_LINEAR);
	tex_drip = dktCreateTextureFromFile("main/textures/drip.tga", DKT_FILTER_LINEAR);
	tex_sky = dktCreateTextureFromFile("main/textures/sky.tga", DKT_FILTER_LINEAR);
	tex_glowTrail = dktCreateTextureFromFile("main/textures/glowTrail.tga", DKT_FILTER_LINEAR);
	
	sfx_ric[0] = dksCreateSoundFromFile("main/sounds/ric1.wav", false);
	sfx_ric[1] = dksCreateSoundFromFile("main/sounds/ric2.wav", false);
	sfx_ric[2] = dksCreateSoundFromFile("main/sounds/ric3.wav", false);
	sfx_ric[3] = dksCreateSoundFromFile("main/sounds/ric4.wav", false);
	sfx_ric[4] = dksCreateSoundFromFile("main/sounds/ric5.wav", false);
	sfx_hit[0] = dksCreateSoundFromFile("main/sounds/hit1.wav", false);
	sfx_hit[1] = dksCreateSoundFromFile("main/sounds/hit2.wav", false);
	sfx_baboCreve[0] = dksCreateSoundFromFile("main/sounds/BaboCreve1.wav", false);
	sfx_baboCreve[1] = dksCreateSoundFromFile("main/sounds/BaboCreve2.wav", false);
	sfx_baboCreve[2] = dksCreateSoundFromFile("main/sounds/BaboCreve3.wav", false);
	sfx_explosion[0] = dksCreateSoundFromFile("main/sounds/Explosion1.wav", false);
	sfx_grenadeRebond = dksCreateSoundFromFile("main/sounds/GrenadeRebond.wav", false);
	sfx_douille[0] = dksCreateSoundFromFile("main/sounds/douille1.wav", false);
	sfx_douille[1] = dksCreateSoundFromFile("main/sounds/douille2.wav", false);
	sfx_douille[2] = dksCreateSoundFromFile("main/sounds/douille3.wav", false);
	sfx_equip = dksCreateSoundFromFile("main/sounds/equip.wav", false);
	sfx_lifePack = dksCreateSoundFromFile("main/sounds/LifePack.wav", false);
	sfx_cocktailMolotov = dksCreateSoundFromFile("main/sounds/cocktailmolotov.wav", false);
	sfx_lavaSteam = dksCreateSoundFromFile("main/sounds/lavasteam.wav", false);
	sfx_overHeat = dksCreateSoundFromFile("main/sounds/overHeat.wav", false);
	sfx_photonStart = dksCreateSoundFromFile("main/sounds/PhotonStart.wav", false);

	dkpp_firingSmoke.angleFrom = 0;
	dkpp_firingSmoke.angleTo = 360;
	dkpp_firingSmoke.angleSpeedFrom = -30;
	dkpp_firingSmoke.angleSpeedTo = 30;
	dkpp_firingSmoke.srcBlend = DKP_SRC_ALPHA;
	dkpp_firingSmoke.dstBlend = DKP_ONE_MINUS_SRC_ALPHA;
	dkpp_firingSmoke.durationFrom = .25f;
	dkpp_firingSmoke.durationTo = .5f;
	dkpp_firingSmoke.endColorFrom.set(1,1,.7f,.0f);
	dkpp_firingSmoke.endColorTo.set(1,1,.7f,.0f);
	dkpp_firingSmoke.startColorFrom.set(.7f,.7f,.7f,1.0f);
	dkpp_firingSmoke.startColorTo.set(.7f,.7f,.7f,1.0f);
	dkpp_firingSmoke.endSizeFrom = .25f;
	dkpp_firingSmoke.endSizeTo = .30f;
	dkpp_firingSmoke.startSizeFrom = .15f;
	dkpp_firingSmoke.startSizeTo = .18f;
	dkpp_firingSmoke.airResistanceInfluence = 0;
	dkpp_firingSmoke.gravityInfluence = 0;
	dkpp_firingSmoke.pitchFrom = 0;
	dkpp_firingSmoke.pitchTo = 180;
	dkpp_firingSmoke.speedFrom = .0f;
	dkpp_firingSmoke.speedTo = 1.8f;
	dkpp_firingSmoke.textureFrameCount = 0;
	dkpp_firingSmoke.texture = &tex_smoke1;
	dkpp_firingSmoke.particleCountFrom = 2;
	dkpp_firingSmoke.particleCountTo = 4;
#endif
}

 

//
// pour effacer les models du jeu
//
void GameVar::deleteModels()
{
#ifndef DEDICATED_SERVER
	dkoDeleteModel(&dko_rocket);
	dkoDeleteModel(&dko_grenade);
	dkoDeleteModel(&dko_douille);
	dkoDeleteModel(&dko_lifePack);
	dkoDeleteModel(&dko_cocktailMolotov);
	dkoDeleteModel(&dko_gib);

#endif

	delete weapons[WEAPON_SMG];
	delete weapons[WEAPON_SHOTGUN];
	delete weapons[WEAPON_SNIPER];
	delete weapons[WEAPON_DUAL_MACHINE_GUN];
	delete weapons[WEAPON_CHAIN_GUN];
	delete weapons[WEAPON_BAZOOKA];
	delete weapons[WEAPON_PHOTON_RIFLE];
	delete weapons[WEAPON_GRENADE];
	delete weapons[WEAPON_FLAME_THROWER];
	delete weapons[WEAPON_COCKTAIL_MOLOTOV];
	delete weapons[WEAPON_KNIVES];
	delete weapons[WEAPON_NUCLEAR];
	delete weapons[WEAPON_SHIELD];
#if defined(_PRO_) && defined(_MINIBOT_)
	delete weapons[WEAPON_MINIBOT];
#endif

#ifndef DEDICATED_SERVER
	dktDeleteTexture(&tex_nuzzleFlash);
	dktDeleteTexture(&tex_smoke1);
	dktDeleteTexture(&tex_shotGlow);
	dktDeleteTexture(&tex_smokeTrail);
	dktDeleteTexture(&tex_blood[0]);
	dktDeleteTexture(&tex_blood[1]);
	dktDeleteTexture(&tex_blood[2]);
	dktDeleteTexture(&tex_blood[3]);
	dktDeleteTexture(&tex_blood[4]);
	dktDeleteTexture(&tex_blood[5]);
	dktDeleteTexture(&tex_blood[6]);
	dktDeleteTexture(&tex_blood[7]);
	dktDeleteTexture(&tex_blood[8]);
	dktDeleteTexture(&tex_blood[9]);
	dktDeleteTexture(&tex_explosionMark);
	dktDeleteTexture(&tex_sniperScope);
	dktDeleteTexture(tex_medals + 0);
	dktDeleteTexture(tex_medals + 1);
	dktDeleteTexture(tex_medals + 2);
	dktDeleteTexture(tex_medals + 3);
	dktDeleteTexture(tex_medals + 4);
	dktDeleteTexture(tex_medals + 5);
	dktDeleteTexture(tex_medals + 6);
	dktDeleteTexture(tex_medals + 7);
	dktDeleteTexture(tex_medals + 8);
	dktDeleteTexture(tex_medals + 9);
	dktDeleteTexture(tex_medals + 10);
	dktDeleteTexture(&tex_drip);
	dktDeleteTexture(&tex_sky);
	dktDeleteTexture(&tex_glowTrail);

	dksDeleteSound(sfx_ric[0]);
	dksDeleteSound(sfx_ric[1]);
	dksDeleteSound(sfx_ric[2]);
	dksDeleteSound(sfx_ric[3]);
	dksDeleteSound(sfx_ric[4]);
	dksDeleteSound(sfx_hit[3]);
	dksDeleteSound(sfx_hit[4]);
	dksDeleteSound(sfx_baboCreve[0]);
	dksDeleteSound(sfx_baboCreve[1]);
	dksDeleteSound(sfx_baboCreve[2]);
	dksDeleteSound(sfx_explosion[0]);
	dksDeleteSound(sfx_grenadeRebond);
	dksDeleteSound(sfx_douille[0]);
	dksDeleteSound(sfx_douille[1]);
	dksDeleteSound(sfx_douille[2]);
	dksDeleteSound(sfx_equip);
	dksDeleteSound(sfx_lifePack);
	dksDeleteSound(sfx_cocktailMolotov);
	dksDeleteSound(sfx_lavaSteam);
	dksDeleteSound(sfx_overHeat);
	dksDeleteSound(sfx_photonStart);
#endif
}


#ifndef DEDICATED_SERVER
// Pour loader les lang_ var
bool GameVar::loadLanguage(char * filename)
{
  int i;
	FILE * fic = fopen(filename, "r");
	
	if (!fic) return false;

	char varName[256];
	char varValue[512];
	while (true)
	{
		fscanf(fic, "%s", varName);
		if (strcmp(varName, "END") == 0)
		{
			languageLoaded = true;
			return true;
		}
		for (i=0;;)
		{
			int character = fgetc(fic);
			if (character == 199) {character = 128;}
			if (character == 252) {character = 129;}
			if (character == 233) {character = 130;}
			if (character == 226) {character = 131;}
			if (character == 228) {character = 132;}
			if (character == 224) {character = 133;}
			if (character == 229) {character = 134;}
			if (character == 231) {character = 135;}
			if (character == 234) {character = 136;}
			if (character == 235) {character = 137;}
			if (character == 232) {character = 138;}
			if (character == 239) {character = 139;}
			if (character == 238) {character = 140;}
			if (character == 236) {character = 141;}
			if (character == 196) {character = 142;}
			if (character == 197) {character = 143;}
			if (character == 201) {character = 144;}
			if (character == 230) {character = 145;}
			if (character == 198) {character = 146;}
			if (character == 244) {character = 147;}
			if (character == 246) {character = 148;}
			if (character == 242) {character = 149;}
			if (character == 251) {character = 150;}
			if (character == 249) {character = 151;}
			if (character == 255) {character = 152;}
			if (character == 214) {character = 153;}
			if (character == 220) {character = 154;}
			if (character == 248) {character = 155;}
			if (character == 163) {character = 156;}
			if (character == 216) {character = 157;}
			if (character == 215) {character = 158;}
			if (character == 170) {character = 159;}
			if (character == '\t')
			{
				continue;
			}
			if (character == '\n')
			{
				varValue[i] = '\0';
				break;
			}
			if (character == '%')
			{
				varValue[i] = character;
				++i;
			}
			if (character == '\\')
			{
				character = fgetc(fic);
				switch (character)
				{
				case 'x':
					character = fgetc(fic);
					switch (character)
					{
					case '1': character = '\x1';break;
					case '2': character = '\x2';break;
					case '3': character = '\x3';break;
					case '4': character = '\x4';break;
					case '5': character = '\x5';break;
					case '6': character = '\x6';break;
					case '7': character = '\x7';break;
					case '8': character = '\x8';break;
					case '9': character = '\x9';break;
					}
					break;
				case 'n':
					character = '\n';
					break;
				case '\\':
					character = '\\';
					break;
				}
			}
			varValue[i] = character;
			++i;
		}
		CString value = varValue;
		for (i=0;i<(int)langs.size();++i)
		{
			if (langs[i].varName == varName)
			{
				*(langs[i].varPtr) = value;
			}
		}
	}
	languageLoaded = true;
	return true;
}

bool GameVar::isLanguageLoaded()
{
	return languageLoaded;
}
#endif

