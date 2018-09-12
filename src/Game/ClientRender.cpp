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
#include "Client.h"
#include "KeyManager.h"
#include "Console.h"
#include "CControl.h"
#include "CMenuManager.h"


#ifdef RENDER_LAYER_TOGGLE
	extern int renderToggle;
#endif



//
// Pour l'afficher
//
void Client::render(float & alphaScope)
{
  int i;
	CVector2i res = dkwGetResolution();

	if (game->thisPlayer)
	{
		if (game->thisPlayer->status == PLAYER_STATUS_ALIVE)
		{
			if (game->map)
			{
				game->map->camLookAt = (
					game->thisPlayer->currentCF.position*5 + 
					game->thisPlayer->currentCF.mousePosOnMap*4) / 9.0f;
				if (game->map->camLookAt[0] < 0) game->map->camLookAt[0] = 0;
				if (game->map->camLookAt[1] < -1) game->map->camLookAt[1] = -1;
				if (game->map->camLookAt[0] > (float)game->map->size[0]) game->map->camLookAt[0] = (float)game->map->size[0];
				if (game->map->camLookAt[1] > (float)game->map->size[1]+1) game->map->camLookAt[1] = (float)game->map->size[1]+1;
			}
		}
	}

	// C'est côté client qu'on fait ça ;)
	if (isConnected) game->render();

	// LE SNIPER SCOPE
	CVector2i cursor = dkwGetCursorPos_main();
	int xM = (int)(((float)cursor[0]/(float)res[0])*800.0f);
	int yM = (int)(((float)cursor[1]/(float)res[1])*600.0f);
#ifndef _DX_
	dkglPushOrtho(800,600);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//--- Sniper scope!!
			if (game->thisPlayer && game->thisPlayer->status == PLAYER_STATUS_ALIVE)
			{
				if (game->thisPlayer->weapon && game->thisPlayer->weapon->weaponID == WEAPON_SNIPER)
				{
					if (game->map && game->map->camPos[2] > 8)
					{
						alphaScope = 10 - (game->map->camPos[2]-2);
						alphaScope = (alphaScope > 0)? 1-(alphaScope/2) : 1;
						glColor4f(0,0,0,alphaScope);
						if (!(menuManager.root && menuManager.root->visible) && !showMenu)
						{
							// render the scope view
							renderTexturedQuad(xM-128,yM-128,256,256,gameVar.tex_sniperScope);
							renderTexturedQuad(0,0,800,yM-128,0);
							renderTexturedQuad(0,yM+128,800,600-(yM+128),0);
							renderTexturedQuad(0,yM-128,xM-128,256,0);
							renderTexturedQuad(xM+128,yM-128,800-(xM+128),256,0);
						}
						else
						{
							// render black background in the menu
							renderTexturedQuad(0,0,800,600,0);
						}
					}
				}
			}

		glPopAttrib();
	dkglPopOrtho();
#endif

	// Si on doit spawner on marque dans combient de temps
	if (game->thisPlayer)
	{
#ifndef _DX_
		dkglPushOrtho(800, 600);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				// Si on c faite tirer dessus
				if (game->thisPlayer->screenHit > 0)
				{
					glColor4f(1,1,1,game->thisPlayer->screenHit*3);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, tex_screenHit);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex2i(0,0);
						glTexCoord2i(0,0);
						glVertex2i(0,600);
						glTexCoord2i(1,0);
						glVertex2i(800,600);
						glTexCoord2i(1,1);
						glVertex2i(800,0);
					glEnd();
				}

				#ifdef RENDER_LAYER_TOGGLE
					if (renderToggle >= 16)
				#endif
				if (game->thisPlayer->status == PLAYER_STATUS_ALIVE)
				{
					// Quand on reload le gun il faut le marquer
					if (game->thisPlayer->weapon)
					{
						if (game->thisPlayer->weapon->currentFireDelay > 0 && game->thisPlayer->weapon->fireDelay >= 1.0f)
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1,1,1,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-5, 440-5);
								glVertex2f(400-100-5, 454+5);
								glVertex2f(400-100+200+10, 454+5);
								glVertex2f(400-100+200+10, 440-5);
							glEnd();
							glColor4f(0,0,0,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-1, 440-1);
								glVertex2f(400-100-1, 454+1);
								glVertex2f(400-100+200+2, 454+1);
								glVertex2f(400-100+200+2, 440-1);
							glEnd();
							glEnable(GL_TEXTURE_2D);
							glColor4f(.5f,1,.5f,game->thisPlayer->weapon->currentFireDelay/game->thisPlayer->weapon->fireDelay*.75f+.25f);
							if (blink < .25f) printCenterText(400,400,32,gameVar.lang_reloading);
							glDisable(GL_TEXTURE_2D);
							// La progress bar
							// La progress bar
							glBegin(GL_QUADS);
								glVertex2f(400-100, 440);
								glVertex2f(400-100, 454);
								glVertex2f(400-100+(1-game->thisPlayer->weapon->currentFireDelay/game->thisPlayer->weapon->fireDelay)*200, 454);
								glVertex2f(400-100+(1-game->thisPlayer->weapon->currentFireDelay/game->thisPlayer->weapon->fireDelay)*200, 440);
							glEnd();
						}
						else if (game->thisPlayer->grenadeDelay > 0)
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1,1,1,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-5, 440-5);
								glVertex2f(400-100-5, 454+5);
								glVertex2f(400-100+200+10, 454+5);
								glVertex2f(400-100+200+10, 440-5);
							glEnd();
							glColor4f(0,0,0,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-1, 440-1);
								glVertex2f(400-100-1, 454+1);
								glVertex2f(400-100+200+2, 454+1);
								glVertex2f(400-100+200+2, 440-1);
							glEnd();
							glEnable(GL_TEXTURE_2D);
							glColor4f(.5f,1,.5f,game->thisPlayer->grenadeDelay/gameVar.weapons[WEAPON_GRENADE]->fireDelay*.75f+.25f);
							if (blink < .25f) printCenterText(400,400,32,gameVar.lang_reloading);
							glDisable(GL_TEXTURE_2D);
							// La progress bar
							glBegin(GL_QUADS);
								glVertex2f(400-100, 440);
								glVertex2f(400-100, 454);
								glVertex2f(400-100+(1-game->thisPlayer->grenadeDelay/gameVar.weapons[WEAPON_GRENADE]->fireDelay)*200, 454);
								glVertex2f(400-100+(1-game->thisPlayer->grenadeDelay/gameVar.weapons[WEAPON_GRENADE]->fireDelay)*200, 440);
							glEnd();
						}
						else if (game->thisPlayer->weapon->weaponID == WEAPON_SHOTGUN && game->thisPlayer->weapon->currentFireDelay > 0)
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1,1,1,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-5, 440-5);
								glVertex2f(400-100-5, 454+5);
								glVertex2f(400-100+200+10, 454+5);
								glVertex2f(400-100+200+10, 440-5);
							glEnd();
							glColor4f(0,0,0,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-1, 440-1);
								glVertex2f(400-100-1, 454+1);
								glVertex2f(400-100+200+2, 454+1);
								glVertex2f(400-100+200+2, 440-1);
							glEnd();
							glEnable(GL_TEXTURE_2D);
							glColor4f(.5f,1,.5f,game->thisPlayer->weapon->currentFireDelay/game->thisPlayer->weapon->fireDelay*.75f+.25f);
							if (blink < .25f) printCenterText(400,400,32,gameVar.lang_reloading);
							glDisable(GL_TEXTURE_2D);
							// La progress bar
							glBegin(GL_QUADS);
								glVertex2f(400-100, 440);
								glVertex2f(400-100, 454);
								glVertex2f(400-100+(1-game->thisPlayer->weapon->currentFireDelay/3)*200, 454);
								glVertex2f(400-100+(1-game->thisPlayer->weapon->currentFireDelay/3)*200, 440);
							glEnd();
						}
						else if (game->thisPlayer->meleeDelay > 0)
						{
							glDisable(GL_TEXTURE_2D);
							glColor4f(1,1,1,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-5, 440-5);
								glVertex2f(400-100-5, 454+5);
								glVertex2f(400-100+200+10, 454+5);
								glVertex2f(400-100+200+10, 440-5);
							glEnd();
							glColor4f(0,0,0,.5f);
							glBegin(GL_QUADS);
								glVertex2f(400-100-1, 440-1);
								glVertex2f(400-100-1, 454+1);
								glVertex2f(400-100+200+2, 454+1);
								glVertex2f(400-100+200+2, 440-1);
							glEnd();
							glEnable(GL_TEXTURE_2D);
							glColor4f(.5f,1,.5f,game->thisPlayer->meleeDelay/game->thisPlayer->meleeWeapon->fireDelay*.75f+.25f);
							if (blink < .25f) printCenterText(400,400,32,gameVar.lang_reloading);
							glDisable(GL_TEXTURE_2D);
							// La progress bar
							glBegin(GL_QUADS);
								glVertex2f(400-100, 440);
								glVertex2f(400-100, 454);
								glVertex2f(400-100+(1-game->thisPlayer->meleeDelay/game->thisPlayer->meleeWeapon->fireDelay)*200, 454);
								glVertex2f(400-100+(1-game->thisPlayer->meleeDelay/game->thisPlayer->meleeWeapon->fireDelay)*200, 440);
							glEnd();
						}
					}

					glDisable(GL_TEXTURE_2D);

					// On affiche sa vie à droite
					glBegin(GL_QUADS);
						glColor3f(1,1,1);
						glVertex2f(760, 390);
						glVertex2f(760, 589);
						glVertex2f(789, 589);
						glVertex2f(789, 390);
						glColor3f(0,0,0);
						glVertex2f(762, 392);
						glVertex2f(762, 587);
						glVertex2f(787, 587);
						glVertex2f(787, 392);
						// La couleur celon sa vie
						if (game->thisPlayer->life > .25f || blink < .25f)
						{
							glColor3f(1-game->thisPlayer->life,game->thisPlayer->life,0);
							glVertex2f(764, 585-game->thisPlayer->life*191);
							glVertex2f(764, 585);
							glVertex2f(785, 585);
							glVertex2f(785, 585-game->thisPlayer->life*191);
						}
					glEnd();

					// Le heat du gun (ChainGun | FlameThrower)
					if (game->thisPlayer->weapon)
					{
						if (game->thisPlayer->weapon->weaponID == WEAPON_CHAIN_GUN)
						{
							glBegin(GL_QUADS);
								glColor4f(1,1,1, 1-game->thisPlayer->weapon->chainOverHeat*.5f);
								glVertex2f(760, 390 - 200);
								glVertex2f(760, 589 - 200);
								glVertex2f(789, 589 - 200);
								glVertex2f(789, 390 - 200);
								glColor4f(0,0,0, 1-game->thisPlayer->weapon->chainOverHeat*.5f);
								glVertex2f(762, 392 - 200);
								glVertex2f(762, 587 - 200);
								glVertex2f(787, 587 - 200);
								glVertex2f(787, 392 - 200);
								// La couleur celon sa vie
								if ((game->thisPlayer->weapon->overHeated && blink < .25f) || !game->thisPlayer->weapon->overHeated)
								{
									glColor4f(1-game->thisPlayer->weapon->chainOverHeat,game->thisPlayer->weapon->chainOverHeat,game->thisPlayer->weapon->chainOverHeat, 1-game->thisPlayer->weapon->chainOverHeat*.5f);
									glVertex2f(764, 585 - 200-game->thisPlayer->weapon->chainOverHeat*191);
									glVertex2f(764, 585 - 200);
									glVertex2f(785, 585 - 200);
									glVertex2f(785, 585 - 200-game->thisPlayer->weapon->chainOverHeat*191);
								}
							glEnd();
						}
					}

					// Le nb de grenade quil lui reste
					if (game->thisPlayer->nbGrenadeLeft > 0)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, tex_grenadeLeft);
						glPushMatrix();
							glTranslatef(686+32, 526+32, 0);
							if (game->thisPlayer->lastShootWasNade)
							{
								glScalef(32+game->thisPlayer->grenadeDelay*16,32+game->thisPlayer->grenadeDelay*16,0);
							}
							else
							{
								glScalef(32,32,0);
							}
							glBegin(GL_QUADS);
								glColor3f(1,1,1);
								glTexCoord2f(0,1);
								glVertex2f(-1, -1);
								glTexCoord2f(0,0);
								glVertex2f(-1, 1);
								glTexCoord2f(1,0);
								glVertex2f(1, 1);
								glTexCoord2f(1,1);
								glVertex2f(1, -1);
							glEnd();
							glPopMatrix();
						printCenterText(686+32, 526+32-16, 32, CString("%i", game->thisPlayer->nbGrenadeLeft));
					}

					// Le nb de molotov quil lui reste
					if (game->thisPlayer->nbMolotovLeft > 0 && gameVar.sv_enableMolotov)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, tex_molotovLeft);
						glPushMatrix();
							glTranslatef(686+32, 474+32, 0);
							if (!game->thisPlayer->lastShootWasNade)
							{
								glScalef(32+game->thisPlayer->grenadeDelay*16,32+game->thisPlayer->grenadeDelay*16,0);
							}
							else
							{
								glScalef(32,32,0);
							}
							glBegin(GL_QUADS);
								glColor3f(1,1,1);
								glTexCoord2f(0,1);
								glVertex2f(-1, -1);
								glTexCoord2f(0,0);
								glVertex2f(-1, 1);
								glTexCoord2f(1,0);
								glVertex2f(1, 1);
								glTexCoord2f(1,1);
								glVertex2f(1, -1);
							glEnd();
							glPopMatrix();
						printCenterText(686+32, 474+32-16, 32, CString("%i", game->thisPlayer->nbMolotovLeft));
					}

					// Le nb de balle de shotgun quil lui reste
					if ( (game->thisPlayer->weapon->weaponID == WEAPON_SHOTGUN) &&
						(gameVar.sv_enableShotgunReload == true) )
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, tex_shotgunLeft);
						glPushMatrix();
							glTranslatef(686+32, 422+32, 0);
							glScalef(32+game->thisPlayer->weapon->currentFireDelay*16,32+game->thisPlayer->weapon->currentFireDelay*16,0);
							glBegin(GL_QUADS);
								glColor3f(1,1,1);
								glTexCoord2f(0,1);
								glVertex2f(-1, -1);
								glTexCoord2f(0,0);
								glVertex2f(-1, 1);
								glTexCoord2f(1,0);
								glVertex2f(1, 1);
								glTexCoord2f(1,1);
								glVertex2f(1, -1);
							glEnd();
							glPopMatrix();
						printCenterText(686+32, 422+32-16, 32, CString("%i", 6 - game->thisPlayer->weapon->shotInc));
					}
				}
				else if ((
					game->thisPlayer->teamID == PLAYER_TEAM_BLUE || 
					game->thisPlayer->teamID == PLAYER_TEAM_RED) &&
					game->thisPlayer->status == PLAYER_STATUS_DEAD  && !game->thisPlayer->spawnRequested)
				{
					glColor3f(1,1,1);
					if (game->thisPlayer->timeToSpawn > 0) printCenterText(400,200,64,CString(gameVar.lang_spawnIn.s, ((int)game->thisPlayer->timeToSpawn+1)/60, ((int)(game->thisPlayer->timeToSpawn+1)%60)));
					else if (!gameVar.sv_forceRespawn) printCenterText(400,200,64,CString("Press shoot key [%s] to respawn", keyManager.getKeyName(gameVar.k_shoot).s));
				}

				//--- Auto balance
				if (gameVar.sv_autoBalance && autoBalanceTimer > 0 && blink < .25f)
				{
					glColor3f(1,1,1);
					printCenterText(400,0,64,CString("Autobalance in %i seconds", (int)autoBalanceTimer));
				}

				/*printCenterText(200,100,20,CString("Time played: %.2f", game->thisPlayer->timePlayedCurGame));
				printCenterText(200,124,20,CString("Flag attempts: %d", game->thisPlayer->flagAttempts));*/

			glPopAttrib();
		dkglPopOrtho();

		if(gameVar.r_widescreen > 1) res[0] = static_cast<int>(res[1]*1.333f);
		if (gameVar.r_showLatency)
		{
			dkglPushOrtho((float)res[0], (float)res[1]);
				glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
					glEnable(GL_BLEND);
					float left = res[0] - (float)(PING_LOG_SIZE) - 15;
					float bottom = 65;
					float height = 40;
					float scaledHeight = height / 1000.0f;
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_QUADS);
						glColor4f(0.7f, 0.7f, 0.7f, 0.4f);
						glVertex2f(left, bottom-height);
						glVertex2f(left, bottom);
						glVertex2f(left+PING_LOG_SIZE, bottom);
						glVertex2f(left+PING_LOG_SIZE, bottom-height);
					glEnd();

					glLineWidth(1.0f);
					glBegin(GL_LINES);

					glColor4f(1.0f, 0.0f, 0.0f, 0.6f);
					glVertex3f(left-1, bottom, 0.0f);
					glVertex3f(left-1, bottom-height, 0.0f);

					glColor4f(1.0f, 1.0f, 0.0f, 0.6f);
					glVertex3f(left-1, bottom, 0.0f);
					glVertex3f(left-1, bottom-200*scaledHeight, 0.0f);

					glColor4f(0.0f, 1.0f, 0.0f, 0.6f);
					glVertex3f(left-1, bottom, 0.0f);
					glVertex3f(left-1, bottom-100*scaledHeight, 0.0f);

					int j = game->thisPlayer->pingLogID;//, k = 0;
					int ping;
					for (int i = 0; i < PING_LOG_SIZE; i++, j++)//, k += 2)
					{
						if (j >= PING_LOG_SIZE)
							j = 0;
						ping = game->thisPlayer->pingLog[j]*33;
						if (ping > 1000 || ping < 0)
							ping = 1000;
						if (ping <= 100)
							glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
						else if (ping <= 200)
							glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
						else
							glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
						glVertex3f(left+i, bottom, 0.0f);
						glVertex3f(left+i, bottom-1-ping*scaledHeight, 0.0f);
					}
					glEnd();
					/*glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
					glBegin(GL_LINES);
						glVertex3f(left, bottom-game->thisPlayer->avgPing*(height/1000.0f), 0.0f);
						glVertex3f(left+PING_LOG_SIZE, bottom-game->thisPlayer->avgPing*(height/1000.0f), 0.0f);
					glEnd();
					glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
					printLeftText(left, bottom + 5, 20, CString("Ang. ping: %d", game->thisPlayer->avgPing));*/
				glPopAttrib();
			dkglPopOrtho();
		}

		// On render les chat message par dessus le jeu
		dkglPushOrtho((float)res[0], (float)res[1]);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// Afficher les win des team ou les score si en TDM ou CTF ou SND
				glColor3f(1,1,1);
				switch (game->gameType)
				{
				case GAME_TYPE_DM:
					// Total Time left
					printLeftText(5,5,64,CString("%01i:%02i", (int)((game->gameTimeLeft+1)/60), (int)(game->gameTimeLeft+1)%60));
					break;
				case GAME_TYPE_TDM:
					// Total Time left
					printLeftText(5,5,64,CString("%01i:%02i", (int)((game->gameTimeLeft+1)/60), (int)(game->gameTimeLeft+1)%60));
					// Score left
					if (game->blueScore >= game->redScore)
					{
						renderTexturedQuad(5,5+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->blueScore, gameVar.sv_scoreLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->redScore, gameVar.sv_scoreLimit));
					}
					else
					{
						renderTexturedQuad(5,5+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->redScore, gameVar.sv_scoreLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->blueScore, gameVar.sv_scoreLimit));
					}
					break;
				case GAME_TYPE_CTF:
					// Total Time left
					printLeftText(5,5,64,CString("%01i:%02i", (int)((game->gameTimeLeft+1)/60), (int)(game->gameTimeLeft+1)%60));
					// win left
					if (game->blueWin >= game->redWin)
					{
						renderTexturedQuad(5,5+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->blueWin, gameVar.sv_winLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->redWin, gameVar.sv_winLimit));
					}
					else
					{
						renderTexturedQuad(5,5+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->redWin, gameVar.sv_winLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->blueWin, gameVar.sv_winLimit));
					}
					break;
				case GAME_TYPE_SND:

#if defined(_PRO_)
               printLeftText(5,5,64,CString("%01i:%02i", (int)((game->gameTimeLeft+1)/60), (int)(game->gameTimeLeft+1)%60));
               printLeftText(5,5+64,64,CString("%01i:%02i", (int)((game->roundTimeLeft+1)/60), (int)(game->roundTimeLeft+1)%60));

#else
					// Round Time left
					printLeftText(5,5,64,CString("%01i:%02i", (int)((game->roundTimeLeft+1)/60), (int)(game->roundTimeLeft+1)%60));
					// win left

					if (game->blueWin >= game->redWin)
					{
						renderTexturedQuad(5,5+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->blueWin, gameVar.sv_winLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->redWin, gameVar.sv_winLimit));
					}
					else
					{
						renderTexturedQuad(5,5+64,64,64,tex_redFlag);
						printLeftText(5+64+5,5+64,64,CString("%i/%i", game->redWin, gameVar.sv_winLimit));
						renderTexturedQuad(5,5+64+64,64,64,tex_blueFlag);
						printLeftText(5+64+5,5+64+64,64,CString("%i/%i", game->blueWin, gameVar.sv_winLimit));
					}


#endif

					break;
				}
         
#if defined(_PRO_)	      

            float textSize = (float)gameVar.r_chatTextSize;
