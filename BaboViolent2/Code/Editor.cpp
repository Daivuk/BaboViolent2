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
#include "Editor.h"
#include "Console.h"
#include "CMenuManager.h"
#include "FileIO.h"
#include "Game.h"
#include <algorithm>
#include "EditorTools.h"
#include "EditorDialogs.h"
#include "Scene.h"

extern Scene * scene;

Editor2::Editor2(CString mapName, unsigned int font, int sizeX, int sizeY)
	:	needToShutDown(false)
	,	map(0)
	,	active(0)
	,	activeDialog(0)
	,	inFrameFor(1.0f)
	,	mouseInMenu(false)
	,	isDirty(false)
{
	if (mapName.len() > 15)
		mapName.resize(15);
	else if (mapName.isNull()) 
	{
		needToShutDown = true;
		return;
	}

	// Lets ensure that the maps size in the interval [16; 64]
	// (i think this interval covers it all)
	
	if(sizeX < 16)
		sizeX = 16;
	else if(sizeX > 64)
		sizeX = 64;

	if(sizeY < 16)
		sizeY = 16;
	else if(sizeY > 64)
		sizeY = 64;
	
	map = new Map(mapName, 0, font, true, sizeX, sizeY);
	if (!map->isValid)
	{
		ZEVEN_SAFE_DELETE(map);
		needToShutDown = true;
		return;
	}
	dkcJumpToFrame(0);

	// Create and initialize the tools
	tools.push_back(new ToolGround());
	tools.push_back(new ToolSplat());
	tools.push_back(new ToolWall(1));
	tools.push_back(new ToolWall(2));
	tools.push_back(new ToolWall(3));
	tools.push_back(new ToolWall(4));
	tools.push_back(new ToolWall(5));
	tools.push_back(new ToolFlag(1));
	tools.push_back(new ToolFlag(0));
	tools.push_back(new ToolBomb(0));
	tools.push_back(new ToolBomb(1));
	tools.push_back(new ToolSpawn(ToolSpawn::SPAWN_COMMON));
	tools.push_back(new ToolSpawn(ToolSpawn::SPAWN_RED));
	tools.push_back(new ToolSpawn(ToolSpawn::SPAWN_BLUE));
	tools.push_back(new ToolAddLine(ToolAddLine::LINE_HORIZONTAL));
	tools.push_back(new ToolAddLine(ToolAddLine::LINE_VERTICAL));
	tools.push_back(new ToolRemoveLine(ToolRemoveLine::LINE_HORIZONTAL));
	tools.push_back(new ToolRemoveLine(ToolRemoveLine::LINE_VERTICAL));

	// Editor root
	editorRoot = new CControl();
	editorRoot->font = font;
	editorRoot->size.set(800,600);
	editorRoot->backColor.set(.3f,.5f,.8f);
	editorRoot->foreColor.set(1,1,1);
	editorRoot->textShadow = true;
	editorRoot->noFill = true;

	// Create the menu items and their hotkeys
	CControl* spacer = 0;
	frameMain = new CControl(editorRoot, CVector2i(0, 0), CVector2i(800, 50), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 0, true);
	btn_file    = new CControl(frameMain, CVector2i(10, 10), CVector2i( 70, 30), "File",    this, "BUTTON");
	spacer      = new CControl(frameMain, CVector2i( 5, 10), CVector2i( 10, 30), "",        this, "LABEL",  btn_file,    CONTROL_SNAP_RIGHT);
	btn_terrain = new CControl(frameMain, CVector2i(10, 10), CVector2i( 70, 30), "Terrain", this, "BUTTON", spacer,      CONTROL_SNAP_RIGHT);
	spacer      = new CControl(frameMain, CVector2i( 5, 10), CVector2i( 10, 30), "",        this, "LABEL",  btn_terrain, CONTROL_SNAP_RIGHT);
	btn_objects = new CControl(frameMain, CVector2i(10, 10), CVector2i( 70, 30), "Objects", this, "BUTTON", spacer,      CONTROL_SNAP_RIGHT);
	spacer      = new CControl(frameMain, CVector2i( 5, 10), CVector2i( 10, 30), "",        this, "LABEL",  btn_objects, CONTROL_SNAP_RIGHT);
	btn_size    = new CControl(frameMain, CVector2i(10, 10), CVector2i( 70, 30), "Size",    this, "BUTTON", spacer,      CONTROL_SNAP_RIGHT);
	spacer      = new CControl(frameMain, CVector2i( 5, 10), CVector2i( 10, 30), "",        this, "LABEL",  btn_size,    CONTROL_SNAP_RIGHT);
	btn_misc    = new CControl(frameMain, CVector2i(10, 10), CVector2i( 70, 30), "Misc",    this, "BUTTON", spacer,      CONTROL_SNAP_RIGHT);
	spacer      = new CControl(frameMain, CVector2i( 5, 10), CVector2i(300, 30), "",        this, "LABEL",  btn_misc,    CONTROL_SNAP_RIGHT);
	btn_quit    = new CControl(frameMain, CVector2i(10, 10), CVector2i( 30, 30), "\x4X",    this, "BUTTON", spacer,      CONTROL_SNAP_RIGHT);

	frameFile = new CControl(editorRoot, CVector2i(0, 50), CVector2i(90, 160), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 10, true);
	btn_new    = new CControl(frameFile, CVector2i(10, 10), CVector2i(70, 30), "New",     this, "BUTTON");
	btn_open   = new CControl(frameFile, CVector2i(10, 10), CVector2i(70, 30), "Open",    this, "BUTTON", btn_new,  CONTROL_SNAP_BOTTOM);
	btn_save   = new CControl(frameFile, CVector2i(10, 10), CVector2i(70, 30), "Save",    this, "BUTTON", btn_open, CONTROL_SNAP_BOTTOM);
	btn_saveAs = new CControl(frameFile, CVector2i(10, 10), CVector2i(70, 30), "Save As", this, "BUTTON", btn_save, CONTROL_SNAP_BOTTOM);

	frameTerrain = new CControl(editorRoot, CVector2i(0, 50), CVector2i(110, 260), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 10, true);
	btn_ground      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Ground",      this, "BUTTON");
	btn_alternative = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Alternative", this, "BUTTON", btn_ground,      CONTROL_SNAP_BOTTOM);
	btn_block1      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Wall 1",      this, "BUTTON", btn_alternative, CONTROL_SNAP_BOTTOM);
	btn_block2      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Wall 2",      this, "BUTTON", btn_block1,      CONTROL_SNAP_BOTTOM);
	btn_block3      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Wall 3",      this, "BUTTON", btn_block2,      CONTROL_SNAP_BOTTOM);
	btn_block4      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Wall 4",      this, "BUTTON", btn_block3,      CONTROL_SNAP_BOTTOM);
	btn_block5      = new CControl(frameTerrain, CVector2i(10, 10), CVector2i(90, 30), "Wall 5",      this, "BUTTON", btn_block4,      CONTROL_SNAP_BOTTOM);

	frameObjects = new CControl(editorRoot, CVector2i(0, 50), CVector2i(90, 260), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 10, true);
	btn_blueFlag  = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "{\x01 flag", this, "BUTTON");
	btn_redFlag   = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "}\x04 flag", this, "BUTTON", btn_blueFlag,  CONTROL_SNAP_BOTTOM);
	btn_bombSite1 = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "Bomb 1",     this, "BUTTON", btn_redFlag,   CONTROL_SNAP_BOTTOM);
	btn_bombSite2 = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "Bomb 2",     this, "BUTTON", btn_bombSite1, CONTROL_SNAP_BOTTOM);
	btn_spawn     = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "\x05Spawn",  this, "BUTTON", btn_bombSite2, CONTROL_SNAP_BOTTOM);
	btn_blueSpawn = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "\x01Spawn",  this, "BUTTON", btn_spawn,     CONTROL_SNAP_BOTTOM);
	btn_redSpawn  = new CControl(frameObjects, CVector2i(10, 10), CVector2i(70, 30), "\x04Spawn",  this, "BUTTON", btn_blueSpawn, CONTROL_SNAP_BOTTOM);

	frameSize = new CControl(editorRoot, CVector2i(0, 50), CVector2i(190, 160), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 10, true);
	btn_insertLineH = new CControl(frameSize, CVector2i(10, 10), CVector2i(170, 30), "Insert horizontal line", this, "BUTTON");
	btn_insertLineV = new CControl(frameSize, CVector2i(10, 10), CVector2i(170, 30), "Insert vertical line",   this, "BUTTON", btn_insertLineH, CONTROL_SNAP_BOTTOM);
	btn_removeLineH = new CControl(frameSize, CVector2i(10, 10), CVector2i(170, 30), "Remove horizontal line", this, "BUTTON", btn_insertLineV, CONTROL_SNAP_BOTTOM);
	btn_removeLineV = new CControl(frameSize, CVector2i(10, 10), CVector2i(170, 30), "Remove vertical line",   this, "BUTTON", btn_removeLineH, CONTROL_SNAP_BOTTOM);

	frameMisc = new CControl(editorRoot, CVector2i(0, 50), CVector2i(90, 90), "", this, "FRAME", 0, CONTROL_SNAP_TOP, 10, true);
	btn_theme    = new CControl(frameMisc, CVector2i(10, 10), CVector2i(70, 30), "Theme",      this, "BUTTON");
	btn_clearMap = new CControl(frameMisc, CVector2i(10, 10), CVector2i(70, 30), "Clear map",  this, "BUTTON", btn_theme, CONTROL_SNAP_BOTTOM);

	// Set different background for frames
	frameMain->backColor.set(.25f,.25f,.25f);
	frameFile->backColor.set(.25f,.25f,.25f);
	frameTerrain->backColor.set(.25f,.25f,.25f);
	frameObjects->backColor.set(.25f,.25f,.25f);
	frameSize->backColor.set(.25f,.25f,.25f);
	frameMisc->backColor.set(.25f,.25f,.25f);
	ShowFrame(0);

	if((0 <= active) && (active <= tools.size()))
	{
		tools[active]->NotifyActivate(true);
	}
}

