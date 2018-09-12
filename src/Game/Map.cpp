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

#include "Map.h"
#include "Helper.h"
#include "FileIO.h"
#include "Console.h"
#include "Game.h"
#include "Scene.h"

#ifndef DEDICATED_SERVER
#include "CRain.h"
#include "CSnow.h"
#include "CLava.h"
#include <direct.h>
#include <algorithm>
#endif

extern Scene * scene;



//
// Constructeur
//
Map::Map(CString mapFilename, Game * _game, unsigned int font, bool editor, int sizeX, int sizeY)
#ifndef DEDICATED_SERVER
: groundMesh(0), shadowMesh(0), wallMesh(0)
#endif
{
#if defined(_PRO_)
	aStar = 0;
#endif
    int i, j, gtnum;
	//-- On print le loading screen! (new)
		// On clear les buffers, on init la camera, etc
#ifndef DEDICATED_SERVER
#ifndef _DX_
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
		CVector2i res = dkwGetResolution();

#ifndef _DX_
		if(gameVar.r_widescreen > 1)
			glViewport( (GLint)((res[0] - res[1]*1.333f)/2.0f) , 0, (GLsizei)(res[1]*1.333f), (GLsizei)res[1]);
		else
			glViewport(0, 0, res[0], res[1]);
		dkglSetProjection(60, 1, 50, (float)res[1]*1.333f, (float)res[1]);

		// Truc par default à enabeler
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
        SDL_GL_SwapWindow(dkwGetHandle());
#endif
#endif
#ifndef DEDICATED_SERVER
	dko_map = 0;
	dko_cam = 0;
	zoom = 0;
#endif
	dko_mapLM = 0;
	game = _game;

	if (game)
	{
		isServer = game->isServerGame;
	}
	else
	{
		isServer = false;
	}

	if (mapFilename.len() > 15) mapFilename.resize(15);

	isValid = true;
#ifndef DEDICATED_SERVER
	isEditor = editor;
#endif
	mapName = mapFilename;
#ifndef DEDICATED_SERVER
	flagAnim = 0;
	m_weather = 0;

	if (!isServer)
	{
#ifndef _DX_
		//qObj = gluNewQuadric();
#endif

		// Les textures
		tex_grass = dktCreateTextureFromFile("main/textures/grass.tga", DKT_FILTER_BILINEAR);
		tex_dirt = dktCreateTextureFromFile("main/textures/dirt1.tga", DKT_FILTER_BILINEAR);
		tex_wall = dktCreateTextureFromFile("main/textures/dirt2.tga", DKT_FILTER_BILINEAR);
	
		tex_bombMark = dktCreateTextureFromFile("main/textures/BombMark.tga", DKT_FILTER_BILINEAR);

		// Les models
		dko_flag[0] = dkoLoadFile("main/models/BlueFlag.DKO");
		dko_flagPod[0] = dkoLoadFile("main/models/BlueFlagPod.DKO");
		dko_flag[1] = dkoLoadFile("main/models/RedFlag.DKO");
		dko_flagPod[1] = dkoLoadFile("main/models/RedFlagPod.DKO");

	//	dko_flagPole = dkoLoadFile("main/models/flagpole.dko");

		flagAnims[0] = 25;
		flagAnims[1] = 25;
		flagAngle[0] = 0;
		flagAngle[1] = 0;

		theme = THEME_GRASS;
		weather = WEATHER_NONE;
	}
#endif

	// Les flags
	flagState[0] = -2;
	flagState[1] = -2;

	// On essaye d'abords de lire la map
	CString fullName = CString("main/maps/") + mapName + ".bvm";

#ifndef DEDICATED_SERVER
	// Hosts will load from file since they are also the server
	// Ugly code was required to avoid modification of the remaining code
	FileIO	fileObj(fullName, "rb");
	FileIO* fptr = 0;
	if (scene->server || isEditor)
		fptr = &fileObj;
	else
		fptr = &_game->mapBuffer;
	FileIO&	file = *fptr;
	
#else
	FileIO file(fullName, "rb");
#endif


	if (!file.isValid())
	{
		console->add("\x4> Map doesnt exist");
#ifndef DEDICATED_SERVER
		if (editor)
		{
			isValid = true;
			mapName = mapFilename;
			author_name = gameVar.cl_mapAuthorName;

			// On init ici
			size[0] = sizeX;
			size[1] = sizeY;

			cells = new map_cell[size[0]*size[1]];

			// On cré tout suite les contours de la map
			for (j=0;j<size[1];++j)
			{
				cells[j*size[0]+0].passable = false;
				cells[j*size[0]+0].height = 3; // Les bords sont plus haut
				cells[j*size[0]+size[0]-1].passable = false;
				cells[j*size[0]+size[0]-1].height = 3;
			}
			for (i=0;i<size[0];++i)
			{
				cells[i].passable = false;
				cells[i].height = 3;
				cells[(size[1]-1)*size[0]+i].passable = false;
				cells[(size[1]-1)*size[0]+i].height = 3;
			}
		}
		else
		{
#endif
			isValid = false;
			return;
#ifndef DEDICATED_SERVER
		}
#endif
	}
	else
	{
		CString mapStr = "\x3> Map file Exist :";
		mapStr += mapName;
		console->add(mapStr);
		// On le load ici
		unsigned long mapVersion = file.getULong();

		switch (mapVersion)
		{
		case 10010:
			{
#ifndef DEDICATED_SERVER
				// if opening older map file in editor - fill in the author field
				if(editor)
				{
					author_name = gameVar.cl_mapAuthorName;
				}
#endif
				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0]*size[1]];
				for (j=0;j<size[1];++j)
				{
					for (i=0;i<size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*size[0]+i].passable = (data & 128)?true:false;
						cells[j*size[0]+i].height = (data & 127);
						data = file.getUByte();
						setTileDirt(i,j,((float)data)/255.0f);
					}
				}
				break;
			}
		case 10011:
			{
#ifndef DEDICATED_SERVER
				// if opening older map file in editor - fill in the author field
				if(editor)
				{
					author_name = gameVar.cl_mapAuthorName;
				}
#endif
				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0]*size[1]];
				for (j=0;j<size[1];++j)
				{
					for (i=0;i<size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*size[0]+i].passable = (data & 128)?true:false;
						cells[j*size[0]+i].height = (data & 127);
						data = file.getUByte();
						setTileDirt(i,j,((float)data)/255.0f);
					}
				}

				// Les flag
				flagPodPos[0] = file.getVector3f();
				flagPodPos[1] = file.getVector3f();

				// Les ojectifs
				objective[0] = file.getVector3f();
				objective[1] = file.getVector3f();

				// Les spawn point
				int nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					dm_spawns.push_back(file.getVector3f());
				}
				nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					blue_spawns.push_back(file.getVector3f());
				}
				nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					red_spawns.push_back(file.getVector3f());
				}
				break;
			}
		case 20201:
			{
#ifndef DEDICATED_SERVER
				// if opening older map file in editor - fill in the author field
				if(editor)
				{
					author_name = gameVar.cl_mapAuthorName;
				}
				theme = file.getInt();
				weather = file.getInt();
#else
				int theme = file.getInt();
				int weather = file.getInt();
#endif
				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0]*size[1]];
				for (j=0;j<size[1];++j)
				{
					for (i=0;i<size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*size[0]+i].passable = (data & 128)?true:false;
						cells[j*size[0]+i].height = (data & 127);
						data = file.getUByte();
						setTileDirt(i,j,((float)data)/255.0f);
					}
				}

				// Les flag
				flagPodPos[0] = file.getVector3f();
				flagPodPos[1] = file.getVector3f();

				// Les ojectifs
				objective[0] = file.getVector3f();
				objective[1] = file.getVector3f();

				// Les spawn point
				int nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					dm_spawns.push_back(file.getVector3f());
				}
				nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					blue_spawns.push_back(file.getVector3f());
				}
				nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					red_spawns.push_back(file.getVector3f());
				}
				break;
			}
		case 20202:
			{
				// Common map data
				char * author_name_buffer = file.getByteArray(25);
#ifndef DEDICATED_SERVER
				author_name_buffer[24] = '\0';
				author_name.set("%.24s", author_name_buffer);
				// Note: we DO NOT want to overwrite the author field if it's being edited
				theme = file.getInt();
				weather = file.getInt();
#else
				int theme = file.getInt();
				int weather = file.getInt();
#endif
				delete[] author_name_buffer;
				author_name_buffer = 0;

				// for gcc compliant code

				int i=0;

				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0]*size[1]];
				for (j=0;j<size[1];++j)
				{
					for (i=0;i<size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*size[0]+i].passable = (data & 128)?true:false;
						cells[j*size[0]+i].height = (data & 127);
						data = file.getUByte();
						setTileDirt(i,j,((float)data)/255.0f);
					}
				}
				// common spawns
				int nbSpawn = file.getInt();
				for (i=0;i<nbSpawn;++i)
				{
					dm_spawns.push_back(file.getVector3f());
				}

				// read game-type specific data
				// there must always be one game-type specific section per one supported game type
				for (gtnum = 0; gtnum < GAME_TYPE_COUNT; ++gtnum)
				{
					int id = file.getInt();
					switch (id)
					{
					case GAME_TYPE_DM:
					case GAME_TYPE_TDM:
						break; // nothing to do for DM and TDM
					case GAME_TYPE_CTF:
						{
							// flags
							flagPodPos[0] = file.getVector3f();
							flagPodPos[1] = file.getVector3f();
						}
						break;
					case GAME_TYPE_SND:
						{
							// bombs
							objective[0] = file.getVector3f();
							objective[1] = file.getVector3f();
							// team-spawns
							nbSpawn = file.getInt();
							for (i=0;i<nbSpawn;++i)
							{
								blue_spawns.push_back(file.getVector3f());
							}
							nbSpawn = file.getInt();
							for (i=0;i<nbSpawn;++i)
							{
								red_spawns.push_back(file.getVector3f());
							}
						}
						break;
					default:
						console->add(CString("\x3> Error: unknown game-type id found in map-file, the value is %d", id));
                            break;
					}
				}
				break;
			}
		}
	}


	//--- on check si on a pas un model 3D de cette map là
	CString checkFor3D = "main/modelmaps_______/";
	checkFor3D += mapName + "/" + mapName + ".DKO";
	FileIO * dkoFile = new FileIO(checkFor3D, "rb");