#endif

            float xPos = ((float)res[0] / 800.0f) * 128 + 40;
				float yPos = res[1] - (((float)res[1] / 600.0f) * 128 + 40)-60;

				for (i=0;i<(int)chatMessages.size();++i)
				{
					if (chatMessages[i].duration > 1)
					{
						glColor4f(0,0,0, .5f);
					}
					else
					{
						glColor4f(0,0,0,chatMessages[i].duration*.5f);
					}
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_QUADS);

         
#if defined(_PRO_)	      
               float chatWidth = dkfGetStringWidth(textSize, chatMessages[i].message.s);

					glVertex2f(8,yPos - (float)((chatMessages.size() - i - 1) * textSize)+1);
					glVertex2f(8,yPos - (float)((chatMessages.size() - i - 1) * textSize)+textSize-1);
					glColor4f(0,0,0,0);
					glVertex2f(8+chatWidth,yPos - (float)((chatMessages.size() - i - 1) * textSize)+textSize-1);
					glVertex2f(8+chatWidth,yPos - (float)((chatMessages.size() - i - 1) * textSize)+1);
#else
					glVertex2f(8,yPos - (float)(chatMessages.size() - i - 1) * 28+1);
					glVertex2f(8,yPos - (float)(chatMessages.size() - i - 1) * 28+27);
					glColor4f(0,0,0,0);
					glVertex2f(500,yPos - (float)(chatMessages.size() - i - 1) * 28+27);
					glVertex2f(500,yPos - (float)(chatMessages.size() - i - 1) * 28+1);
