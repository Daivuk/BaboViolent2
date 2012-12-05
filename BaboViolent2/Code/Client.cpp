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
#include "Helper.h"
#include "GameVar.h"
#include "Scene.h"
#include "CStatus.h"

#ifdef _PRO_
#include "Screengrab.h"
#endif

extern Scene * scene;

extern CControl * btn_resume;



//
// Constructeur
//
Client::Client(Game * pGame)
{
	requestedAdmin = false;
	wrongVersionReason = false;
	serverFrameID = 0;
	font = dkfCreateFont("main/fonts/babo.tga");
	game = pGame;
	isRunning = false;
	needToShutDown = false;
	showMenu = true;
	isServer = false;
	isChattingTeam = false;
	isConnected = false;
	gotGameState = false;

#ifdef _PRO_
   proServer = false;
#endif

	m_sfxChat = dksCreateSoundFromFile("main/Sounds/Chat.wav", false);
	sfxHit = dksCreateSoundFromFile("main/Sounds/Hit.wav", false);
	sfxShotyReload = dksCreateSoundFromFile("main/Sounds/shotgunReload.wav", false);
	m_sfxClic = dksCreateSoundFromFile("main/Sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/Sounds/ControlOver.wav", false);

	blink = 0;
	tex_screenHit = dktCreateTextureFromFile("main/textures/screenHit.tga", DKT_FILTER_LINEAR);
	tex_grenadeLeft = dktCreateTextureFromFile("main/textures/GrenadeIcon.tga", DKT_FILTER_LINEAR);
	tex_shotgunLeft = dktCreateTextureFromFile("main/textures/CartridgeIcon.tga", DKT_FILTER_LINEAR);
	tex_molotovLeft = dktCreateTextureFromFile("main/textures/molotovIcon.tga", DKT_FILTER_LINEAR);
	tex_blueFlag = dktCreateTextureFromFile("main/textures/BlueFlag.tga", DKT_FILTER_LINEAR);
	tex_redFlag = dktCreateTextureFromFile("main/textures/RedFlag.tga", DKT_FILTER_LINEAR);
	tex_crossHit = dktCreateTextureFromFile("main/textures/CrossHit.tga", DKT_FILTER_LINEAR);
	hitIndicator = 0;

	// On hide la mouse
//	ShowCursor(FALSE);

	//--- Overall control
	clientRoot = new CControl();
	clientRoot->font = font;
	clientRoot->size.set(800,600);
	clientRoot->backColor.set(.3f,.5f,.8f);
	clientRoot->foreColor.set(1,1,1);
	clientRoot->textShadow = true;
	clientRoot->noFill = true;

	// On cré nos menu bitch
	btn_autoAssign = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), "Auto assign team", this, "BUTTON");
	btn_blueTeam = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), "\x1Join blue team", this, "BUTTON", btn_autoAssign, CONTROL_SNAP_BOTTOM);
	btn_redTeam = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), "\x4Join red team", this, "BUTTON", btn_blueTeam, CONTROL_SNAP_BOTTOM);
	btn_spectator = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), "Join spectator", this, "BUTTON", btn_redTeam, CONTROL_SNAP_BOTTOM);
	btn_mainMenu = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), "<- Main menu", this, "BUTTON", btn_spectator, CONTROL_SNAP_BOTTOM, 30);
	btn_disconnect = new CControl(clientRoot, CVector2i(25, 130), CVector2i(200,30), CString("\x4") + "Disconnect", this, "BUTTON", btn_mainMenu, CONTROL_SNAP_BOTTOM);

	// Nos boutons pour choisir notre gun
	btn_guns[0] = new CControl(clientRoot, CVector2i(250, 130 + 0 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_SMG]->weaponName, this, "BUTTON");
	btn_guns[1] = new CControl(clientRoot, CVector2i(250, 130 + 1 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_SHOTGUN]->weaponName, this, "BUTTON");
	btn_guns[2] = new CControl(clientRoot, CVector2i(250, 130 + 2 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_SNIPER]->weaponName, this, "BUTTON");
	btn_guns[3] = new CControl(clientRoot, CVector2i(250, 130 + 3 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_DUAL_MACHINE_GUN]->weaponName, this, "BUTTON");
	btn_guns[4] = new CControl(clientRoot, CVector2i(250, 130 + 4 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_CHAIN_GUN]->weaponName, this, "BUTTON");
	btn_guns[5] = new CControl(clientRoot, CVector2i(250, 130 + 5 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_BAZOOKA]->weaponName, this, "BUTTON");
	btn_guns[6] = new CControl(clientRoot, CVector2i(250, 130 + 6 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_PHOTON_RIFLE]->weaponName, this, "BUTTON");
	btn_guns[7] = new CControl(clientRoot, CVector2i(250, 130 + 7 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_FLAME_THROWER]->weaponName, this, "BUTTON");
	btn_meleeguns[0] = new CControl(clientRoot, CVector2i(475, 130 + 0 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_KNIVES]->weaponName, this, "BUTTON");
	btn_meleeguns[1] = new CControl(clientRoot, CVector2i(475, 130 + 1 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_NUCLEAR]->weaponName, this, "BUTTON");
	btn_meleeguns[2] = new CControl(clientRoot, CVector2i(475, 130 + 2 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_SHIELD]->weaponName, this, "BUTTON");
	#ifdef _PRO_
		btn_meleeguns[3] = new CControl(clientRoot, CVector2i(475, 130 + 3 * 40), CVector2i(200,30), gameVar.weapons[WEAPON_MINIBOT]->weaponName, this, "BUTTON");
	#endif
	currentGun = btn_guns[gameVar.cl_primaryWeapon/*0*/];
	currentMelee = btn_meleeguns[gameVar.cl_secondaryWeapon/*0*/];

	btn_resume->enable = true;

	uniqueClientID = 0;

	isAdmin = false;

	autoBalanceTimer = 0;

	isDownloadingMap = false;
	timeSinseLastQMsg = 10.0f;
}