#ifndef DEDICATED_SERVER
	introAnim = 0;
	introAnimLenght = 0;
#endif
	if (dkoFile->isValid())
	{
		delete dkoFile;
#ifndef DEDICATED_SERVER
		if (!isServer) dko_map = dkoLoadFile(checkFor3D.s);
#endif
		checkFor3D.resize(checkFor3D.len() - 4);
		checkFor3D += "LM.DKO";
		dko_mapLM = dkoLoadFile(checkFor3D.s);

		if (dko_mapLM)
		{
			//--- Generate octree for collisions
			dkoBuildOctree(dko_mapLM);
		}

		//--- Cam animation
#ifndef DEDICATED_SERVER
		checkFor3D.resize(checkFor3D.len() - 6);
		checkFor3D += "CAM.DKO";
		dko_cam = dkoLoadFile(checkFor3D.s);
		if (dko_cam)
		{
			introAnimLenght = dkoGetTotalFrame(dko_cam);
		}
#endif
	}
	else
	{
		delete dkoFile;
	}

#if defined(_PRO_)
	//--- Create the A* path
	if (aStar) delete aStar;
	aStar = new CAStar();

	//--- Get the closest to power of two size
	CVector2i aStarSize = size;

	int s = 1;
	for (s=1;s<=512;s*=2)
	{
		if (aStarSize[0] == s) break;
		if (aStarSize[0] > s && aStarSize[0] < s * 2) 
		{
			aStarSize[0] = s * 2;
			break;
		}
	}
	for (s=1;s<=512;s*=2)
	{
		if (aStarSize[1] == s) break;
		if (aStarSize[1] > s && aStarSize[1] < s * 2) 
		{
			aStarSize[1] = s * 2;
			break;
		}
	}

	unsigned char * passables = new unsigned char [aStarSize[0] * aStarSize[1]];
	memset(passables, 0, sizeof(unsigned char) * (aStarSize[0] * aStarSize[1]));

	for (j=0;j<size[1];++j)
	{
		for (i=0;i<size[0];++i)
		{
			if (cells[j*size[0]+i].passable)
			{
				passables[j * aStarSize[0] + i] = 255;
			}
		}
	}

	if (!aStar->Build(passables, aStarSize[0], aStarSize[1]))
	{
		delete aStar;
		aStar = 0;
	}

	delete [] passables;
#endif

	if (isServer)
	{
		return;
	}

#ifndef DEDICATED_SERVER
	if (dko_map)
	{
		//--- Load dummies
		int i=0;
		char* dummyName = 0;
		while (dummyName = dkoGetDummyName(i, dko_map))
		{
			CVector3f dumPos;
			int dumType = -1;
			dkoGetDummyPosition(i+1, dko_map, dumPos.s);
			if (strnicmp("flame", dummyName, 5) == 0)
			{
				dumType = DUMMY_TYPE_FLAME;
			}
			dummies.push_back(SMapDummy(dumPos * .1f, dumType));
			++i;
		}
	}
#endif

