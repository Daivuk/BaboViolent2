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

#ifndef MAP_H
#define MAP_H


#include "Zeven.h"
#include <vector>
#include "GameVar.h"
#include "Player.h"
#ifdef _PRO_
#include "CAStar.h"
#endif

#ifndef DEDICATED_SERVER
#include "CMeshBuilder.h"
#include "CWeather.h"

#define SHADOW_DETAIL 32

#define THEME_START    0
// Classic themes
#define THEME_GRASS    (THEME_START +  0)
#define THEME_SNOW     (THEME_START +  1)
#define THEME_SAND     (THEME_START +  2)
#define THEME_CITY     (THEME_START +  3)
#define THEME_MODERN   (THEME_START +  4)
#define THEME_LAVA     (THEME_START +  5)
#define THEME_ANIMAL   (THEME_START +  6)
#define THEME_ORANGE   (THEME_START +  7)
// Pacifist's themes (WOW!)
#define THEME_CORE     (THEME_START +  8)
#define THEME_FROZEN   (THEME_START +  9)
#define THEME_GRAIN    (THEME_START + 10)
#define THEME_MEDIEVAL (THEME_START + 11)
#define THEME_METAL    (THEME_START + 12)
#define THEME_RAINY    (THEME_START + 13)
#define THEME_REAL     (THEME_START + 14)
#define THEME_ROAD     (THEME_START + 15)
#define THEME_ROCK     (THEME_START + 16)
#define THEME_SAVANA   (THEME_START + 17)
#define THEME_SOFT     (THEME_START + 18)
#define THEME_STREET   (THEME_START + 19)
#define THEME_TROPICAL (THEME_START + 20)
#define THEME_WINTER   (THEME_START + 21)
#define THEME_WOODEN   (THEME_START + 22)
// THEME_END must always be the same as the last theme number!
#define THEME_END      THEME_WOODEN

#define THEME_GRASS_STR    "grass"
#define THEME_SNOW_STR     "snow"
#define THEME_SAND_STR     "sand"
#define THEME_CITY_STR     "city"
#define THEME_MODERN_STR   "modern"
#define THEME_LAVA_STR     "lava"
#define THEME_ANIMAL_STR   "animal"
#define THEME_ORANGE_STR   "orange"
#define THEME_CORE_STR     "core"
#define THEME_FROZEN_STR   "frozen"
#define THEME_GRAIN_STR    "grain"
#define THEME_MEDIEVAL_STR "medieval"
#define THEME_METAL_STR    "metal"
#define THEME_RAINY_STR    "rainy"
#define THEME_REAL_STR     "real"
#define THEME_ROAD_STR     "road"
#define THEME_ROCK_STR     "rock"
#define THEME_SAVANA_STR   "savana"
#define THEME_SOFT_STR     "soft"
#define THEME_STREET_STR   "street"
#define THEME_TROPICAL_STR "tropical"
#define THEME_WINTER_STR   "winter"
#define THEME_WOODEN_STR   "wooden"

#define WEATHER_NONE      0
#define WEATHER_FOG       1
#define WEATHER_SNOW      2
#define WEATHER_RAIN      3
#define WEATHER_SANDSTORM 4
#define WEATHER_LAVA      5
#endif

#define COLLISION_EPSILON 0.05f
#define BOUNCE_FACTOR 0.45f

#define MAP_VERSION 20202

#define FLAG_BLUE 0
#define FLAG_RED 1


class Game;


struct mapInfo
{
	CString mapName;
	int mapArea;
	int lastPlayed;
};

struct map_cell
{
	// La liste d'objet que cette cellule contient
	bool passable;

	// Notre splater
	float splater[4];

	// Sa hauteur
	int height;

#ifndef DEDICATED_SERVER
	unsigned int dl;
#endif

	map_cell()
	{
		splater[0] = 0;
		splater[1] = 0;
		splater[2] = 0;
		splater[3] = 0;
		passable = true;
		height = 1; // Par default un wall est 1 de haut
#ifndef DEDICATED_SERVER
		dl = 0;
#endif
	}

	virtual ~map_cell()
	{
#ifndef DEDICATED_SERVER
#ifndef _DX_
		if (dl) glDeleteLists(dl, 1);
#endif
#endif
	}
};



#define DUMMY_TYPE_FLAME 0
struct SMapDummy
{
	CVector3f position;
	int type;
	int spawnParticleTime;
	SMapDummy(CVector3f in_position, int in_type)
	{
		spawnParticleTime = 0;
		position = in_position;
		type = in_type;
	}
};


class Map
{
public:
	// Sa grosseur
	CVector2i size;
#ifdef _PRO_

	CAStar * aStar;
#endif
	// Ses cells
	map_cell * cells;
#ifndef DEDICATED_SERVER
	// La position de la camera
	CVector3f camPos;

	// Où elle regarde
	CVector3f camLookAt;

	// La destination de la camera
	CVector3f camDest;
	// Pour dessiner notre sphere
#ifndef _DX_
	//GLUquadricObj* qObj;
#endif