#endif

					glEnd();
					if (chatMessages[i].duration > 1)
					{
						glColor3f(1,1,1);
					}
					else
					{
						glColor4f(1,1,1,chatMessages[i].duration);
					}
					// On l'écris à peut pret au tier de l'écran à gauche
					glEnable(GL_TEXTURE_2D);

#if defined(_PRO_)	
					printLeftText(10,yPos - (float)(chatMessages.size() - i - 1) * textSize, textSize, chatMessages[i].message);
#else
               printLeftText(10,yPos - (float)(chatMessages.size() - i - 1) * 28, 28, chatMessages[i].message);
#endif
				}

				// Si on est apres chatter
				if (chatting.haveFocus())
				{
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_QUADS);
						glColor4f(0,0,0, .5f);
						glVertex2f(8,yPos +32+1);
						glVertex2f(8,yPos +32+27);
						glColor4f(0,0,0,0);
						glVertex2f(500,yPos +32+27);
						glVertex2f(500,yPos +32+1);
					glEnd();
					glEnable(GL_TEXTURE_2D);
					glColor3f(1,1,1);
					CString chattingTo;
					if (isChattingTeam)
						chattingTo = "sayteam : ";
					else
						chattingTo = "say : ";

					
               float chattingToWidth = dkfGetStringWidth(28, chattingTo.s);
					printLeftText(10,yPos + 32,28,chattingTo);
					chatting.print(28, 10+chattingToWidth,yPos + 32, 0);
				}


				// Les events
				for (i=0;i<(int)eventMessages.size();++i)
				{
					if (eventMessages[i].duration > 1)
					{
						glColor3f(1,1,1);
					}
					else
					{
						glColor4f(1,1,1,eventMessages[i].duration);
					}
					// On l'écris à peut pret au 2 tier de l'écran à gauche

#if defined(_PRO_)	                  
            float eventTextSize = (float)gameVar.r_eventTextSize;

            if (gameVar.r_showEventText)
            {
               printLeftText(xPos,(float)res[1] - (float)(eventMessages.size() - i - 1) * eventTextSize-20-eventTextSize, eventTextSize, eventMessages[i].message);
            }
#else
            printLeftText(xPos,(float)res[1] - (float)(eventMessages.size() - i - 1) * 28-20-28, 28, eventMessages[i].message);
#endif

					
				}

			glPopAttrib();
		dkglPopOrtho();

		// Les stats
		if (game->showStats)
		{
			game->renderStats();

			if (blink < .25f)
			{
				dkglPushOrtho(800,600);
					glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
						glEnable(GL_BLEND);
						glColor3f(1,1,1);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						switch (game->roundState)
						{
						case GAME_PLAYING: break;
						case GAME_BLUE_WIN:
							printCenterText(475, 5, 64, gameVar.lang_blueTeamWin);
							break;
						case GAME_RED_WIN:
							printCenterText(475, 5, 64, gameVar.lang_redTeamWin);
							break;
						case GAME_DRAW:
							printCenterText(475, 5, 64, gameVar.lang_roundDraw);
							break;
						case GAME_MAP_CHANGE:
							printCenterText(475, 5, 64, gameVar.lang_changingMap);
							break;
						}
					glPopAttrib();
				dkglPopOrtho();
			}
		}
	}

	// On render le menu si c'est le cas
	if (showMenu && isConnected)
	{
		dkwClipMouse( false );
		menuManager.render(clientRoot);
	//	if (clientRoot) clientRoot->render();
	
	//	renderMenu();
		dkglPushOrtho(800,600);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_BLEND);
				glColor3f(1,1,1);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				//--- Square around the choosen gun
				glPolygonMode(GL_FRONT, GL_LINE);
				glLineWidth(2);
				glColor3f(0,1,0);
				glBegin(GL_QUADS);
					glVertex2i(currentGun->pos[0] - 5, currentGun->pos[1] - 5);
					glVertex2i(currentGun->pos[0] - 5, currentGun->pos[1] + currentGun->size[1] + 5);
					glVertex2i(currentGun->pos[0] + currentGun->size[0] + 5, currentGun->pos[1] + currentGun->size[1] + 5);
					glVertex2i(currentGun->pos[0] + currentGun->size[0] + 5, currentGun->pos[1] - 5);
				glEnd();
				glBegin(GL_QUADS);
					glVertex2i(currentMelee->pos[0] - 5, currentMelee->pos[1] - 5);
					glVertex2i(currentMelee->pos[0] - 5, currentMelee->pos[1] + currentMelee->size[1] + 5);
					glVertex2i(currentMelee->pos[0] + currentMelee->size[0] + 5, currentMelee->pos[1] + currentMelee->size[1] + 5);
					glVertex2i(currentMelee->pos[0] + currentMelee->size[0] + 5, currentMelee->pos[1] - 5);
				glEnd();
				glPolygonMode(GL_FRONT, GL_FILL);
				glColor3f(1,1,1);

				printCenterText(400, 5+48, 32, gameVar.sv_gameName);
				switch (game->gameType)
				{
				case GAME_TYPE_DM:
					printCenterText(400, 5, 64, gameVar.lang_deathmatchC);
					printCenterText(400, 5+88, 32, gameVar.lang_deathmatchD);
					break;
				case GAME_TYPE_TDM:
					printCenterText(400, 5, 64, gameVar.lang_teamDeathmatchC);
					printCenterText(400, 5+88, 32, gameVar.lang_teamDeathmatchD);
					break;
				case GAME_TYPE_CTF:
					printCenterText(400, 5, 64, gameVar.lang_captureTheFlagC);
					printCenterText(400, 5+88, 32, gameVar.lang_captureTheFlagD);
					break;
				case GAME_TYPE_SND:
#if defined(_PRO_)
					printCenterText(400, 5, 64, gameVar.lang_championC);
					printCenterText(400, 5+88, 32, gameVar.lang_championD);
#else
					printCenterText(400, 5, 64, gameVar.lang_counterBaboristC);
					printCenterText(400, 5+88, 32, gameVar.lang_counterBaboristD);
#endif
					break;
				}
				CString mapInfo (game->map->mapName);
				if(game->map->author_name.len() > 0)
					mapInfo.set("%s created by %s", game->map->mapName.s, game->map->author_name.s);
				printCenterText(400, 5+64, 32, mapInfo);
			glPopAttrib();
		dkglPopOrtho();