#ifndef DEDICATED_SERVER
	//--- We load now the minimap thumbnail
	if (dko_map)
	{
		CString checkFor3D = "main/modelmaps_______/";
		checkFor3D += mapName + "/minimap.tga";
		texMap = dktCreateTextureFromFile(checkFor3D.s, DKT_FILTER_NEAREST);
	}
	else
	{
		// On cré l'espace pour la texture
		texMap = dktCreateEmptyTexture(size[0], size[1], 3, DKT_FILTER_NEAREST);
		regenTex();
	}

	tex_floor = 0;
	tex_floor_dirt = 0;
	tex_wall_bottom = 0;
	tex_wall_center =0;
	tex_wall_up = 0;
	tex_wall_top = 0;
	tex_wall_both = 0;

	reloadWeather();
	reloadTheme();

	//--- Rebuild the map
	buildAll();

	//--- Position the camera at the center
	setCameraPos(CVector3f((float)size[0] / 2, (float)size[1] / 2, 0));

#endif

	//--- Reset timer
	dkcJumpToFrame(0);
}

#ifndef DEDICATED_SERVER
void Map::reloadWeather()
{
	ZEVEN_SAFE_DELETE(m_weather);

	fogDensity = 1;
	fogStart = -3;
	fogEnd = -3;
	fogColor.set(1,1,1,1);

	if(!gameVar.r_weatherEffects) {
		weather = WEATHER_NONE;
		fogDensity = 0;
		return;
	}

	fogDensity = 0;

	// Set weather to match theme
	if (theme == THEME_GRAIN)
		weather = WEATHER_FOG;
	if ((theme == THEME_SNOW) || (theme == THEME_FROZEN) || (theme == THEME_WINTER))
		weather = WEATHER_SNOW;
	else if ((theme == THEME_SAND) || (theme == THEME_STREET))
		weather = WEATHER_SANDSTORM;
	else if ((theme == THEME_CITY) || (theme == THEME_RAINY) || (theme == THEME_ROAD))
		weather = WEATHER_RAIN;
	else if ((theme == THEME_LAVA) || (theme == THEME_CORE) || (theme == THEME_ROCK))
		weather = WEATHER_LAVA;
	else weather = WEATHER_NONE;

	if (weather == WEATHER_RAIN)
	{
		fogStart = 4;
		fogEnd = -3;
		fogColor.set(.15f,.25f,.25f,1);
		m_weather = new CRain();
	}
	if (weather == WEATHER_FOG)
	{
		fogStart = 1;
		fogEnd = -.25f;
		fogColor.set(.3f,.4f,.4f,1);
	}
	if (weather == WEATHER_SNOW)
	{
		fogDensity = 0;
		m_weather = new CSnow();
	}
	if (weather == WEATHER_LAVA)
	{
		fogDensity = 0;
		m_weather = new CLava();
	}
}

