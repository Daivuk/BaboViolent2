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
#ifndef CHOST_H_INCLUDED
#define CHOST_H_INCLUDED


#include "CPanel.h"


class CHost : public CPanel
{
public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	CControl * btn_start;

	//--- Some controls (Login and create acount)
	CControl * txt_gameName;
	CControl * txt_password;
	CControl * lst_gameType;
   CControl * lst_serverType;
#ifdef _PRO_
   CControl * lst_spawnType;
#endif
	CControl * txt_maxPlayer; CControl * btn_maxPlayerPreset[4];
	CControl * txt_port;

	CControl * txt_scoreLimit;
	CControl * txt_winLimit;
	CControl * txt_gameTimeLimit; CControl * btn_gameTimePreset[4];
	CControl * txt_timeToSpawn;
	CControl * txt_roundTimeLimit;
	CControl * txt_bombTime;

	CControl * chk_ff;
	CControl * chk_ffReflect;
	CControl * chk_forceRespawn;

	//--- Map list!
	std::vector<CControl *> chk_maps;

public:
	CHost(CControl * in_parent, CControl * in_alignTo);
	virtual ~CHost();

	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
	void Validate(CControl * control);
};


#endif
#endif

