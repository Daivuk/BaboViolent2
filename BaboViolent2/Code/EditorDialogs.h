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

#ifndef EDITORDIALOGS_H
#define EDITORDIALOGS_H

#include "Zeven.h"
#include "CControl.h"
#include "Dialog.h"
//#include "CPanel.h"
#include <vector>
#include <map>
class Map;
class Editor2;

class IEditorDialog : public IDialog
{
public:
	// Editor (just in case)
	Editor2 * editor;

public:
	// Constructor/Destructor
	IEditorDialog(Editor2 * in_editor, unsigned int font, CString dialogTitle, CVector2i size, DialogButtons buttons);
	virtual ~IEditorDialog();
};

class SaveDirtyDialog : public IEditorDialog
{
public:
	// Must close editor after answer
	bool mustClose;

	// Controls
	CControl * lbl_questionLine1;
	CControl * lbl_questionLine2;
public:
	// Constructor/Destructor
	SaveDirtyDialog(Editor2 * editor, unsigned int font, bool closing = false);
	~SaveDirtyDialog();

	// Dialog events
	virtual void OnOk();
};

class AlreadyExistDialog : public IEditorDialog
{
public:
	// Must close editor after answer
	bool * flag;

	// Controls
	CControl * lbl_questionLine1;
	CControl * lbl_questionLine2;
public:
	// Constructor/Destructor
	AlreadyExistDialog(Editor2 * editor, unsigned int font, bool & in_flag);
	~AlreadyExistDialog();

	// Dialog events
	virtual void OnOk();
};

class NewMapDialog : public IEditorDialog
{
public:
	// Used for sub dialogs (errors, warnings)
	IDialog * subDialog;
	// Controls
	CControl * lbl_authorName;
	CControl * txt_authorName;
	CControl * lbl_mapName;
	CControl * txt_mapName;
	CControl * lbl_mapWidth;
	CControl * sld_mapWidth;
	CControl * btn_mapWidth[5];
	CControl * lbl_mapHeight;
	CControl * sld_mapHeight;
	CControl * btn_mapHeight[5];
public:
	// Constructor/Destructor
	NewMapDialog(Editor2 * editor, unsigned int font, CString authorName = "", CString mapName = "", CVector2i size = CVector2i(32, 32));
	~NewMapDialog();

	// update
	virtual void update(float delay);

	// render
	virtual void render();

	// Events
	virtual void Click(CControl * control);
	virtual void Validate(CControl * control);

	// Dialog events
	virtual void OnOk();
};

struct MapInfo
{
	CString author;
	CVector2i size;
	CVector2i textureSize;
	unsigned int texture;
};

class OpenMapDialog : public IEditorDialog
{
public:
	// Cached map infos
	std::map< CString, MapInfo > mapInfo;
	// Used for sub dialogs (errors, warnings)
	IDialog * subDialog;
	// Controls
	CControl * lbl_mapName;
	CControl * lst_mapName;
	std::vector< CControl * > lbl_mapNameItem;
	CControl * lbl_mapAuthor;
	CControl * lbl_mapSize;
	CControl * frm_mapPreview;
public:
	// Constructor/Destructor
	OpenMapDialog(Editor2 * editor, unsigned int font);
	~OpenMapDialog();

	// update
	virtual void update(float delay);

	// render
	virtual void render();

	// Events
	virtual void Click(CControl * control);

	// Dialog events
	virtual void OnOk();
};

class SaveAsMapDialog : public IEditorDialog
{
public:
	// Cached map infos
	std::map< CString, MapInfo > mapInfo;
	// overwrite flag
	bool overwrite;
	// Close after sub dialog is finished
	bool closeAfterSub;
	// Used for sub dialogs (errors, warnings)
	IDialog * subDialog;
	// Controls
	CControl * lbl_authorName;
	CControl * txt_authorName;
	CControl * lbl_mapName;
	CControl * txt_mapName;
	CControl * lst_mapName;
	std::vector< CControl * > lbl_mapNameItem;
	CControl * lbl_mapAuthor;
	CControl * lbl_mapSize;
	CControl * frm_mapPreview;
private:
	void LoadInfo(CString name);
public:
	// Constructor/Destructor
	SaveAsMapDialog(Editor2 * editor, unsigned int font, CString author = "", CString name = "");
	~SaveAsMapDialog();

	// update
	virtual void update(float delay);

	// render
	virtual void render();

	// Events
	virtual void Click(CControl * control);
	virtual void Validate(CControl * control);

	// Dialog events
	virtual void OnOk();
};

#endif