void Map::regenCell(int i, int j)
{
	if (i < 0 || j < 0 || i >= size[0] || j >= size[1]) return;
	int pos = j*size[0]+i;
#ifndef _DX_
	if (cells[pos].dl) glDeleteLists(cells[pos].dl, 1);
	cells[pos].dl = 0;
	if (!cells[pos].passable)
	{
		cells[pos].dl = glGenLists(1);
		glNewList(cells[pos].dl, GL_COMPILE);
			if (cells[pos].height == 1)
			{
				glBindTexture(GL_TEXTURE_2D, tex_wall_both);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, tex_wall_bottom);
			}

			if (j > 0)
			{
				if (cells[(j-1)*size[0]+(i)].passable)
				{
					glColor3f(.3f,.3f,.3f);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex3i(i,j,1);
						glTexCoord2i(0,0);
						glVertex3i(i,j,0);
						glTexCoord2i(1,0);
						glVertex3i(i+1,j,0);
						glTexCoord2i(1,1);
						glVertex3i(i+1,j,1);
					glEnd();
				}
			}

			if (i < size[0]-1)
			{
				if (cells[(j)*size[0]+(i+1)].passable)
				{
					glColor3f(.4f,.4f,.4f);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex3i(i+1,j,1);
						glTexCoord2i(0,0);
						glVertex3i(i+1,j,0);
						glTexCoord2i(1,0);
						glVertex3i(i+1,j+1,0);
						glTexCoord2i(1,1);
						glVertex3i(i+1,j+1,1);
					glEnd();
				}
			}

			if (j < size[1]-1)
			{
				if (cells[(j+1)*size[0]+(i)].passable)
				{
					glColor3f(.7f,.7f,.7f);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex3i(i+1,j+1,1);
						glTexCoord2i(0,0);
						glVertex3i(i+1,j+1,0);
						glTexCoord2i(1,0);
						glVertex3i(i,j+1,0);
						glTexCoord2i(1,1);
						glVertex3i(i,j+1,1);
					glEnd();
				}
			}

			if (i > 0)
			{
				if (cells[(j)*size[0]+(i-1)].passable)
				{
					glColor3f(.8f,.8f,.8f);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex3i(i,j+1,1);
						glTexCoord2i(0,0);
						glVertex3i(i,j+1,0);
						glTexCoord2i(1,0);
						glVertex3i(i,j,0);
						glTexCoord2i(1,1);
						glVertex3i(i,j,1);
					glEnd();
				}
			}

			//--- Les partie central
			if (cells[pos].height > 2)
			{
				int h = 1;
				glBindTexture(GL_TEXTURE_2D, tex_wall_center);
				if (j > 0)
				{
					if (cells[(j-1)*size[0]+(i)].passable || cells[pos].height - 1 > cells[(j-1)*size[0]+(i)].height)
					{
						if (!cells[(j-1)*size[0]+(i)].passable)
						{
							h = cells[(j-1)*size[0]+(i)].height;
						}
						glColor3f(.3f,.3f,.3f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,cells[pos].height-1 - h);
							glVertex3i(i,j,cells[pos].height-1);
							glTexCoord2i(0,0);
							glVertex3i(i,j,h);
							glTexCoord2i(1,0);
							glVertex3i(i+1,j,h);
							glTexCoord2i(1,cells[pos].height-1 - h);
							glVertex3i(i+1,j,cells[pos].height-1);
						glEnd();
					}
				}

				if (i < size[0]-1)
				{
					if (cells[(j)*size[0]+(i+1)].passable || cells[pos].height > cells[(j)*size[0]+(i+1)].height)
					{
						if (!cells[(j)*size[0]+(i+1)].passable)
						{
							h = cells[(j)*size[0]+(i+1)].height;
						}
						glColor3f(.4f,.4f,.4f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,cells[pos].height-1 - h);
							glVertex3i(i+1,j,cells[pos].height-1);
							glTexCoord2i(0,0);
							glVertex3i(i+1,j,h);
							glTexCoord2i(1,0);
							glVertex3i(i+1,j+1,h);
							glTexCoord2i(1,cells[pos].height-1 - h);
							glVertex3i(i+1,j+1,cells[pos].height-1);
						glEnd();
					}
				}

				if (j < size[1]-1)
				{
					if (cells[(j+1)*size[0]+(i)].passable || cells[pos].height > cells[(j+1)*size[0]+(i)].height)
					{
						if (!cells[(j+1)*size[0]+(i)].passable)
						{
							h = cells[(j+1)*size[0]+(i)].height;
						}
						glColor3f(.7f,.7f,.7f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,cells[pos].height-1 - h);
							glVertex3i(i+1,j+1,cells[pos].height-1);
							glTexCoord2i(0,0);
							glVertex3i(i+1,j+1,h);
							glTexCoord2i(1,0);
							glVertex3i(i,j+1,h);
							glTexCoord2i(1,cells[pos].height-1 - h);
							glVertex3i(i,j+1,cells[pos].height-1);
						glEnd();
					}
				}

				if (i > 0)
				{
					if (cells[(j)*size[0]+(i-1)].passable || cells[pos].height > cells[(j)*size[0]+(i-1)].height)
					{
						if (!cells[(j)*size[0]+(i-1)].passable)
						{
							h = cells[(j)*size[0]+(i-1)].height;
						}
						glColor3f(.8f,.8f,.8f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,cells[pos].height-1 - h);
							glVertex3i(i,j+1,cells[pos].height-1);
							glTexCoord2i(0,0);
							glVertex3i(i,j+1,h);
							glTexCoord2i(1,0);
							glVertex3i(i,j,h);
							glTexCoord2i(1,cells[pos].height-1 - h);
							glVertex3i(i,j,cells[pos].height-1);
						glEnd();
					}
				}
			}

			//--- La partie du haut
			if (cells[pos].height > 1)
			{
				glBindTexture(GL_TEXTURE_2D, tex_wall_up);
				if (j > 0)
				{
					if (cells[(j-1)*size[0]+(i)].passable || cells[pos].height > cells[(j-1)*size[0]+(i)].height)
					{
						glColor3f(.3f,.3f,.3f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,1);
							glVertex3i(i,j,cells[pos].height);
							glTexCoord2i(0,0);
							glVertex3i(i,j,cells[pos].height-1);
							glTexCoord2i(1,0);
							glVertex3i(i+1,j,cells[pos].height-1);
							glTexCoord2i(1,1);
							glVertex3i(i+1,j,cells[pos].height);
						glEnd();
					}
				}

				if (i < size[0]-1)
				{
					if (cells[(j)*size[0]+(i+1)].passable || cells[pos].height > cells[(j)*size[0]+(i+1)].height)
					{
						glColor3f(.4f,.4f,.4f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,1);
							glVertex3i(i+1,j,cells[pos].height);
							glTexCoord2i(0,0);
							glVertex3i(i+1,j,cells[pos].height-1);
							glTexCoord2i(1,0);
							glVertex3i(i+1,j+1,cells[pos].height-1);
							glTexCoord2i(1,1);
							glVertex3i(i+1,j+1,cells[pos].height);
						glEnd();
					}
				}

				if (j < size[1]-1)
				{
					if (cells[(j+1)*size[0]+(i)].passable || cells[pos].height > cells[(j+1)*size[0]+(i)].height)
					{
						glColor3f(.7f,.7f,.7f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,1);
							glVertex3i(i+1,j+1,cells[pos].height);
							glTexCoord2i(0,0);
							glVertex3i(i+1,j+1,cells[pos].height-1);
							glTexCoord2i(1,0);
							glVertex3i(i,j+1,cells[pos].height-1);
							glTexCoord2i(1,1);
							glVertex3i(i,j+1,cells[pos].height);
						glEnd();
					}
				}

				if (i > 0)
				{
					if (cells[(j)*size[0]+(i-1)].passable || cells[pos].height > cells[(j)*size[0]+(i-1)].height)
					{
						glColor3f(.8f,.8f,.8f);
						glBegin(GL_QUADS);
							glTexCoord2i(0,1);
							glVertex3i(i,j+1,cells[pos].height);
							glTexCoord2i(0,0);
							glVertex3i(i,j+1,cells[pos].height-1);
							glTexCoord2i(1,0);
							glVertex3i(i,j,cells[pos].height-1);
							glTexCoord2i(1,1);
							glVertex3i(i,j,cells[pos].height);
						glEnd();
					}
				}
			}

			//--- Le top
			glBindTexture(GL_TEXTURE_2D, tex_wall_top);
			glColor3f(1,1,1);
			glBegin(GL_QUADS);
				glTexCoord2i(0,1);
				glVertex3i(i,j+1,cells[pos].height);
				glTexCoord2i(0,0);
				glVertex3i(i,j,cells[pos].height);
				glTexCoord2i(1,0);
				glVertex3i(i+1,j,cells[pos].height);
				glTexCoord2i(1,1);
				glVertex3i(i+1,j+1,cells[pos].height);
			glEnd();
		glEndList();
	}
#endif
}


void Map::regenDL()
{
	//---We create now all the wall's display list (HEaDShOt)
	int i, j;
	for (j=0;j<size[1];++j)
	{
		for (i=0;i<size[0];++i)
		{
			regenCell(i,j);
		}
	}

}


