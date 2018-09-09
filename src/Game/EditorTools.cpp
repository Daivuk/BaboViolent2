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
#include "EditorTools.h"
#include "Editor.h"
#include "Console.h"
#include "CMenuManager.h"
#include "FileIO.h"
#include "Game.h"
#include <algorithm>

const float Selection = 0.001f;

ITool::~ITool()
{
}

void ITool::NotifyActivate(bool active)
{
	// do nothing
	(void)active;
}

void ITool::ValidateCursor(CVector2i & cursor, const CVector2i & size) const
{
	if (cursor[0] < 1) cursor[0] = 1;
	if (cursor[1] < 1) cursor[1] = 1;
	if (cursor[0] > size[0] - 2) cursor[0] = size[0] - 2;
	if (cursor[1] > size[1] - 2) cursor[1] = size[1] - 2;
}

unsigned int ITool::MapCellIndex(Map * map, const CVector2i & cell) const
{
	return cell[1] * map->size[0] + cell[0];
}

void ITool::RegenerateTextures(Map * map, const CVector2i & cell) const
{
	map->regenTex();
	map->regenCell(cell[0]-1, cell[1]+1);
	map->regenCell(cell[0]-1, cell[1]);
	map->regenCell(cell[0]-1, cell[1]-1);
	map->regenCell(cell[0],   cell[1]-1);
	map->regenCell(cell[0]+1, cell[1]-1);
	map->regenCell(cell[0]+1, cell[1]);
	map->regenCell(cell[0]+1, cell[1]+1);
	map->regenCell(cell[0],   cell[1]+1);
	map->regenCell(cell[0],   cell[1]);
}

ToolGround::ToolGround()
{
}

ToolGround::~ToolGround()
{
}

void ToolGround::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2);
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_LINE_LOOP);
			glVertex2f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection);
			glVertex2f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection);
			glVertex2f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection);
			glVertex2f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection);
		glEnd();
	glPopAttrib();
#endif
}

void ToolGround::LeftClick(Editor2 * editor, float delay)
{
	editor->map->buildGround();
	unsigned int index = MapCellIndex(editor->map, editor->cellCursor);
	if(!editor->map->cells[index].passable && (editor->map->cells[index].height != 0))
	{
		editor->map->cells[index].passable = true;
		editor->map->cells[index].height = 0;
		RegenerateTextures(editor->map, editor->cellCursor);
		editor->isDirty = true;
	}
	(void) delay;
}

void ToolGround::RightClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

void ToolGround::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolSplat::ToolSplat()
{
}

ToolSplat::~ToolSplat()
{
}

void ToolSplat::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2);
		glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_LINE_LOOP);
			glVertex2f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection);
			glVertex2f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection);
			glVertex2f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection);
			glVertex2f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection);
		glEnd();
	glPopAttrib();
#endif
}

void ToolSplat::LeftClick(Editor2 * editor, float delay)
{
	editor->map->buildGround();
	for (int j = (editor->cellCursor[1] - 3); j <= (editor->cellCursor[1] + 3); ++j)
	{
		for (int i = (editor->cellCursor[0] - 3); i <= (editor->cellCursor[0] + 3); ++i)
		{
			float dis = distance(CVector2f((float)i, (float)j), CVector2f((float)editor->cellCursor[0], (float)editor->cellCursor[1]));
			if (dis < 2)
			{
				float percent = 1 - dis / 2.0f;
				editor->map->addTileDirt(i, j, delay * percent * 2);
				editor->isDirty = true;
			}
		}
	}
}

void ToolSplat::RightClick(Editor2 * editor, float delay)
{
	editor->map->buildGround();
	for (int j = (editor->cellCursor[1] - 3); j <= (editor->cellCursor[1] + 3); ++j)
	{
		for (int i = (editor->cellCursor[0] - 3); i <= (editor->cellCursor[0] + 3); ++i)
		{
			float dis = distance(CVector2f((float)i, (float)j), CVector2f((float)editor->cellCursor[0], (float)editor->cellCursor[1]));
			if (dis < 2)
			{
				float percent = 1 - dis / 2.0f;
				editor->map->removeTileDirt(i, j, delay * percent * 2);
				editor->isDirty = true;
			}
		}
	}
}

void ToolSplat::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolWall::ToolWall(unsigned char wallHeight)
	:	height(wallHeight)
{
}

ToolWall::~ToolWall()
{
}

