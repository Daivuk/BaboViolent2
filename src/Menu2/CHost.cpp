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

#include "CHost.h"
#include <direct.h>
#include "Map.h"
#include "FileIO.h"
#include "Scene.h"
#include "Console.h"


extern Scene * scene;


CVector2i g_size;
CVector2f g_sizeReal;


unsigned int loadMapPreview(CString mapFilename)
{
	if (mapFilename.len() > 15) mapFilename.resize(15);

	// Ses cells
	map_cell * cells = 0;

	// On essaye d'abords de lire la map
	FileIO file(CString("main/maps/%s.bvm", mapFilename.s), "rb");
	if (!file.isValid())
	{
		return 0;
	}
	else
	{
		// On le load ici
		unsigned long mapVersion = file.getULong();

		switch (mapVersion)
		{
		case 10010:
			g_size[0] = file.getInt();
			g_size[1] = file.getInt();
			cells = new map_cell[g_size[0]*g_size[1]];
			for (int j=0;j<g_size[1];++j)
			{
				for (int i=0;i<g_size[0];++i)
				{
					unsigned char data = file.getUByte();
					cells[j*g_size[0]+i].passable = (data & 128)?true:false;
					cells[j*g_size[0]+i].height = (data & 127);
					data = file.getUByte();
				}
			}
			break;
		case 10011:
			g_size[0] = file.getInt();
			g_size[1] = file.getInt();
			cells = new map_cell[g_size[0]*g_size[1]];
			for (int j=0;j<g_size[1];++j)
			{
				for (int i=0;i<g_size[0];++i)
				{
					unsigned char data = file.getUByte();
					cells[j*g_size[0]+i].passable = (data & 128)?true:false;
					cells[j*g_size[0]+i].height = (data & 127);
					data = file.getUByte();
				}
			}
			break;
		case 20201:
			{
				int theme = file.getInt();
				int weather = file.getInt();
				g_size[0] = file.getInt();
				g_size[1] = file.getInt();
				cells = new map_cell[g_size[0]*g_size[1]];
				for (int j=0;j<g_size[1];++j)
				{
					for (int i=0;i<g_size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*g_size[0]+i].passable = (data & 128)?true:false;
						cells[j*g_size[0]+i].height = (data & 127);
						data = file.getUByte();
					}
				}
				break;
			}
		case 20202:
			{
				char * author_name_buffer = file.getByteArray(25);
				delete[] author_name_buffer;
				int theme = file.getInt();
				int weather = file.getInt();
				g_size[0] = file.getInt();
				g_size[1] = file.getInt();
				cells = new map_cell[g_size[0]*g_size[1]];
				for (int j=0;j<g_size[1];++j)
				{
					for (int i=0;i<g_size[0];++i)
					{
						unsigned char data = file.getUByte();
						cells[j*g_size[0]+i].passable = (data & 128)?true:false;
						cells[j*g_size[0]+i].height = (data & 127);
						data = file.getUByte();
					}
				}
				break;
			}
		}
	}

	// On cré l'espace pour la texture
	//--- Est-ce qu'on a une minimap possible de ça?
	FileIO* fileTGA = new FileIO(CString("main/modelmaps___/") + mapFilename + "/minimap.tga", "rb");
	unsigned int texMap = 0;
	if (fileTGA->isValid())
	{
		delete fileTGA;
		texMap = dktCreateTextureFromFile((CString("main/modelmaps___/") + mapFilename + "/minimap.tga").s, DKT_FILTER_NEAREST);
	}
	else
	{
		delete fileTGA;
		g_sizeReal[0] = 1.0f;
		g_sizeReal[1] = 1.0f;
		int width = 1;
		while(width < g_size[0])
		{
			width *= 2;
		}
		if(width != g_size[0])
		{
			g_sizeReal[0] = float(g_size[0]) / float(width);
		}
		int height = 1;
		while(height < g_size[1])
		{
			height *= 2;
		}
		if(height != g_size[1])
		{
			g_sizeReal[1] = float(g_size[1]) / float(height);
		}
		texMap = dktCreateEmptyTexture(width, height, 3, DKT_FILTER_NEAREST);

		unsigned char * textureData = new unsigned char [width*height*3];
		memset(textureData, 100, width*height*3);

		// On rempli le data dans la texture
		for (int j=0;j<height;++j)
		{
			for (int i=0;i<width;++i)
			{
				if((i < g_size[0]) && (j < g_size[1]))
				{
					if (!cells[j*g_size[0]+i].passable)
					{
						textureData[(j*width+i)*3+0] = 200;
						textureData[(j*width+i)*3+1] = 200;
						textureData[(j*width+i)*3+2] = 200;
					}
				}
			}
		}

		dktCreateTextureFromBuffer(&texMap, textureData, width, height, 3, DKT_FILTER_NEAREST);

		// On efface notre data tempon
		delete [] textureData;
	}


	ZEVEN_SAFE_DELETE_ARRAY(cells);
	return texMap;
}