//--- To reload the theme
void Map::reloadTheme()
{
	if (tex_grass) dktDeleteTexture(&tex_grass);
	if (tex_dirt) dktDeleteTexture(&tex_dirt);
	if (tex_wall) dktDeleteTexture(&tex_wall);

/*	if (tex_floor) dktDeleteTexture(&tex_floor);
	if (tex_floor_dirt) dktDeleteTexture(&tex_floor_dirt);
	if (tex_wall_bottom) dktDeleteTexture(&tex_wall_bottom);
	if (tex_wall_center) dktDeleteTexture(&tex_wall_center);
	if (tex_wall_up) dktDeleteTexture(&tex_wall_up);
	if (tex_wall_top) dktDeleteTexture(&tex_wall_top);*/

	CString themeStr = "";

	switch (theme)
	{
	case THEME_GRASS:    themeStr = THEME_GRASS_STR;    break;
	case THEME_SNOW:     themeStr = THEME_SNOW_STR;     break;
	case THEME_SAND:     themeStr = THEME_SAND_STR;     break;
	case THEME_CITY:     themeStr = THEME_CITY_STR;     break;
	case THEME_MODERN:   themeStr = THEME_MODERN_STR;   break;
	case THEME_LAVA:     themeStr = THEME_LAVA_STR;     break;
	case THEME_ANIMAL:   themeStr = THEME_ANIMAL_STR;   break;
	case THEME_ORANGE:   themeStr = THEME_ORANGE_STR;   break;
	case THEME_CORE:     themeStr = THEME_CORE_STR;     break;
	case THEME_FROZEN:   themeStr = THEME_FROZEN_STR;   break;
	case THEME_GRAIN:    themeStr = THEME_GRAIN_STR;    break;
	case THEME_MEDIEVAL: themeStr = THEME_MEDIEVAL_STR; break;
	case THEME_METAL:    themeStr = THEME_METAL_STR;    break;
	case THEME_RAINY:    themeStr = THEME_RAINY_STR;    break;
	case THEME_REAL:     themeStr = THEME_REAL_STR;     break;
	case THEME_ROAD:     themeStr = THEME_ROAD_STR;     break;
	case THEME_ROCK:     themeStr = THEME_ROCK_STR;     break;
	case THEME_SAVANA:   themeStr = THEME_SAVANA_STR;   break;
	case THEME_SOFT:     themeStr = THEME_SOFT_STR;     break;
	case THEME_STREET:   themeStr = THEME_STREET_STR;   break;
	case THEME_TROPICAL: themeStr = THEME_TROPICAL_STR; break;
	case THEME_WINTER:   themeStr = THEME_WINTER_STR;   break;
	case THEME_WOODEN:   themeStr = THEME_WOODEN_STR;   break;
	}
	if(themeStr == "" || gameVar.cl_grassTextureForAllMaps)
	{
		theme = THEME_GRASS;
		themeStr = THEME_GRASS_STR;
	}

	// Les textures
/*	tex_grass = dktCreateTextureFromFile("main/textures/grass.tga", DKT_FILTER_BILINEAR);
	tex_dirt = dktCreateTextureFromFile("main/textures/dirt1.tga", DKT_FILTER_BILINEAR);
	tex_wall = dktCreateTextureFromFile("main/textures/dirt2.tga", DKT_FILTER_BILINEAR);*/

	tex_grass = dktCreateTextureFromFile(CString("main/textures/themes/%s/tex_floor.tga", themeStr.s).s, DKT_FILTER_BILINEAR);
	if(tex_grass == 0)
		tex_grass = dktCreateTextureFromFile(CString("main/textures/themes/grass/tex_floor.tga").s, DKT_FILTER_BILINEAR);
	tex_dirt = dktCreateTextureFromFile(CString("main/textures/themes/%s/tex_floor_dirt.tga", themeStr.s).s, DKT_FILTER_BILINEAR);
	if(tex_dirt == 0)
		tex_dirt = dktCreateTextureFromFile(CString("main/textures/themes/grass/tex_floor_dirt.tga").s, DKT_FILTER_BILINEAR);
//	tex_wall_bottom = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_bottom.tga").s, DKT_FILTER_BILINEAR);
	tex_wall = dktCreateTextureFromFile(CString("main/textures/themes/%s/tex_wall_center.tga", themeStr.s).s, DKT_FILTER_BILINEAR);
	if(tex_wall == 0)
		tex_wall = dktCreateTextureFromFile(CString("main/textures/themes/grass/tex_wall_center.tga").s, DKT_FILTER_BILINEAR);
//	tex_wall_up = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_up.tga").s, DKT_FILTER_BILINEAR);
//	tex_wall_top = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_top.tga").s, DKT_FILTER_BILINEAR);
//	tex_wall_both = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_both.tga").s, DKT_FILTER_BILINEAR);
/*
	tex_floor = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_floor.tga").s, DKT_FILTER_BILINEAR);
	tex_floor_dirt = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_floor_dirt.tga").s, DKT_FILTER_BILINEAR);
	tex_wall_bottom = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_bottom.tga").s, DKT_FILTER_BILINEAR);
	tex_wall_center = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_center.tga").s, DKT_FILTER_BILINEAR);
	tex_wall_up = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_up.tga").s, DKT_FILTER_BILINEAR);
	tex_wall_top = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_top.tga").s, DKT_FILTER_BILINEAR);
	tex_wall_both = dktCreateTextureFromFile((CString("main/textures/themes/") + themeStr + "/tex_wall_both.tga").s, DKT_FILTER_BILINEAR);
*/
	buildAll();
}
#endif



//
// Destructeur
//
Map::~Map()
{
#if defined(_PRO_)
	if (aStar) delete aStar;
#endif
#ifndef DEDICATED_SERVER
	if (dko_map) dkoDeleteModel(&dko_map);
#endif
	if (dko_mapLM) dkoDeleteModel(&dko_mapLM);

#ifndef DEDICATED_SERVER
	if (!isServer)
	{
#ifndef _DX_
		//gluDeleteQuadric(qObj);
#endif
	}

#endif
	if (isValid)
	{
		ZEVEN_SAFE_DELETE_ARRAY(cells);
	}

	dm_spawns.clear();
	blue_spawns.clear();
	red_spawns.clear();
#ifndef DEDICATED_SERVER
	if (!isServer)
	{
		ZEVEN_SAFE_DELETE(m_weather);

		dktDeleteTexture(&texMap);
		dktDeleteTexture(&tex_grass);
		dktDeleteTexture(&tex_dirt);
		dktDeleteTexture(&tex_wall);
		dktDeleteTexture(&tex_bombMark);
		
	//	dktDeleteTexture(&tex_floor);
	//	dktDeleteTexture(&tex_floor_dirt);
	//	dktDeleteTexture(&tex_wall_bottom);
	//	dktDeleteTexture(&tex_wall_center);
	//	dktDeleteTexture(&tex_wall_up);
	//	dktDeleteTexture(&tex_wall_top);

		dkoDeleteModel(&dko_flag[0]);
		dkoDeleteModel(&dko_flagPod[0]);
		dkoDeleteModel(&dko_flag[1]);
		dkoDeleteModel(&dko_flagPod[1]);

		if(groundMesh) delete groundMesh;
		if(shadowMesh) delete shadowMesh;
		if(wallMesh) delete wallMesh;
	}
#endif
}



#ifndef DEDICATED_SERVER
//
// Pour générer la texture de la minimap
//
void Map::regenTex()
{
    int i, j;

	texMapSize[0] = 1.0f;
	texMapSize[1] = 1.0f;
	int width = 1;
	while(width < size[0])
	{
		width *= 2;
	}
	if(width != size[0])
	{
		texMapSize[0] = float(size[0]) / float(width);
	}
	int height = 1;
	while(height < size[1])
	{
		height *= 2;
	}
	if(height != size[1])
	{
		texMapSize[1] = float(size[1]) / float(height);
	}
	unsigned char * textureData = new unsigned char [width*height*3];
	memset(textureData, 0, width*height*3);
	// On rempli le data dans la texture
	for (j=0;j < height;++j)
	{
		for (i=0;i < width;++i)
		{
			if((i < size[0]) && (j < size[1]))
			{
				if (!cells[j*size[0]+i].passable)
				{
					textureData[(j*width+i)*3+0] = 255;
					textureData[(j*width+i)*3+1] = 255;
					textureData[(j*width+i)*3+2] = 255;
				}
			}
		}
	}

	dktCreateTextureFromBuffer(&texMap, textureData, width, height, 3, DKT_FILTER_NEAREST);

	// On efface notre data tempon
	delete [] textureData;

	//--- Rebuild map
	buildAll();
}



//
// Pour seter la position de la cam
//
void Map::setCameraPos(const CVector3f & pCamPos)
{
	camLookAt = pCamPos;
	camDest = camLookAt + CVector3f(0, 0, 7);
	camPos = camDest;
}
#endif