void Client::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}


//
// Destructeur
//
Client::~Client()
{
	ZEVEN_SAFE_DELETE(clientRoot);
	dkfDeleteFont(&font);
	if (uniqueClientID) bb_clientDisconnect(uniqueClientID);
	ZEVEN_SAFE_DELETE(game);
	chatMessages.clear();
	eventMessages.clear();
	dksDeleteSound(sfxHit);
	dksDeleteSound(m_sfxChat);
	dksDeleteSound(sfxShotyReload);
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
	dktDeleteTexture(&tex_screenHit);
	dktDeleteTexture(&tex_grenadeLeft);
	dktDeleteTexture(&tex_molotovLeft);
	dktDeleteTexture(&tex_blueFlag);
	dktDeleteTexture(&tex_redFlag);
	dktDeleteTexture(&tex_crossHit);
	dktDeleteTexture(&tex_shotgunLeft);

	btn_resume->enable = false;
}



//
// Pour l'updater
//
void Client::update(float delay)
{
  int i;
	status->processQueue();

	if (isRunning)
	{
		if (game)
		{
			if (game->voting.votingInProgress && game->thisPlayer)
			{
				if (!game->voting.voted && !(menuManager.root && menuManager.root->visible) && !showMenu)
				{
					if (dkiGetState(DIK_F1) == DKI_DOWN)
					{
						game->voting.voted = true;
						net_clsv_vote vote;
						vote.value = true;
						vote.playerID = game->thisPlayer->playerID;
						bb_clientSend(uniqueClientID, (char*)(&vote), sizeof(net_clsv_vote), NET_CLSV_VOTE);
					}
					else if (dkiGetState(DIK_F2) == DKI_DOWN)
					{
						game->voting.voted = true;
						net_clsv_vote vote;
						vote.value = false;
						vote.playerID = game->thisPlayer->playerID;
						bb_clientSend(uniqueClientID, (char*)(&vote), sizeof(net_clsv_vote), NET_CLSV_VOTE);
					}
				}
			}
		}

		if (autoBalanceTimer > 0)
		{
			autoBalanceTimer -= delay;
			if (autoBalanceTimer < 0) autoBalanceTimer = 0;
		}

		timeSinseLastQMsg += delay;

      // Enable you to set the maximum number of players allowed to be playing         
      bool bEnableJoin = true;
         if (gameVar.sv_maxPlayerInGame != 0)
         {
	      int nbPlayer = 0;
	      for (i=0;i<MAX_PLAYER;++i)
	      {
            if ((game->players[i]) && 
                ((game->players[i]->teamID == PLAYER_TEAM_BLUE)||(game->players[i]->teamID == PLAYER_TEAM_RED)))
		      {
			      nbPlayer++;
		      }
	      }

         // Check if we are allowed to join the game
         if ((nbPlayer >= gameVar.sv_maxPlayerInGame) &&  (game->thisPlayer->teamID == PLAYER_TEAM_SPECTATOR))
            {
            bEnableJoin = false;
            }
         }
      
      // Disable joining
      if (bEnableJoin)
         {
         btn_autoAssign->enable = true;
		   btn_blueTeam->enable =  !gameVar.sv_autoBalance;
		   btn_redTeam->enable =  !gameVar.sv_autoBalance;
         }
      else
         {
         btn_autoAssign->enable = false;
		   btn_blueTeam->enable   = false;
		   btn_redTeam->enable    = false;
         }

		blink += delay;
		while (blink >= .5f) blink -= .5f;
		hitIndicator -= delay;
		if (hitIndicator < 0) hitIndicator = 0;

		if (game->thisPlayer)
		{
			if (dkiGetState(DKI_MOUSE_BUTTON1) == DKI_UP)
				game->thisPlayer->initedMouseClic = false;
		}
		game->showMenu = showMenu;

		// On update le client
		int result = bb_clientUpdate(uniqueClientID, delay, UPDATE_SEND_RECV);
		console->debugBBNET(true, false);
		if (result == 1)
		{
			// Une erreur !!! On arrête tout !!!
			console->add(CString("\x3> Error : %s", bb_clientGetLastError(uniqueClientID)));
			needToShutDown = true;
			status->set(CStatus::ONLINE);
		//	scene->menu->currentMenu = MENU_CONNECTION_FAILED;
		}
		else if (result == 2)
		{
			// Le server a foutu le CAMP !
			console->add(CString("\x3> ") + gameVar.lang_serverDisconnected);
			needToShutDown = true;
			status->set(CStatus::ONLINE);
		//	scene->menu->currentMenu = MENU_SERVER_SHUTDOWN;
		}
		else if (result == 3)
		{
	//		isConnected = true;
		}

		// Si on pèse sur T, on tombe en mode chatting
		/*if(game->roundState != GAME_PLAYING)
		{
			console->unlock();
			chatting.loseFocus();
		}
		else */
		if (!(menuManager.root && menuManager.root->visible) && !showMenu)
		{
			if (!console->isActive() && dkiGetState(gameVar.k_chatAll) == DKI_DOWN && !chatting.haveFocus())
			{
				isChattingTeam = false;
				console->lock();
				chatting.giveFocus();
			}
			if (!console->isActive() && dkiGetState(gameVar.k_chatTeam) == DKI_DOWN && !chatting.haveFocus())
			{
				isChattingTeam = true;
				console->lock();
				chatting.giveFocus();
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg01) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg01.s[1]));
				if(gameVar.cl_qMsg01.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg02) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg02.s[1]));
				if(gameVar.cl_qMsg02.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg03) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg03.s[1]));
				if(gameVar.cl_qMsg03.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg04) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg04.s[1]));
				if(gameVar.cl_qMsg04.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg05) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg05.s[1]));
				if(gameVar.cl_qMsg05.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg06) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg06.s[1]));
				if(gameVar.cl_qMsg06.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg07) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg07.s[1]));
				if(gameVar.cl_qMsg07.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg08) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg08.s[1]));
				if(gameVar.cl_qMsg08.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg09) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg09.s[1]));
				if(gameVar.cl_qMsg09.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}
			if((timeSinseLastQMsg >= MIN_TIME_BETWEEN_QMSG) && !console->isActive() && dkiGetState(gameVar.k_qMsg10) == DKI_DOWN && !chatting.haveFocus())
			{
				CString msg("%s", &(gameVar.cl_qMsg10.s[1]));
				if(gameVar.cl_qMsg10.s[0] == 'a')
					this->sayall(msg);
				else
					this->sayteam(msg);
				timeSinseLastQMsg = 0.0f;
			}

			// Si on est entrein de chatter et qu'on pèse sur Enter on send le chat
			if (chatting.isActivated())
			{
				if (isChattingTeam)
					sayteam(chatting.s);
				else
					sayall(chatting.s);
				chatting.clear();
				chatting.loseFocus();
				console->unlock();
			}
		}

		// On recv les messages
		char * buffer;
		int messageID;
		while (buffer = bb_clientReceive(uniqueClientID, &messageID))
		{
			// On gère les messages reçu
			recvPacket(buffer, messageID);
		}

		// Si on fait Esc, on spawn un menu
		if (dkiGetState(gameVar.k_menuAccess) == DKI_DOWN && !console->isActive() && isConnected && !(menuManager.root && menuManager.root->visible))
		{
			if (!chatting.haveFocus())
			{
				showMenu = !showMenu;
			}
			else
			{
				chatting.clear();
				chatting.loseFocus();
				console->unlock();
			}
		}

		// Screenshot
