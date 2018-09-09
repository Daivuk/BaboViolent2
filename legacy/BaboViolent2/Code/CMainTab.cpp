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

#ifndef DEDICATED_SERVER

#include "CMainTab.h"
#include "CMenuManager.h"
#include "Console.h"
#include "CMaster.h"
#include "CCurl.h"
#include "Scene.h"


CControl * btn_resume;

extern Scene* scene;


CMainTab::CMainTab(CControl * in_parent)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//btn_userLogin = new CControl(parent, CVector2i(32,32),CVector2i(128,25),"Babo Profile",this,"BUTTON");
	btn_news = new CControl(parent, CVector2i(32,32),CVector2i(128,25),"News",this,"BUTTON");

	//userLogin = new CUserLogin(in_parent, btn_userLogin);
	news = new CNews(in_parent, btn_news);
	profile = new CProfile(in_parent, btn_news);
	browser = new CBrowser(in_parent, btn_news);
	host = new CHost(in_parent, btn_news);
	editor = new CEditor(in_parent, btn_news);
	credit = new CCredit(in_parent, btn_news);
	option = new COption(in_parent, btn_news);

	if(gameVar.r_animatedMenu)
	{
		unsigned int tex = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_NEAREST);
		option->instance->superPanel = true;
		//userLogin->instance->superPanel = true;
		news->lst_news->superPanel = true;
		profile->instance->superPanel = true;
		browser->lst_browseList->superPanel = true;
		//friends->lst_browseList->superPanel = true;
		host->instance->superPanel = true;
		editor->instance->superPanel = true;
		option->instance->superPanel = true;

		option->instance->texture = tex;
		//userLogin->instance->texture = tex;
		news->lst_news->texture = tex;
		profile->instance->texture = tex;
		browser->lst_browseList->texture = tex;
		//friends->lst_browseList->texture = tex;
		host->instance->texture = tex;
		editor->instance->texture = tex;
		option->instance->texture = tex;
	}
	

	btn_resume = new CControl(parent, CVector2i(0,0), CVector2i(100,25), "Resume Game", this, "BUTTON");
	btn_resume->enable = false;

	//--- Buttons at the top
	//delete btn_userLogin;
	delete btn_news;
	//btn_userLogin = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"Babo Profile",this,"BUTTON");
	//btn_userLogin->toolTips = "User login information and stats.";
	btn_news = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"News",this,"BUTTON");
	btn_news->toolTips = "News from Babo World.";
	btn_profile = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"Profile",this,"BUTTON", btn_news, CONTROL_SNAP_RIGHT);
	btn_profile->toolTips = "User login information and stats.";
	btn_browser = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"Game Browser",this,"BUTTON", btn_profile, CONTROL_SNAP_RIGHT);
	btn_browser->toolTips = "Browse games over the Internet or on a local network.";
	//btn_friends = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"Friends",this,"BUTTON", btn_browser, CONTROL_SNAP_RIGHT);
	//btn_friends->toolTips = "Find out where your friends are playing.";
	btn_create = new CControl(parent, CVector2i(32,32),CVector2i(90,25),"Host",this,"BUTTON", btn_browser, CONTROL_SNAP_RIGHT);
	btn_create->toolTips = "Host a game and play in it.\nSee CTF.cfg for more informations about dedicating a server.";
	btn_editor = new CControl(parent, CVector2i(32,32),CVector2i(100,25),"Map Editor",this,"BUTTON", btn_create, CONTROL_SNAP_RIGHT);
	btn_editor->toolTips = "Edit or create a new map.";
	btn_option = new CControl(parent, CVector2i(32,32),CVector2i(80,25),"Options",this,"BUTTON", btn_editor, CONTROL_SNAP_RIGHT);
	btn_option->toolTips = "Setup all game options.";
	btn_credits = new CControl(parent, CVector2i(32,32),CVector2i(80,25),"Credits",this,"BUTTON", btn_option, CONTROL_SNAP_RIGHT);
	btn_credits->toolTips = "See who are the dudes who created this bloody hell!!";
	btn_quit = new CControl(parent, CVector2i(32,32),CVector2i(48,25),"\x4X",this,"BUTTON", btn_credits, CONTROL_SNAP_RIGHT);
	btn_quit->toolTips = "Quit the game. :(";

	//userLogin->setVisible(true);
	news->setVisible(true);
	//btn_userLogin->enable = false;
	btn_news->enable = false;
	profile->setVisible(false);
	browser->setVisible(false);
	//friends->setVisible(false);
	host->setVisible(false);
	editor->setVisible(false);
	credit->setVisible(false);
	option->setVisible(false);

	//current = userLogin;
	current = news;
	lastButtonPress = btn_news;//btn_userLogin;

	timeBrowseDelay = -1;
}


