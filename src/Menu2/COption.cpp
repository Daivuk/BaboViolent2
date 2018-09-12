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

#include "COption.h"
#include "GameVar.h"
#include "KeyManager.h"
#include "Scene.h"

extern Scene * scene;



COption::COption(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->texture = dktCreateTextureFromFile("main/textures/Menu4Back.tga", DKT_FILTER_LINEAR);
	instance->borderColor.set(1,.5f,.25f);

	//--- Labels and controls
	btn_apply = new CControl(instance, CVector2i(20,10), CVector2i(75, 25), "Apply", this, "BUTTON");
	btn_apply->toolTips = "Apply the above settings. It's the only way to save them.";
	btn_apply->visible = false;

	//--- RENDERING OPTIONS
	CControl * separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Rendering options", this, "SEPARATOR");

		//--- Show stats
		CControl * label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show stats:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show particles count, bandwith and FPS.";
		chk_showStats = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_showStats->check = gameVar.r_showStats;

		//--- Show latency
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show latency:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show latency box.";
		chk_showLatency = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_showLatency->check = gameVar.r_showLatency;

		//--- Precise cursor
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Precise Cursor:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Change how the gun points";
		chk_preciseCursor = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_preciseCursor->check = gameVar.cl_preciseCursor;

		//--- Full screen
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Full screen:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Play the game in fullscreen (Need to reboot the game).";
		chk_fullScreen = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_fullScreen->check = gameVar.r_fullScreen;

		//--- High details menu
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"High detail menus:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show round corner menus (Required good video card).";
		chk_highDetailMenus = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_highDetailMenus->check = gameVar.r_highDetailMenu;

		//--- Animated menus
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Animated menus:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Animated menu background (Requires a good video card, Restart Required).";
		chk_animatedMenus = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_animatedMenus->check = gameVar.r_animatedMenu;

		//--- Widescreen
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,90),"View Mode:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "(Need to reboot the game).";
		lst_viewMode = new CControl(instance, CVector2i(10,10), CVector2i(300,90),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
		CControl * item = new CControl(lst_viewMode, CVector2i(10,10), CVector2i(150,20),"Standard", this, "LABEL");
		item = new CControl(lst_viewMode, CVector2i(10,10), CVector2i(150,20),"Widescreen (Stretch)", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
		item = new CControl(lst_viewMode, CVector2i(10,10), CVector2i(150,20),"Widescreen (Bars)", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
		if (gameVar.r_widescreen == 0) lst_viewMode->selectChild(0);
		if (gameVar.r_widescreen == 1) lst_viewMode->selectChild(1);
		if (gameVar.r_widescreen == 2) lst_viewMode->selectChild(2);

		//--- Screen resolution
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,150),"Screen resolution:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "(Need to reboot the game).";

		if(gameVar.r_widescreen)
		{
			lst_resolution = new CControl(instance, CVector2i(10,10), CVector2i(300,150),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			CControl * item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"960 x 600", this, "LABEL");
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1280 x 800", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1440 x 900", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1680 x 1050", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1920 x 1200", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			if (gameVar.r_resolution == CVector2i(960, 600)) lst_resolution->selectChild(0);
			if (gameVar.r_resolution == CVector2i(1280, 800)) lst_resolution->selectChild(1);
			if (gameVar.r_resolution == CVector2i(1440, 900)) lst_resolution->selectChild(2);
			if (gameVar.r_resolution == CVector2i(1680, 1050)) lst_resolution->selectChild(3);
			if (gameVar.r_resolution == CVector2i(1920, 1200)) lst_resolution->selectChild(4);
		}
		else
		{
			lst_resolution = new CControl(instance, CVector2i(10,10), CVector2i(300,150),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			CControl * item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"640 x 480", this, "LABEL");
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"800 x 600", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1024 x 768", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1280 x 960", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_resolution, CVector2i(10,10), CVector2i(150,20),"1600 x 1200", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			if (gameVar.r_resolution == CVector2i(640, 480)) lst_resolution->selectChild(0);
			if (gameVar.r_resolution == CVector2i(800, 600)) lst_resolution->selectChild(1);
			if (gameVar.r_resolution == CVector2i(1024, 768)) lst_resolution->selectChild(2);
			if (gameVar.r_resolution == CVector2i(1280, 960)) lst_resolution->selectChild(3);
			if (gameVar.r_resolution == CVector2i(1600, 1200)) lst_resolution->selectChild(4);
		}
		

		//--- Color depth resolution
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,70),"Color depth:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "(Need to reboot the game).";
		lst_bitDepth = new CControl(instance, CVector2i(10,10), CVector2i(300,70),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_bitDepth, CVector2i(10,10), CVector2i(150,20),"16 bits", this, "LABEL");
			item = new CControl(lst_bitDepth, CVector2i(10,10), CVector2i(150,20),"32 bits", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
		if (gameVar.r_bitdepth == 16) lst_bitDepth->selectChild(0);
		if (gameVar.r_bitdepth == 32) lst_bitDepth->selectChild(1);
#if defined(_PRO_)
		//--- Display refresh rate
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,170),"Display refresh rate:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "(Need to reboot the game).";
		lst_refreshRate = new CControl(instance, CVector2i(10,10), CVector2i(300,170),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"default", this, "LABEL");
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"60", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"70", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"72", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"75", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item = new CControl(lst_refreshRate, CVector2i(10,10), CVector2i(150,20),"85", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
		if (gameVar.r_refreshRate == -1) lst_refreshRate->selectChild(0);
		if (gameVar.r_refreshRate == 60) lst_refreshRate->selectChild(1);
		if (gameVar.r_refreshRate == 70) lst_refreshRate->selectChild(2);
		if (gameVar.r_refreshRate == 72) lst_refreshRate->selectChild(3);
		if (gameVar.r_refreshRate == 75) lst_refreshRate->selectChild(4);
		if (gameVar.r_refreshRate == 85) lst_refreshRate->selectChild(5);

		//--- Team Indicator Type
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,90),"Team Indicator Type:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Choose a team indicator type.";
		lst_teamIndicatorType = new CControl(instance, CVector2i(10,10), CVector2i(300,90),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_teamIndicatorType, CVector2i(10,10), CVector2i(250,20),"Identify team by skin", this, "LABEL");
			item->toolTips = "Team affiliation is indicated by skin color, custom colors for skins are overwritten.";
			item = new CControl(lst_teamIndicatorType, CVector2i(10,10), CVector2i(250,20),"Identify all teams by halo", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Team affiliation is indicated by a glowing halo around the babo, custom colors for skins are not overwritten.";
			item = new CControl(lst_teamIndicatorType, CVector2i(10,10), CVector2i(250,20),"Identify your team by halo", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Team is indicated by a glowing halo around the babo, custom colors for skins are not overwritten.";
			lst_teamIndicatorType->selectChild(gameVar.cl_teamIndicatorType);
#endif
		//--- Weather Effects
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Weather effects:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Enable weather effects such as rain and fog.";
		chk_weatherEffects = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_weatherEffects->check = gameVar.r_weatherEffects;

		//--- Reflection
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Reflection:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "When weather effects are enabled, adds reflection to some themes.";
		chk_reflection = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_reflection->check = gameVar.r_reflection;

		//--- Wall's shadow
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Wall's shadow:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		chk_wallShadow = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_wallShadow->check = (gameVar.r_shadowQuality == 2)?true:false;

		//--- Player's shadow
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Player's shadow:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		chk_playerShadow = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_playerShadow->check = gameVar.r_playerShadow;

		//--- Projectile's shadow
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Projectile's shadow:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		chk_projectileShadow = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_projectileShadow->check = gameVar.r_projectileShadow;

		//--- Particles Lighting
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Particles lighting:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Enable lighting effect on particles.";
		chk_particleLighting = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_particleLighting->check = gameVar.r_particleLighting;

		//--- Particles Sorting
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Particles sorting:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sort particle in depth for more accurate result.\nNote: In this game the difference is not really apparent\nand it use a lot of CPU time.";
		chk_particleSorting = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_particleSorting->check = gameVar.r_particleSort;

		//--- Reduced Particles
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Reduced Particles:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Reduce number of particles in the game";
		chk_reducedParticles = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_reducedParticles->check = gameVar.r_reducedParticles;

		//--- Simple Text
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Basic Text:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Draw in a simplified way, usually increases frame rate";
		chk_simpleText = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_simpleText->check = gameVar.r_simpleText;


		//--- Terrain Splater
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Terrain splater:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Enable dirt on the terrain.";
		chk_terrainSplater = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_terrainSplater->check = gameVar.r_terrainSplater;

		//--- Casing
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show casing:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Spawn bullet's casing when firing.";
		chk_showCasing = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_showCasing->check = gameVar.r_showCasing;

		//--- Gound mark
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show ground mark:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show blood and explosion mark on ground.";
		chk_groundMark = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_groundMark->check = gameVar.r_showGroundMark;



   //--- GUI OPTIONS
#if defined(_PRO_)
	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"User Interface options", this, "SEPARATOR",label1, CONTROL_SNAP_BOTTOM, 15);

   //--- Chat Text Size
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(200,40),"Chat Text Size:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "The size of player chat text.\n";
	slc_chatTextSize = new CControl(instance, CVector2i(10,10), CVector2i(300,25),"28", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
	slc_chatTextSize->value = gameVar.r_chatTextSize;
	slc_chatTextSize->valueMin = 8;
	slc_chatTextSize->valueMax = 28;

   //--- Event Text Size
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(200,40),"Game Event Text Size:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "The size of game events text.\n";
	slc_eventTextSize = new CControl(instance, CVector2i(10,10), CVector2i(300,25),"28", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
	slc_eventTextSize->value = gameVar.r_eventTextSize;
	slc_eventTextSize->valueMin = 8;
	slc_eventTextSize->valueMax = 28;

	//--- Show Event Text
	label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show Game Event Text:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Show Game Event Text.";
	chk_showEventText = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
   chk_showEventText->check = gameVar.r_showEventText;
#endif
	//--- SOUND OPTIONS
	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Sounds and music options", this, "SEPARATOR",label1, CONTROL_SNAP_BOTTOM, 15);

		//--- Show stats
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,70),"Sound quality:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "(Need to reboot the game).";
		lst_mixRate = new CControl(instance, CVector2i(10,10), CVector2i(300,70),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_mixRate, CVector2i(10,10), CVector2i(150,20),"22050 Hz", this, "LABEL");
			item = new CControl(lst_mixRate, CVector2i(10,10), CVector2i(150,20),"44100 Hz", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
		if (gameVar.s_mixRate == 22050) lst_mixRate->selectChild(0);
		if (gameVar.s_mixRate == 44100) lst_mixRate->selectChild(1);

		//--- Max software channel
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(200,40),"Max software channels:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The max number of sounds that can be played in same time.\nNote: With 16 channels bv2 can play all the sounds.";
		txt_maxSoftwareChannels = new CControl(instance, CVector2i(10,10), CVector2i(300,40),"16", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_maxSoftwareChannels->value = gameVar.s_maxSoftwareChannels;
		txt_maxSoftwareChannels->valueMin = 4;
		txt_maxSoftwareChannels->valueMax = 64;

		//--- Volume
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Master volume:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		slc_volume = new CControl(instance, CVector2i(10,10), CVector2i(300,25),"", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		slc_volume->value = (int)(gameVar.s_masterVolume * 100);
		slc_volume->valueMin = 0;
		slc_volume->valueMax = 100;

		//--- In game music
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"In game music:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Play in game music.";
		chk_inGameMusic = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_inGameMusic->check = gameVar.s_inGameMusic;


	//--- KEY OPTIONS
	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Key options", this, "SEPARATOR",label1, CONTROL_SNAP_BOTTOM, 15);

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Move up:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Rolling up or forward.";
		key_moveUp = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_moveUp), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_moveUp->selectedIndex = gameVar.k_moveUp;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Move down:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Rolling down or backward.";
		key_moveDown = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_moveDown), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_moveDown->selectedIndex = gameVar.k_moveDown;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Move right:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Rolling right.";
		key_moveRight = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_moveRight), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_moveRight->selectedIndex = gameVar.k_moveRight;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Move left:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Rolling left.";
		key_moveLeft = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_moveLeft), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_moveLeft->selectedIndex = gameVar.k_moveLeft;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Shoot:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Trigger to shoot, and also used for spawning.";
		key_shoot = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_shoot), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_shoot->selectedIndex = gameVar.k_shoot;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Use secondary:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Trigger to activate your secondary weapons or power up.";
		key_secondary = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_melee), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_secondary->selectedIndex = gameVar.k_melee;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Throw grenade:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Throw a grenade.";
		key_throwGrenade = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_throwGrenade), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_throwGrenade->selectedIndex = gameVar.k_throwGrenade;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Throw molotov:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Throw a molotov Cocktail.";
		key_throwMolotov = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_throwMolotov), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_throwMolotov->selectedIndex = gameVar.k_throwMolotov;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Use key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Pick up a gun on ground.\nPlank bomb.\nDesamorse bomb.";
		key_pickUp = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_pickUp), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_pickUp->selectedIndex = gameVar.k_pickUp;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Chat all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Chat to all (Friendly and ennemy).";
		key_chatAll = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_chatAll), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_chatAll->selectedIndex = gameVar.k_chatAll;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Chat team:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Chat to team only.";
		key_chatTeam = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_chatTeam), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_chatTeam->selectedIndex = gameVar.k_chatTeam;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Show score:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show the score board.";
		key_showScore = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_showScore), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_showScore->selectedIndex = gameVar.k_showScore;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Menu access:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Show the menu to select team and weapon.";
		key_menuAccess = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_menuAccess), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_menuAccess->selectedIndex = gameVar.k_menuAccess;