CHost::CHost(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->texture = dktCreateTextureFromFile("main/textures/Menu3Back.tga", DKT_FILTER_LINEAR);
	instance->borderColor.set(1,.5f,.25f);

	//--- Labels and controls
	btn_start = new CControl(instance, CVector2i(20,10), CVector2i(75, 25), "Start", this, "BUTTON");
	btn_start->toolTips = "Start the game.\nIf you choose no map, daivuk will be the default.";

	//--- GENERAL GAME OPTIONS
	CControl * separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"General game options", this, "SEPARATOR",btn_start, CONTROL_SNAP_BOTTOM, 15);

		//--- Game name
		CControl * label1 = new CControl(instance, CVector2i(20,10), CVector2i(150,25),"Game name:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The game name shown into the browse list.";
		txt_gameName = new CControl(instance, CVector2i(10,10), CVector2i(300,25),gameVar.sv_gameName, this, "EDIT", label1, CONTROL_SNAP_RIGHT);

		//--- Password
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"Password:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Password your game to make it private.";
		txt_password = new CControl(instance, CVector2i(10,10), CVector2i(200,25),gameVar.sv_password, this, "EDIT", label1, CONTROL_SNAP_RIGHT);
		txt_password->password = true;

		//--- Game type
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,120),"Game type:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Choose a game type for your game.";
		lst_gameType = new CControl(instance, CVector2i(10,10), CVector2i(200,120),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			CControl * item = new CControl(lst_gameType, CVector2i(10,10), CVector2i(150,20),"Free for all", this, "LABEL");
			item->toolTips = "No teams, everyone against all the others.\nThe player with the more kills wins.";
			item = new CControl(lst_gameType, CVector2i(10,10), CVector2i(150,20),"Team deathmatch", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Play as a team and kill the oponent team.\nThe team with the more kills wins.";
			item = new CControl(lst_gameType, CVector2i(10,10), CVector2i(150,20),"Capture the flag", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Capture the enemy\'s flag and return it to your base.\nThe team with the more captures wins.";
			//item = new CControl(lst_gameType, CVector2i(10,10), CVector2i(150,20),"Counter Baborist", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			//item->toolTips = "Red team plank the bomb,\nblue team should deactivate it in time!.";
#if defined(_PRO_)
			item = new CControl(lst_gameType, CVector2i(10,10), CVector2i(150,20),"Champion", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "1v1 Duel your way to the top!.";
#endif
		lst_gameType->selectChild(gameVar.sv_gameType);


		//--- Spawn type
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,60),"Server type:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Choose a server type for your game.";
		lst_serverType = new CControl(instance, CVector2i(10,10), CVector2i(200,60),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_serverType, CVector2i(10,10), CVector2i(150,20),"Normal Server", this, "LABEL");
			item->toolTips = "Standard Game Rules and Balance.";
			item = new CControl(lst_serverType, CVector2i(10,10), CVector2i(150,20),"Pro Server", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Pro Game Rules and Balance.";
		lst_serverType->selectChild(gameVar.sv_serverType);


#if defined(_PRO_)
		//--- Spawn type
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,60),"Spawn type:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Choose a spawn type for your game.";
		lst_spawnType = new CControl(instance, CVector2i(10,10), CVector2i(200,60),"", this, "LISTBOX", label1, CONTROL_SNAP_RIGHT);
			item = new CControl(lst_spawnType, CVector2i(10,10), CVector2i(150,20),"Normal Style", this, "LABEL");
			item->toolTips = "Standard Random Spawns.";
			item = new CControl(lst_spawnType, CVector2i(10,10), CVector2i(150,20),"Ladder Style", this, "LABEL", item, CONTROL_SNAP_BOTTOM);
			item->toolTips = "Initially spawn happen on the team flag.";
		lst_spawnType->selectChild(gameVar.sv_spawnType);
#endif

		//--- Max player
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Max player:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Max number of players allowed in the game.";
		txt_maxPlayer = new CControl(instance, CVector2i(10,10), CVector2i(100,40),"8", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_maxPlayer->value = gameVar.sv_maxPlayer;
		txt_maxPlayer->valueMin = 1;
		txt_maxPlayer->valueMax = 32;
		btn_maxPlayerPreset[0] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"4", this, "BUTTON", txt_maxPlayer, CONTROL_SNAP_RIGHT);
		btn_maxPlayerPreset[1] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"8", this, "BUTTON", btn_maxPlayerPreset[0], CONTROL_SNAP_RIGHT);
		btn_maxPlayerPreset[2] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"16", this, "BUTTON", btn_maxPlayerPreset[1], CONTROL_SNAP_RIGHT);
		btn_maxPlayerPreset[3] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"32", this, "BUTTON", btn_maxPlayerPreset[2], CONTROL_SNAP_RIGHT);

		//--- TCP/UDP post
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"TCP/UDP port:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "If you want people to join your game,\nyou need to forward this port both TCP/UDP\non your router and your firewall.";
		txt_port = new CControl(instance, CVector2i(10,10), CVector2i(100,40),CString("%i", gameVar.sv_port), this, "SPIN", label1, CONTROL_SNAP_RIGHT);
		txt_port->value = gameVar.sv_port;
		txt_port->valueMin = 1000;
		txt_port->valueMax = 65000;

	//--- GAME LIMITS
	separator = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"Game limits", this, "SEPARATOR", txt_port, CONTROL_SNAP_BOTTOM, 15);

		//--- Score limit
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Score limit:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 15);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The frags limit for FFA and TDM.\n0 = infinite";
		txt_scoreLimit = new CControl(instance, CVector2i(10,10), CVector2i(500,40),"No limits", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_scoreLimit->value = gameVar.sv_scoreLimit;
		txt_scoreLimit->valueMin = 0;
		txt_scoreLimit->valueMax = 500;

		//--- Win limits
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Win limit:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The wins limit for CTF and CB.\n0 = infinite";
		txt_winLimit = new CControl(instance, CVector2i(10,10), CVector2i(500,40),"No limits", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_winLimit->value = gameVar.sv_winLimit;
		txt_winLimit->valueMin = 0;
		txt_winLimit->valueMax = 500;

		//--- Game time limit
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Game time limit:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The total game time limit for FFA, TDM, CTF in minute.\n0 = infinite";
		txt_gameTimeLimit = new CControl(instance, CVector2i(10,10), CVector2i(300,40),"15", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_gameTimeLimit->value = (int)(gameVar.sv_gameTimeLimit / 60.0f);
		txt_gameTimeLimit->valueMin = 0;
		txt_gameTimeLimit->valueMax = 120;
		btn_gameTimePreset[0] = new CControl(instance, CVector2i(10,10), CVector2i(60,40),"10mins", this, "BUTTON", txt_gameTimeLimit, CONTROL_SNAP_RIGHT);
		btn_gameTimePreset[1] = new CControl(instance, CVector2i(10,10), CVector2i(60,40),"15mins", this, "BUTTON", btn_gameTimePreset[0], CONTROL_SNAP_RIGHT);
		btn_gameTimePreset[2] = new CControl(instance, CVector2i(10,10), CVector2i(60,40),"30mins", this, "BUTTON", btn_gameTimePreset[1], CONTROL_SNAP_RIGHT);
		btn_gameTimePreset[3] = new CControl(instance, CVector2i(10,10), CVector2i(60,40),"1hour", this, "BUTTON", btn_gameTimePreset[2], CONTROL_SNAP_RIGHT);

		//--- Time to spawn
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Time to spawn:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->toolTips = "The time babos wait before respawning in second.";
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		txt_timeToSpawn = new CControl(instance, CVector2i(10,10), CVector2i(100,40),"7", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_timeToSpawn->value = (int)gameVar.sv_timeToSpawn;
		txt_timeToSpawn->valueMin = 0;
		txt_timeToSpawn->valueMax = 60;

		//--- Round time
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Round time limit:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->toolTips = "The time the red team has to plank the bomb in minute.";
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		txt_roundTimeLimit = new CControl(instance, CVector2i(10,10), CVector2i(200,40),"3", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
		txt_roundTimeLimit->value = (int)(gameVar.sv_roundTimeLimit / 60.0f);
		txt_roundTimeLimit->valueMin = 1;
		txt_roundTimeLimit->valueMax = 120;

		//--- Bomb time
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Bomb time:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The time the blue team has to deactivate the bomb before it exploses in second.";
		txt_bombTime = new CControl(instance, CVector2i(10,10), CVector2i(100,40),"60", this, "SPIN", label1, CONTROL_SNAP_RIGHT);
		txt_bombTime->value = (int)gameVar.sv_bombTime;
		txt_bombTime->valueMin = 10;
		txt_bombTime->valueMax = 1200;

	//--- FRIENDLY FIRE
	separator = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"Friendly Fire Setup", this, "SEPARATOR", label1, CONTROL_SNAP_BOTTOM, 15);

		//--- Friendly fire
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Friendly fire:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "Enable friendly fire.\nMeans you can kill teammate. (Not very babolike)";
		chk_ff = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_ff->check = gameVar.sv_friendlyFire;

		//--- Reflect damage
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Reflect damage:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "You receive damage when hitting a teammate. (Not very babolike)";
		chk_ffReflect = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_ffReflect->check = gameVar.sv_reflectedDamage;

		//--- Force respawn
		label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Force respawn:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
		label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		label1->toolTips = "The player will not be prompted to press use key\nto respawn.";
		chk_forceRespawn = new CControl(instance, CVector2i(10,10), CVector2i(25,25),"", this, "CHECK", label1, CONTROL_SNAP_RIGHT);
		chk_forceRespawn->check = gameVar.sv_forceRespawn;

	//--- MAPS
	separator = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"Map list", this, "SEPARATOR", label1, CONTROL_SNAP_BOTTOM, 15);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH];  // directory specification
	DWORD dwError;
	char appPath[_MAX_PATH];

	// Chercher le path du "current working directory".
	_getcwd(appPath, _MAX_PATH);

	strncpy(DirSpec, appPath, strlen(appPath)+1);
	strncat(DirSpec, "\\main\\maps\\*.bvm", strlen("\\main\\maps\\*.bvm")+1);

	hFind = FindFirstFile(DirSpec, &FindFileData);

	label1 = new CControl(instance, CVector2i(10,10), CVector2i(600,40),"Check the maps you want to include on this server.", this, "LABEL", separator, CONTROL_SNAP_BOTTOM,15);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Si on ne trouve pas le répertoire désiré.
	}
	else
	{
		CString filename = CString(FindFileData.cFileName);
		filename.resize(filename.len() - 4);

		int tileX, tileY;
		int topH;
		tileX = label1->localPos[0] + 15;
		tileY = label1->localPos[1] + label1->size[1] + 15;

		//--- Map
	//	label1 = new CControl(instance, CVector2i(10,10), CVector2i(200,64),filename, this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	//	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
		CControl * mapImg = new CControl(instance, CVector2i(tileX,tileY), CVector2i(64,64),filename, this, "CHECK");
		mapImg->imgColor.set(1,1,1);
//		mapImg->perfectFitTexture = true;
		mapImg->texture = loadMapPreview(filename);
		mapImg->textureCorner = g_sizeReal;
		mapImg->useTextureCorner = true;
		mapImg->size = g_size * 5;
		mapImg->foreColor.set(1,1,1);
		chk_maps.push_back(mapImg);
		topH = mapImg->size[1] + 15;
		tileX = label1->localPos[0] + 15;
		tileX += mapImg->size[0] + 15;



		//--- We tile it

		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString filename = CString(FindFileData.cFileName);
				filename.resize(filename.len() - 4);


				//--- Map
			//	label1 = new CControl(instance, CVector2i(10,10), CVector2i(200,64),filename, this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
			//	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
				mapImg = new CControl(instance, CVector2i(10,10), CVector2i(64,64),filename, this, "CHECK");
				mapImg->imgColor.set(1,1,1);
		//		mapImg->perfectFitTexture = true;
				mapImg->texture = loadMapPreview(filename);
				mapImg->size = g_size * 5;
				mapImg->textureCorner = g_sizeReal;
				mapImg->useTextureCorner = true;

				if (tileX + mapImg->size[0] > 700)
				{
					tileX = label1->localPos[0] + 15;
					tileY += topH;
					topH = 0;
				}
				mapImg->localPos[0] = tileX;
				mapImg->localPos[1] = tileY;

				mapImg->foreColor.set(1,1,1);
				chk_maps.push_back(mapImg);

				mapImg->localPos[0] = tileX;
				mapImg->localPos[1] = tileY;
				tileX += mapImg->size[0] + 15;
				if (topH < mapImg->size[1] + 15) topH = mapImg->size[1] + 15;
			}
		}
		
		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES)
		{
			// Si il y a une error en détectant qu'il n'y a plus de fichiers.
		}
	}


	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}