void ToolWall::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2);
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glBegin(GL_LINE_LOOP);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection, Selection);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection,     Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection,     Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection, Selection);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection, height + Selection);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection,     height + Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection,     height + Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection, height + Selection);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection, Selection);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + 1 - Selection, height + Selection);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection,     Selection);
			glVertex3f(editor->cellCursor[0] + Selection,     editor->cellCursor[1] + Selection,     height + Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection,     Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + Selection,     height + Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection, Selection);
			glVertex3f(editor->cellCursor[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection, height + Selection);
		glEnd();
	glPopAttrib();
#endif
}

void ToolWall::LeftClick(Editor2 * editor, float delay)
{
	// Do not make the wall if the location isn't empty
	CVector3f spawnAt((float)editor->cellCursor[0] + 0.5f, (float)editor->cellCursor[1] + 0.5f, 0.25f);
	if(
		(CVector2i((int)editor->map->flagPodPos[0][0], (int)editor->map->flagPodPos[0][1]) == editor->cellCursor) ||
		(CVector2i((int)editor->map->flagPodPos[1][0], (int)editor->map->flagPodPos[1][1]) == editor->cellCursor) ||
		(CVector2i((int)editor->map->objective[0][0], (int)editor->map->objective[0][1]) == editor->cellCursor) ||
		(CVector2i((int)editor->map->objective[1][0], (int)editor->map->objective[1][1]) == editor->cellCursor) ||
		(std::find(editor->map->dm_spawns.begin(), editor->map->dm_spawns.end(), spawnAt) != editor->map->dm_spawns.end()) ||
		(std::find(editor->map->blue_spawns.begin(), editor->map->blue_spawns.end(), spawnAt) != editor->map->blue_spawns.end()) ||
		(std::find(editor->map->red_spawns.begin(), editor->map->red_spawns.end(), spawnAt) != editor->map->red_spawns.end())
		)
	{
		return;
	}
	unsigned int index = MapCellIndex(editor->map, editor->cellCursor);
	if(editor->map->cells[index].passable || (editor->map->cells[index].height != height))
	{
		editor->map->cells[index].passable = false;
		editor->map->cells[index].height = height;
		RegenerateTextures(editor->map, editor->cellCursor);
		editor->isDirty = true;
	}
	(void) delay;
}

void ToolWall::RightClick(Editor2 * editor, float delay)
{
	unsigned int index = MapCellIndex(editor->map, editor->cellCursor);
	if(!editor->map->cells[index].passable && (editor->map->cells[index].height != 0))
	{
		editor->map->cells[index].passable = true;
		editor->map->cells[index].height = 0;
		RegenerateTextures(editor->map, editor->cellCursor);
		editor->isDirty = true;
	}
	(void) delay;
}

void ToolWall::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolFlag::ToolFlag(unsigned int flagIndex)
	:	flag(flagIndex)
{
}

ToolFlag::~ToolFlag()
{
}

void ToolFlag::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		dkoPushRenderState();
			dkoEnable(DKO_FORCE_WIREFRAME);
			glPushMatrix();
				glTranslatef(editor->cellCursor[0] + 0.5f, editor->cellCursor[1] + 0.5f, 0.0f);
				glScalef(0.005f, 0.005f, 0.005f);
				//dkoRender(editor->map->dko_flagPod[flag]);
				dkoRender(editor->map->dko_flag[flag]);
			glPopMatrix();
		dkoPopRenderState();
	glPopAttrib();
#endif
}

void ToolFlag::LeftClick(Editor2 * editor, float delay)
{
	unsigned int index = this->MapCellIndex(editor->map, editor->cellCursor);
	if(editor->map->cells[index].passable)
	{
		CVector3f newPos(float(editor->cellCursor[0]) + 0.5f, float(editor->cellCursor[1]) + 0.5f, 0.0f);
		if(newPos != editor->map->flagPodPos[flag])
		{
			editor->map->flagPodPos[flag] = newPos;
			editor->isDirty = true;
		}
	}
	(void) delay;
}

void ToolFlag::RightClick(Editor2 * editor, float delay)
{
	CVector2i flagPos((int)editor->map->flagPodPos[flag][0], (int)editor->map->flagPodPos[flag][1]);
	if(flagPos == editor->cellCursor)
	{
		CVector3f newPos(0.0f, 0.0f, 0.0f);
		if(newPos != editor->map->flagPodPos[flag])
		{
			editor->map->flagPodPos[flag] = newPos;
			editor->isDirty = true;
		}
	}
	(void) delay;
}