CMainTab::~CMainTab()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CMainTab::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CMainTab::MouseLeave(CControl * control)
{
}
void CMainTab::MouseDown(CControl * control)
{
}
void CMainTab::MouseUp(CControl * control)
{
}
void CMainTab::MouseMove(CControl * control)
{
}
void CMainTab::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	/*if (control == btn_userLogin)
	{
		if (current) current->setVisible(false);
		current = userLogin;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}*/
	if (control == btn_news)
	{
		if (current) current->setVisible(false);
		current = news;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}
	if (control == btn_profile)
	{
		if (current) current->setVisible(false);
		current = profile;
		//friends->updateList();
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
		profile->current->setVisible(true);
	}
	if (control == btn_browser)
	{
		//--- If first time, browse
		if (current) current->setVisible(false);
		current = browser;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);

		if (timeBrowseDelay == -1)
		{
			timeBrowseDelay = 2;
		}
	}
	if (control == btn_create)
	{
		if (current) current->setVisible(false);
		current = host;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}
	/*if (control == btn_friends)
	{
		if (current) current->setVisible(false);
		current = friends;
		friends->updateList();
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}*/
	if (control == btn_editor)
	{
		if (current) current->setVisible(false);
		current = editor;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}
	if (control == btn_option)
	{
		if (current) current->setVisible(false);
		current = option;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}
	if (control == btn_credits)
	{
		if (current) current->setVisible(false);
		current = credit;
		lastButtonPress->enable = true;
		control->enable = false;
		lastButtonPress = control;
		current->setVisible(true);
	}
	if (control == btn_quit)
	{
		dkwForceQuit();
	}
	if (btn_resume == control)
	{
		if (scene && scene->client && scene->client->game->thisPlayer && !scene->client->game->isServerGame)
		{
			scene->client->game->thisPlayer->updateSkin();

#ifdef _PRO_
            Player* tplayer = scene->client->game->thisPlayer;
			net_clsv_svcl_player_update_skin updateSkin;
			updateSkin.playerID = tplayer->playerID;
			memcpy(updateSkin.skin, tplayer->skin.s, (tplayer->skin.len() <= 6)?tplayer->skin.len()+1:7);
			updateSkin.blueDecal[0] = (unsigned char)(tplayer->blueDecal[0] * 255.0f);
			updateSkin.blueDecal[1] = (unsigned char)(tplayer->blueDecal[1] * 255.0f);
			updateSkin.blueDecal[2] = (unsigned char)(tplayer->blueDecal[2] * 255.0f);
			updateSkin.greenDecal[0] = (unsigned char)(tplayer->greenDecal[0] * 255.0f);
			updateSkin.greenDecal[1] = (unsigned char)(tplayer->greenDecal[1] * 255.0f);
			updateSkin.greenDecal[2] = (unsigned char)(tplayer->greenDecal[2] * 255.0f);
			updateSkin.redDecal[0] = (unsigned char)(tplayer->redDecal[0] * 255.0f);
			updateSkin.redDecal[1] = (unsigned char)(tplayer->redDecal[1] * 255.0f);
			updateSkin.redDecal[2] = (unsigned char)(tplayer->redDecal[2] * 255.0f);
			bb_clientSend(scene->client->uniqueClientID, (char*)&updateSkin, sizeof(net_clsv_svcl_player_update_skin), NET_CLSV_SVCL_PLAYER_UPDATE_SKIN);
#endif
		}

		menuManager.root->visible = false;
	}
}


#endif