	CVector4f fogColor;
	float fogDensity;
	float fogStart;
	float fogEnd;

	// Textures
	unsigned int tex_grass;
	unsigned int tex_dirt;
	unsigned int tex_wall;

	unsigned int tex_bombMark;

	unsigned int tex_floor;
	unsigned int tex_floor_dirt;
	unsigned int tex_wall_bottom;
	unsigned int tex_wall_center;
	unsigned int tex_wall_up;
	unsigned int tex_wall_top;
	unsigned int tex_wall_both;

	// La texture de la map (la meme en tant que tel !)
	unsigned int texMap;
	CVector2f texMapSize;

	unsigned int dko_map;

	unsigned int dko_cam;
	int introAnim;
	int introAnimLenght;

	// [PM] Author name
	CString author_name;

	//--- Son thème
	int theme;
	int weather;
	CWeather * m_weather;

	// [NUV] Display Lists
	bool listCreated[3];
	unsigned int displayList[3];

	// Spec Cam Controls
	float		zoom;

#endif
	unsigned int dko_mapLM;

	CString mapName;

	bool isValid;
	bool isServer;
#ifndef DEDICATED_SERVER
	bool isEditor;
#endif

	// Les deux flag point
	CVector3f flagPodPos[2];
#ifndef DEDICATED_SERVER
	float flagAnim;
	unsigned int dko_flag[2];
	unsigned int dko_flagPod[2];
	unsigned int dko_flagPole;
	float flagAnims[2];
	float flagAngle[2];

	//--- Meshes
	CMesh* groundMesh;
	CMesh* shadowMesh;
	CMesh* wallMesh;
#endif
	CVector3f flagPos[2];
	CVector3f flagLastPos[2];
	CVector3f flagLastAccel[2];
	CVector3f flagBendOffset[2];
	char flagState[2]; // -1 = par terre, -2 = on pod, 0+ on player

	// Les objectifs
	CVector3f objective[2];

	// Les spawn point
	std::vector<CVector3f> dm_spawns;
	std::vector<CVector3f> blue_spawns;
	std::vector<CVector3f> red_spawns;

	// Les dummies
	std::vector<SMapDummy> dummies;

	Game * game;

public:
	// Constructeur
	Map(CString mapFilename, Game * game, unsigned int font, bool editor=false, int sizeX=32, int sizeY=32);

	// Destructeur
	virtual ~Map();

	// Update
	void update(float delay, Player * thisPlayer);

	// render
#ifndef DEDICATED_SERVER
	void buildAll();

	void buildGround();
	void buildGroundLayer(CMeshBuilder& builder, bool splat = false);
	void buildShadow();
	void buildWalls();

	// builds a wall block at the specified x,y coordinates with height h
	void buildWallBlock(CMeshBuilder& builder,int x,int y,int h);
	void buildWallTop(CMeshBuilder& builder,float* vert1, float* vert2, float* vert3, float* vert4, bool top, bool left, bool diag);

	//given 4 vertices, a brightness indicator, and the height of the wall side, builds the quad
	void buildWallSide(CMeshBuilder& builder,float*,float*,float*,float*,float brightness,float h);

	void renderGround();
	void renderShadow();
	void renderWalls();
	
	void renderMisc();
	void renderBombMark();
	void renderFlag(int index);

	// Pour seter la position de la cam
	void setCameraPos(const CVector3f & pCamPos);
#endif

	//-- Dummies are visual only
	void renderDummies();
	void updateDummies();

	// Pour faire une collision sur un coordFrame
	void performCollision(CoordFrame & lastCF, CoordFrame & coordFrame, float radius);
	void collisionClip(CoordFrame & coordFrame, float radius);

	// Pour faire un ray tracing
	bool rayTest(CVector3f & p1, CVector3f & p2, CVector3f & normal);

	// Pour générer la texture de la minimap
#ifndef DEDICATED_SERVER
	void regenTex();
#endif