void ToolFlag::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolBomb::ToolBomb(unsigned int bombIndex)
	:	bomb(bombIndex)
{
}

ToolBomb::~ToolBomb()
{
}

void ToolBomb::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if(bomb == 0)
		{
			glColor4f(0.0f, 1.0f, 0.5f, 0.5f);
		}
		else
		{
			glColor4f(0.5f, 1.0f, 0.0f, 0.5f);
		}
		glBindTexture(GL_TEXTURE_2D, editor->map->tex_bombMark);
		glPushMatrix();
			glTranslatef(editor->cellCursor[0] + 0.5f, editor->cellCursor[1] + 0.5f, Selection);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 1);
				glVertex2f(-0.5f,  0.5f);
				glTexCoord2i(0, 0);
				glVertex2f(-0.5f, -0.5f);
				glTexCoord2i(1, 0);
				glVertex2f( 0.5f, -0.5f);
				glTexCoord2i(1, 1);
				glVertex2f( 0.5f,  0.5f);
			glEnd();
		glPopMatrix();
	glPopAttrib();
#endif
}

void ToolBomb::LeftClick(Editor2 * editor, float delay)
{
	unsigned int index = MapCellIndex(editor->map, editor->cellCursor);
	if(editor->map->cells[index].passable)
	{
		CVector3f newPos(float(editor->cellCursor[0]) + 0.5f, float(editor->cellCursor[1]) + 0.5f, 0.0f);
		if(newPos != editor->map->objective[bomb])
		{
			editor->map->objective[bomb] = newPos;
			editor->isDirty = true;
		}
	}
	(void) delay;
}

void ToolBomb::RightClick(Editor2 * editor, float delay)
{
	CVector2i bombPos((int)editor->map->objective[bomb][0], (int)editor->map->objective[bomb][1]);
	if(bombPos == editor->cellCursor)
	{
		CVector3f newPos(0.0f, 0.0f, 0.0f);
		if(newPos != editor->map->objective[bomb])
		{
			editor->map->objective[bomb] = newPos;
			editor->isDirty = true;
		}
	}
	(void) delay;
}

void ToolBomb::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolSpawn::ToolSpawn(SpawnTypes spawnType)
	:	spawn(spawnType)
{
}

ToolSpawn::~ToolSpawn()
{
}

void ToolSpawn::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2);
		switch(spawn)
		{
		case SPAWN_COMMON:
			glColor4f(1.0f, 0.5f, 1.0f, 0.25f);
			break;
		case SPAWN_BLUE:
			glColor4f(5.0f, 5.0f, 1.0f, 0.25f);
			break;
		case SPAWN_RED:
			glColor4f(1.0f, 5.0f, 5.0f, 0.25f);
			break;
		}
		glPushMatrix();
			glTranslatef(editor->cellCursor[0] + 0.5f, editor->cellCursor[1] + 0.5f, 0.25f);
			gluQuadricDrawStyle(editor->map->qObj, GLU_SILHOUETTE);
			gluSphere(editor->map->qObj, 0.25f, 8, 4);
			gluQuadricDrawStyle(editor->map->qObj, GLU_FILL);
		glPopMatrix();
	glPopAttrib();
#endif
}

void ToolSpawn::LeftClick(Editor2 * editor, float delay)
{
	unsigned int index = MapCellIndex(editor->map, editor->cellCursor);
	if(editor->map->cells[index].passable)
	{
		std::vector<CVector3f> * spawns = 0;
		switch(spawn)
		{
		case SPAWN_COMMON:
			spawns = &(editor->map->dm_spawns);
			break;
		case SPAWN_BLUE:
			spawns = &(editor->map->blue_spawns);
			break;
		case SPAWN_RED:
			spawns = &(editor->map->red_spawns);
			break;
		}
		CVector3f spawnAt((float)editor->cellCursor[0] + 0.5f, (float)editor->cellCursor[1] + 0.5f, 0.25f);
		if(std::find(spawns->begin(), spawns->end(), spawnAt) == spawns->end())
		{
			spawns->push_back(spawnAt);
			editor->isDirty = true;
		}
	}
	(void) delay;
}

