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
#include "EditorDialogs.h"
#include "CMenuManager.h"
#include "MessageDialog.h"
#include "Editor.h"
#include "FileIO.h"
#include <dkt.h>
#include <algorithm>

IEditorDialog::IEditorDialog(Editor2 * in_editor, unsigned int font, CString dialogTitle, CVector2i size, DialogButtons buttons)
	:	IDialog(font, dialogTitle, size, buttons)
	,	editor(in_editor)
{
}

IEditorDialog::~IEditorDialog()
{
}

SaveDirtyDialog::SaveDirtyDialog(Editor2 * editor, unsigned int font, bool closing)
	:	IEditorDialog(editor, font, "Warning!", CVector2i(300, 100), ButtonsYesNo)
	,	mustClose(closing)
	,	lbl_questionLine1(0)
	,	lbl_questionLine2(0)
{
	lbl_questionLine1 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280,  40), "Your current map wasn't saved yet.", this, "LABEL");
	lbl_questionLine2 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280,  40), "Would you like to save it first?", this, "LABEL", lbl_questionLine1, CONTROL_SNAP_BOTTOM);
	lbl_questionLine1->textAlign = CONTROL_TEXTALIGN_BOTTOMCENTER;
	lbl_questionLine2->textAlign = CONTROL_TEXTALIGN_TOPCENTER;
	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
}

SaveDirtyDialog::~SaveDirtyDialog()
{
	if(mustClose)
	{
		editor->needToShutDown = true;
		editor->ShowFrame(0);
	}
}

void SaveDirtyDialog::OnOk()
{
	editor->Save(editor->map->mapName);
}

AlreadyExistDialog::AlreadyExistDialog(Editor2 * editor, unsigned int font, bool & in_flag)
	:	IEditorDialog(editor, font, "Warning!", CVector2i(300, 100), ButtonsYesNo)
	,	flag(&in_flag)
	,	lbl_questionLine1(0)
	,	lbl_questionLine2(0)
{
	lbl_questionLine1 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280,  40), "Map with this name already exists.", this, "LABEL");
	lbl_questionLine2 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280,  40), "Do you want to overwrite?", this, "LABEL", lbl_questionLine1, CONTROL_SNAP_BOTTOM);
	lbl_questionLine1->textAlign = CONTROL_TEXTALIGN_BOTTOMCENTER;
	lbl_questionLine2->textAlign = CONTROL_TEXTALIGN_TOPCENTER;
	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
	*flag = false;
}

AlreadyExistDialog::~AlreadyExistDialog()
{
}

void AlreadyExistDialog::OnOk()
{
	*flag = true;
}