#ifdef _PRO_
#ifdef WIN32
		if (dkiGetState(gameVar.k_screenShot) == DKI_DOWN && !console->isActive() && !chatting.haveFocus() && isConnected && !(menuManager.root && menuManager.root->visible))
		{
      SaveScreenGrabAuto();
		}

		if (dkiGetState(gameVar.k_stats) == DKI_DOWN && !console->isActive() && !chatting.haveFocus() && isConnected && !(menuManager.root && menuManager.root->visible))
		{
      SaveStatsAuto();
		}

#endif
#endif

		// On gère le menu (important, toujours tester si la console est là ou pas)
		if (showMenu && !console->isActive() && isConnected)
		{
			menuManager.update(delay, clientRoot);
//			clientRoot->update(delay);
		}
	}

	// On update le jeu (Ça c'est autant client que server side)
	if (game) game->update(delay);

	// On update les chat messages
	for (i=0;i<(int)chatMessages.size();++i)
	{
		if ((int)chatMessages.size() - i > 5 && chatMessages[i].duration > 1) chatMessages[i].duration = 1;
		chatMessages[i].duration -= delay;
		if (chatMessages[i].duration <= 0)
		{
			chatMessages.erase(chatMessages.begin() + i);
			i--;
		}
	}
	for (i=0;i<(int)eventMessages.size();++i)
	{
		if ((int)eventMessages.size() - i > 5 && eventMessages[i].duration > 1) 
		{
			eventMessages.erase(eventMessages.begin() + i);
			i--;
			continue;
		}
		eventMessages[i].duration -= delay;
		if (eventMessages[i].duration <= 0)
		{
			eventMessages.erase(eventMessages.begin() + i);
			i--;
		}
	}

	// On update le client une dernière fois
	int result = bb_clientUpdate(uniqueClientID, delay, UPDATE_SEND_RECV);
	console->debugBBNET(true, false);
	if (result == 1)
	{
		// Une erreur !!! On arrête tout !!!
		console->add(CString("\x3> Error : %s", bb_clientGetLastError(uniqueClientID)));
		needToShutDown = true;
	}
	else if (result == 2)
	{
		// Le server a foutu le CAMP !
		console->add(CString("\x3> ") + gameVar.lang_serverDisconnected);
		needToShutDown = true;
	}
	else if (result == 3)
	{
	//	isConnected = true;
	}

	if (game)
	{
		if (game->map)
		{
			FSOUND_3D_Listener_SetAttributes(game->map->camPos.s, 0,0,0,1,0,1,0);
			FSOUND_Update();
		}
	}
}