Editor2::~Editor2()
{
	if((0 <= active) && (active <= tools.size()))
	{
		tools[active]->NotifyActivate(false);
	}
	// destroy everything
	delete map;
	delete editorRoot;
	for(std::vector< ITool* >::iterator i = tools.begin(); i != tools.end(); ++i)
	{
		delete (*i);
	}

//	scene->createMenu();

	delete scene->mainTab->host;
	scene->mainTab->host = new CHost(scene->mainTab->parent, scene->mainTab->btn_news);//btn_userLogin);
	scene->mainTab->host->setVisible(false);


	//--- Recreate the host tab
}

void Editor2::ActivateTool(std::vector< ITool* >::size_type tool)
{
	if((0 <= tool) && (tool <= tools.size()))
	{
		if((0 <= active) && (active <= tools.size()))
		{
			tools[active]->NotifyActivate(false);
		}
		active = tool;
		tools[active]->NotifyActivate(true);
	}
}

bool Editor2::isCursorInControl(CControl * control) const
{
	return 
		control->visible &&
		(
			(control->pos[0] <= menuManager.mousePos[0]) && (menuManager.mousePos[0] <= (control->pos[0] + control->size[0])) &&
			(control->pos[1] <= menuManager.mousePos[1]) && (menuManager.mousePos[1] <= (control->pos[1] + control->size[1]))
		);
}