NewMapDialog::NewMapDialog(Editor2 * editor, unsigned int font, CString authorName, CString mapName, CVector2i size)
	:	IEditorDialog(editor, font, "Create New Map", CVector2i(450, 160), ButtonsOKCancel)
	,	subDialog(0)
{
	if(editor->isDirty)
	{
		// Ask for confirmation
		subDialog = new SaveDirtyDialog(editor, font);
	}
	if(authorName.len() > 24) authorName.resize(24);
	if(mapName.len() > 15) mapName.resize(15);
	if(size[0] < 16) size[0] = 16;
	if(size[1] < 16) size[1] = 16;
	if(size[0] > 64) size[0] = 64;
	if(size[1] > 64) size[1] = 64;

	lbl_authorName   = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  25), "Author name:", this, "LABEL");
	lbl_authorName->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_authorName->toolTips = "Author name (maximum 24 characters)";
	txt_authorName   = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(325,  25), authorName,             this, "EDIT",   lbl_authorName,   CONTROL_SNAP_RIGHT);
	lbl_mapName      = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  25), "Map name:",            this, "LABEL",  lbl_authorName,   CONTROL_SNAP_BOTTOM);
	lbl_mapName->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_mapName->toolTips = "Map name (maximum 15 characters). You can leave it blank and use \"Save As\" later.";
	txt_mapName      = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(325,  25), mapName,                this, "EDIT",   txt_authorName,   CONTROL_SNAP_BOTTOM);
	lbl_mapWidth     = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  35), "Map width:",           this, "LABEL",  lbl_mapName,      CONTROL_SNAP_BOTTOM);
	lbl_mapWidth->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_mapWidth->toolTips = "Width of the map [16; 64]. You can change the maps size later.";
	sld_mapWidth     = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  35), CString("%d", size[0]), this, "SLIDER", lbl_mapWidth,     CONTROL_SNAP_RIGHT);
	sld_mapWidth->borderColor.set(1.0f, 0.5f, 0.25f);
	sld_mapWidth->value = size[0];
	sld_mapWidth->valueMin = 16;
	sld_mapWidth->valueMax = 64;
	btn_mapWidth[0]  = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "16",                   this, "BUTTON", sld_mapWidth,     CONTROL_SNAP_RIGHT);
	btn_mapWidth[1]  = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "24",                   this, "BUTTON", btn_mapWidth[0],  CONTROL_SNAP_RIGHT);
	btn_mapWidth[2]  = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "32",                   this, "BUTTON", btn_mapWidth[1],  CONTROL_SNAP_RIGHT);
	btn_mapWidth[3]  = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "48",                   this, "BUTTON", btn_mapWidth[2],  CONTROL_SNAP_RIGHT);
	btn_mapWidth[4]  = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "64",                   this, "BUTTON", btn_mapWidth[3],  CONTROL_SNAP_RIGHT);
	lbl_mapHeight    = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  35), "Map height:",          this, "LABEL",  lbl_mapWidth,     CONTROL_SNAP_BOTTOM);
	lbl_mapHeight->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_mapHeight->toolTips = "Height of the map [16; 64]. You can change the maps size later.";
	sld_mapHeight    = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  35), CString("%d", size[1]), this, "SLIDER", lbl_mapHeight,    CONTROL_SNAP_RIGHT);
	sld_mapHeight->borderColor.set(1.0f, 0.5f, 0.25f);
	sld_mapHeight->value = size[1];
	sld_mapHeight->valueMin = 16;
	sld_mapHeight->valueMax = 64;
	btn_mapHeight[0] = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "16",                   this, "BUTTON", sld_mapHeight,    CONTROL_SNAP_RIGHT);
	btn_mapHeight[1] = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "24",                   this, "BUTTON", btn_mapHeight[0], CONTROL_SNAP_RIGHT);
	btn_mapHeight[2] = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "32",                   this, "BUTTON", btn_mapHeight[1], CONTROL_SNAP_RIGHT);
	btn_mapHeight[3] = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "48",                   this, "BUTTON", btn_mapHeight[2], CONTROL_SNAP_RIGHT);
	btn_mapHeight[4] = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i( 40,  35), "64",                   this, "BUTTON", btn_mapHeight[3], CONTROL_SNAP_RIGHT);

	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
	btn_ok->text.set("Create");

	if(subDialog != 0)
	{
		dialogRoot->visible = false;
	}
}

NewMapDialog::~NewMapDialog()
{
	delete subDialog;
}

void NewMapDialog::update(float delay)
{
	if(subDialog != 0)
	{
		//subDialog->update(delay);
		if(subDialog->state == StateYes)
		{
			menuManager.hideDialog();
			delete subDialog;
			subDialog = 0;
			dialogRoot->visible = true;
		}
	}
	else
	{
		IDialog::update(delay);
	}
}

void NewMapDialog::render()
{
	/*if(subDialog != 0)
	{
		subDialog->render();
	}
	else*/
	{
		IDialog::render();;
	}
}

void NewMapDialog::Click(CControl * control)
{
	// Notify parent
	IDialog::Click(control);

	Validate(control);
	if (control == btn_mapWidth[0])
	{
		sld_mapWidth->value = 16;
		sld_mapWidth->text.set("%d", sld_mapWidth->value);
		return;
	}
	if (control == btn_mapWidth[1])
	{
		sld_mapWidth->value = 24;
		sld_mapWidth->text.set("%d", sld_mapWidth->value);
		return;
	}
	if (control == btn_mapWidth[2])
	{
		sld_mapWidth->value = 32;
		sld_mapWidth->text.set("%d", sld_mapWidth->value);
		return;
	}
	if (control == btn_mapWidth[3])
	{
		sld_mapWidth->value = 48;
		sld_mapWidth->text.set("%d", sld_mapWidth->value);
		return;
	}
	if (control == btn_mapWidth[4])
	{
		sld_mapWidth->value = 64;
		sld_mapWidth->text.set("%d", sld_mapWidth->value);
		return;
	}
	if (control == btn_mapHeight[0])
	{
		sld_mapHeight->value = 16;
		sld_mapHeight->text.set("%d", sld_mapHeight->value);
		return;
	}
	if (control == btn_mapHeight[1])
	{
		sld_mapHeight->value = 24;
		sld_mapHeight->text.set("%d", sld_mapHeight->value);
		return;
	}
	if (control == btn_mapHeight[2])
	{
		sld_mapHeight->value = 32;
		sld_mapHeight->text.set("%d", sld_mapHeight->value);
		return;
	}
	if (control == btn_mapHeight[3])
	{
		sld_mapHeight->value = 48;
		sld_mapHeight->text.set("%d", sld_mapHeight->value);
		return;
	}
	if (control == btn_mapHeight[4])
	{
		sld_mapHeight->value = 64;
		sld_mapHeight->text.set("%d", sld_mapHeight->value);
		return;
	}
}