void Client::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}

	if (control == btn_mainMenu)
	{
		// On show les menus
		if (menuManager.root)
		{
			menuManager.root->visible = true;
			showMenu = false;
		}
//		scene->menu->isShowing = true;
//		scene->menu->currentMenu = MENU_MAIN;
		if (writting) writting->loseFocus();
		return;
	}
	if (control == btn_disconnect)
	{
		needToShutDown = true;
		return;
	}
	if (control == btn_autoAssign)
	{
		// On auto assign un team, yup bitch
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_AUTO_ASSIGN;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
			game->map->introAnim = game->map->introAnimLenght;
		}
		return;
	}
	if (control == btn_blueTeam && !gameVar.sv_autoBalance)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_BLUE;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
			game->map->introAnim = game->map->introAnimLenght;
		}
		return;
	}
	if (control == btn_redTeam && !gameVar.sv_autoBalance)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_RED;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
			game->map->introAnim = game->map->introAnimLenght;
		}
		return;
	}
	if (control == btn_spectator)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_SPECTATOR;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
			game->map->introAnim = game->map->introAnimLenght;
		}
		return;
	}
	if (control == btn_guns[0])
	{
		currentGun = btn_guns[0];
		game->thisPlayer->nextSpawnWeapon = WEAPON_SMG;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[1])
	{
		currentGun = btn_guns[1];
		game->thisPlayer->nextSpawnWeapon = WEAPON_SHOTGUN;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[2])
	{
		currentGun = btn_guns[2];
		game->thisPlayer->nextSpawnWeapon = WEAPON_SNIPER;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[3])
	{
		currentGun = btn_guns[3];
		game->thisPlayer->nextSpawnWeapon = WEAPON_DUAL_MACHINE_GUN;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[4])
	{
		currentGun = btn_guns[4];
		game->thisPlayer->nextSpawnWeapon = WEAPON_CHAIN_GUN;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[5])
	{
		currentGun = btn_guns[5];
		game->thisPlayer->nextSpawnWeapon = WEAPON_BAZOOKA;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[6])
	{
		currentGun = btn_guns[6];
		game->thisPlayer->nextSpawnWeapon = WEAPON_PHOTON_RIFLE;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_guns[7])
	{
		currentGun = btn_guns[7];
		game->thisPlayer->nextSpawnWeapon = WEAPON_FLAME_THROWER;
		if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
		{
			showMenu = false;
		}
		return;
	}
	if (control == btn_meleeguns[0])
	{
		if (game->thisPlayer)
		{
			currentMelee = btn_meleeguns[0];
			game->thisPlayer->nextMeleeWeapon = WEAPON_KNIVES;
			if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
			{
				showMenu = false;
			}
		}
		return;
	}
	if (control == btn_meleeguns[1])
	{
		if (game->thisPlayer)
		{
			currentMelee = btn_meleeguns[1];
			game->thisPlayer->nextMeleeWeapon = WEAPON_NUCLEAR;
			if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
			{
				showMenu = false;
			}
		}
		return;
	}
	if (control == btn_meleeguns[2])
	{
		if (game->thisPlayer)
		{
			currentMelee = btn_meleeguns[2];
			game->thisPlayer->nextMeleeWeapon = WEAPON_SHIELD;
			if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
			{
				showMenu = false;
			}
		}
		return;
	}
	#ifdef _PRO_
		if (control == btn_meleeguns[3])
		{
			if (game->thisPlayer)
			{
				currentMelee = btn_meleeguns[3];
				game->thisPlayer->nextMeleeWeapon = WEAPON_MINIBOT;
				if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
				{
					showMenu = false;
				}
			}
			return;
		}
	#endif
}



