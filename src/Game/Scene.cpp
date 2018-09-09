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

#include "Scene.h"
#include "Console.h"
#include "GameVar.h"
#include "Helper.h"
#include "CStatus.h"


extern char* bbNetVersion;

//int masterServerID = -1; // Make it extern



//
// Constructeur
//
Scene::Scene()
{
	frameID = 0;
#ifndef DEDICATED_SERVER
	surveySent = false;
	mainTab = 0;
	survey = 0;
	font = dkfCreateFont("main/fonts/babo.tga");
	tex_crosshair = dktCreateTextureFromFile("main/textures/Cross01.tga", DKT_FILTER_LINEAR);
	tex_menuCursor = dktCreateTextureFromFile("main/textures/MenuCursor.tga", DKT_FILTER_LINEAR);
	//tex_miniHeadGames = dktCreateTextureFromFile("main/textures/miniHeadGames.tga", DKT_FILTER_LINEAR);
#endif
	server = 0;
#ifndef DEDICATED_SERVER
	client = 0;
	editor = 0;

	//-- On print le loading screen! (new)
		// On clear les buffers, on init la camera, etc
#ifndef _DX_
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CVector2i res = dkwGetResolution();

		if(gameVar.r_widescreen > 1)
			glViewport((GLint)((res[0] - res[1]*1.333f)/2.0f), 0, (GLsizei)(res[1]*1.333f), (GLsizei)res[1]);
		else
			glViewport(0, 0, res[0], res[1]);
		dkglSetProjection(60, 1, 50, (float)res[1]*1.333f, (float)res[1]);

		// Truc par default � enabeler
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1,1,1);

		dkglPushOrtho(800, 600);

		// Print au millieu
		glColor3f(1,1,1);
		dkfBindFont(font);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		printCenterText(400, 268, 64, CString("LOADING"));

		dkglPopOrtho();

		// On swap les buffers
		SwapBuffers(dkwGetDC());
#endif
#endif
	gameVar.loadModels();
#ifndef DEDICATED_SERVER
	introScreen = new IntroScreen();
#endif
	serverInfoDelay = GAME_UPDATE_DELAY;
	masterReady = true;
#ifndef DEDICATED_SERVER
	// Les menu seront toujours load� dans ram. Car on peut y aller pendant le jeu
//	menu = new Menu();
//	createSurvey();
//	createMenu();
	
	//--- Overall control
	/*menuManager.root = new CControl();
	menuManager.root->font = font;
	menuManager.root->size.set(800,600);
	menuManager.root->backColor.set(.3f,.5f,.8f);
	menuManager.root->foreColor.set(1,1,1);
	menuManager.root->textShadow = true;
	menuManager.root->noFill = true;
	if(gameVar.r_animatedMenu)
	{
		menuManager.root->superPanel = true;
		menuManager.root->texture = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_NEAREST);
	}

//	menu->isReady = true;
	menuManager.root->enable = true;
	menuManager.root->visible = false; //--- Not now */
	FSOUND_SetSFXMasterVolume((int)(255.0f*gameVar.s_masterVolume));
//	dksPlayMusic("main/sounds/menu.ogg", -1);

	//--- Query from data base if we sent the survey already
	sqlite3 *DB=0;
	sqlite3_open("bv2.db",&DB);

	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int	nRow;			// number of record
	int	nColumn;		// number of column
	char	SQL[256];		// the query

	// Get infos of master servers and choose the one with the lowest Score
	sprintf(SQL,"Select Value From LauncherSettings where Name = 'DidSurvey';");
	sqlite3_get_table(DB,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	int didSurvey = atoi(azResult[1]);
	sqlite3_free_table(azResult);
	sqlite3_close( DB );
	
	surveySent = (didSurvey == 0 ? false : true);

#endif
	// On reset el timer
	dkcJumpToFrame(0);
}



//
// Destructeur
//
Scene::~Scene()
{
	disconnect();
	gameVar.deleteModels();
#ifndef DEDICATED_SERVER
	dkfDeleteFont(&font);
	dktDeleteTexture(&tex_crosshair);
	//dktDeleteTexture(&tex_miniHeadGames);
	dktDeleteTexture(&tex_menuCursor);
//	ZEVEN_SAFE_DELETE(menu);
	ZEVEN_SAFE_DELETE(introScreen);
	ZEVEN_SAFE_DELETE(mainTab);
	ZEVEN_SAFE_DELETE(survey);
#endif
}