#ifdef RENDER_LAYER_TOGGLE
	int renderToggle = 0;
#endif

#ifndef DEDICATED_SERVER
//
// Update
//
void Map::update(float delay, Player * thisPlayer)
{
	#ifdef RENDER_LAYER_TOGGLE
		if (dkiGetState(DIK_F5) == 1)
		{
			renderToggle++;
			if (renderToggle > 16)
			{
				renderToggle = 0;
			}
		}
		if (dkiGetState(DIK_F6) == 1)
		{
			renderToggle = 16;
		}
	#endif

	if (introAnim < introAnimLenght && dko_cam) 
	{
		introAnim++;
		if (introAnim == introAnimLenght)
		{
			//...
		}
	}

	if (m_weather) m_weather->update(delay, this);

	updateDummies();

	// Snipers should be able to scope at map edges
	if (isEditor || (thisPlayer && thisPlayer->weapon && thisPlayer->weapon->weaponID != WEAPON_SNIPER) || (thisPlayer && thisPlayer->teamID == PLAYER_TEAM_SPECTATOR))
	{
		if (camLookAt[0] < 5) camLookAt[0] = 5;
		if (camLookAt[0] > (float)size[0]-5) camLookAt[0] = (float)size[0]-5;
		if (camLookAt[1] < 4) camLookAt[1] = 4;
		if (camLookAt[1] > (float)size[1]-4) camLookAt[1] = (float)size[1]-4;
	}

	camDest = camLookAt + CVector3f(0, 0, 7);
	if (thisPlayer)
	{
		if (thisPlayer->status == PLAYER_STATUS_ALIVE)
		{
			if (thisPlayer->weapon)
			{
				if (thisPlayer->weapon->weaponID == WEAPON_SNIPER)
				{
					//--- Dépendament de la distance entre la mouse et le player
					float dis = distance(thisPlayer->currentCF.mousePosOnMap, thisPlayer->currentCF.position) * 2;
					if (dis > 12) dis = 12;
					if (dis < 5) dis = 5;
					camDest = camLookAt + CVector3f(0, 0, dis); // On voit de plus loin pour mieux sniper ;)
				}
			}
		}
		else if (thisPlayer->status == PLAYER_STATUS_DEAD)
		{
		}

		if (thisPlayer->teamID == PLAYER_TEAM_SPECTATOR)
		{
			// Update zoom
			int longestSide = (size[0]>size[1])?size[0]:size[1];;
			zoom += -dkiGetMouseWheelVel()*0.01f;
			zoom = (zoom < -8)?-8:zoom;
			zoom = (zoom > longestSide/2)?longestSide/2:zoom;
			
			// Set camera
			camDest = camLookAt + CVector3f(0, 0, 14 + zoom); // On voit de plus loin pour mieux bombarder lol (joke)
		}
	}
	else if(isEditor)
	{
		// Update zoom
		int longestSide = (size[0]>size[1])?size[0]:size[1];;
		zoom += -dkiGetMouseWheelVel()*0.01f;
		zoom = (zoom < -8)?-8:zoom;
		zoom = (zoom > longestSide/2)?longestSide/2:zoom;
		
		// Set camera
		camDest = camLookAt + CVector3f(0, 0, 14 + zoom); // On voit de plus loin pour mieux bombarder lol (joke)
	}
	else
	{
		// Hum, on est dans l'editeur
		camDest = camLookAt + CVector3f(0, 0, 14);
	}
	CVector3f lastCamPos = camPos;
	camPos += (camDest - camPos) * 2.5f * delay;

	// On anim les flag
	flagAnim += delay * 10;
	while (flagAnim>=10) flagAnim-= 10;

		if (flagState[0] == -2) flagPos[0] = flagPodPos[0];
		if (flagState[1] == -2) flagPos[1] = flagPodPos[1];
		if (flagState[0] >= 0) if (game->players[flagState[0]]) flagPos[0] = game->players[flagState[0]]->currentCF.position;
		if (flagState[1] >= 0) if (game->players[flagState[1]]) flagPos[1] = game->players[flagState[1]]->currentCF.position;

/*
	CVector3f newAccel[2];
	for (int i=0;i<2;++i)
	{
		newAccel[i] = (flagPos[i] - flagLastPos[i]) / delay;
		flagLastAccel[i] = newAccel[i];
		flagLastPos[i] = flagPos[i];

		if (flagLastAccel[i].length() > 3.0f)
		{
			//--- Determine angle
			if (flagAnims[i] > 10)
			{
				CVector3f dir = -flagLastAccel[i];
				normalize(dir);
				float angleX = dot(dir, CVector3f(1,0,0));
				float angleY = dot(dir, CVector3f(0,1,0));
				flagAngle[i] = acosf(angleX) * TO_DEGREE;
				if (angleY < 0) flagAngle[i] = 180 + (180 - flagAngle[i]);

				flagAnims[i] = 0;
			}
		}
		else if (flagLastAccel[i].length() > 1.5f)
		{
			//--- Determine angle
			if (flagAnims[i] > 18)
			{
				//--- Determine angle
				CVector3f dir = -flagLastAccel[i];
				normalize(dir);
				float angleX = dot(dir, CVector3f(1,0,0));
				float angleY = dot(dir, CVector3f(0,1,0));
				flagAngle[i] = acosf(angleX) * TO_DEGREE;
				if (angleY < 0) flagAngle[i] = 180 + (180 - flagAngle[i]);

				flagAnims[i] = 11;
			}
		}

		if (flagAnims[i] < 25)
		{
			flagAnims[i] += delay * 10;
			if (flagAnims[i] >= 25) flagAnims[i] = 25;
		}
	}*/

/*	if (game)
	{
		if (game->gameType == GAME_TYPE_CTF)
		{
			//--- Get dummy position
			CVector3f dumPos;
			dkoGetDummyPosition("Dummy01", dko_flagPole, dumPos.s, (short)flagAnims[0]);
			dumPos *= .005f; // Scale
			dumPos = rotateAboutAxis(dumPos, flagAngle[0], CVector3f(0,0,1)); // Rotate
			dumPos += flagPos[0]; // Translate

			//--- Spawn particule for the flag
			dkpCreateParticle(	dumPos.s,//float *position,
								CVector3f(0,0,1).s,//float *vel,
								CVector4f(.2f,.2f,1,1.0f).s,//float *startColor,
								CVector4f(.2f,.2f,1,0.0f).s,//float *endColor,
								0,//float startSize,
								.5f,//float endSize,
								.5f,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								360,//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE,//unsigned int dstBlend,
								0);//int transitionFunc);

			// Red flag
			dkoGetDummyPosition("Dummy01", dko_flagPole, dumPos.s, (short)flagAnims[1]);
			dumPos *= .005f; // Scale
			dumPos = rotateAboutAxis(dumPos, flagAngle[1], CVector3f(0,0,1)); // Rotate
			dumPos += flagPos[1]; // Translate

			//--- Spawn particule for the flag
			dkpCreateParticle(	dumPos.s,//float *position,
								CVector3f(0,0,0).s,//float *vel,
								CVector4f(1,.2f,.2f,1.0f).s,//float *startColor,
								CVector4f(1,.2f,.2f,0.0f).s,//float *endColor,
								0,//float startSize,
								.5f,//float endSize,
								.5f,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								360,//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE,//unsigned int dstBlend,
								0);//int transitionFunc);
		}
	}*/
}
#endif