#if defined(_PRO_)	
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Screen shot:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Take a screen shot.";
		key_screenShot = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_screenShot), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_screenShot->selectedIndex = gameVar.k_screenShot;

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Stats:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Write statistics.";
		key_stats = new CControl(instance, CVector2i(10,10), CVector2i(100,25),
			keyManager.getKeyName(gameVar.k_stats), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_stats->selectedIndex = gameVar.k_stats;
#endif

	//--- QUICK MESSAGES OPTIONS
	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Quick messages' options", this, "SEPARATOR", label1, CONTROL_SNAP_BOTTOM, 15);

		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  1:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 1.";
		txt_qMsg01 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg01.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg01 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg01->check = gameVar.cl_qMsg01.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg01 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg01), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg01->selectedIndex = gameVar.k_qMsg01;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  2:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 2.";
		txt_qMsg02 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg02.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg02 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg02->check = gameVar.cl_qMsg02.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg02 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg02), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg02->selectedIndex = gameVar.k_qMsg02;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  3:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 3.";
		txt_qMsg03 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg03.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg03 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg03->check = gameVar.cl_qMsg03.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg03 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg03), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg03->selectedIndex = gameVar.k_qMsg03;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  4:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 4.";
		txt_qMsg04 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg04.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg04 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg04->check = gameVar.cl_qMsg04.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg04 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg04), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg04->selectedIndex = gameVar.k_qMsg04;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  5:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 5.";
		txt_qMsg05 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg05.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg05 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg05->check = gameVar.cl_qMsg05.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg05 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg05), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg05->selectedIndex = gameVar.k_qMsg05;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  6:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 6.";
		txt_qMsg06 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg06.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg06 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg06->check = gameVar.cl_qMsg06.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg06 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg06), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg06->selectedIndex = gameVar.k_qMsg06;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  7:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 7.";
		txt_qMsg07 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg07.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg07 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg07->check = gameVar.cl_qMsg07.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg07 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg07), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg07->selectedIndex = gameVar.k_qMsg07;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  8:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 8.";
		txt_qMsg08 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg08.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg08 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg08->check = gameVar.cl_qMsg08.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg08 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg08), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg08->selectedIndex = gameVar.k_qMsg08;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message  9:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 9.";
		txt_qMsg09 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg09.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg09 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg09->check = gameVar.cl_qMsg09.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg09 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg09), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg09->selectedIndex = gameVar.k_qMsg09;
		//--- Key
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Quick Message 10:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Quick Message 10.";
		txt_qMsg10 = new CControl(instance, CVector2i(10,10), CVector2i(300,25), CString("%s", &(gameVar.cl_qMsg10.s[1])), this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"To all:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Sends the message to everyone (or just the team).";
		chk_qMsg10 = new CControl(instance, CVector2i(10,10), CVector2i(25, 25), "", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_qMsg10->check = gameVar.cl_qMsg10.s[0] == 'a';
		label1 = new CControl(instance, CVector2i(20,10), CVector2i(200,25),"Key:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Select the preferd key.";
		key_qMsg10 = new CControl(instance, CVector2i(10,10), CVector2i(200, 25), keyManager.getKeyName(gameVar.k_qMsg10), this, "KEY", label1, CONTROL_SNAP_RIGHT);
		key_qMsg10->selectedIndex = gameVar.k_qMsg10;

	//--- FINAL SEPARATOR
	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"", this, "SEPARATOR",label1, CONTROL_SNAP_BOTTOM, 15);


	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}


COption::~COption()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void COption::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK" || control->style == "KEY")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void COption::MouseLeave(CControl * control)
{
}
void COption::MouseDown(CControl * control)
{
}
void COption::MouseUp(CControl * control)
{
}
void COption::MouseMove(CControl * control)
{
}
void COption::Click(CControl * control)
{
	if (control->style == "EDIT" || control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK" || control->style == "KEY")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}

/*	if (control == btn_apply)
	{*/
		//--- Save all da settings!

		// Rendering options
		gameVar.r_showStats = chk_showStats->check;
		gameVar.r_showLatency = chk_showLatency->check;
		gameVar.r_fullScreen = chk_fullScreen->check;
		gameVar.cl_preciseCursor = chk_preciseCursor->check;

		if(lst_viewMode->selectedIndex != gameVar.r_widescreen)
		{
			if(lst_viewMode->selectedIndex > 0)
			{
				lst_resolution->children[2]->text = "960 x 600";
				lst_resolution->children[3]->text = "1280 x 800";
				lst_resolution->children[4]->text = "1440 x 900";
				lst_resolution->children[5]->text = "1680 x 1050";
				lst_resolution->children[6]->text = "1920 x 1200";		
			}
			else
			{
				lst_resolution->children[2]->text = "640 x 480";
				lst_resolution->children[3]->text = "800 x 600";
				lst_resolution->children[4]->text = "1024 x 768";
				lst_resolution->children[5]->text = "1280 x 960";
				lst_resolution->children[6]->text = "1600 x 1200";
			}
		}
		gameVar.r_widescreen = lst_viewMode->selectedIndex;

		if(gameVar.r_widescreen > 0)
		{
			switch (lst_resolution->selectedIndex)
			{
			case 0: gameVar.r_resolution.set(960, 600); break;
			case 1: gameVar.r_resolution.set(1280, 800); break;
			case 2: gameVar.r_resolution.set(1440, 900); break;
			case 3: gameVar.r_resolution.set(1680, 1050); break;
			case 4: gameVar.r_resolution.set(1920, 1200); break;
			default: gameVar.r_resolution.set(1280, 800); break;
			}
		}
		else
		{
			switch (lst_resolution->selectedIndex)
			{
			case 0: gameVar.r_resolution.set(640, 480); break;
			case 1: gameVar.r_resolution.set(800, 600); break;
			case 2: gameVar.r_resolution.set(1024, 768); break;
			case 3: gameVar.r_resolution.set(1280, 960); break;
			case 4: gameVar.r_resolution.set(1600, 1200); break;
			default: gameVar.r_resolution.set(800, 600); break;
			}
		}

		switch (lst_bitDepth->selectedIndex)
		{
		case 0: gameVar.r_bitdepth = 16; break;
		case 1: gameVar.r_bitdepth = 32; break;
		default: gameVar.r_bitdepth = 32; break;
		}
#if defined(_PRO_)
		switch (lst_refreshRate->selectedIndex)
		{
		case 0: gameVar.r_refreshRate = -1; break;
		case 1: gameVar.r_refreshRate = 60; break;
		case 2: gameVar.r_refreshRate = 70; break;
		case 3: gameVar.r_refreshRate = 72; break;
		case 4: gameVar.r_refreshRate = 75; break;
		case 5: gameVar.r_refreshRate = 85; break;
		default: gameVar.r_refreshRate = -1; break;
		}

		gameVar.cl_teamIndicatorType = lst_teamIndicatorType->selectedIndex;
#endif
		gameVar.r_shadowQuality = (chk_wallShadow->check)?2:0;
		gameVar.r_playerShadow = chk_playerShadow->check;
		gameVar.r_projectileShadow = chk_projectileShadow->check;
		gameVar.r_particleLighting = chk_particleLighting->check;
		gameVar.r_particleSort = chk_particleSorting->check;
		gameVar.r_terrainSplater = chk_terrainSplater->check;
		gameVar.r_showCasing = chk_showCasing->check;
		gameVar.r_showGroundMark = chk_groundMark->check;
		gameVar.r_reflection = chk_reflection->check;
		gameVar.r_highDetailMenu = chk_highDetailMenus->check;
		gameVar.r_animatedMenu = chk_animatedMenus->check;
		gameVar.r_reducedParticles = chk_reducedParticles->check;
		gameVar.r_simpleText= chk_simpleText->check;

		// Must reload weather if changed
		if(gameVar.r_weatherEffects != chk_weatherEffects->check) {
			gameVar.r_weatherEffects = chk_weatherEffects->check;
			if(scene && scene->client && scene->client->game && scene->client->game->map)
				scene->client->game->map->reloadWeather();
		}

#if defined(_PRO_)
      // GUI Options
      gameVar.r_chatTextSize = slc_chatTextSize->value;
      gameVar.r_eventTextSize = slc_eventTextSize->value;
      gameVar.r_showEventText = chk_showEventText->check;
#endif

		// Sound options
		switch (lst_mixRate->selectedIndex)
		{
		case 0: gameVar.s_mixRate = 22050; break;
		case 1: gameVar.s_mixRate = 44100; break;
		default: gameVar.s_mixRate = 22050; break;
		}
		gameVar.s_maxSoftwareChannels = txt_maxSoftwareChannels->value;
		gameVar.s_masterVolume = (float)(slc_volume->value) / 100.0f;
		gameVar.s_inGameMusic = chk_inGameMusic->check;

		// Key options
		gameVar.k_moveUp = key_moveUp->selectedIndex;
		gameVar.k_moveDown = key_moveDown->selectedIndex;
		gameVar.k_moveRight = key_moveRight->selectedIndex;
		gameVar.k_moveLeft = key_moveLeft->selectedIndex;
		gameVar.k_shoot = key_shoot->selectedIndex;
		gameVar.k_throwGrenade = key_throwGrenade->selectedIndex;
		gameVar.k_throwMolotov = key_throwMolotov->selectedIndex;
		gameVar.k_pickUp = key_pickUp->selectedIndex;
		gameVar.k_chatAll = key_chatAll->selectedIndex;
		gameVar.k_chatTeam = key_chatTeam->selectedIndex;
		gameVar.k_showScore = key_showScore->selectedIndex;
		gameVar.k_menuAccess = key_menuAccess->selectedIndex;

#if defined(_PRO_)	
      gameVar.k_screenShot = key_screenShot->selectedIndex;
      gameVar.k_stats = key_stats->selectedIndex;
#endif

		gameVar.cl_qMsg01.set("%c%s", (chk_qMsg01->check ? 'a' : 't'), txt_qMsg01->text.s);
		gameVar.cl_qMsg02.set("%c%s", (chk_qMsg02->check ? 'a' : 't'), txt_qMsg02->text.s);
		gameVar.cl_qMsg03.set("%c%s", (chk_qMsg03->check ? 'a' : 't'), txt_qMsg03->text.s);
		gameVar.cl_qMsg04.set("%c%s", (chk_qMsg04->check ? 'a' : 't'), txt_qMsg04->text.s);
		gameVar.cl_qMsg05.set("%c%s", (chk_qMsg05->check ? 'a' : 't'), txt_qMsg05->text.s);
		gameVar.cl_qMsg06.set("%c%s", (chk_qMsg06->check ? 'a' : 't'), txt_qMsg06->text.s);
		gameVar.cl_qMsg07.set("%c%s", (chk_qMsg07->check ? 'a' : 't'), txt_qMsg07->text.s);
		gameVar.cl_qMsg08.set("%c%s", (chk_qMsg08->check ? 'a' : 't'), txt_qMsg08->text.s);
		gameVar.cl_qMsg09.set("%c%s", (chk_qMsg09->check ? 'a' : 't'), txt_qMsg09->text.s);
		gameVar.cl_qMsg10.set("%c%s", (chk_qMsg10->check ? 'a' : 't'), txt_qMsg10->text.s);

		gameVar.k_qMsg01 = key_qMsg01->selectedIndex;
		gameVar.k_qMsg02 = key_qMsg02->selectedIndex;
		gameVar.k_qMsg03 = key_qMsg03->selectedIndex;
		gameVar.k_qMsg04 = key_qMsg04->selectedIndex;
		gameVar.k_qMsg05 = key_qMsg05->selectedIndex;
		gameVar.k_qMsg06 = key_qMsg06->selectedIndex;
		gameVar.k_qMsg07 = key_qMsg07->selectedIndex;
		gameVar.k_qMsg08 = key_qMsg08->selectedIndex;
		gameVar.k_qMsg09 = key_qMsg09->selectedIndex;
		gameVar.k_qMsg10 = key_qMsg10->selectedIndex;
//	}
}
void COption::Validate(CControl * control)
{
	if (control->style == "EDIT" || control->style == "KEY")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}

		//--- Save all da settings!

		//gameVar.cl_playerName = txt_playerName->text;

		// Rendering options
		gameVar.r_showStats = chk_showStats->check;
		gameVar.r_showLatency = chk_showLatency->check;
		gameVar.r_fullScreen = chk_fullScreen->check;
		gameVar.r_widescreen = lst_viewMode->selectedIndex;
		gameVar.cl_preciseCursor = chk_preciseCursor->check;
		if(gameVar.r_widescreen > 0)
		{
			switch (lst_resolution->selectedIndex)
			{
			case 0: gameVar.r_resolution.set(960, 600); break;
			case 1: gameVar.r_resolution.set(1280, 800); break;
			case 2: gameVar.r_resolution.set(1440, 900); break;
			case 3: gameVar.r_resolution.set(1680, 1050); break;
			case 4: gameVar.r_resolution.set(1920, 1200); break;
			default: gameVar.r_resolution.set(1280, 800); break;
			}
		}
		else
		{
			switch (lst_resolution->selectedIndex)
			{
			case 0: gameVar.r_resolution.set(640, 480); break;
			case 1: gameVar.r_resolution.set(800, 600); break;
			case 2: gameVar.r_resolution.set(1024, 768); break;
			case 3: gameVar.r_resolution.set(1280, 960); break;
			case 4: gameVar.r_resolution.set(1600, 1200); break;
			default: gameVar.r_resolution.set(800, 600); break;
			}
		}
		switch (lst_bitDepth->selectedIndex)
		{
		case 0: gameVar.r_bitdepth = 16; break;
		case 1: gameVar.r_bitdepth = 32; break;
		default: gameVar.r_bitdepth = 32; break;
		}
		gameVar.r_shadowQuality = (chk_wallShadow->check)?2:0;
		gameVar.r_playerShadow = chk_playerShadow->check;
		gameVar.r_projectileShadow = chk_projectileShadow->check;
		gameVar.r_particleLighting = chk_particleLighting->check;
		gameVar.r_particleSort = chk_particleSorting->check;
		gameVar.r_terrainSplater = chk_terrainSplater->check;
		gameVar.r_showCasing = chk_showCasing->check;
		gameVar.r_showGroundMark = chk_groundMark->check;
		gameVar.r_reflection = chk_reflection->check;
		gameVar.r_highDetailMenu = chk_highDetailMenus->check;
		gameVar.r_animatedMenu = chk_animatedMenus->check;

		// Must reload weather if changed
		if(gameVar.r_weatherEffects != chk_weatherEffects->check) {
			gameVar.r_weatherEffects = chk_weatherEffects->check;
			if(scene && scene->client && scene->client->game && scene->client->game->map)
				scene->client->game->map->reloadWeather();
		}

#if defined(_PRO_)
		switch (lst_refreshRate->selectedIndex)
		{
		case 0: gameVar.r_refreshRate = -1; break;
		case 1: gameVar.r_refreshRate = 60; break;
		case 2: gameVar.r_refreshRate = 70; break;
		case 3: gameVar.r_refreshRate = 72; break;
		case 4: gameVar.r_refreshRate = 75; break;
		case 5: gameVar.r_refreshRate = 85; break;
		default: gameVar.r_refreshRate = -1; break;
		}
      // GUI Options
      gameVar.r_chatTextSize = slc_chatTextSize->value;
      gameVar.r_eventTextSize = slc_eventTextSize->value;
      gameVar.r_showEventText = chk_showEventText->check;
#endif

		// Sound options
		switch (lst_mixRate->selectedIndex)
		{
		case 0: gameVar.s_mixRate = 22050; break;
		case 1: gameVar.s_mixRate = 44100; break;
		default: gameVar.s_mixRate = 22050; break;
		}
		gameVar.s_maxSoftwareChannels = txt_maxSoftwareChannels->value;
		gameVar.s_masterVolume = (float)(slc_volume->value) / 100.0f;
		gameVar.s_inGameMusic = chk_inGameMusic->check;

		// Key options
		gameVar.k_moveUp = key_moveUp->selectedIndex;
		gameVar.k_moveDown = key_moveDown->selectedIndex;
		gameVar.k_moveRight = key_moveRight->selectedIndex;
		gameVar.k_moveLeft = key_moveLeft->selectedIndex;
		gameVar.k_shoot = key_shoot->selectedIndex;
		gameVar.k_melee = key_secondary->selectedIndex;
		gameVar.k_throwGrenade = key_throwGrenade->selectedIndex;
		gameVar.k_throwMolotov = key_throwMolotov->selectedIndex;
		gameVar.k_pickUp = key_pickUp->selectedIndex;
		gameVar.k_chatAll = key_chatAll->selectedIndex;
		gameVar.k_chatTeam = key_chatTeam->selectedIndex;
		gameVar.k_showScore = key_showScore->selectedIndex;
		gameVar.k_menuAccess = key_menuAccess->selectedIndex;

#if defined(_PRO_)
      gameVar.k_screenShot = key_screenShot->selectedIndex;
      gameVar.k_stats = key_stats->selectedIndex;
#endif

		gameVar.cl_qMsg01.set("%c%s", (chk_qMsg01->check ? 'a' : 't'), txt_qMsg01->text.s);
		gameVar.cl_qMsg02.set("%c%s", (chk_qMsg02->check ? 'a' : 't'), txt_qMsg02->text.s);
		gameVar.cl_qMsg03.set("%c%s", (chk_qMsg03->check ? 'a' : 't'), txt_qMsg03->text.s);
		gameVar.cl_qMsg04.set("%c%s", (chk_qMsg04->check ? 'a' : 't'), txt_qMsg04->text.s);
		gameVar.cl_qMsg05.set("%c%s", (chk_qMsg05->check ? 'a' : 't'), txt_qMsg05->text.s);
		gameVar.cl_qMsg06.set("%c%s", (chk_qMsg06->check ? 'a' : 't'), txt_qMsg06->text.s);
		gameVar.cl_qMsg07.set("%c%s", (chk_qMsg07->check ? 'a' : 't'), txt_qMsg07->text.s);
		gameVar.cl_qMsg08.set("%c%s", (chk_qMsg08->check ? 'a' : 't'), txt_qMsg08->text.s);
		gameVar.cl_qMsg09.set("%c%s", (chk_qMsg09->check ? 'a' : 't'), txt_qMsg09->text.s);
		gameVar.cl_qMsg10.set("%c%s", (chk_qMsg10->check ? 'a' : 't'), txt_qMsg10->text.s);

		gameVar.k_qMsg01 = key_qMsg01->selectedIndex;
		gameVar.k_qMsg02 = key_qMsg02->selectedIndex;
		gameVar.k_qMsg03 = key_qMsg03->selectedIndex;
		gameVar.k_qMsg04 = key_qMsg04->selectedIndex;
		gameVar.k_qMsg05 = key_qMsg05->selectedIndex;
		gameVar.k_qMsg06 = key_qMsg06->selectedIndex;
		gameVar.k_qMsg07 = key_qMsg07->selectedIndex;
		gameVar.k_qMsg08 = key_qMsg08->selectedIndex;
		gameVar.k_qMsg09 = key_qMsg09->selectedIndex;
		gameVar.k_qMsg10 = key_qMsg10->selectedIndex;
}


#endif

