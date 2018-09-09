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

#ifndef SCENE_H
#define SCENE_H


#include "Zeven.h"
#include "Game.h"
#include "Server.h"
#include "CMaster.h"
#ifndef DEDICATED_SERVER
#include "Client.h"
#include "Editor.h"
//#include "Menu.h"
#include "CMainTab.h"
#include "CMenuManager.h"
#include "IntroScreen.h"
#include "CSurvey.h"
#endif

#define ICONE_DEDICATED			0x0001
#define ICONE_PASSWORDED		0x0002
#define ICONE_FRIENDLYFIRE		0x0004
#define ICONE_3DVIEW			0x0008


class CCurl;


class Scene
{
public:
	// La font
	unsigned int font;

	// Le server
	Server * server;

#ifndef DEDICATED_SERVER
	// Le client
	Client * client;

	// Le Map editor (Ça ne peut pas coexister avec client et server)
	Editor * editor;

	// Le menu
//	Menu * menu;
	CMainTab * mainTab;

	// Notre intro screen (quand que lui n'est pas fini, on update ni affiche rien d'autre)
	IntroScreen * introScreen;

	//--- Survey screen
	CSurvey * survey;

	// Notre curseur
	unsigned int tex_crosshair;
	unsigned int tex_menuCursor;

	//--- The guy already sent the survey
	bool surveySent;

	//--- Head games logo bottom right
	//unsigned int tex_miniHeadGames;
#endif
	// Le delait pour sender les server info (à chaque 20sec)
	float serverInfoDelay;
	bool masterReady;

	long frameID;

public:
	// Constructeur
	Scene();

	// Destructeur
	virtual ~Scene();

	// Update
	void update(float delay);
#ifndef DEDICATED_SERVER
	// Renderer
	void render();

	// Creating menu
	void createMenu();
	void createSurvey();
#endif
	// Pour créer, join, disconnecter d'une game
	void host(CString mapName);
	void dedicate(CString mapName);
#ifndef DEDICATED_SERVER
	void join(CString IPAddress, int port, CString password = "");
#endif
	void disconnect();
	void edit(CString command);
	void kick(CString playerName);
	void kick(int ID);
	void ban(CString playerName);
	void ban(int ID);
	void banIP(CString playerIP);
	void unban(int banID);

	// Pour chatter
	void sayall(CString message);
#ifndef DEDICATED_SERVER
	void sayteam(CString message);
#endif
};


#endif