#endif
	}
	else
	{
		if ((menuManager.root) && (menuManager.root->visible))
		{
			dkwClipMouse( false );
		}
		else
		{
			dkwClipMouse( true );
		}
	}

	// Finalement, par dessus tout, le crosshair
//	CVector2i cursor = dkwGetCursorPos_main();
//	int xM = (int)(((float)cursor[0]/(float)res[0])*800.0f);
//	int yM = (int)(((float)cursor[1]/(float)res[1])*600.0f);
#ifndef _DX_
	dkglPushOrtho(800,600);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1,.2f,.2f, hitIndicator);
			renderTexturedQuad(xM-16,yM-16,32,32,tex_crossHit);			
		glPopAttrib();
	dkglPopOrtho();
#endif

	// On se connecte
	if (!isConnected)
	{
#ifndef _DX_
		dkglPushOrtho(800,600);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_BLEND);
				glColor3f(1,1,1);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				if (blink > .25f) printCenterText(400, 300-32, 64, gameVar.lang_connectingC);
				printCenterText(400, 332, 48, gameVar.lang_pressF10ToCancel);
				if (dkiGetState(KeyF10) == DKI_DOWN) console->sendCommand("disconnect");
			glPopAttrib();
		dkglPopOrtho();
#endif
	}
}
#endif