CHost::~CHost()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CHost::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CHost::MouseLeave(CControl * control)
{
}
void CHost::MouseDown(CControl * control)
{
}
void CHost::MouseUp(CControl * control)
{
}
void CHost::MouseMove(CControl * control)
{
}
void CHost::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_maxPlayerPreset[0])
	{
		txt_maxPlayer->value = 4;
		txt_maxPlayer->text = CString("") + txt_maxPlayer->value;
	}
	if (control == btn_maxPlayerPreset[1])
	{
		txt_maxPlayer->value = 8;
		txt_maxPlayer->text = CString("") + txt_maxPlayer->value;
	}
	if (control == btn_maxPlayerPreset[2])
	{
		txt_maxPlayer->value = 16;
		txt_maxPlayer->text = CString("") + txt_maxPlayer->value;
	}
	if (control == btn_maxPlayerPreset[3])
	{
		txt_maxPlayer->value = 32;
		txt_maxPlayer->text = CString("") + txt_maxPlayer->value;
	}
	if (control == btn_gameTimePreset[0])
	{
		txt_gameTimeLimit->value = 10;
		txt_gameTimeLimit->text = CString("") + txt_gameTimeLimit->value;
	}
	if (control == btn_gameTimePreset[1])
	{
		txt_gameTimeLimit->value = 15;
		txt_gameTimeLimit->text = CString("") + txt_gameTimeLimit->value;
	}
	if (control == btn_gameTimePreset[2])
	{
		txt_gameTimeLimit->value = 30;
		txt_gameTimeLimit->text = CString("") + txt_gameTimeLimit->value;
	}
	if (control == btn_gameTimePreset[3])
	{
		txt_gameTimeLimit->value = 60;
		txt_gameTimeLimit->text = CString("") + txt_gameTimeLimit->value;
	}
	if (control == btn_start)
	{
		//--- We firstly save the options
		gameVar.sv_gameName = txt_gameName->text;
		gameVar.sv_password = txt_password->text;
		gameVar.sv_gameType = lst_gameType->selectedIndex;
      gameVar.sv_serverType = lst_serverType->selectedIndex;
#if defined(_PRO_)
      gameVar.sv_spawnType = lst_spawnType->selectedIndex;
#endif
		gameVar.sv_maxPlayer = txt_maxPlayer->value;      
		gameVar.sv_port = txt_port->value;

		gameVar.sv_scoreLimit = txt_scoreLimit->value;
		gameVar.sv_winLimit = txt_winLimit->value;
		gameVar.sv_gameTimeLimit = (float)txt_gameTimeLimit->value * 60;
		gameVar.sv_timeToSpawn = (float)txt_timeToSpawn->value;
		gameVar.sv_roundTimeLimit = (float)txt_roundTimeLimit->value * 60;
		gameVar.sv_bombTime = (float)txt_bombTime->value;

		gameVar.sv_friendlyFire = chk_ff->check;
		gameVar.sv_reflectedDamage = chk_ffReflect->check;
		gameVar.sv_forceRespawn = chk_forceRespawn->check;

		//--- We launch the server, then add the maps
		// On colle la première map
		CString command = "host ";
		bool firstCheck = true;

		// On ajoute les maps
		for (int i=0;i<(int)chk_maps.size();++i)
		{
			if (chk_maps[i]->check)
			{
				if (firstCheck)
				{
					command += chk_maps[i]->text;
					console->sendCommand(command);
					firstCheck = false;
				}
				else
				{
					command = CString("addmap ") + chk_maps[i]->text;
					console->sendCommand(command);
				}
			}
		}

		if (firstCheck)
		{
			console->sendCommand("host CTF-Daivuk");
			console->add("\x9> Warning : No map selected, default CTF-Daivuk launched.");
		}

		//--- That's it
	}
}
void CHost::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}

	if (control == txt_gameTimeLimit)
	{
		int value = txt_gameTimeLimit->text.toInt();
		if (txt_gameTimeLimit->text == "No limits") return;
		if (value <= 0) {txt_gameTimeLimit->text = "No limits"; return;}
		if (value > 65000) value = 65000;
		txt_gameTimeLimit->text = CString("") + value;
	}
	if (control == txt_port)
	{
		int value = txt_port->text.toInt();
		if (value < 1000) value = 1000;
		if (value > 65000) value = 65000;
		txt_port->text = CString("") + value;
	}
	if (control == txt_scoreLimit)
	{
		int value = txt_scoreLimit->text.toInt();
		if (txt_scoreLimit->text == "No limits") return;
		if (value <= 0) {txt_scoreLimit->text = "No limits"; return;}
		if (value > 65000) value = 65000;
		txt_scoreLimit->text = CString("") + value;
	}
	if (control == txt_winLimit)
	{
		int value = txt_winLimit->text.toInt();
		if (txt_winLimit->text == "No limits") return;
		if (value <= 0) {txt_winLimit->text = "No limits"; return;}
		if (value > 65000) value = 65000;
		txt_winLimit->text = CString("") + value;
	}
}


#endif