void NewMapDialog::Validate(CControl * control)
{
	txt_authorName->text.resize(24);
	txt_mapName->text.resize(15);
}

void NewMapDialog::OnOk()
{
	CString fileName("main\\maps\\%s.bvm", txt_mapName->text.s);
	FileIO f(fileName, "rb");
	if(!f.isValid())
	{
		txt_authorName->text.resize(24);
		txt_mapName->text.resize(15);
		delete editor->map;
		gameVar.cl_mapAuthorName = txt_authorName->text;
		editor->map = new Map(txt_mapName->text, 0, dialogRoot->font, true, sld_mapWidth->value, sld_mapHeight->value);
	}
	else
	{
		subDialog = new MessageDialog(dialogRoot->font, ButtonsOK,
			"Map with this name already exists!",
			"If you want to open the existing map,",
			"choose File -> Open.");
		state = StateNothing;
	}
}

OpenMapDialog::OpenMapDialog(Editor2 * editor, unsigned int font)
	:	IEditorDialog(editor, font, "Open Existing Map", CVector2i(500, 300), ButtonsOKCancel)
	,	mapInfo()
	,	subDialog(0)
{
	if(editor->isDirty)
	{
		// Ask for confirmation
		subDialog = new SaveDirtyDialog(editor, font);
	}

	lbl_mapName    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(255,  25), "Map:",     this, "LABEL");
	lst_mapName    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(255, 250), "",         this, "LISTBOX", lbl_mapName,   CONTROL_SNAP_BOTTOM);
	lbl_mapAuthor  = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220,  25), "Author: ", this, "LABEL",   lbl_mapName,   CONTROL_SNAP_RIGHT);
	lbl_mapSize    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220,  25), "Size: ",   this, "LABEL",   lbl_mapAuthor, CONTROL_SNAP_BOTTOM);
	frm_mapPreview = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220, 220), "Preview",  this, "FRAME",   lbl_mapSize,   CONTROL_SNAP_BOTTOM);
	//btn_mapName    = new CControl();

	std::vector< CString > maps;
	GetMapList(maps);
	std::sort(maps.begin(), maps.end());
	CControl * lbl_mapNameTemp = 0;
	for(std::vector< CString >::iterator it = maps.begin(); it != maps.end(); ++it)
	{
		mapInfo[*it].author = "";
		mapInfo[*it].textureSize.set(0, 0);
		mapInfo[*it].size.set(0, 0);
		mapInfo[*it].texture = 0;
		lbl_mapNameTemp = new CControl(lst_mapName, CVector2i(10, 10), CVector2i(150, 25), *it, this, "LABEL", lbl_mapNameTemp, CONTROL_SNAP_BOTTOM);
		lbl_mapNameItem.push_back(lbl_mapNameTemp);
	}
	lst_mapName->selectChild(-10);

	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
	btn_ok->text.set("Open");

	if(subDialog != 0)
	{
		dialogRoot->visible = false;
	}
}

OpenMapDialog::~OpenMapDialog()
{
	for(std::map< CString, MapInfo >::iterator i = mapInfo.begin();
		i != mapInfo.end(); ++i)
	{
		if((*i).second.texture)
		{
			dktDeleteTexture(&((*i).second.texture));
		}
	}
	delete subDialog;
}

void OpenMapDialog::update(float delay)
{
	if(subDialog != 0)
	{
		//subDialog->update(delay);
		if(subDialog->state == StateYes)
		{
			menuManager.hideDialog();
			delete subDialog;
			subDialog = 0;
			dialogRoot->visible = true;
		}
	}
	else
	{
		IDialog::update(delay);
	}
}

void OpenMapDialog::render()
{
	/*if(subDialog != 0)
	{
		subDialog->render();
	}
	else*/
	{
		IDialog::render();;
	}
}