//
// Update
//
void Scene::update(float delay)
{
#ifndef DEDICATED_SERVER
	if (!gameVar.s_inGameMusic)
	{
		dksStopMusic();
	}
#endif
	frameID++;

	//--- Update master server client
	if (master) master->update(delay);
#ifndef DEDICATED_SERVER
	if (mainTab)
	{
		if (master->isConnected())
		{
			mainTab->browser->btn_refresh->enable = false;
		}
		else
		{
			mainTab->browser->btn_refresh->enable = true;
		}
	}
	if (introScreen)
	{
		introScreen->update(delay);
	//	menu->update(delay); // ??? wtf
		if (introScreen->showDelay <= 0) 
		{
			ZEVEN_SAFE_DELETE(introScreen);
		//	menu->isReady = true;
		//	ZEVEN_SAFE_DELETE(menuManager.root);

			// [dsl] No more survey!
		/*	if (surveySent)
			{*/
				if (gameVar.s_inGameMusic) dksPlayMusic("main/sounds/Menu.ogg", -1);
				createMenu();
		/*	}
			else
			{
				createSurvey();
			}*/
		//	survey = new CSurvey();
			menuManager.root->visible = true;
		/*	if (gameVar.s_inGameMusic) dksPlayMusic("main/sounds/Menu.ogg", -1);*/
		}
	}
	else if (survey)
	{
		survey->update(delay);

		// On update les menu
		if ((menuManager.root) && (menuManager.root->visible))
		{
			menuManager.update(delay);
		}

		if (survey->IsDone())
		{
			ZEVEN_SAFE_DELETE(survey);
			ZEVEN_SAFE_DELETE(menuManager.root);
		//	menu->isReady = true;
			createMenu();
			menuManager.root->visible = true;
			if (gameVar.s_inGameMusic) dksPlayMusic("main/sounds/Menu.ogg", -1);
		}
	}
	else
#endif
	{
#ifndef DEDICATED_SERVER
		if (mainTab) mainTab->update(delay);
#endif

		// On update le server, tr�s important
		if (server)
		{
			server->update(delay);
			if (server->needToShutDown)
			{
				disconnect();
				return;
			}
		}

		//--- Update master server client
	//	if (master) master->update(delay);

#ifndef DEDICATED_SERVER
		// On update le client, tr�s important aussi
		if (client)
		{
			// On set le volume avec �a :D:D trop hot
			if (client->game)
			{
				FSOUND_SetSFXMasterVolume((int)((255.0f - client->game->viewShake*100.0f)*gameVar.s_masterVolume));
			//	FSOUND_SetFrequency(FSOUND_ALL, gameVar.s_mixRate+(int)(-client->game->viewShake*(float)gameVar.s_mixRate*.25f));
			}
			else
			{
				FSOUND_SetSFXMasterVolume((int)(255.0f*gameVar.s_masterVolume));
			//	FSOUND_SetFrequency(FSOUND_ALL, gameVar.s_mixRate);
			}

			client->update(delay);
			if (client->needToShutDown)
			{
				disconnect();
				return;
			}
		}
		else
		{
			FSOUND_SetSFXMasterVolume((int)(255.0f*gameVar.s_masterVolume));
		//	FSOUND_SetFrequency(FSOUND_ALL, gameVar.s_mixRate);
		}

		// On update l'editor
		if (editor)
		{
			editor->update(delay);
			if (editor->needToShutDown)
			{
				disconnect();
				return;
			}
		}

		// On update les menu
		if ((menuManager.root))
		{
			if (menuManager.root->visible)
			{
				menuManager.update(delay);
			}
		}
		menuManager.updateDialogs(delay);
#endif
	}

#ifndef DEDICATED_SERVER
	gameVar.ro_nbParticle = dkpUpdate(delay);
#endif
}