bool Editor2::isCursorInFrame(float delay)
{
	inFrameFor += delay;
	bool inFrame =
		// In main frame
		isCursorInControl(frameMain) ||
		// In file frame
		isCursorInControl(frameFile) ||
		// In terrain frame
		isCursorInControl(frameTerrain) ||
		// In objects frame
		isCursorInControl(frameObjects) ||
		// In size frame
		isCursorInControl(frameSize) ||
		// In misc frame
		isCursorInControl(frameMisc);
	if(inFrame)
	{
		inFrameFor = 0.0f;
	}
	return (inFrameFor < 0.2f);
}

void Editor2::ShowFrame(CControl * frame)
{
	frameMain->visible = true;
	frameFile->visible = false;
	frameTerrain->visible = false;
	frameObjects->visible = false;
	frameSize->visible = false;
	frameMisc->visible = false;
	if(frame) frame->visible = true;
}

void Editor2::update(float delay)
{
	if(activeDialog != 0)
	{
		//activeDialog->update(delay);
		if(activeDialog->state != StateNothing)
		{
			menuManager.hideDialog(true);
			activeDialog = 0;
		}
		else
			return;
	}
	menuManager.update(delay, editorRoot);

	// Don't process clicks if in the menu area
	mouseInMenu = isCursorInFrame(delay);

	if (map && !console->isActive())
	{
		// Weather update
		if (map->m_weather)
			map->m_weather->update(delay, map);
		// Tool update
		if ((0 <= this->active) && (this->active < tools.size()) && !mouseInMenu)
		{
			if (dkiGetState(DKI_MOUSE_BUTTON1) && !mouseInMenu)
			{
				tools.at(active)->LeftClick(this, delay);
				//ShowFrame(0);
			}
			if (dkiGetState(DKI_MOUSE_BUTTON2) && !mouseInMenu)
			{
				tools.at(active)->RightClick(this, delay);
				//ShowFrame(0);
			}
			if (dkiGetState(DKI_MOUSE_BUTTON3) && !mouseInMenu)
			{
				tools.at(active)->MiddleClick(this, delay);
				//ShowFrame(0);
			}
		}
		// Hotkeys
		if (dkiGetState(DIK_G))
		{
			Click(btn_ground);
		}
		if (dkiGetState(DIK_H))
		{
			Click(btn_alternative);
		}
		if (dkiGetState(DIK_1))
		{
			Click(btn_block1);
		}
		if (dkiGetState(DIK_2))
		{
			Click(btn_block2);
		}
		if (dkiGetState(DIK_3))
		{
			Click(btn_block3);
		}
		if (dkiGetState(DIK_4))
		{
			Click(btn_block4);
		}
		if (dkiGetState(DIK_5))
		{
			Click(btn_block5);
		}
		if (dkiGetState(DIK_B))
		{
			Click(btn_blueFlag);
		}
		if (dkiGetState(DIK_N))
		{
			Click(btn_redFlag);
		}
		if (dkiGetState(DIK_T))
		{
			Click(btn_bombSite1);
		}
		if (dkiGetState(DIK_Y))
		{
			Click(btn_bombSite2);
		}
		if (dkiGetState(DIK_U))
		{
			Click(btn_spawn);
		}
		if (dkiGetState(DIK_J))
		{
			Click(btn_blueSpawn);
		}
		if (dkiGetState(DIK_M))
		{
			Click(btn_redSpawn);
		}
		if (dkiGetState(DIK_PGUP))
		{
			Click(btn_theme);
		}
		if (dkiGetState(DIK_ESCAPE))
		{
			Click(btn_quit);
		}
		// Movement
		if (dkiGetState(DIK_D))
		{
			map->camLookAt[0] += 20 * delay;
		}
		if (dkiGetState(DIK_A))
		{
			map->camLookAt[0] -= 20 * delay;
		}
		if (dkiGetState(DIK_W))
		{
			map->camLookAt[1] += 20 * delay;
		}
		if (dkiGetState(DIK_S))
		{
			map->camLookAt[1] -= 20 * delay;
		}
		map->update(delay, 0);
	}
}

