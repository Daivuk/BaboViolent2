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
#include "Game.h"
#include "Helper.h"


void renderStatsSlice(const CVector4f & sliceColor, char * text1, char* c1, char* c2, char* c3, char* c4, char* c5, char * pingText, int & vPos)
{
#ifndef _DX_
	glDisable(GL_TEXTURE_2D);
	glColor4fv(sliceColor.s);
	glBegin(GL_QUADS);
		glVertex2i(150, vPos);
		glVertex2i(150, vPos+22);
		glVertex2i(450, vPos+22);
		glVertex2i(450, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(452, vPos);
		glVertex2i(452, vPos+22);
		glVertex2i(500, vPos+22);
		glVertex2i(500, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(502, vPos);
		glVertex2i(502, vPos+22);
		glVertex2i(550, vPos+22);
		glVertex2i(550, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(552, vPos);
		glVertex2i(552, vPos+22);
		glVertex2i(600, vPos+22);
		glVertex2i(600, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(602, vPos);
		glVertex2i(602, vPos+22);
		glVertex2i(650, vPos+22);
		glVertex2i(650, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(652, vPos);
		glVertex2i(652, vPos+22);
		glVertex2i(700, vPos+22);
		glVertex2i(700, vPos);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2i(702, vPos);
		glVertex2i(702, vPos+22);
		glVertex2i(750, vPos+22);
		glVertex2i(750, vPos);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	glColor3f(1,1,1);
#endif
	printLeftText(154, (float)vPos-2, 28, CString(text1));
	printRightText(500, (float)vPos-2, 28, CString(c1));
	printRightText(550, (float)vPos-2, 28, CString(c2));
	printRightText(600, (float)vPos-2, 28, CString(c3));
	printRightText(650, (float)vPos-2, 28, CString(c4));
	printRightText(700, (float)vPos-2, 28, CString(c5));
	printRightText(750, (float)vPos-2, 28, CString(pingText));
	// Ping
	vPos += 26;
}



void Game::renderBlueTeam(std::vector<Player*> & blueTeam, int & vPos)
{
	// Blue Team
	renderStatsSlice(CVector4f(0, 0, 1, .75f), gameVar.lang_blueTeamC.s, "","","","",CString("%i", blueScore).s, CString(""/*%i", bluePing*33*/).s, vPos);
	for (int j=0;j<(int)blueTeam.size();++j)
	{
		CString showName = blueTeam[j]->name;
		showName.insert("\x1", 0);
		if (blueTeam[j]->status == PLAYER_STATUS_DEAD) showName.insert(CString("(%s) ", gameVar.lang_dead.s).s, 0);

		CString pingStr;
		if (blueTeam[j]->ping*33 < 100) /*"Good"*/
			pingStr = CString("\x2") + blueTeam[j]->ping*33; 
		else if (blueTeam[j]->ping*33 < 200) /*"Average"*/
			pingStr = CString("\x9") + blueTeam[j]->ping*33; 
		else if (blueTeam[j]->ping*33 < 999) /*"Bad"*/
			pingStr = CString("\x4") + blueTeam[j]->ping*33;
		else
			pingStr = "\x5???";

		renderStatsSlice(	CVector4f(0,0,0,.75f), showName.s,
							CString("%i",(int)blueTeam[j]->kills).s,
							CString("%i",(int)blueTeam[j]->deaths).s,
							CString("%.1f",blueTeam[j]->dmg).s,
							CString("%i",(int)blueTeam[j]->returns).s,
							CString("%i",(int)blueTeam[j]->score).s,
							pingStr.s, vPos);

	}
	vPos += 10;
}

void Game::renderRedTeam(std::vector<Player*> & redTeam, int & vPos)
{
	// Red Team
	renderStatsSlice(CVector4f(1, 0, 0, .75f), gameVar.lang_redTeamC.s,"","","","", CString("%i", redScore).s, CString(""/*%i", redPing*33*/).s, vPos);
	for (int j=0;j<(int)redTeam.size();++j)
	{
		CString showName = redTeam[j]->name;
		showName.insert("\x4", 0);
		if (redTeam[j]->status == PLAYER_STATUS_DEAD) showName.insert(CString("(%s) ", gameVar.lang_dead.s).s, 0);

		CString pingStr;
		if (redTeam[j]->ping*33 < 100) /*"Good"*/
			pingStr = CString("\x2") + redTeam[j]->ping*33; 
		else if (redTeam[j]->ping*33 < 200) /*"Average"*/
			pingStr = CString("\x9") + redTeam[j]->ping*33; 
		else if (redTeam[j]->ping*33 < 999) /*"Bad"*/
			pingStr = CString("\x4") + redTeam[j]->ping*33;
		else
			pingStr = "\x5???";

		renderStatsSlice(	CVector4f(0,0,0,.75f), showName.s,
							CString("%i",(int)redTeam[j]->kills).s,
							CString("%i",(int)redTeam[j]->deaths).s,
							CString("%.1f",redTeam[j]->dmg).s,
							CString("%i",(int)redTeam[j]->returns).s,
							CString("%i",(int)redTeam[j]->score).s,
							pingStr.s, vPos);
	}
	vPos += 10;
}

void Game::renderFFA(std::vector<Player*> & ffaTeam, int & vPos)
{
	// All Team
	renderStatsSlice(CVector4f(1, 1, 1, .75f), CString(gameVar.lang_freeForAllC.s, redWin+blueWin).s,"","","","", CString(""/*%i", blueScore + redScore*/).s, CString("%i", ffaPing*33).s, vPos);
	for (int j=0;j<(int)ffaTeam.size();++j)
	{
		CString showName = ffaTeam[j]->name;
		showName.insert("\x8", 0);
		if (ffaTeam[j]->status == PLAYER_STATUS_DEAD) showName.insert((CString("(") + gameVar.lang_dead + ") ").s, 0);
		CString pingStr;
		if (ffaTeam[j]->ping*33 < 100) /*"Good"*/
			pingStr = CString("\x2") + ffaTeam[j]->ping*33; 
		else if (ffaTeam[j]->ping*33 < 200) /*"Average"*/
			pingStr = CString("\x9") + ffaTeam[j]->ping*33; 
		else if (ffaTeam[j]->ping*33 < 999) /*"Bad"*/
			pingStr = CString("\x4") + ffaTeam[j]->ping*33;
		else
			pingStr = "\x5???";

		renderStatsSlice(CVector4f(0,0,0,.75f), showName.s,CString("%i",(int)ffaTeam[j]->score).s, CString("%i", (int)ffaTeam[j]->deaths).s, CString("%.1f", ffaTeam[j]->dmg).s,"","", pingStr.s, vPos);


	}
	vPos += 10;
}

void Game::renderSpectator(std::vector<Player*> & spectatorTeam, int & vPos)
{
	// Spectators
	renderStatsSlice(CVector4f(.5f, .5f, .5f, .75f), gameVar.lang_spectatorC.s, "","","","","", CString(""/*%i", spectatorPing*33*/).s, vPos);
	for (int j=0;j<(int)spectatorTeam.size();++j)
	{
		CString showName = spectatorTeam[j]->name;
		showName.insert("\x8", 0);
		CString pingStr;
		if (spectatorTeam[j]->ping*33 < 100) /*"Good"*/
			pingStr = CString("\x2") + spectatorTeam[j]->ping*33; 
		else if (spectatorTeam[j]->ping*33 < 200) /*"Average"*/
			pingStr = CString("\x9") + spectatorTeam[j]->ping*33; 
		else if (spectatorTeam[j]->ping*33 < 999) /*"Bad"*/
			pingStr = CString("\x4") + spectatorTeam[j]->ping*33;
		else
			pingStr = "\x5???";

		renderStatsSlice(	CVector4f(0,0,0,.75f), showName.s,
							CString("%i",(int)spectatorTeam[j]->kills).s,
							CString("%i",(int)spectatorTeam[j]->deaths).s,
							CString("%.1f",spectatorTeam[j]->dmg).s,
							CString("%i",(int)spectatorTeam[j]->returns).s,
							CString("%i",(int)spectatorTeam[j]->score).s,
							pingStr.s, vPos);
	}
}


//
// Afficher les stats
//
void Game::renderStats()
{
	CVector2i res(800,600);// = dkwGetResolution();
#ifndef _DX_
	dkglPushOrtho((float)res[0], (float)res[1]);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex2f(0,0);
				glColor4f(0,0,0,0);
				glVertex2f(0,300);
				glVertex2f(800,300);
				glColor4f(0,0,0,1);
				glVertex2f(800,0);
			glEnd();
			glBegin(GL_QUADS);
				glColor4f(0,0,0,0);
				glVertex2f(0,300);
				glColor4f(0,0,0,1);
				glVertex2f(0,600);
				glVertex2f(800,600);
				glColor4f(0,0,0,0);
				glVertex2f(800,300);
			glEnd();
			glEnable(GL_TEXTURE_2D);
#endif

			// On construit la blue team vector et la red team vector puis on tri
			std::vector<Player*> blueTeam;
			std::vector<Player*> redTeam;
			std::vector<Player*> ffaTeam;
			std::vector<Player*> spectatorTeam;
			spectatorPing=0;
			bluePing = 0;
			redPing = 0;
			ffaPing = 0;
			int j;
			for (int i=0;i<MAX_PLAYER;++i)
			{
				if (players[i])
				{
					switch (players[i]->teamID)
					{
					case PLAYER_TEAM_BLUE:
						for (j=0;j<(int)blueTeam.size();++j)
						{
							if (players[i]->kills > blueTeam[j]->kills) break;
						}
						bluePing+=players[i]->ping;
						blueTeam.insert(blueTeam.begin()+j, players[i]);
						break;
					case PLAYER_TEAM_RED:
						for (j=0;j<(int)redTeam.size();++j)
						{
							if (players[i]->kills > redTeam[j]->kills) break;
						}
						redPing+=players[i]->ping;
						redTeam.insert(redTeam.begin()+j, players[i]);
						break;
					case PLAYER_TEAM_SPECTATOR:
						for (j=0;j<(int)spectatorTeam.size();++j)
						{
							if (players[i]->kills > spectatorTeam[j]->kills) break;
						}
						spectatorPing+=players[i]->ping;
						spectatorTeam.insert(spectatorTeam.begin()+j, players[i]);
						break;
					}

					// On les class tous dans le ffa (sauf spectator)
					if (players[i]->teamID != PLAYER_TEAM_SPECTATOR)
					{
						for (j=0;j<(int)ffaTeam.size();++j)
						{
							if (players[i]->score > ffaTeam[j]->score) break;
						}
						ffaPing+=players[i]->ping;
						ffaTeam.insert(ffaTeam.begin()+j, players[i]);
					}
				}
			}

			if (blueTeam.size() > 0) bluePing /= (int)blueTeam.size();
			if (redTeam.size() > 0) redPing /= (int)redTeam.size();
			if (spectatorTeam.size() > 0) spectatorPing /= (int)spectatorTeam.size();
			if (ffaTeam.size() > 0) ffaPing /= (int)ffaTeam.size();

			// Temporairement juste la liste des joueurs pas trié là pis toute
			dkfBindFont(font);
			int vPos = 50;

			// Title [FIX]: Does not use language file


			switch (gameType)
			{
         case GAME_TYPE_SND:
			case GAME_TYPE_DM:
				renderStatsSlice(CVector4f(0, 0, 0, .75f), gameVar.lang_playerNameC.s, "Kills", "Death", "Damage", "", "", gameVar.lang_pingC.s, vPos);
				vPos += 10;
				renderFFA(ffaTeam, vPos);
				renderSpectator(spectatorTeam, vPos);
				break;
			case GAME_TYPE_TDM:
				renderStatsSlice(CVector4f(0, 0, 0, .75f), gameVar.lang_playerNameC.s, "Kills", "Death", "Damage", "","", gameVar.lang_pingC.s, vPos);
				vPos += 10;
				if (blueScore >= redScore) 
				{
					renderBlueTeam(blueTeam, vPos);
					renderRedTeam(redTeam, vPos);
				}
				else
				{
					renderRedTeam(redTeam, vPos);
					renderBlueTeam(blueTeam, vPos);
				}
				renderSpectator(spectatorTeam, vPos);
				break;
			case GAME_TYPE_CTF:			
				renderStatsSlice(CVector4f(0, 0, 0, .75f), gameVar.lang_playerNameC.s, "Kills", "Death", "Damage", "Retrn", "Caps", gameVar.lang_pingC.s, vPos);
				vPos += 10;
				if (blueWin >= redWin) 
				{
					renderBlueTeam(blueTeam, vPos);
					renderRedTeam(redTeam, vPos);
				}
				else
				{
					renderRedTeam(redTeam, vPos);
					renderBlueTeam(blueTeam, vPos);
				}
				renderSpectator(spectatorTeam, vPos);
				break;
			}

			blueTeam.clear();
			redTeam.clear();
			spectatorTeam.clear();
			ffaTeam.clear();
#ifndef _DX_
		glPopAttrib();
	dkglPopOrtho();
#endif
}
#endif