#ifndef DEDICATED_SERVER
//
// Renderer
//
void Scene::render()
{
#ifndef _DX_
	// On clear les buffers, on init la camera, etc
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
	dkglGetDXDevice()->Clear(
					0,  //Number of rectangles to clear, we're clearing everything so set it to 0
                    NULL, //Pointer to the rectangles to clear, NULL to clear whole display
                    D3DCLEAR_TARGET,   //What to clear.
                    0x00000000, //Colour to clear to (AARRGGBB)
                    1.0f,  //Value to clear ZBuffer to
                    0 );   //Stencil clear value, we don't have one, this value doesn't matter
#endif
	CVector2i res = dkwGetResolution();
#ifndef _DX_

	if(gameVar.r_widescreen > 1)
		glViewport( (GLint)((res[0] - res[1]*1.333f)/2.0f), 0, (GLsizei)(res[1]*1.333f), (GLsizei)res[1]);
	else
		glViewport(0, 0, res[0], res[1]);
	dkglSetProjection(60, 1, 50, (float)res[1]*1.333f, (float)res[1]);

	// Truc par default � enabeler
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
#endif

	if (introScreen)
	{
		introScreen->render();
	}
	else if (survey)
	{
	//	survey->render();
		// On render les menus
		if ((menuManager.root) && (menuManager.root->visible))
		{
			menuManager.render();
			dkwClipMouse( false );
		}

		// Non, le curseur sur TOUUTEEE
		CVector2i cursor = dkwGetCursorPos_main();
		int xM = (int)(((float)cursor[0]/(float)res[0])*800.0f);
		int yM = (int)(((float)cursor[1]/(float)res[1])*600.0f);
#ifndef _DX_
		dkglPushOrtho(800,600);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor3f(1,1,1);
			//	glColor3f(1,1,.6f);
				renderTexturedQuad(xM,yM,32,32,tex_menuCursor);
			glPopAttrib();
		dkglPopOrtho();
#endif

		// On afficher le fps
		if(gameVar.r_widescreen > 1) res[0] = static_cast<int>(res[1]*1.333f);
#ifndef _DX_
		dkglPushOrtho((float)res[0], (float)res[1]);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				dkfBindFont(font);

				glColor3f(1,1,1);
				//--- Render if logged in or not
				if (status)
				{
					if (status->get() == CStatus::ONLINE)
					{
						glColor3f(0,1,0);
						printLeftText(10, (float)res[1] - 20, 20, CString("\x2 Current user: ") + gameVar.cl_accountUsername);
					}
					else if (status->get() == CStatus::OFFLINE)
					{
						glColor3f(1,0,0);
						printLeftText(10, (float)res[1] - 20, 20, " Offline");
					}
				}

				glColor3f(1,1,1);
				if (gameVar.r_showStats)
				{
					printRightText((float)res[0], 0, 20, CString("FPS : %i", (int)dkcGetFPS()));
				//	printRightText((float)res[0], 32, 32, CString("NB PARTICLE : %i", gameVar.ro_nbParticle));
				//	unsigned long byteSent = (client)?bb_clientGetBytesSent(client->uniqueClientID) + bb_serverGetBytesSent() + bb_peerGetBytesSent():0;
				//	unsigned long byteRecv = (client)?bb_clientGetBytesReceived(client->uniqueClientID) + bb_serverGetBytesReceived() + bb_peerGetBytesReceived():0;
				//	printRightText((float)res[0], 64, 32, CString("NET BYTE SENT : %i Kb", (int)byteSent / 1024));
				//	printRightText((float)res[0], 96, 32, CString("NET BYTE RECV : %i Kb", (int)byteRecv / 1024));
				}

				// On affiche la version du jeu
				if (!editor && !client)
				{
					if (server)
					{
						printRightText((float)res[0]-5, (float)res[1]-32-5, 32, CString(gameVar.lang_serverVersion.s, (int)GAME_VERSION_SV/10000, (int)(GAME_VERSION_SV%10000)/100, ((int)GAME_VERSION_SV%100)));
					}
					else
					{
						printRightText((float)res[0]-5, (float)res[1]-32-5, 32, CString(gameVar.lang_clientVersion.s, (int)GAME_VERSION_CL/10000, (int)(GAME_VERSION_CL%10000)/100, ((int)GAME_VERSION_CL%100)));
					}

					//--- Copyrights (replaced by head games logo)
				//	printRightText((float)res[0]-5, (float)res[1]-100-5, 16, CString("Copyright (c) RndLabs Inc. 2006-2007"));
				//	printRightText((float)res[0]-5, (float)res[1]-100-5+16, 16, CString("All Rights Reserved"));
					
				//	printRightText((float)res[0]-5, (float)res[1]-32-5-32, 32, CString("server version %01i.%02i.%02i", (int)GAME_VERSION_SV/10000, (int)(GAME_VERSION_SV%1000)/100, ((int)GAME_VERSION_SV%100)));
				//	printRightText((float)res[0]-5, (float)res[1]-32-5-64, 32, CString("Babonet version %s", bbNetVersion));
				}
			glPopAttrib();
		dkglPopOrtho();
#endif
	}
	else
	{
		// On render le server (sert pas � grand chose)
		if (server) server->render();

		// On render le client
		float alphaScope = 0;
		if (client) client->render(alphaScope);

#ifndef DEDICATED_SERVER
		// On render l'editor
		if (editor) editor->render();
#endif
		// On render les menus
		if ((menuManager.root) && (menuManager.root->visible))
		{
			menuManager.render();
			dkwClipMouse( false );
		}
		menuManager.renderDialogs();

/*		if (menu->isShowing) menu->render();
		else if (menu->soundPlayChannel >= 0)
		{
			FSOUND_StopSound(menu->soundPlayChannel);
			menu->soundPlayChannel = -1;
		}*/

		// On afficher le fps
#ifndef _DX_
		dkglPushOrtho((float)res[0], (float)res[1]);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				dkfBindFont(font);

				glColor3f(1,1,1);
				//--- Render if logged in or not
				if (status)
				{
					if (status->get() == CStatus::ONLINE)
					{
						glColor3f(0,1,0);
						printLeftText(10, (float)res[1] - 20, 20, CString("\x2 Current user: ") + gameVar.cl_accountUsername);
					}
					else if (status->get() == CStatus::OFFLINE)
					{
						glColor3f(1,0,0);
						printLeftText(10, (float)res[1] - 20, 20, " Offline");
					}
				}

				glColor3f(1,1,1);
				if (gameVar.r_showStats)
				{
					printRightText((float)res[0], 0, 20, CString("FPS : %i", (int)dkcGetFPS()));
				//dkfPrint(20,0, 0, 0, CString("FPS : %i", (int)dkcGetFPS()).s);
               //(size,x-width,y,0,text.s);
					//printRightText((float)res[0], 32, 32, CString("NB PARTICLE : %i", gameVar.ro_nbParticle));
				//	unsigned long byteSent = (client)?bb_clientGetBytesSent(client->uniqueClientID) + bb_serverGetBytesSent() + bb_peerGetBytesSent():0;
				//	unsigned long byteRecv = (client)?bb_clientGetBytesReceived(client->uniqueClientID) + bb_serverGetBytesReceived() + bb_peerGetBytesReceived():0;
				//	printRightText((float)res[0], 64, 32, CString("NET BYTE SENT : %i Kb", (int)byteSent / 1024));
				//	printRightText((float)res[0], 96, 32, CString("NET BYTE RECV : %i Kb", (int)byteRecv / 1024));
				}

				// On affiche la version du jeu
				if (!editor && !client)
				{
					if (server)
					{
						printRightText((float)res[0]-5, (float)res[1]-32-5, 32, CString(gameVar.lang_serverVersion.s, (int)GAME_VERSION_SV/10000, (int)(GAME_VERSION_SV%10000)/100, ((int)GAME_VERSION_SV%100)));
					}
					else
					{
						printRightText((float)res[0]-5, (float)res[1]-32-5, 32, CString(gameVar.lang_clientVersion.s, (int)GAME_VERSION_CL/10000, (int)(GAME_VERSION_CL%10000)/100, ((int)GAME_VERSION_CL%100)));
					}

					//--- Copyrights (replaced by head games logo)
				//	printRightText((float)res[0]-5, (float)res[1]-100-5, 16, CString("Copyright (c) RndLabs Inc. 2006-2007"));
				//	printRightText((float)res[0]-5, (float)res[1]-100-5+16, 16, CString("All Rights Reserved"));
					
				//	printRightText((float)res[0]-5, (float)res[1]-32-5-32, 32, CString("server version %01i.%02i.%02i", (int)GAME_VERSION_SV/10000, (int)(GAME_VERSION_SV%1000)/100, ((int)GAME_VERSION_SV%100)));
				//	printRightText((float)res[0]-5, (float)res[1]-32-5-64, 32, CString("Babonet version %s", bbNetVersion));
				}
			glPopAttrib();
		dkglPopOrtho();
#endif

		// Render la console sur toute
		console->render();

		// Non, le curseur sur TOUUTEEE
		CVector2i cursor = dkwGetCursorPos_main();
		int xM = (int)(((float)cursor[0]/(float)res[0])*800.0f);
		int yM = (int)(((float)cursor[1]/(float)res[1])*600.0f);
#ifndef _DX_
		dkglPushOrtho(800,600);
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				if (menuManager.root)
				if (!menuManager.root->visible && client)
				{
					if (client->showMenu)
					{
						glColor3f(1,1,1);
					//	glColor3f(1,1,.6f);
						renderTexturedQuad(xM,yM,32,32,tex_menuCursor);
					}
					else
					{
						glColor4f(0,0,0,1-alphaScope);
						renderTexturedQuad(xM-15,yM-15,32,32,tex_crosshair);
					//	glColor4f(1,1,.6f,1-alphaScope);
						glColor4f(1,1,1,1-alphaScope);
						renderTexturedQuad(xM-16,yM-16,32,32,tex_crosshair);
					}
				}
				else
				{
					glColor3f(1,1,1);
				//	glColor3f(1,1,.6f);
					renderTexturedQuad(xM,yM,32,32,tex_menuCursor);
				}
			glPopAttrib();
		dkglPopOrtho();
#endif
	}
}
#endif