void Editor2::render()
{
	dkoDisable(DKO_MULTIPASS);

	if (map)
	{
		// Position the camera
#ifndef _DX_
		glLoadIdentity();
		CVector3f up(0,1,1);
		normalize(up);
		if (gameVar.sv_topView)
		{
			gluLookAt(
				map->camPos[0], map->camPos[1], map->camPos[2], 
				map->camPos[0], map->camPos[1], 0, 
				up[0], up[1], up[2]);
		}
		else
		{
			gluLookAt(
				map->camPos[0], map->camPos[1]-4.0f, map->camPos[2], 
				map->camPos[0], map->camPos[1]-1.0f, 0, 
				up[0], up[1], up[2]);
		}

		// finding mouse position
		CVector2i res = dkwGetResolution();
		CVector2i mousePos = dkwGetCursorPos_main();

		if(gameVar.r_widescreen > 1)
			mousePos[0] = static_cast<int>(mousePos[0] * res[1]*1.333f / res[0] + (res[0] - res[1]*1.333f)/2); 

		mousePos[1] = res[1] - mousePos[1];
		CVector3f nearMouse = dkglUnProject(mousePos, 0.0f);
		CVector3f farMouse = dkglUnProject(mousePos, 1.0f);
		float percent = nearMouse[2] / (nearMouse[2] - farMouse[2]);
		mouseOnMap = nearMouse + (farMouse - nearMouse) * percent;
		cellCursor[0] = (int)mouseOnMap[0];
		cellCursor[1] = (int)mouseOnMap[1];
		if ((0 <= active) && (active < tools.size()))
		{
			tools.at(active)->ValidateCursor(cellCursor, map->size);
		}
		else
		{
			if (cellCursor[0] < 1) cellCursor[0] = 1;
			if (cellCursor[1] < 1) cellCursor[1] = 1;
			if (cellCursor[0] > map->size[0] - 2) cellCursor[0] = map->size[0] - 2;
			if (cellCursor[1] > map->size[1] - 2) cellCursor[1] = map->size[1] - 2;
		}

		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

			// Da Sun!
			glEnable(GL_LIGHTING);
			dkglSetPointLight(1, -1000, 1000, 2000, 1, 1, 1);

			/*if(gameVar.r_weatherEffects) {
				glEnable(GL_FOG);
				glFogi(GL_FOG_MODE, GL_LINEAR);
				glFogfv(GL_FOG_COLOR, map->fogColor.s);
				glFogf(GL_FOG_DENSITY, map->fogDensity);
				glFogf(GL_FOG_START, map->camPos[2] - map->fogStart);
				glFogf(GL_FOG_END, map->camPos[2] - map->fogEnd);
			}
			else*/
				glDisable(GL_FOG);

			// Render the map
			map->renderGround();

			// Render 3D map (do we still need this? really?!)
			if (map->dko_map)
			{
				glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
				glDisable(GL_LIGHTING);
			//	glCullFace(GL_FRONT);
					glPushMatrix();
						glPolygonOffset(0,-.1f);
						glScalef(.1f,.1f,.1f);
						dkoRender(map->dko_map);

						glEnable(GL_POLYGON_OFFSET_FILL);

						glDepthFunc(GL_LEQUAL);
						glEnable(GL_BLEND);
						glBlendFunc(GL_DST_COLOR, GL_ZERO);
						dkoRender(map->dko_mapLM);
					glPopMatrix();
			//	glCullFace(GL_BACK);
				glPopAttrib();
			}

			// Render the bombs
			glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
				glDepthMask(GL_FALSE);
				glDisable(GL_LIGHTING);
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				map->renderBombMark();
			glPopAttrib();

			// Render flags, spawns and other shit
			map->renderMisc();

			if (!map->dko_map)
			{
				// Render shadows
				map->renderShadow();

				// Render walls
				map->renderWalls();
			}

			if (map->m_weather) map->m_weather->render();

		glPopAttrib();

		// Minimap
		renderMiniMap();

		if((activeDialog == 0) && !mouseInMenu)
		{
			// render active tool
			if((0 <= active) && (active < tools.size()))
				tools.at(active)->render(this);

			// Render cursor position
			glClear(GL_DEPTH_BUFFER_BIT);
			dkglPushOrtho(800, 600);
				glPushAttrib(GL_ENABLE_BIT);
					glDisable(GL_DEPTH_TEST);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					printRightText(796, 564, 32, CString("(%d; %d)", cellCursor[0], cellCursor[1]));
				glPopAttrib();
			dkglPopOrtho();
		}
#endif
	}

	menuManager.render(editorRoot);
	renderSelection();
	/*if(activeDialog != 0)
	{
		activeDialog->render();
	}*/
}