//
// Pour gèrer les events de son menu
//
/*
void Client::onClick(Control * control)
{
	if (control == btn_mainMenu)
	{
		// On show les menus
		clientRoot->visible = true;
//		scene->menu->isShowing = true;
//		scene->menu->currentMenu = MENU_MAIN;
		if (writting) writting->loseFocus();
		return;
	}
	if (control == btn_disconnect)
	{
		needToShutDown = true;
		return;
	}
	if (control == btn_autoAssign)
	{
		// On auto assign un team, yup bitch
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_AUTO_ASSIGN;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
		}
		return;
	}
	if (control == btn_blueTeam)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_BLUE;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
		}
		return;
	}
	if (control == btn_redTeam)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_RED;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
		}
		return;
	}
	if (control == btn_spectator)
	{
		if (game->thisPlayer)
		{
			// On le dit au server qu'on change de state!
			// En fait, on le demannnnde au server, lui va décider si t'as le droit ou pas
			net_clsv_svcl_team_request teamRequest;
			teamRequest.playerID = game->thisPlayer->playerID;
			teamRequest.teamRequested = PLAYER_TEAM_SPECTATOR;
			bb_clientSend(uniqueClientID, (char*)&teamRequest, sizeof(net_clsv_svcl_team_request), NET_CLSV_SVCL_TEAM_REQUEST);
			showMenu = false;
		}
		return;
	}
	for (int i=0;i<6;++i)
	{
		if (control == btn_guns[i])
		{
			if (game->thisPlayer)
			{
				currentGun =	i;
				game->thisPlayer->nextSpawnWeapon = i;
				if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
				{
					showMenu = false;
				}
			}
			return;
		}
	}
}*/



//
// Pour rejoindre un server
//
int Client::join(CString IPAddress, int port, CString password)
{
	server_ip = IPAddress;
	m_password = password;
	m_password.resize(15);
	uniqueClientID = bb_clientConnect(IPAddress.s, port);
	isRunning = true;
	return 1;
}



