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
#ifndef COPTION_H_INCLUDED
#define COPTION_H_INCLUDED


#include "CPanel.h"


class COption : public CPanel
{
public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	//--- Apply
	CControl * btn_apply;

	// Rendering options
	CControl * chk_showStats;
	CControl * chk_showLatency;
	CControl * chk_preciseCursor;
	CControl * chk_fullScreen;
	CControl * chk_highDetailMenus;
	CControl * chk_animatedMenus;
	CControl * lst_resolution;
	CControl * lst_bitDepth;
	CControl * lst_refreshRate;
	CControl * lst_viewMode;
	CControl * lst_teamIndicatorType;
	CControl * chk_weatherEffects;
	CControl * chk_reflection;
	CControl * chk_wallShadow;
	CControl * chk_playerShadow;
	CControl * chk_projectileShadow;
	CControl * chk_particleLighting;
	CControl * chk_particleSorting;
	CControl * chk_terrainSplater;
	CControl * chk_showCasing;
	CControl * chk_groundMark;
    CControl * chk_reducedParticles;
	CControl * chk_simpleText;
	CControl * chk_inGameMusic;

#if defined(_PRO_)
   // GUI options
   CControl * slc_chatTextSize;
   CControl * slc_eventTextSize;
   CControl * chk_showEventText;
#endif
	// Sound options
	CControl * lst_mixRate;
	CControl * txt_maxSoftwareChannels;
	CControl * slc_volume;

	// Key options
	CControl * key_moveUp;
	CControl * key_moveDown;
	CControl * key_moveRight;
	CControl * key_moveLeft;
	CControl * key_shoot;
	CControl * key_throwGrenade;
	CControl * key_throwMolotov;
	CControl * key_secondary;
	CControl * key_pickUp;
	CControl * key_chatAll;
	CControl * key_chatTeam;
	CControl * key_showScore;
	CControl * key_menuAccess;
#if defined(_PRO_)	
   CControl * key_screenShot;
   CControl * key_stats;
#endif
#ifndef DISABLE_QUICK_MESSAGES
	// Quick Messages options
	CControl * txt_qMsg01;
	CControl * chk_qMsg01;
	CControl * key_qMsg01;
	CControl * txt_qMsg02;
	CControl * chk_qMsg02;
	CControl * key_qMsg02;
	CControl * txt_qMsg03;
	CControl * chk_qMsg03;
	CControl * key_qMsg03;
	CControl * txt_qMsg04;
	CControl * chk_qMsg04;
	CControl * key_qMsg04;
	CControl * txt_qMsg05;
	CControl * chk_qMsg05;
	CControl * key_qMsg05;
	CControl * txt_qMsg06;
	CControl * chk_qMsg06;
	CControl * key_qMsg06;
	CControl * txt_qMsg07;
	CControl * chk_qMsg07;
	CControl * key_qMsg07;
	CControl * txt_qMsg08;
	CControl * chk_qMsg08;
	CControl * key_qMsg08;
	CControl * txt_qMsg09;
	CControl * chk_qMsg09;
	CControl * key_qMsg09;
	CControl * txt_qMsg10;
	CControl * chk_qMsg10;
	CControl * key_qMsg10;
#endif
public:
	COption(CControl * in_parent, CControl * in_alignTo);
	virtual ~COption();

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