void Editor2::renderMiniMap()
{
	CVector2i res(800,600);// = dkwGetResolution();
#ifndef _DX_
	dkglPushOrtho((float)res[0], (float)res[1]);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_2D, map->texMap);
			glPushMatrix();
				float scalar = 128.0f / (float)max(map->size[0], map->size[1]);
				glTranslatef(20, (float)res[1]-20,0);
				glScalef(scalar,-scalar,scalar); // Invert the Y axis
				glBegin(GL_QUADS);
					glColor4f(1,1,1,.5f);
					glTexCoord2f(0.0f, map->texMapSize[1]);
					glVertex2i(0, map->size[1]);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2i(0, 0);
					glTexCoord2f(map->texMapSize[0], 0.0f);
					glVertex2i(map->size[0], 0);
					glTexCoord2f(map->texMapSize[0], map->texMapSize[1]);
					glVertex2i(map->size[0], map->size[1]);
				glEnd();
				if((activeDialog == 0) && !mouseInMenu)
				{
					glTranslatef((float)cellCursor[0], (float)cellCursor[1], 0);
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_TEXTURE_2D);
					glLineWidth(1);
					glColor3f(1,1,0);
					glBegin(GL_LINE_LOOP);
						glVertex2i(0,1);
						glVertex2i(0,0);
						glVertex2i(1,0);
						glVertex2i(1,1);
					glEnd();
				}
			glPopMatrix();
		glPopAttrib();
	dkglPopOrtho();