void OpenMapDialog::Click(CControl * control)
{
	// Notify parent
	IDialog::Click(control);
	// If clicked on one of the map names
	std::vector< CControl * >::const_iterator map =
		std::find(lbl_mapNameItem.begin(), lbl_mapNameItem.end(), control);
	if(map != lbl_mapNameItem.end())
	{
		if(!mapInfo[(*map)->text].texture)
		{
			GetMapData((*map)->text, mapInfo[(*map)->text].texture, mapInfo[(*map)->text].textureSize, mapInfo[(*map)->text].size, mapInfo[(*map)->text].author);
		}
		lbl_mapAuthor->text.set("Author: %s", mapInfo[(*map)->text].author.s);
		lbl_mapSize->text.set("Size: %dx%d", mapInfo[(*map)->text].size[0], mapInfo[(*map)->text].size[1]);
		frm_mapPreview->imgColor.set(1,1,1);
		frm_mapPreview->texture = mapInfo[(*map)->text].texture;
		frm_mapPreview->textureCorner.set(float(mapInfo[(*map)->text].size[0]) / float(mapInfo[(*map)->text].textureSize[0]), float(mapInfo[(*map)->text].size[1]) / float(mapInfo[(*map)->text].textureSize[1]));
		frm_mapPreview->useTextureCorner = true;
		frm_mapPreview->foreColor.set(1,1,1);
		frm_mapPreview->text.set("");
		frm_mapPreview->size.set(int(mapInfo[(*map)->text].size[0] * 220.0f / std::max<int>(mapInfo[(*map)->text].size[0], mapInfo[(*map)->text].size[1])), int(mapInfo[(*map)->text].size[1] * 220.0f / std::max<int>(mapInfo[(*map)->text].size[0], mapInfo[(*map)->text].size[1])));
	}
}

void OpenMapDialog::OnOk()
{
	delete editor->map;
	CString & mapName = lst_mapName->selectedChild->text;
	editor->map = new Map(mapName, 0, dialogRoot->font, true);
	editor->isDirty = false;
}

void SaveAsMapDialog::LoadInfo(CString name)
{
	if(mapInfo.find(name) != mapInfo.end())
	{
		if(!mapInfo[name].texture)
		{
			GetMapData(name, mapInfo[name].texture, mapInfo[name].textureSize, mapInfo[name].size, mapInfo[name].author);
		}
		txt_mapName->text.set("%s", name.s);
		lbl_mapAuthor->text.set("Author: %s", mapInfo[name].author.s);
		lbl_mapSize->text.set("Size: %dx%d", mapInfo[name].size[0], mapInfo[name].size[1]);
		frm_mapPreview->imgColor.set(1,1,1);
		frm_mapPreview->texture = mapInfo[name].texture;
		frm_mapPreview->textureCorner.set(float(mapInfo[name].size[0]) / float(mapInfo[name].textureSize[0]), float(mapInfo[name].size[1]) / float(mapInfo[name].textureSize[1]));
		frm_mapPreview->useTextureCorner = true;
		frm_mapPreview->foreColor.set(1,1,1);
		frm_mapPreview->text.set("");
		frm_mapPreview->size.set(int(mapInfo[name].size[0] * 220.0f / std::max<int>(mapInfo[name].size[0], mapInfo[name].size[1])), int(mapInfo[name].size[1] * 220.0f / std::max<int>(mapInfo[name].size[0], mapInfo[name].size[1])));
	}
	else
	{
		frm_mapPreview->texture = 0;
		lbl_mapAuthor->text.set("Author: ");
		lbl_mapSize->text.set("Size: ");
	}
}