	// Pour ajouter du dirt
	inline void setTileDirt(int x, int y, float value)
	{
		if (x < 0) return;
		if (y < 0) return;
		if (x >= size[0]) return;
		if (y >= size[1]) return;

		cells[y*size[0]+x].splater[1] = value;

		// Ses voisins qui sharent ce vertex
		if (x > 0)
		{
			cells[y*size[0]+x-1].splater[2] = cells[y*size[0]+x].splater[1];
			if (y > 0)
			{
				cells[(y-1)*size[0]+x-1].splater[3] = cells[y*size[0]+x].splater[1];
			}
		}
		if (y > 0)
		{
			cells[(y-1)*size[0]+x].splater[0] = cells[y*size[0]+x].splater[1];
		}
	}
	inline void addTileDirt(int x, int y, float value)
	{
		if (x < 0) return;
		if (y < 0) return;
		if (x >= size[0]) return;
		if (y >= size[1]) return;

		cells[y*size[0]+x].splater[1] += value;
		if (cells[y*size[0]+x].splater[1] > 1) cells[y*size[0]+x].splater[1] = 1;

		// Ses voisins qui sharent ce vertex
		if (x > 0)
		{
			cells[y*size[0]+x-1].splater[2] = cells[y*size[0]+x].splater[1];
			if (y > 0)
			{
				cells[(y-1)*size[0]+x-1].splater[3] = cells[y*size[0]+x].splater[1];
			}
		}
		if (y > 0)
		{
			cells[(y-1)*size[0]+x].splater[0] = cells[y*size[0]+x].splater[1];
		}
	}
	inline void removeTileDirt(int x, int y, float value)
	{
		if (x < 0) return;
		if (y < 0) return;
		if (x >= size[0]) return;
		if (y >= size[1]) return;

		cells[y*size[0]+x].splater[1] -= value;
		if (cells[y*size[0]+x].splater[1] < 0) cells[y*size[0]+x].splater[1] = 0;

		// Ses voisins qui sharent ce vertex
		if (x > 0)
		{
			cells[y*size[0]+x-1].splater[2] = cells[y*size[0]+x].splater[1];
			if (y > 0)
			{
				cells[(y-1)*size[0]+x-1].splater[3] = cells[y*size[0]+x].splater[1];
			}
		}
		if (y > 0)
		{
			cells[(y-1)*size[0]+x].splater[0] = cells[y*size[0]+x].splater[1];
		}
	}

	// Pour tester une tuile (inline celle là)
	inline bool rayTileTest(int x, int y, CVector3f & p1, CVector3f & p2, CVector3f & normal)
	{
		if (x>=0 && x<size[0] && y>=0 && y<size[1])
		{
			float x1 = (float)x;
			float x2 = (float)x+1;
			float y1 = (float)y;
			float y2 = (float)y+1;
			float percent;
			float height = (float)cells[y*size[0]+x].height;
			CVector3f p;

			if (cells[y*size[0]+x].passable) 
			{
				// On check juste si on pogne le plancher !
				if (p1[2] > 0 && p2[2] <= 0)
				{
					percent = p1[2] / fabsf(p2[2] - p1[2]);
					p = p1 + (p2 - p1) * percent;
					if (p[0] >= x1 && p[0] <= x2 &&
						p[1] >= y1 && p[1] <= y2)
					{
						p2 = p;
						normal.set(0,0,1);
						return true;
					}
					return false;
				}
				else
				{
					return false;
				}
			}

			if (!cells[y*size[0]+x].passable) 
			{
				// On check si on pogne le plafond
				if (p1[2] > height && p2[2] <= height)
				{
					percent = (p1[2]-height) / fabsf((p2[2]-height) - (p1[2]-height));
					p = p1 + (p2 - p1) * percent;
					if (p[0] >= x1 && p[0] <= x2 &&
						p[1] >= y1 && p[1] <= y2)
					{
						p2 = p;
						normal.set(0,0,1);
						return true;
					}
				}
			}

			// Le côté x1 en premier
			if (p1[0] <= x1 && p2[0] > x1)
			{
				percent = fabsf(x1 - p1[0]) / fabsf(p2[0] - p1[0]);
				p = p1 + (p2 - p1) * percent;
				if (p[1] <= y2 && p[1] >= y1 && p[2] < height)
				{
					p2 = p;
					normal.set(-1,0,0);
					return true;
				}
			}

			// Le côté oposé
			if (p1[0] >= x2 && p2[0] < x2)
			{
				percent = fabsf(p1[0] - x2) / fabsf(p2[0] - p1[0]);
				p = p1 + (p2 - p1) * percent;
				if (p[1] <= y2 && p[1] >= y1 && p[2] < height)
				{
					p2 = p;
					normal.set(1,0,0);
					return true;
				}
			}

			// Le côté y1
			if (p1[1] <= y1 && p2[1] > y1)
			{
				percent = fabsf(y1 - p1[1]) / fabsf(p2[1] - p1[1]);
				p = p1 + (p2 - p1) * percent;
				if (p[0] <= x2 && p[0] >= x1 && p[2] < height)
				{
					p2 = p;
					normal.set(0,-1,0);
					return true;
				}
			}

			// Le côté oposé
			if (p1[1] >= y2 && p2[1] < y2)
			{
				percent = fabsf(p1[1] - y2) / fabsf(p2[1] - p1[1]);
				p = p1 + (p2 - p1) * percent;
				if (p[0] <= x2 && p[0] >= x1 && p[2] < height)
				{
					p2 = p;
					normal.set(0,1,0);
					return true;
				}
			}

		}

		return false;
	}

#ifndef DEDICATED_SERVER
	//--- To reload the theme
	void reloadTheme();
	void regenDL();
	void regenCell(int i, int j);
	void reloadWeather();
#endif
};

// Returns the names of all existing maps
void GetMapList(std::vector< CString > & maps);

// Returns basic info about the map
bool GetMapData(CString name, unsigned int & texture, CVector2i & textureSize, CVector2i & size, CString & author);

// Tells if a map has everything that is needed for a given game-type
bool IsMapValid(const Map & map, int gameType);

#endif