#endif
}

void Editor2::renderSquare(const CControl * control, int distance, const CColor4f & color) const
{
	if(control->visible)
	{
#ifndef _DX_
		glColor4fv(color.s);
		glBegin(GL_LINE_LOOP);
			glVertex2i(control->pos[0] - distance,                    control->pos[1] - distance);
			glVertex2i(control->pos[0] - distance,                    control->pos[1] + control->size[1] + distance);
			glVertex2i(control->pos[0] + control->size[0] + distance, control->pos[1] + control->size[1] + distance);
			glVertex2i(control->pos[0] + control->size[0] + distance, control->pos[1] - distance);
		glEnd();
#endif
	}
}

void Editor2::renderSelection() const
{
#ifndef _DX_
	dkglPushOrtho(800, 600);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glLineWidth(2);
			// Render selected menu buttons
			if(frameFile->visible)
			{
				renderSquare(btn_file, 5, CColor4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
			if(frameTerrain->visible)
			{
				renderSquare(btn_terrain, 5, CColor4f(0.0f, 1.0f, 0.0f, 1.0f));
				switch(active)
				{
				case  0: renderSquare(btn_ground,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  1: renderSquare(btn_alternative,  3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  2: renderSquare(btn_block1,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  3: renderSquare(btn_block2,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  4: renderSquare(btn_block3,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  5: renderSquare(btn_block4,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  6: renderSquare(btn_block5,       3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				}
			}
			if(frameObjects->visible)
			{
				renderSquare(btn_objects, 5, CColor4f(0.0f, 1.0f, 0.0f, 1.0f));
				switch(active)
				{
				case  7: renderSquare(btn_redFlag,      3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  8: renderSquare(btn_blueFlag,     3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case  9: renderSquare(btn_bombSite1,    3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 10: renderSquare(btn_bombSite2,    3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 11: renderSquare(btn_spawn,        3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 12: renderSquare(btn_redSpawn,     3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 13: renderSquare(btn_blueSpawn,    3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				}
			}
			if(frameSize->visible)
			{
				renderSquare(btn_size, 5, CColor4f(0.0f, 1.0f, 0.0f, 1.0f));
				switch(active)
				{
				case 14: renderSquare(btn_insertLineH,  3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 15: renderSquare(btn_insertLineV,  3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 16: renderSquare(btn_removeLineH,  3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				case 17: renderSquare(btn_removeLineV,  3, CColor4f(0.0f, 1.0f, 0.0f, 1.0f)); break;
				}
			}
			if(frameMisc->visible)
			{
				renderSquare(btn_misc, 5, CColor4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		glPopAttrib();
	dkglPopOrtho();
#endif
}

void Editor2::Open(CString mapName)
{
	FileIO test(mapName, "rb");
	if(test.isValid())
	{
		test.Close();
		delete map;
		map = new Map(mapName, 0, editorRoot->font, true);
	}
	else
	{
		console->add("\x4> Error opening map - no such file.");
	}
}

void Editor2::Save(CString mapName)
{
    int i, j;
	map->mapName = mapName;
	// On save la map dans ce cas!
	FileIO file(CString("main/maps/%s.bvm", mapName.s), "wb");
	if (!file.isValid())
	{
		console->add("\x4> Error saving map");
	}
	else
	{
// 20202 version save
		// save version
		file.put((unsigned long)MAP_VERSION);
		// save author name
		char author_name_buffer[25] = {0};
		strncpy(author_name_buffer, map->author_name.s, 24);
		file.put(author_name_buffer, 25);
		// save theme and weather
		file.put(map->theme);
		file.put(map->weather);

		// map size
		file.put(map->size[0]);
		file.put(map->size[1]);

		// map cells
		for (j = 0; j < map->size[1]; ++j)
		{
			for (i = 0; i < map->size[0]; ++i)
			{
				unsigned int index = j * map->size[0] + i;
				// passable or not
				unsigned char data = 0;
				if (map->cells[index].passable)
					data = 128;
				data = data | (unsigned char)(map->cells[index].height);
				file.put(data);
				file.put((unsigned char)(map->cells[index].splater[1]*255.0f));
			}
		}

		// Trim duplicate spawns (if any) before saving
		std::vector<CVector3f> temp_spawns = map->dm_spawns;
		map->dm_spawns.clear();
		for(i = 0; i < (int)temp_spawns.size(); ++i)
		{
			if(std::find(map->dm_spawns.begin(), map->dm_spawns.end(), temp_spawns[i]) == map->dm_spawns.end())
			{
				map->dm_spawns.push_back(temp_spawns[i]);
			}
		}
		temp_spawns.clear();

		// now save the common spawns
		file.put((int)map->dm_spawns.size());
		for (i=0;i<(int)map->dm_spawns.size();++i) file.put(map->dm_spawns[i]);

		// save game-type specific data
		// although DM and TDM doesn't have any data, we put their ID's, too
		file.put((int)GAME_TYPE_DM);
		file.put((int)GAME_TYPE_TDM);
		// save CTF data
		file.put((int)GAME_TYPE_CTF);
		// flags
		file.put(map->flagPodPos[0]);
		file.put(map->flagPodPos[1]);
		// save CB data
		file.put((int)GAME_TYPE_SND);
		// bombs
		file.put(map->objective[0]);
		file.put(map->objective[1]);
		// Trim duplicate spawns (if any) before saving
		temp_spawns = map->blue_spawns;
		map->blue_spawns.clear();
		for(i = 0; i < (int)temp_spawns.size(); ++i)
		{
			if(std::find(map->blue_spawns.begin(), map->blue_spawns.end(), temp_spawns[i]) == map->blue_spawns.end())
			{
				map->blue_spawns.push_back(temp_spawns[i]);
			}
		}
		temp_spawns.clear();
		// now save the spawns
		file.put((int)map->blue_spawns.size());
		for (i=0;i<(int)map->blue_spawns.size();++i) file.put(map->blue_spawns[i]);
		// Trim duplicate spawns (if any) before saving
		temp_spawns = map->red_spawns;
		map->red_spawns.clear();
		for(i = 0; i < (int)temp_spawns.size(); ++i)
		{
			if(std::find(map->red_spawns.begin(), map->red_spawns.end(), temp_spawns[i]) == map->red_spawns.end())
			{
				map->red_spawns.push_back(temp_spawns[i]);
			}
		}
		temp_spawns.clear();
		// now save the spawns
		file.put((int)map->red_spawns.size());
		for (i=0;i<(int)map->red_spawns.size();++i) file.put(map->red_spawns[i]);

		isDirty = false;
	}
}

void Editor2::Click(CControl * control)
{
	if (control == btn_file)
	{
		ShowFrame(frameFile->visible ? 0 : frameFile);
		return;
	}
	if (control == btn_terrain)
	{
		ShowFrame(frameTerrain->visible ? 0 : frameTerrain);
		return;
	}
	if (control == btn_objects)
	{
		ShowFrame(frameObjects->visible ? 0 : frameObjects);
		return;
	}
	if (control == btn_size)
	{
		ShowFrame(frameSize->visible ? 0 : frameSize);
		return;
	}
	if (control == btn_misc)
	{
		ShowFrame(frameMisc->visible ? 0 : frameMisc);
		return;
	}
	if (control == btn_new)
	{
		activeDialog = new NewMapDialog(this, editorRoot->font, gameVar.cl_mapAuthorName);
		return;
	}
	if (control == btn_open)
	{
		activeDialog = new OpenMapDialog(this, editorRoot->font);
		return;
	}
	if (control == btn_save)
	{
		if(isDirty)
		{
			if(map->mapName.len() > 0)
			{
				Save(map->mapName);
			}
			else
			{
				activeDialog = new SaveAsMapDialog(this, editorRoot->font, map->author_name, map->mapName);
			}
		}
		return;
	}
	if (control == btn_saveAs)
	{
		activeDialog = new SaveAsMapDialog(this, editorRoot->font, map->author_name, map->mapName);
		return;
	}
	if (control == btn_ground)
	{
		ActivateTool(0);
		return;
	}
	if (control == btn_alternative)
	{
		ActivateTool(1);
		return;
	}
	if (control == btn_block1)
	{
		ActivateTool(2);
		return;
	}
	if (control == btn_block2)
	{
		ActivateTool(3);
		return;
	}
	if (control == btn_block3)
	{
		ActivateTool(4);
		return;
	}
	if (control == btn_block4)
	{
		ActivateTool(5);
		return;
	}
	if (control == btn_block5)
	{
		ActivateTool(6);
		return;
	}
	if (control == btn_redFlag)
	{
		ActivateTool(7);
		return;
	}
	if (control == btn_blueFlag)
	{
		ActivateTool(8);
		return;
	}
	if (control == btn_bombSite1)
	{
		ActivateTool(9);
		return;
	}
	if (control == btn_bombSite2)
	{
		ActivateTool(10);
		return;
	}
	if (control == btn_spawn)
	{
		ActivateTool(11);
		return;
	}
	if (control == btn_redSpawn)
	{
		ActivateTool(12);
		return;
	}
	if (control == btn_blueSpawn)
	{
		ActivateTool(13);
		return;
	}
	if (control == btn_insertLineH)
	{
		ActivateTool(14);
		return;
	}
	if (control == btn_insertLineV)
	{
		ActivateTool(15);
		return;
	}
	if (control == btn_removeLineH)
	{
		ActivateTool(16);
		return;
	}
	if (control == btn_removeLineV)
	{
		ActivateTool(17);
		return;
	}
	if (control == btn_theme)
	{
		map->theme++;
		if (map->theme > THEME_END)
		{
			map->theme = THEME_START;
		}
		map->reloadTheme();
		map->reloadWeather();
		isDirty = true;
		return;
	}
	if (control == btn_clearMap)
	{
		map->theme = THEME_START;
		map->weather = WEATHER_NONE;
		map->dm_spawns.clear();
		map->blue_spawns.clear();
		map->red_spawns.clear();
		map->flagPodPos[0].set(0.0f, 0.0f, 0.0f);
		map->flagPodPos[1].set(0.0f, 0.0f, 0.0f);
		map->objective[0].set(0.0f, 0.0f, 0.0f);
		map->objective[1].set(0.0f, 0.0f, 0.0f);
		for(int j = 0; j < map->size[1]; ++j)
		{
			for(int i = 0; i < map->size[0]; ++i)
			{
				int index = (j * map->size[0]) + i;
				if((i == 0) || (i == (map->size[0] - 1)) || (j == 0) || (j == (map->size[1] - 1)))
				{
					map->cells[index].passable = false;
					map->cells[index].height = 3;
				}
				else
				{
					map->cells[index].passable = true;
					map->cells[index].height = 0;
				}
				map->cells[index].splater[0] = 0.0f;
				map->cells[index].splater[1] = 0.0f;
				map->cells[index].splater[2] = 0.0f;
				map->cells[index].splater[3] = 0.0f;
			}
		}
		map->regenTex();
		map->reloadTheme();
		map->reloadWeather();
		isDirty = true;
		return;
	}
	if (control == btn_quit)
	{
		if (!isDirty)
		{
			needToShutDown = true;
			ShowFrame(0);
		}
		else
		{
			activeDialog = new SaveDirtyDialog(this, editorRoot->font, true);
		}
		return;
	}
}

#endif

