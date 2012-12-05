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
#include "CSurvey.h"
#include "CControl.h"
#include "cMSstruct.h"
#include "CMaster.h"



//
//--- Constructor
//
CSurvey::CSurvey(CControl * in_parent)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	m_timeOut = 0;
	parent = in_parent;

	//--- Main frame to hold the survey
	pnl_main = new CControl(parent, CVector2i(0,0), CVector2i(800, 600), "", this, "SUPER_PANEL");
		pnl_main->borderColor.set(1,.5f,.25f);
		pnl_main->texture = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_NEAREST);

	pnl_sending = new CControl(parent, CVector2i(0,0), CVector2i(800, 600), "", this, "SUPER_PANEL");
		pnl_sending->borderColor.set(1,.5f,.25f);
		pnl_sending->texture = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_NEAREST);


	//--- Header
	CControl * control = new CControl(pnl_main, CVector2i(544 - 64, 16), CVector2i(256, 256), "", this, "FRAME");
		control->perfectFitTexture = true;
		control->texture = dktCreateTextureFromFile("main/textures/BerretGuy.tga", DKT_FILTER_NEAREST);
		control->frame2D = false;
		control->frame3D = false;
		control->noFrame = true;
		control->imgColor.set(1,1,1);
		control->backColor.set(1,1,1);
	control = new CControl(pnl_main, CVector2i(32,0),CVector2i(736,60),"BaboViolent2 Survey               ",this,"LABEL",0,CONTROL_SNAP_BOTTOM,15);
		control->textAlign = CONTROL_TEXTALIGN_TOPCENTER;
		control->textSize = 60;
	control = new CControl(pnl_main, CVector2i(32,0),CVector2i(736,330),
		"Help us chart the future of BaboViolent.\nPlease take two minutes to fill out our survey\nand you could win \x3HeadGames\x8 gear.\n\nPlease note that if it is the first time for you playing this game, you better click\n\'not now' and try it first :)",
		this,"LABEL",control,CONTROL_SNAP_BOTTOM,5);
		control->textSize = 30;
	btn_notNow = new CControl(pnl_main, CVector2i(32,0),CVector2i(128,25),"Not Now",this,"BUTTON",control,CONTROL_SNAP_BOTTOM,15);
	btn_never = new CControl(pnl_main, CVector2i(32,0),CVector2i(128,25),"Never",this,"BUTTON",btn_notNow,CONTROL_SNAP_RIGHT,15);

	control = new CControl(pnl_sending, CVector2i(32,300),CVector2i(736,200),"Sending info to master\n\x7This can take few seconds",this,"LABEL");
		control->textAlign = CONTROL_TEXTALIGN_TOPCENTER;
		control->textSize = 60;
	pnl_sending->visible = false;


	//------------------------------------------------------------------------------------------------
	//--- GENERAL GAMER PROFILE
	//------------------------------------------------------------------------------------------------
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 30), "General Gamer Profile",this,"SEPARATOR", btn_never, CONTROL_SNAP_BOTTOM, 15);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Gender", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 15);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_gender[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "Male", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_gender[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "Female", this, "CHECK", q_gender[0], CONTROL_SNAP_RIGHT, 5);
		q_gender[0]->registerRadio(q_gender[1]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_gender[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Age", this, "LABEL", q_gender[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_age[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "0-13", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_age[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "14-17", this, "CHECK", q_age[0], CONTROL_SNAP_RIGHT, 5);
	q_age[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "18-25", this, "CHECK", q_age[1], CONTROL_SNAP_RIGHT, 5);
	q_age[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "26+", this, "CHECK", q_age[2], CONTROL_SNAP_RIGHT, 5);
		q_age[0]->registerRadio(q_age[1]);
		q_age[0]->registerRadio(q_age[2]);
		q_age[0]->registerRadio(q_age[3]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_age[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Continent/Country", this, "LABEL", q_age[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_continent[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "North America", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_continent[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "South America", this, "CHECK", q_continent[0], CONTROL_SNAP_RIGHT, 5);
	q_continent[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "Africa", this, "CHECK", q_continent[1], CONTROL_SNAP_RIGHT, 5);
	q_continent[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "Europe", this, "CHECK", q_continent[0], CONTROL_SNAP_BOTTOM, 5);
	q_continent[4] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "Asia", this, "CHECK", q_continent[3], CONTROL_SNAP_RIGHT, 5);
	q_continent[5] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "Australia", this, "CHECK", q_continent[4], CONTROL_SNAP_RIGHT, 5);
		q_continent[0]->registerRadio(q_continent[1]);
		q_continent[0]->registerRadio(q_continent[2]);
		q_continent[0]->registerRadio(q_continent[3]);
		q_continent[0]->registerRadio(q_continent[4]);
		q_continent[0]->registerRadio(q_continent[5]);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Country", this, "LABEL", q_continent[3], CONTROL_SNAP_BOTTOM, 5);
	q_country[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", q_continent[4], CONTROL_SNAP_BOTTOM, 5);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", control, CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "What other game genre do you currently play the most?", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_gameGenre[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "FPS/Action", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_gameGenre[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "RPG/Adventure", this, "CHECK", q_gameGenre[0], CONTROL_SNAP_RIGHT, 5);
	q_gameGenre[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(120, 30), "RTS/Strategy", this, "CHECK", q_gameGenre[1], CONTROL_SNAP_RIGHT, 5);
		q_gameGenre[0]->registerRadio(q_gameGenre[1]);
		q_gameGenre[0]->registerRadio(q_gameGenre[2]);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Specify", this, "LABEL", q_gameGenre[0], CONTROL_SNAP_BOTTOM, 5);
	q_games[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", q_gameGenre[1], CONTROL_SNAP_BOTTOM, 5);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "About how many hours each week do you play games?", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_playingHours[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "0-2", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_playingHours[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "3-6", this, "CHECK", q_playingHours[0], CONTROL_SNAP_RIGHT, 5);
	q_playingHours[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "7-10", this, "CHECK", q_playingHours[1], CONTROL_SNAP_RIGHT, 5);
	q_playingHours[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "11-20", this, "CHECK", q_playingHours[2], CONTROL_SNAP_RIGHT, 5);
	q_playingHours[4] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "21+", this, "CHECK", q_playingHours[3], CONTROL_SNAP_RIGHT, 5);
		q_playingHours[0]->registerRadio(q_playingHours[1]);
		q_playingHours[0]->registerRadio(q_playingHours[2]);
		q_playingHours[0]->registerRadio(q_playingHours[3]);
		q_playingHours[0]->registerRadio(q_playingHours[4]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", control, CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "What are your favorite gaming magazines and/or web sites?", this, "LABEL", q_playingHours[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_magasine[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", control, CONTROL_SNAP_BOTTOM, 5);


//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_magasine[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Which game console(s) do you own? (Check all that apply)", this, "LABEL", q_magasine[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_console[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "XBox", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_console[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "XBox360", this, "CHECK", q_console[0], CONTROL_SNAP_RIGHT, 5);
	q_console[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Wii", this, "CHECK", q_console[1], CONTROL_SNAP_RIGHT, 5);
	q_console[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "GameCube", this, "CHECK", q_console[2], CONTROL_SNAP_RIGHT, 5);
	q_console[4] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Ps2", this, "CHECK", q_console[0], CONTROL_SNAP_BOTTOM, 5);
	q_console[5] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Ps3", this, "CHECK", q_console[4], CONTROL_SNAP_RIGHT, 5);
	q_console[6] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Other", this, "CHECK", q_console[5], CONTROL_SNAP_RIGHT, 5);



	//------------------------------------------------------------------------------------------------
	//--- YOUR BABOVIOLENT EXPERIENCE
	//------------------------------------------------------------------------------------------------
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 30), "Your BaboViolent Experience",this,"SEPARATOR", q_console[4], CONTROL_SNAP_BOTTOM, 30);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "About how many hours each week do you play BaboViolent?", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_baboPlayingHours[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "0-2", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_baboPlayingHours[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "3-7", this, "CHECK", q_baboPlayingHours[0], CONTROL_SNAP_RIGHT, 5);
	q_baboPlayingHours[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "8-12", this, "CHECK", q_baboPlayingHours[1], CONTROL_SNAP_RIGHT, 5);
	q_baboPlayingHours[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "13+", this, "CHECK", q_baboPlayingHours[2], CONTROL_SNAP_RIGHT, 5);
		q_baboPlayingHours[0]->registerRadio(q_baboPlayingHours[1]);
		q_baboPlayingHours[0]->registerRadio(q_baboPlayingHours[2]);
		q_baboPlayingHours[0]->registerRadio(q_baboPlayingHours[3]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "What kind of servers are most of your BaboViolent games played on?", this, "LABEL", q_baboPlayingHours[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_playingServer[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(150, 30), "Public (Internet)", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_playingServer[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(150, 30), "Private (Lan)", this, "CHECK", q_playingServer[0], CONTROL_SNAP_RIGHT, 5);
		q_playingServer[0]->registerRadio(q_playingServer[1]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", control, CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "How did you first learn about BaboViolent?", this, "LABEL", q_playingServer[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_learnedAboutBabo[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", control, CONTROL_SNAP_BOTTOM, 5);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Who do you play BaboViolent with? (Check all that apply)", this, "LABEL", q_learnedAboutBabo[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_whoYouPlayWith[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(400, 30), "I usually don't know the other players personally", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_whoYouPlayWith[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(400, 30), "I arrange for friends to play at the same time", this, "CHECK", q_whoYouPlayWith[0], CONTROL_SNAP_BOTTOM, 5);
	q_whoYouPlayWith[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(400, 30), "I play in teams/clans", this, "CHECK", q_whoYouPlayWith[1], CONTROL_SNAP_BOTTOM, 5);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "What do you like the most about BaboViolent? (Check all that apply)", this, "LABEL", q_whoYouPlayWith[2], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_whatYouLike[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "The top-down format is a nice change from most shooter games", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_whatYouLike[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "The continuous, fast action", this, "CHECK", q_whatYouLike[0], CONTROL_SNAP_BOTTOM, 5);
	q_whatYouLike[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "It's easy to jump in for a quick fix whenever I want", this, "CHECK", q_whatYouLike[1], CONTROL_SNAP_BOTTOM, 5);
	q_whatYouLike[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "The violence and blood", this, "CHECK", q_whatYouLike[2], CONTROL_SNAP_BOTTOM, 5);
	q_whatYouLike[4] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Other: ", this, "CHECK", q_whatYouLike[3], CONTROL_SNAP_BOTTOM, 5);
	q_youLikeOther[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", q_whatYouLike[4], CONTROL_SNAP_RIGHT, 5);


	//------------------------------------------------------------------------------------------------
	//--- IDEAS FOR THE FUTURE
	//------------------------------------------------------------------------------------------------
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 30), "Ideas for the Future (BaboViolent3, currently under development)",this,"SEPARATOR", q_whatYouLike[4], CONTROL_SNAP_BOTTOM, 30);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "What would you like to see in BaboViolent's future? (Check all that apply)", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_BVFuture[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "More opportunities for team play", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_BVFuture[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "More violence and blood", this, "CHECK", q_BVFuture[0], CONTROL_SNAP_BOTTOM, 5);
	q_BVFuture[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(500, 30), "More open source development of add-on weapons and levels (modding)", this, "CHECK", q_BVFuture[1], CONTROL_SNAP_BOTTOM, 5);
	q_BVFuture[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(100, 30), "Other: ", this, "CHECK", q_BVFuture[2], CONTROL_SNAP_BOTTOM, 5);
	q_futureOther[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", q_BVFuture[3], CONTROL_SNAP_RIGHT, 5);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Should in-game or pre-game ads be used to keep the game free?", this, "LABEL", q_BVFuture[3], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_Ads[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "Yes", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_Ads[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(50, 30), "No", this, "CHECK", q_Ads[0], CONTROL_SNAP_RIGHT, 5);
		q_Ads[1]->registerRadio(q_Ads[0]);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", q_games[0], CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Would you be willing to pay to play ad-free? (For a professional version)", this, "LABEL", q_Ads[0], CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_payAdFree[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "No", this, "CHECK", control, CONTROL_SNAP_BOTTOM, 5);
	q_payAdFree[1] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "Yes $5", this, "CHECK", q_payAdFree[0], CONTROL_SNAP_RIGHT, 5);
	q_payAdFree[2] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "Yes $10", this, "CHECK", q_payAdFree[1], CONTROL_SNAP_RIGHT, 5);
	q_payAdFree[3] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(75, 30), "Yes $20", this, "CHECK", q_payAdFree[2], CONTROL_SNAP_RIGHT, 5);
		q_payAdFree[0]->registerRadio(q_payAdFree[1]);
		q_payAdFree[0]->registerRadio(q_payAdFree[2]);
		q_payAdFree[0]->registerRadio(q_payAdFree[3]);


	//------------------------------------------------------------------------------------------------
	//--- SUBMISSION
	//------------------------------------------------------------------------------------------------
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 30), "Submission",this,"SEPARATOR", q_payAdFree[0], CONTROL_SNAP_BOTTOM, 30);

//	new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 3), "",this,"SEPARATOR_THIN", control, CONTROL_SNAP_BOTTOM, 7);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "Sign up for BaboViolent news by email*", this, "LABEL", control, CONTROL_SNAP_BOTTOM, 30);
		control->textSize = 30; control->foreColor.set(1, 1, .5f);
	q_email[0] = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 30), "", this, "EDIT", control, CONTROL_SNAP_BOTTOM, 5);
	btn_submit = new CControl(pnl_main, CVector2i(32,0),CVector2i(128,25),"Submit survey",this,"BUTTON",q_email[0],CONTROL_SNAP_RIGHT,15);
	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(600, 80), "*This information is required to get a chance to win a prize.\nWe respect your privacy and will never spam you.", this, "LABEL", q_email[0], CONTROL_SNAP_BOTTOM, 15);

	control = new CControl(pnl_main, CVector2i(32, 0), CVector2i(32, 30), "",this,"SEPARATOR", control, CONTROL_SNAP_BOTTOM, 30);

	m_isDone = false;

	//--- Set main frame background color
	pnl_main->backColor.set(.1f, .1f, .2f);
	pnl_sending->backColor.set(.1f, .1f, .2f);
}



//
//--- Destructor
//
CSurvey::~CSurvey()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}