//-- Dummies are visual only
void Map::renderDummies()
{
}

void Map::updateDummies()
{
    int i;
	for (i=0;i<(int)dummies.size();++i)
	{
		switch (dummies[i].type)
		{
		case DUMMY_TYPE_FLAME:
			if (!isServer)
			{
#ifndef DEDICATED_SERVER
				dummies[i].spawnParticleTime++;
				if (dummies[i].spawnParticleTime >= 30) dummies[i].spawnParticleTime = 0;

				if (dummies[i].spawnParticleTime%3 == 0)
				{
					// On spawn des particules de feu dans son cul (une par frame)
					dkpCreateParticle(	(dummies[i].position+rand(CVector3f(-.10f,-.10f,0),CVector3f(.10f,.10f,0))).s,//float *position,
										(CVector3f(0,0,1)).s,//float *vel,
										rand(CVector4f(1,0,0,0.0f),CVector4f(1,.75f,0,0.0f)).s,//float *startColor,
										CVector4f(1,.75f,0,1.0f).s,//float *endColor,
										.3f,//float startSize,
										.0f,//float endSize,
										1.0f,//float duration,
										0,//float gravityInfluence,
										0,//float airResistanceInfluence,
										rand(0.0f, 30.0f),//float rotationSpeed,
										gameVar.tex_smoke1,//unsigned int texture,
										DKP_SRC_ALPHA,//unsigned int srcBlend,
										DKP_ONE,//unsigned int dstBlend,
										0);//int transitionFunc);
					// On spawn des particules de feu dans son cul (une par frame)
					dkpCreateParticle(	(dummies[i].position+rand(CVector3f(-.10f,-.10f,0),CVector3f(.10f,.10f,0))).s,//float *position,
										(CVector3f(0,0,1)+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *vel,
										rand(CVector4f(1,0,0,1.0f),CVector4f(1,.75f,0,1.0f)).s,//float *startColor,
										CVector4f(1,.75f,0,0.0f).s,//float *endColor,
										.0f,//float startSize,
										.3f,//float endSize,
										0.5f,//float duration,
										0,//float gravityInfluence,
										0,//float airResistanceInfluence,
										rand(0.0f, 30.0f),//float rotationSpeed,
										gameVar.tex_smoke1,//unsigned int texture,
										DKP_SRC_ALPHA,//unsigned int srcBlend,
										DKP_ONE,//unsigned int dstBlend,
										0);//int transitionFunc);
				}
#endif
			}
			break;
		}
	}
}


#define TEST_DIR_X 0
#define TEST_DIR_X_NEG 1
#define TEST_DIR_Y 2
#define TEST_DIR_Y_NEG 3

//
// Pour faire un ray tracing
//
bool Map::rayTest(CVector3f & p1, CVector3f & p2, CVector3f & normal)
{
	if (dko_mapLM)
	{
		//--- Simple dko ray test
		int n;
		CVector3f intersect;
		p1 *= 10;
		p2 *= 10;
		bool result = dkoRayIntersection(dko_mapLM, p1.s, p2.s, intersect.s, normal.s, n);
		if (result)
		{
			p2 = intersect;
		}
		p1 *= .1f;
		p2 *= .1f;

		//--- Check for the floor now if the dko collision failed for it
		if (p1[2] > 0 && p2[2] <= 0)
		{
			float percent = p1[2] / fabsf(p2[2] - p1[2]);
			intersect = p1 + (p2 - p1) * percent;
			p2 = intersect;
			normal.set(0,0,1);
			result = true;
		}

		return result;
	}

	// On pogne notre cell de départ
	int i = (int)p1[0];
	int j = (int)p1[1];

	// On check que notre tuile n'est pas déjà occupé
	if (i >= 0 &&
		i < size[0] &&
		j >= 0 &&
		j < size[1])
	{
		if (!cells[j*size[0]+i].passable && p1[2] < cells[j*size[0]+i].height) 
		{
			p2 = p1;
			return true;
		}
	}
	else
	{
		return false;
	}


	// On défini dans quel sens on va voyager (4 sens)
	int sens;
	if (fabsf(p2[0] - p1[0]) > fabsf(p2[1] - p1[1]))
	{
		if (p2[0] > p1[0])
		{
			sens = TEST_DIR_X;
		}
		else
		{
			sens = TEST_DIR_X_NEG;
		}
	}
	else
	{
		if (p2[1] > p1[1])
		{
			sens = TEST_DIR_Y;
		}
		else
		{
			sens = TEST_DIR_Y_NEG;
		}
	}

	// On while tant qu'on ne l'a pas trouvé
	float percent;
	while (true)
	{
		// On check qu'on n'a pas dépassé
		if (i < 0 ||
			i >= size[0] ||
			j < 0 ||
			j >= size[1] ||
			(sens == TEST_DIR_X && i > (int)p2[0]) ||
			(sens == TEST_DIR_X_NEG && i < (int)p2[0]) ||
			(sens == TEST_DIR_Y && j > (int)p2[1]) ||
			(sens == TEST_DIR_Y_NEG && j < (int)p2[1]))
		{
			return false;
		}

		switch (sens)
		{
		case TEST_DIR_X:
			// On test nos 3 tuiles
			if (rayTileTest(i, j, p1, p2, normal)) return true;
			if (rayTileTest(i, j-1, p1, p2, normal)) return true;
			if (rayTileTest(i, j+1, p1, p2, normal)) return true;

			// On incrémente à la prochaine tuile
			i++;
			percent = ((float)i - p1[0]) / fabsf(p2[0] - p1[0]);
			j = (int)(p1[1] + (p2[1] - p1[1]) * percent);
			break;

		case TEST_DIR_X_NEG:
			// On test nos 3 tuiles
			if (rayTileTest(i, j, p1, p2, normal)) return true;
			if (rayTileTest(i, j-1, p1, p2, normal)) return true;
			if (rayTileTest(i, j+1, p1, p2, normal)) return true;

			// On incrémente à la prochaine tuile
			i--;
			percent = (p1[0] - (float)(i+1)) / fabsf(p2[0] - p1[0]);
			j = (int)(p1[1] + (p2[1] - p1[1]) * percent);
			break;

		case TEST_DIR_Y:
			// On test nos 3 tuiles
			if (rayTileTest(i, j, p1, p2, normal)) return true;
			if (rayTileTest(i-1, j, p1, p2, normal)) return true;
			if (rayTileTest(i+1, j, p1, p2, normal)) return true;

			// On incrémente à la prochaine tuile
			j++;
			percent = ((float)j - p1[1]) / fabsf(p2[1] - p1[1]);
			i = (int)(p1[0] + (p2[0] - p1[0]) * percent);
			break;

		case TEST_DIR_Y_NEG:
			// On test nos 3 tuiles
			if (rayTileTest(i, j, p1, p2, normal)) return true;
			if (rayTileTest(i-1, j, p1, p2, normal)) return true;
			if (rayTileTest(i+1, j, p1, p2, normal)) return true;

			// On incrémente à la prochaine tuile
			j--;
			percent = (p1[1] - (float)(j+1)) / fabsf(p2[1] - p1[1]);
			i = (int)(p1[0] + (p2[0] - p1[0]) * percent);
			break;
		}
	}

	return false;
}

#ifndef DEDICATED_SERVER

void GetMapList(std::vector< CString > & maps)
{
	maps.clear();
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH];  // directory specification
	char appPath[MAX_PATH];

	// Chercher le path du "current working directory".
	_getcwd(appPath, MAX_PATH);

	strncpy(DirSpec, appPath, strlen(appPath) + 1);
	strncat(DirSpec, "\\main\\maps\\*.bvm", strlen("\\main\\maps\\*.bvm") + 1);

	hFind = FindFirstFile(DirSpec, &FindFileData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		CString filename ("%s", FindFileData.cFileName);
		// Drop the extension
		filename.resize(filename.len() - 4);
		maps.push_back(filename);
		while(FindNextFile(hFind, &FindFileData) != 0)
		{
			CString filename ("%s", FindFileData.cFileName);
			// Drop the extension
			filename.resize(filename.len() - 4);
			maps.push_back(filename);
		}
		FindClose(hFind);
	}
}