//
// Pour printer un chat message sur l'écran
//
void Client::printMessage(CString message)
{
}



//
// Pour chatter
//
void Client::sayall(CString message)
{
	if (message.isNull()) return;
	if (game && isRunning)
	{
		if (game->thisPlayer)
		{
			// On send ça sur la network oui messieur
			net_clsv_svcl_chat chat_message;

		//	chat_message.fromID = game->thisPlayer->playerID;
			chat_message.teamID = PLAYER_TEAM_SPECTATOR - 1; // All player!

			// On insert la couleur dépendament du team 
			// (une fois apres le nom du joueur, parce que ce dernier a surement 
			// mis plein de caractères de couleurs)
			message.insert(" : \x8", 0);

			// On insert le nom du joueur
			message.insert(game->thisPlayer->name.s, 0);

			// On insert son état (mort)
			if (game->thisPlayer->status == PLAYER_STATUS_DEAD && game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR)
			{
				message.insert(CString("(%s)", gameVar.lang_dead.s).s, 0);
			}

			// On insert la couleur dépendament du team
			switch (game->thisPlayer->teamID)
			{
			case PLAYER_TEAM_SPECTATOR:
				message.insert(CString("\x8(%s)", gameVar.lang_spectator.s).s, 0);
				break;
			case PLAYER_TEAM_BLUE:
				message.insert("{\x1 ", 0);
				break;
			case PLAYER_TEAM_RED:
				message.insert("}\x4 ", 0);
				break;
			}

			// Si le message est trop grand, on le resize
			if (message.len() > 49+80) message.resize(49+80);

			// Voilà, on copie le finale
			memcpy(chat_message.message, message.s, sizeof(char) * (message.len() + 1));

			// Voilà, on send ça sur le network!
			bb_clientSend(uniqueClientID, (char*)&chat_message, sizeof(net_clsv_svcl_chat), NET_CLSV_SVCL_CHAT);
		}
	}
}

void Client::sayteam(CString message)
{
	if (message.isNull()) return;
	if (game && isRunning)
	{
		if (game->thisPlayer)
		{
			// On send ça sur la network oui messieur
			net_clsv_svcl_chat chat_message;

		//	chat_message.fromID = game->thisPlayer->playerID;
			chat_message.teamID = game->thisPlayer->teamID;

			// On insert la couleur dépendament du team 
			// (une fois apres le nom du joueur, parce que ce dernier a surement 
			// mis plein de caractères de couleurs)
			message.insert(" : \x8", 0);

			// On insert le nom du joueur
			message.insert(game->thisPlayer->name.s, 0);

			// La couleur de son nom
			switch (game->thisPlayer->teamID)
			{
			case PLAYER_TEAM_SPECTATOR:
				message.insert("\x8", 0);
				break;
			case PLAYER_TEAM_BLUE:
				message.insert("\x1", 0);
				break;
			case PLAYER_TEAM_RED:
				message.insert("\x4", 0);
				break;
			}

			// C'est un team message
			message.insert(CString("(%s)", gameVar.lang_team.s).s, 0);

			if (game->thisPlayer->teamID != PLAYER_TEAM_SPECTATOR) 
			{
				// On insert son état (mort)
				if (game->thisPlayer->status == PLAYER_STATUS_DEAD)
				{
					message.insert(CString("(%s)", gameVar.lang_dead.s).s, 0);
				}
			}

			// On insert la couleur dépendament du team
			switch (game->thisPlayer->teamID)
			{
			case PLAYER_TEAM_SPECTATOR:
				message.insert(CString("\x8(%s)", gameVar.lang_spectator.s).s, 0);
				break;
			case PLAYER_TEAM_BLUE:
				message.insert("{\x1 ", 0);
				break;
			case PLAYER_TEAM_RED:
				message.insert("}\x4 ", 0);
				break;
			}

			// Si le message est trop grand, on le resize
			if (message.len() > 49+80) message.resize(49+80);

			// Voilà, on copie le finale
			memcpy(chat_message.message, message.s, sizeof(char) * (message.len() + 1));

			// Voilà, on send ça sur le network!
			bb_clientSend(uniqueClientID, (char*)&chat_message, sizeof(net_clsv_svcl_chat), NET_CLSV_SVCL_CHAT);
		}
	}
}

#endif


