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

#ifndef EDITOR_H
#define EDITOR_H

#include "Zeven.h"
#include "Map.h"
#include "CControl.h"
#include "CListener.h"
#include <vector>

class ITool;
class IDialog;

class Editor2 : public CListener
{
public:
	// Set to true when need to shut down the editor
	bool needToShutDown;

	// Is the map saved or dirty?
	bool isDirty;

	// Time since last (cursor in frame state)
	float inFrameFor;

	// The map we're working on
	Map * map;

	// Position of the cursor
	CVector3f mouseOnMap;
	CVector2i cellCursor;
	bool mouseInMenu;

	// All of our tools
	std::vector< ITool* > tools;

	// Currently active tool
	std::vector< ITool* >::size_type active;

	// Root for all the controls
	CControl * editorRoot;

	// Editor menu
	CControl * frameMain;
	CControl * btn_file;
	CControl * btn_terrain;
	CControl * btn_objects;
	CControl * btn_size;
	CControl * btn_misc;
	CControl * btn_quit;

	CControl * frameFile;
	CControl * btn_new;
	CControl * btn_open;
	CControl * btn_save;
	CControl * btn_saveAs;

	CControl * frameTerrain;
	CControl * btn_ground;
	CControl * btn_alternative;
	CControl * btn_block1;
	CControl * btn_block2;
	CControl * btn_block3;
	CControl * btn_block4;
	CControl * btn_block5;

	CControl * frameObjects;
	CControl * btn_redFlag;
	CControl * btn_blueFlag;
	CControl * btn_bombSite1;
	CControl * btn_bombSite2;
	CControl * btn_spawn;
	CControl * btn_redSpawn;
	CControl * btn_blueSpawn;

	CControl * frameSize;
	CControl * btn_insertLineH;
	CControl * btn_insertLineV;
	CControl * btn_removeLineH;
	CControl * btn_removeLineV;

	CControl * frameMisc;
	CControl * btn_theme;
	CControl * btn_clearMap;
	
	// Dialogs
	IDialog * activeDialog;
public:
	// Constructor / Destructor
	Editor2(CString mapName, unsigned int font, int sizeX = 32, int sizeY = 32);
	virtual ~Editor2();

	// Activates the tool
	void ActivateTool(std::vector< ITool* >::size_type tool);

	// Returns true when mouse cursor is inside any of the frames
	bool isCursorInControl(CControl * control) const;

	// Returns true when mouse cursor is inside any of the frames
	bool isCursorInFrame(float delay);

	// Hides all frames except the main frame and the one you pass as an argument
	void ShowFrame(CControl * frame);

	// Updates everything
	void update(float delay);

	// Renders everything
	void render();

	// Renders the minimap
	void renderMiniMap();

	// Renders a square around the control
	void renderSquare(const CControl * control, int distance, const CColor4f & color) const;

	// Render selection on tool
	void renderSelection() const;

	// Open exisiting map
	void Open(CString mapName);

	// Save the map (moved here from Map::~Map)
	void Save(CString mapName);

	// Performs an action when one of the buttons is clicked
	void Click(CControl * control);
};

typedef Editor2 Editor;

#endif