void ToolSpawn::RightClick(Editor2 * editor, float delay)
{
	std::vector<CVector3f> * spawns = 0;
	switch(spawn)
	{
	case SPAWN_COMMON:
		spawns = &(editor->map->dm_spawns);
		break;
	case SPAWN_BLUE:
		spawns = &(editor->map->blue_spawns);
		break;
	case SPAWN_RED:
		spawns = &(editor->map->red_spawns);
		break;
	}
	CVector3f spawnAt((float)editor->cellCursor[0] + 0.5f, (float)editor->cellCursor[1] + 0.5f, 0.25f);
	std::vector<CVector3f>::iterator f = std::find(spawns->begin(), spawns->end(), spawnAt);
	if(f != spawns->end())
	{
		spawns->erase(f);
		editor->isDirty = true;
	}
	(void) delay;
}

void ToolSpawn::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

ToolAddLine::ToolAddLine(ToolAddLineType addLineType)
	:	lineType(addLineType)
	,	lastAction(1.0f)
{
}

ToolAddLine::~ToolAddLine()
{
}

void ToolAddLine::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(3);
		CVector2f start, end;
		switch(lineType)
		{
		case LINE_HORIZONTAL:
			start.set(0.0f, float(editor->cellCursor[1]));
			end.set(float(editor->map->size[0] + 1), float(editor->cellCursor[1]));
			break;
		case LINE_VERTICAL:
			start.set(float(editor->cellCursor[0]), 0.0f);
			end.set(float(editor->cellCursor[0]), float(editor->map->size[1] + 1));
			break;
		}
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_LINES);
			glVertex3f(start[0], start[1], Selection);
			glVertex3f(end[0], end[1], Selection);
		glEnd();
	glPopAttrib();
#endif
}

void ToolAddLine::LeftClick(Editor2 * editor, float delay)
{
	lastAction += delay;
	switch(lineType)
	{
	case LINE_HORIZONTAL:
		if((editor->map->size[1] < 64) && (lastAction > 0.1f))
		{
			map_cell * temp = editor->map->cells;
			editor->map->size[1] += 1;
			editor->map->cells = new map_cell[editor->map->size[0] * editor->map->size[1]];
			// Copy old cells
			for(int j = 0; j < editor->cellCursor[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					unsigned int index = j * editor->map->size[0] + i;
					editor->map->cells[index] = temp[index];
				}
			}
			for(int i = 0; i < editor->map->size[0]; ++i)
			{
				unsigned int index = editor->cellCursor[1] * editor->map->size[0] + i;
				editor->map->cells[index].passable = true;
				editor->map->cells[index].height = 0;
			}
			for(int j = editor->cellCursor[1] + 1; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					unsigned int index = j * editor->map->size[0] + i;
					editor->map->cells[index] = temp[index - editor->map->size[0]];
				}
			}
			// Set border walls
			editor->map->cells[editor->cellCursor[1] * editor->map->size[0]].passable = false;
			editor->map->cells[editor->cellCursor[1] * editor->map->size[0]].height = 3;
			editor->map->cells[editor->cellCursor[1] * editor->map->size[0] + editor->map->size[0] - 1].passable = false;
			editor->map->cells[editor->cellCursor[1] * editor->map->size[0] + editor->map->size[0] - 1].height = 3;
			// Update objects
			if(editor->map->flagPodPos[0][1] >= editor->cellCursor[1])
				editor->map->flagPodPos[0][1] += 1;
			if(editor->map->flagPodPos[1][1] >= editor->cellCursor[1])
				editor->map->flagPodPos[1][1] += 1;
			if(editor->map->objective[0][1] >= editor->cellCursor[1])
				editor->map->objective[0][1] += 1;
			if(editor->map->objective[1][1] >= editor->cellCursor[1])
				editor->map->objective[1][1] += 1;
			for(std::vector<CVector3f>::iterator i = editor->map->dm_spawns.begin();
				i < editor->map->dm_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] += 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->blue_spawns.begin();
				i < editor->map->blue_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] += 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->red_spawns.begin();
				i < editor->map->red_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] += 1;
			}
			lastAction = 0.0f;
			// Destroy old cells
			delete[] temp;
			editor->map->regenTex();
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					editor->map->regenCell(i, j);
				}
			}
			editor->isDirty = true;
		}
		break;
	case LINE_VERTICAL:
		if((editor->map->size[0] < 64) && (lastAction > 0.1f))
		{
			map_cell * temp = editor->map->cells;
			editor->map->size[0] += 1;
			editor->map->cells = new map_cell[editor->map->size[0] * editor->map->size[1]];
			// Copy old cells
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				unsigned int index = j * editor->map->size[0];
				for(int i = 0; i < editor->cellCursor[0]; ++i)
				{
					unsigned int indexNew = index + i;
					unsigned int indexOld = index - j + i;
					editor->map->cells[indexNew] = temp[indexOld];
				}
				editor->map->cells[index + editor->cellCursor[0]].passable = true;
				editor->map->cells[index + editor->cellCursor[0]].height = 0;
				for(int i = editor->cellCursor[0] + 1; i < editor->map->size[0]; ++i)
				{
					unsigned int indexNew = index + i;
					unsigned int indexOld = index - j + i - 1;
					editor->map->cells[indexNew] = temp[indexOld];
				}
			}
			// Set border walls
			editor->map->cells[editor->cellCursor[0]].passable = false;
			editor->map->cells[editor->cellCursor[0]].height = 3;
			editor->map->cells[(editor->map->size[1] - 1) * editor->map->size[0] + editor->cellCursor[0]].passable = false;
			editor->map->cells[(editor->map->size[1] - 1) * editor->map->size[0] + editor->cellCursor[0]].height = 3;
			// Update objects
			if(editor->map->flagPodPos[0][0] >= editor->cellCursor[0])
				editor->map->flagPodPos[0][0] += 1;
			if(editor->map->flagPodPos[1][0] >= editor->cellCursor[0])
				editor->map->flagPodPos[1][0] += 1;
			if(editor->map->objective[0][0] >= editor->cellCursor[0])
				editor->map->objective[0][0] += 1;
			if(editor->map->objective[1][0] >= editor->cellCursor[0])
				editor->map->objective[1][0] += 1;
			for(std::vector<CVector3f>::iterator i = editor->map->dm_spawns.begin();
				i < editor->map->dm_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] += 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->blue_spawns.begin();
				i < editor->map->blue_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] += 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->red_spawns.begin();
				i < editor->map->red_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] += 1;
			}
			lastAction = 0.0f;
			// Destroy old cells
			delete[] temp;
			editor->map->regenTex();
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					editor->map->regenCell(i, j);
				}
			}
			editor->isDirty = true;
		}
		break;
	}
}