SaveAsMapDialog::SaveAsMapDialog(Editor2 * editor, unsigned int font, CString author, CString name)
	:	IEditorDialog(editor, font, "Save Map As", CVector2i(500, 300), ButtonsOKCancel)
	,	mapInfo()
	,	overwrite(false)
	,	closeAfterSub(false)
	,	subDialog(0)
{
	if(author == "")
	{
		author = gameVar.cl_mapAuthorName;
	}
	lbl_authorName = new CControl(dialogFrame, CVector2i(10, 10), CVector2i( 75,  25), "Author:",  this, "LABEL");
	txt_authorName = new CControl(dialogFrame, CVector2i( 5, 10), CVector2i(175,  25), author,     this, "EDIT",    lbl_authorName, CONTROL_SNAP_RIGHT);
	lbl_mapName    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i( 75,  25), "Map:",     this, "LABEL",   lbl_authorName, CONTROL_SNAP_BOTTOM);
	txt_mapName    = new CControl(dialogFrame, CVector2i( 5, 10), CVector2i(175,  25), name,       this, "EDIT",    lbl_mapName,    CONTROL_SNAP_RIGHT);
	lst_mapName    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(255, 220), "",         this, "LISTBOX", lbl_mapName,    CONTROL_SNAP_BOTTOM);
	lbl_mapAuthor  = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220,  25), "Author: ", this, "LABEL",   txt_authorName, CONTROL_SNAP_RIGHT);
	lbl_mapSize    = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220,  25), "Size: ",   this, "LABEL",   lbl_mapAuthor,  CONTROL_SNAP_BOTTOM);
	frm_mapPreview = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(220, 220), "Preview",  this, "FRAME",   lbl_mapSize,    CONTROL_SNAP_BOTTOM);
	//btn_mapName    = new CControl();

	std::vector< CString > maps;
	GetMapList(maps);
	std::sort(maps.begin(), maps.end());
	CControl * lbl_mapNameTemp = 0;
	for(std::vector< CString >::iterator it = maps.begin(); it != maps.end(); ++it)
	{
		mapInfo[*it].author = "";
		mapInfo[*it].textureSize.set(0, 0);
		mapInfo[*it].size.set(0, 0);
		mapInfo[*it].texture = 0;
		lbl_mapNameTemp = new CControl(lst_mapName, CVector2i(10, 10), CVector2i(150, 25), *it, this, "LABEL", lbl_mapNameTemp, CONTROL_SNAP_BOTTOM);
		lbl_mapNameItem.push_back(lbl_mapNameTemp);
		if(txt_mapName->text == *it)
		{
			lst_mapName->selectedChild = lbl_mapNameTemp;
			LoadInfo(*it);
		}
	}

	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
	btn_ok->text.set("Save");
}

SaveAsMapDialog::~SaveAsMapDialog()
{
	for(std::map< CString, MapInfo >::iterator i = mapInfo.begin();
		i != mapInfo.end(); ++i)
	{
		if((*i).second.texture)
		{
			dktDeleteTexture(&((*i).second.texture));
		}
	}
	delete subDialog;
}

void SaveAsMapDialog::update(float delay)
{
	if(subDialog != 0)
	{
		//subDialog->update(delay);
		if(subDialog->state == StateYes || subDialog->state == StateOK)
		{
			menuManager.hideDialog();
			delete subDialog;
			subDialog = 0;
			dialogRoot->visible = true;
			if(closeAfterSub)
			{
				//if(overwrite)
				{
					/*done = true;
					canceled = false;*/
					state = StateOK;
					Finish();
				}
				/*else
				{
					done = true;
					canceled = true;
					Finish();
				}*/
			}
		}
	}
	else
	{
		IDialog::update(delay);
	}
}

void SaveAsMapDialog::render()
{
	/*if(subDialog != 0)
	{
		subDialog->render();
	}
	else*/
	{
		IDialog::render();;
	}
}

void SaveAsMapDialog::Click(CControl * control)
{
	// Notify parent
	IDialog::Click(control);
	// If clicked on one of the map names
	std::vector< CControl * >::const_iterator map =
		std::find(lbl_mapNameItem.begin(), lbl_mapNameItem.end(), control);
	if(map != lbl_mapNameItem.end())
	{
		LoadInfo((*map)->text);
	}
}

void SaveAsMapDialog::Validate(CControl * control)
{
	if(control == txt_mapName)
	{
		bool found = false;
		int index = 0;
		for(std::vector< CControl * >::const_iterator map = lst_mapName->children.begin();
			map != lst_mapName->children.end(); ++map, ++index)
		{
			if(txt_mapName->text == (*map)->text)
			{
				lst_mapName->selectChild(index - 2);
				found = true;
				LoadInfo(txt_mapName->text);
			}
		}
		if(!found)
		{
			lbl_mapName->selectChild(-10);
			LoadInfo("");
		}
	}
}

void SaveAsMapDialog::OnOk()
{
	if(txt_mapName->text == "")
	{
		state = StateNothing;
		closeAfterSub = false;
		subDialog = new MessageDialog(dialogRoot->font, ButtonsOK, "Error!", "You forgot to enter the file name.");
		return;
	}
	FileIO file(CString("main\\maps\\%s.bvm", txt_mapName->text.s), "rb");
	if(file.isValid() && !overwrite)
	{
		state = StateNothing;
		closeAfterSub = true;
		subDialog = new AlreadyExistDialog(editor, dialogRoot->font, overwrite);
		return;
	}
	gameVar.cl_mapAuthorName = txt_authorName->text;
	editor->Save(txt_mapName->text);
}

#endif

