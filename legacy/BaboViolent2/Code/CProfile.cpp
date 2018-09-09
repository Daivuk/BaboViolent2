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

#include "CProfile.h"

CProfile::CProfile(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	//instance->texture = dktCreateTextureFromFile("main/textures/Menu3Back.tga", DKT_FILTER_LINEAR);
	//instance->borderColor.set(1,.5f,.25f);

	btn_baboProfile = new CControl(instance, CVector2i(10,10),CVector2i(100,25),"Babo Profile",this,"BUTTON");
	btn_baboProfile->toolTips = "Customize you Babo!";
	btn_account = new CControl(instance, CVector2i(10,0),CVector2i(100,25),"My Account",this,"BUTTON", btn_baboProfile, CONTROL_SNAP_RIGHT);
	btn_account->toolTips = "User login information and stats.";
	btn_friends = new CControl(instance, CVector2i(10,0),CVector2i(90,25),"Friends",this,"BUTTON", btn_account, CONTROL_SNAP_RIGHT);
	btn_friends->toolTips = "Find out where your friends are playing.";
	btn_players = new CControl(instance, CVector2i(10,0),CVector2i(90,25),"Players",this,"BUTTON", btn_friends, CONTROL_SNAP_RIGHT);
	btn_players->toolTips = "Search for a player.";
	btn_stats = new CControl(instance, CVector2i(10,0),CVector2i(100,25),"Statistics",this,"BUTTON", btn_players, CONTROL_SNAP_RIGHT);
	btn_stats->toolTips = "Best of all!";
	btn_registerClan = new CControl(instance, CVector2i(10,0),CVector2i(110,25),"Register Clan",this,"BUTTON", btn_stats, CONTROL_SNAP_RIGHT);
	btn_registerClan->toolTips = "Register clan and invite players.";
	btn_clans = new CControl(instance, CVector2i(10,0),CVector2i(90,25),"Clans",this,"BUTTON", btn_registerClan, CONTROL_SNAP_RIGHT);
	btn_clans->toolTips = "View list of registered clans.";

	userLogin = new CUserLogin(instance, btn_baboProfile);
	friends = new CFriends(instance, btn_baboProfile);
	account = new CAccount(instance, btn_baboProfile);
	registerClan = new CRegisterClan(instance, btn_baboProfile);
	clans = new CClans(instance, btn_baboProfile);
	players = new CPlayers(instance, btn_baboProfile);
	stats = new CStats(instance, btn_baboProfile);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];

	current = userLogin;
	userLogin->setVisible(false);
	friends->setVisible(false);
	account->setVisible(false);
	registerClan->setVisible(false);
	clans->setVisible(false);
	players->setVisible(false);
	stats->setVisible(false);
	lastButtonPressed = btn_baboProfile;
	lastButtonPressed->enable = false;
}

CProfile::~CProfile()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CProfile::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CProfile::MouseLeave(CControl * control)
{
}
void CProfile::MouseDown(CControl * control)
{
}
void CProfile::MouseUp(CControl * control)
{
}
void CProfile::MouseMove(CControl * control)
{
}
void CProfile::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	CListener * newL = 0;
	CControl * newC = 0;
	if (control == btn_baboProfile)
	{
		newL = userLogin;
		newC = btn_baboProfile;
	}
	else if (control == btn_stats)
	{
		newL = stats;
		newC = btn_stats;
	}
	else if (control == btn_account)
	{
		newL = account;
		newC = btn_account;
	}
	else if (control == btn_friends)
	{
		newL = friends;
		newC = btn_friends;
	}
	else if (control == btn_registerClan)
	{
		newL = registerClan;
		newC = btn_registerClan;
	}
	else if (control == btn_players)
	{
		newL = players;
		newC = btn_players;
	}
	else if (control == btn_clans)
	{
		newL = clans;
		newC = btn_clans;
	}

	if (newL)
	{
		if (current)
			current->setVisible(false);
		current = newL;
		current->setVisible(true);
	}
	
	if (newC)
	{
		if (lastButtonPressed)
			lastButtonPressed->enable = true;
		lastButtonPressed = newC;
		lastButtonPressed->enable = false;
	}
}
void CProfile::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}

#endif