bool GetMapData(CString name, unsigned int & texture, CVector2i & textureSize, CVector2i & size, CString & author)
{
	FileIO file(CString("main\\maps\\%s.bvm", name.s), "rb");
	if(file.isValid())
	{
		map_cell * cells = 0;
		unsigned long mapVersion = file.getULong();

		switch (mapVersion)
		{
		case 10010:
			size[0] = file.getInt();
			size[1] = file.getInt();
			cells = new map_cell[size[0] * size[1]];
			for (int j = 0; j < size[1]; ++j)
			{
				for (int i = 0; i < size[0]; ++i)
				{
					unsigned char data = file.getUByte();
					cells[j * size[0] + i].passable = (data & 128) ? true : false;
					cells[j * size[0] + i].height = (data & 127);
					data = file.getUByte();
				}
			}
			break;
		case 10011:
			size[0] = file.getInt();
			size[1] = file.getInt();
			cells = new map_cell[size[0] * size[1]];
			for (int j = 0; j < size[1]; ++j)
			{
				for (int i = 0; i < size[0]; ++i)
				{
					unsigned char data = file.getUByte();
					cells[j * size[0] + i].passable = (data & 128) ? true : false;
					cells[j * size[0] + i].height = (data & 127);
					data = file.getUByte();
				}
			}
			break;
		case 20201:
			{
				int theme = file.getInt();
				int weather = file.getInt();
				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0] * size[1]];
				for (int j = 0; j < size[1]; ++j)
				{
					for (int i = 0; i < size[0]; ++i)
					{
						unsigned char data = file.getUByte();
						cells[j * size[0] + i].passable = (data & 128) ? true : false;
						cells[j * size[0] + i].height = (data & 127);
						data = file.getUByte();
					}
				}
				break;
			}
		case 20202:
			{
				char * author_name_buffer = file.getByteArray(25);
				author_name_buffer[24] = '\0';
				author.set("%s", author_name_buffer);
				delete[] author_name_buffer;
				int theme = file.getInt();
				int weather = file.getInt();
				size[0] = file.getInt();
				size[1] = file.getInt();
				cells = new map_cell[size[0] * size[1]];
				for (int j = 0; j < size[1]; ++j)
				{
					for (int i = 0; i < size[0]; ++i)
					{
						unsigned char data = file.getUByte();
						cells[j * size[0] + i].passable = (data & 128) ? true : false;
						cells[j * size[0] + i].height = (data & 127);
						data = file.getUByte();
					}
				}
				break;
			}
		}

		textureSize.set(1, 1);
		while(textureSize[0] < size[0])
		{
			textureSize[0] *= 2;
		}
		while(textureSize[1] < size[1])
		{
			textureSize[1] *= 2;
		}
		unsigned char * textureBytes = new unsigned char[textureSize[0] * textureSize[1] * 3];
		memset(textureBytes, 0, textureSize[0] * textureSize[1] * 3);
		for(int j = 0; j < size[1]; ++j)
		{
			for(int i = 0; i < size[0]; ++i)
			{
				unsigned int m = (j * size[0] + i);
				unsigned int t = (j * textureSize[0] + i) * 3;
				if(!cells[m].passable)
				{
					textureBytes[t + 0] = 200;
					textureBytes[t + 1] = 200;
					textureBytes[t + 2] = 200;
				}
			}
		}
		delete[] cells;
		texture = dktCreateEmptyTexture(textureSize[0], textureSize[1], 3, DKT_FILTER_NEAREST);
		dktCreateTextureFromBuffer(&texture, textureBytes, textureSize[0], textureSize[1], 3, DKT_FILTER_NEAREST);
		delete[] textureBytes;
	}
	else
	{
		return false;
	}
	return true;
}

#endif

bool IsMapValid(const Map & map, int gameType)
{
	bool isGoodMap = true;
	//--- game type specific map-check
	switch(gameType)
	{
	case GAME_TYPE_DM: 
	case GAME_TYPE_TDM:
		// there must be at least 1 item in dm_spawns
		isGoodMap = (map.dm_spawns.size() >= 1);
		break;
	case GAME_TYPE_CTF:
		// there must be at least 1 item in dm_spawns,
		// both flags must be set
		isGoodMap = (map.dm_spawns.size() >= 1)
			&& (map.flagPodPos[0] != CVector3f(0.0f, 0.0f, 0.0f))
			&& (map.flagPodPos[1] != CVector3f(0.0f, 0.0f, 0.0f));
		break;
	case GAME_TYPE_SND:
		// there must be at least 1 item in blue_spawns
		// there must be at least 1 item in red_spawns
		// both objectives (bombs) must be set
#if defined(_PRO_)
      isGoodMap = (map.dm_spawns.size() >= 1);
#else
		isGoodMap = (map.blue_spawns.size() >= 1) && (map.red_spawns.size() >= 1)
			&& (map.objective[0] != CVector3f(0.0f, 0.0f, 0.0f))
			&& (map.objective[1] != CVector3f(0.0f, 0.0f, 0.0f));
#endif
		break;
	}
	return isGoodMap;
}