//
//--- Events
//
void CSurvey::MouseEnter(CControl * control)
{
	if (control->style == "EDIT" || control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CSurvey::MouseLeave(CControl * control)
{
}
void CSurvey::MouseDown(CControl * control)
{
}
void CSurvey::MouseUp(CControl * control)
{
}
void CSurvey::MouseMove(CControl * control)
{
}
void CSurvey::Click(CControl * control)
{
	if (control->style == "EDIT" || control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_notNow)
	{
		m_isDone = true;
	}
	if (control == btn_never)
	{
		//--- Dont bug me again
		if (master)
		{
			master->sendPacket(0, 0, SURVEY_NEVER);
		}
		pnl_main->visible = false;
		pnl_sending->visible = true;
	}
	if (control == btn_submit)
	{
		SSurvey survey;
		memset(&survey, 0, sizeof(SSurvey));

		if (q_gender[0]->check) survey.gender = 0;
		if (q_gender[1]->check) survey.gender = 1;
		if (q_age[0]->check) survey.age = 0;
		if (q_age[1]->check) survey.age = 1;
		if (q_age[2]->check) survey.age = 2;
		if (q_age[3]->check) survey.age = 3;
		if (q_continent[0]->check) survey.continent = 0;
		if (q_continent[1]->check) survey.continent = 1;
		if (q_continent[2]->check) survey.continent = 2;
		if (q_continent[3]->check) survey.continent = 3;
		if (q_continent[4]->check) survey.continent = 4;
		if (q_continent[5]->check) survey.continent = 5;
		memcpy(survey.country, q_country[0]->text.s, 80);
		if (q_gameGenre[0]->check) survey.gameGenre = 0;
		if (q_gameGenre[1]->check) survey.gameGenre = 1;
		if (q_gameGenre[2]->check) survey.gameGenre = 2;
		memcpy(survey.specifiedGenre, q_games[0]->text.s, 80);
		if (q_playingHours[0]->check) survey.hourPlayerGames = 0;
		if (q_playingHours[1]->check) survey.hourPlayerGames = 1;
		if (q_playingHours[2]->check) survey.hourPlayerGames = 2;
		if (q_playingHours[3]->check) survey.hourPlayerGames = 3;
		if (q_playingHours[4]->check) survey.hourPlayerGames = 4;
		memcpy(survey.magazine, q_magasine[0]->text.s, 80);
		if (q_console[0]->check) survey.console |= 0x01;
		if (q_console[1]->check) survey.console |= 0x02;
		if (q_console[2]->check) survey.console |= 0x04;
		if (q_console[3]->check) survey.console |= 0x08;
		if (q_console[4]->check) survey.console |= 0x10;
		if (q_console[5]->check) survey.console |= 0x20;
		if (q_console[6]->check) survey.console |= 0x40;

		if (q_baboPlayingHours[0]->check) survey.hourPlayingBV2 = 0;
		if (q_baboPlayingHours[1]->check) survey.hourPlayingBV2 = 1;
		if (q_baboPlayingHours[2]->check) survey.hourPlayingBV2 = 2;
		if (q_baboPlayingHours[3]->check) survey.hourPlayingBV2 = 3;
		if (q_playingServer[0]->check) survey.kindOfServer = 0;
		if (q_playingServer[1]->check) survey.kindOfServer = 1;
		memcpy(survey.howYouLearnAboutBabo, q_learnedAboutBabo[0]->text.s, 80);
		if (q_whoYouPlayWith[0]->check) survey.whoDoYouPlayWith |= 0x01;
		if (q_whoYouPlayWith[1]->check) survey.whoDoYouPlayWith |= 0x02;
		if (q_whoYouPlayWith[2]->check) survey.whoDoYouPlayWith |= 0x04;
		if (q_whatYouLike[0]->check) survey.whatDoYouLike |= 0x01;
		if (q_whatYouLike[1]->check) survey.whatDoYouLike |= 0x02;
		if (q_whatYouLike[2]->check) survey.whatDoYouLike |= 0x04;
		if (q_whatYouLike[3]->check) survey.whatDoYouLike |= 0x08;
		if (q_whatYouLike[4]->check) survey.whatDoYouLike |= 0x10;
		memcpy(survey.whatDoYouLikeOther, q_youLikeOther[0]->text.s, 80);

		if (q_BVFuture[0]->check) survey.whatYouWouldLikeForBV3 |= 0x01;
		if (q_BVFuture[1]->check) survey.whatYouWouldLikeForBV3 |= 0x02;
		if (q_BVFuture[2]->check) survey.whatYouWouldLikeForBV3 |= 0x04;
		if (q_BVFuture[3]->check) survey.whatYouWouldLikeForBV3 |= 0x08;
		memcpy(survey.whatYouWouldLikeForBV3Other, q_futureOther[0]->text.s, 80);
		if (q_Ads[0]->check) survey.adsAndFree = 0;
		if (q_Ads[1]->check) survey.adsAndFree = 1;
		if (q_payAdFree[0]->check) survey.payNoAds = 0;
		if (q_payAdFree[1]->check) survey.payNoAds = 1;
		if (q_payAdFree[2]->check) survey.payNoAds = 2;
		if (q_payAdFree[3]->check) survey.payNoAds = 3;

		memcpy(survey.email, q_email[0]->text.s, 80);

		//--- Send that to the master server
		if (master)
		{
			master->sendPacket((char*)(&survey), sizeof(SSurvey), SURVEY_SUBMIT);
		}

		pnl_main->visible = false;
		pnl_sending->visible = true;
	}
}



//
//--- Update wait for master confirmation
//
void CSurvey::update(float delay)
{
	if (surveyReceived && !m_isDone)
	{
		//--- Done
		master->disconnectMaster();
		m_isDone = true;
	}

	if (!pnl_main->visible)
	{
		m_timeOut += delay;
		if (m_timeOut > 10 && !m_isDone)
		{
			//--- Screw this
			master->disconnectMaster();
			m_isDone = true;
		}
	}
}
#endif