void ToolAddLine::RightClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

void ToolAddLine::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

void ToolAddLine::NotifyActivate(bool active)
{
	if(!active)
	{
		lastAction = 1.0f;
	}
}

void ToolAddLine::ValidateCursor(CVector2i & cursor, const CVector2i & size) const
{
	if (cursor[0] < 1) cursor[0] = 1;
	if (cursor[1] < 1) cursor[1] = 1;
	if (cursor[0] > size[0] - 1) cursor[0] = size[0] - 1;
	if (cursor[1] > size[1] - 1) cursor[1] = size[1] - 1;
}

ToolRemoveLine::ToolRemoveLine(ToolRemoveLineType removeLineType)
	:	lineType(removeLineType)
	,	lastAction(1.0f)
{
}

ToolRemoveLine::~ToolRemoveLine()
{
}

void ToolRemoveLine::render(Editor2 * editor)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(3);
		CVector2f pt1, pt2, pt3, pt4;
		switch(lineType)
		{
		case LINE_HORIZONTAL:
			pt1.set(Selection,                            editor->cellCursor[1] + Selection);
			pt2.set(editor->map->size[0] + 1 - Selection, editor->cellCursor[1] + Selection);
			pt3.set(editor->map->size[0] + 1 - Selection, editor->cellCursor[1] + 1 - Selection);
			pt4.set(Selection,                            editor->cellCursor[1] + 1 - Selection);
			break;
		case LINE_VERTICAL:
			pt1.set(editor->cellCursor[0] + Selection,     editor->map->size[1] + 1 - Selection);
			pt2.set(editor->cellCursor[0] + Selection,     Selection);
			pt3.set(editor->cellCursor[0] + 1 - Selection, Selection);
			pt4.set(editor->cellCursor[0] + 1 - Selection, editor->map->size[1] + 1 - Selection);
			break;
		}
		glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
		glBegin(GL_QUADS);
			glVertex3f(pt1[0], pt1[1], Selection);
			glVertex3f(pt2[0], pt2[1], Selection);
			glVertex3f(pt3[0], pt3[1], Selection);
			glVertex3f(pt4[0], pt4[1], Selection);
		glEnd();
	glPopAttrib();
#endif
}

