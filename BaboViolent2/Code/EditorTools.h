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

#ifndef EDITORTOOLS_H
#define EDITORTOOLS_H

#include "Zeven.h"
class Map;
class Editor2;

// Tool interface
class ITool
{
public:
	virtual ~ITool();
	virtual void render(Editor2 * editor) = 0;
	virtual void LeftClick(Editor2 * editor, float delay) = 0;
	virtual void RightClick(Editor2 * editor, float delay) = 0;
	virtual void MiddleClick(Editor2 * editor, float delay) = 0;
	virtual void NotifyActivate(bool active);
	virtual void ValidateCursor(CVector2i & cursor, const CVector2i & size) const;
protected:
	unsigned int MapCellIndex(Map * map, const CVector2i & cell) const;
	void RegenerateTextures(Map * map, const CVector2i & cell) const;
};

class ToolGround : public ITool
{
public:
	ToolGround();
	virtual ~ToolGround();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolSplat : public ITool
{
public:
	ToolSplat();
	virtual ~ToolSplat();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolWall : public ITool
{
private:
	unsigned char height;
public:
	ToolWall(unsigned char wallHeight);
	virtual ~ToolWall();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolFlag : public ITool
{
private:
	unsigned int flag;
public:
	ToolFlag(unsigned int flagIndex);
	virtual ~ToolFlag();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolBomb : public ITool
{
private:
	unsigned int bomb;
public:
	ToolBomb(unsigned int bombIndex);
	virtual ~ToolBomb();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolSpawn : public ITool
{
public:
	enum SpawnTypes
	{
		SPAWN_COMMON,
		SPAWN_BLUE,
		SPAWN_RED
	};
private:
	SpawnTypes spawn;
public:
	ToolSpawn(SpawnTypes spawnType);
	virtual ~ToolSpawn();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
};

class ToolAddLine : public ITool
{
public:
	enum ToolAddLineType
	{
		LINE_HORIZONTAL,
		LINE_VERTICAL
	};
private:
	ToolAddLineType lineType;
	float lastAction;
public:
	ToolAddLine(ToolAddLineType addLineType);
	virtual ~ToolAddLine();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
	virtual void NotifyActivate(bool active);
	virtual void ValidateCursor(CVector2i & cursor, const CVector2i & size) const;
};

class ToolRemoveLine : public ITool
{
public:
	enum ToolRemoveLineType
	{
		LINE_HORIZONTAL,
		LINE_VERTICAL
	};
private:
	ToolRemoveLineType lineType;
	float lastAction;
public:
	ToolRemoveLine(ToolRemoveLineType removeLineType);
	virtual ~ToolRemoveLine();
	virtual void render(Editor2 * editor);
	virtual void LeftClick(Editor2 * editor, float delay);
	virtual void RightClick(Editor2 * editor, float delay);
	virtual void MiddleClick(Editor2 * editor, float delay);
	virtual void NotifyActivate(bool active);
};

#endif