void ToolRemoveLine::LeftClick(Editor2 * editor, float delay)
{
	lastAction += delay;
	switch(lineType)
	{
	case LINE_HORIZONTAL:
		if((editor->map->size[1] > 16) && (lastAction > 0.1f))
		{
			map_cell * temp = editor->map->cells;
			editor->map->size[1] -= 1;
			editor->map->cells = new map_cell[editor->map->size[0] * editor->map->size[1]];
			// Copy old cells
			for(int j = 0; j < editor->cellCursor[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					unsigned int index = j * editor->map->size[0] + i;
					editor->map->cells[index] = temp[index];
				}
			}
			for(int j = editor->cellCursor[1]; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					unsigned int index = j * editor->map->size[0] + i;
					editor->map->cells[index] = temp[index + editor->map->size[0]];
				}
			}
			// Update objects
			if(editor->map->flagPodPos[0][1] >= editor->cellCursor[1])
				editor->map->flagPodPos[0][1] -= 1;
			if(editor->map->flagPodPos[1][1] >= editor->cellCursor[1])
				editor->map->flagPodPos[1][1] -= 1;
			if(editor->map->objective[0][1] >= editor->cellCursor[1])
				editor->map->objective[0][1] -= 1;
			if(editor->map->objective[1][1] >= editor->cellCursor[1])
				editor->map->objective[1][1] -= 1;
			for(std::vector<CVector3f>::iterator i = editor->map->dm_spawns.begin();
				i < editor->map->dm_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] -= 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->blue_spawns.begin();
				i < editor->map->blue_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] -= 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->red_spawns.begin();
				i < editor->map->red_spawns.end(); ++i)
			{
				if((*i)[1] >= editor->cellCursor[1])
					(*i)[1] -= 1;
			}
			lastAction = 0.0f;
			// Destroy old cells
			delete[] temp;
			editor->map->regenTex();
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					editor->map->regenCell(i, j);
				}
			}
			editor->isDirty = true;
		}
		break;
	case LINE_VERTICAL:
		if((editor->map->size[0] > 16) && (lastAction > 0.1f))
		{
			map_cell * temp = editor->map->cells;
			editor->map->size[0] -= 1;
			editor->map->cells = new map_cell[editor->map->size[0] * editor->map->size[1]];
			// Copy old cells
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				unsigned int indexNew = j * editor->map->size[0];
				unsigned int indexOld = j * (editor->map->size[0] + 1);
				for(int i = 0; i < editor->cellCursor[0]; ++i)
				{
					editor->map->cells[indexNew + i] = temp[indexOld + i];
				}
				for(int i = editor->cellCursor[0]; i < editor->map->size[0]; ++i)
				{
					editor->map->cells[indexNew + i] = temp[indexOld + i + 1];
				}
			}
			// Update objects
			if(editor->map->flagPodPos[0][0] >= editor->cellCursor[0])
				editor->map->flagPodPos[0][0] -= 1;
			if(editor->map->flagPodPos[1][0] >= editor->cellCursor[0])
				editor->map->flagPodPos[1][0] -= 1;
			if(editor->map->objective[0][0] >= editor->cellCursor[0])
				editor->map->objective[0][0] -= 1;
			if(editor->map->objective[1][0] >= editor->cellCursor[0])
				editor->map->objective[1][0] -= 1;
			for(std::vector<CVector3f>::iterator i = editor->map->dm_spawns.begin();
				i < editor->map->dm_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] -= 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->blue_spawns.begin();
				i < editor->map->blue_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] -= 1;
			}
			for(std::vector<CVector3f>::iterator i = editor->map->red_spawns.begin();
				i < editor->map->red_spawns.end(); ++i)
			{
				if((*i)[0] >= editor->cellCursor[0])
					(*i)[0] -= 1;
			}
			lastAction = 0.0f;
			// Destroy old cells
			delete[] temp;
			editor->map->regenTex();
			for(int j = 0; j < editor->map->size[1]; ++j)
			{
				for(int i = 0; i < editor->map->size[0]; ++i)
				{
					editor->map->regenCell(i, j);
				}
			}
			editor->isDirty = true;
		}
		break;
	}
}

void ToolRemoveLine::RightClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

void ToolRemoveLine::MiddleClick(Editor2 * editor, float delay)
{
	// We don't need the warnings!
	(void) editor;
	(void) delay;
}

void ToolRemoveLine::NotifyActivate(bool active)
{
	if(!active)
	{
		lastAction = 1.0f;
	}
}

#endif

