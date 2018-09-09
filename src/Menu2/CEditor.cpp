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

#include "CEditor.h"
#include "Console.h"
#include "GameVar.h"

CEditor::CEditor(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->texture = dktCreateTextureFromFile("main/textures/Menu2Back.tga", DKT_FILTER_LINEAR);
	instance->borderColor.set(1,.5f,.25f);

	btn_edit = new CControl(instance, CVector2i(20,10), CVector2i(75, 25), "Edit", this, "BUTTON");
	btn_edit->toolTips = "Edit the specified map. If no map of that name exists, one will be created.";

	//--- Labels and controls
	CControl * label1 = new CControl(instance, CVector2i(20,10), CVector2i(150,25),"Author name:", this, "LABEL", btn_edit, CONTROL_SNAP_BOTTOM, 10);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Author name (maximum 24 characters)";
	txt_authorName = new CControl(instance, CVector2i(10,10), CVector2i(300,25), gameVar.cl_mapAuthorName, this, "EDIT", label1, CONTROL_SNAP_RIGHT);

	label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"Map name:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Map name (maximum 15 characters)";
	txt_mapName = new CControl(instance, CVector2i(10,10), CVector2i(300,25),"", this, "EDIT", label1, CONTROL_SNAP_RIGHT);

	label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Width:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Width of map.";
	txt_width = new CControl(instance, CVector2i(10,10), CVector2i(100,40),"32", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
	txt_width->value = 32;
	txt_width->valueMin = 16;
	txt_width->valueMax = 64;
	btn_widthPreset[0] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"16", this, "BUTTON", txt_width, CONTROL_SNAP_RIGHT);
	btn_widthPreset[1] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"24", this, "BUTTON", btn_widthPreset[0], CONTROL_SNAP_RIGHT);
	btn_widthPreset[2] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"32", this, "BUTTON", btn_widthPreset[1], CONTROL_SNAP_RIGHT);
	btn_widthPreset[3] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"48", this, "BUTTON", btn_widthPreset[2], CONTROL_SNAP_RIGHT);
	btn_widthPreset[4] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"64", this, "BUTTON", btn_widthPreset[3], CONTROL_SNAP_RIGHT);

	label1 = new CControl(instance, CVector2i(10,10), CVector2i(150,40),"Height:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Height of map.";
	txt_height = new CControl(instance, CVector2i(10,10), CVector2i(100,40),"32", this, "SLIDER", label1, CONTROL_SNAP_RIGHT);
	txt_height->value = 32;
	txt_height->valueMin = 16;
	txt_height->valueMax = 64;
	btn_heightPreset[0] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"16", this, "BUTTON", txt_height, CONTROL_SNAP_RIGHT);
	btn_heightPreset[1] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"24", this, "BUTTON", btn_heightPreset[0], CONTROL_SNAP_RIGHT);
	btn_heightPreset[2] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"32", this, "BUTTON", btn_heightPreset[1], CONTROL_SNAP_RIGHT);
	btn_heightPreset[3] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"48", this, "BUTTON", btn_heightPreset[2], CONTROL_SNAP_RIGHT);
	btn_heightPreset[4] = new CControl(instance, CVector2i(10,10), CVector2i(40,40),"64", this, "BUTTON", btn_heightPreset[3], CONTROL_SNAP_RIGHT);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}

CEditor::~CEditor()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CEditor::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CEditor::MouseLeave(CControl * control)
{
}
void CEditor::MouseDown(CControl * control)
{
}
void CEditor::MouseUp(CControl * control)
{
}
void CEditor::MouseMove(CControl * control)
{
}
void CEditor::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	gameVar.cl_mapAuthorName.set("%.24s", txt_authorName->text.s);
	if (control == btn_widthPreset[0])
	{
		txt_width->value = 16;
		txt_width->text = CString("") + txt_width->value;
	}
	if (control == btn_widthPreset[1])
	{
		txt_width->value = 24;
		txt_width->text = CString("") + txt_width->value;
	}
	if (control == btn_widthPreset[2])
	{
		txt_width->value = 32;
		txt_width->text = CString("") + txt_width->value;
	}
	if (control == btn_widthPreset[3])
	{
		txt_width->value = 48;
		txt_width->text = CString("") + txt_width->value;
	}
	if (control == btn_widthPreset[4])
	{
		txt_width->value = 64;
		txt_width->text = CString("") + txt_width->value;
	}
	if (control == btn_heightPreset[0])
	{
		txt_height->value = 16;
		txt_height->text = CString("") + txt_height->value;
	}
	if (control == btn_heightPreset[1])
	{
		txt_height->value = 24;
		txt_height->text = CString("") + txt_height->value;
	}
	if (control == btn_heightPreset[2])
	{
		txt_height->value = 32;
		txt_height->text = CString("") + txt_height->value;
	}
	if (control == btn_heightPreset[3])
	{
		txt_height->value = 48;
		txt_height->text = CString("") + txt_height->value;
	}
	if (control == btn_heightPreset[4])
	{
		txt_height->value = 64;
		txt_height->text = CString("") + txt_height->value;
	}
	if (control == btn_edit)
	{
		// Ensure map name is 15 chars or less
		CString name ("%s", txt_mapName->text.s);
		name.resize(15);
		name.trim(' ');
		if (name.isNull()) return;

		CString command("edit %s %i %i", name.s, txt_width->value, txt_height->value);
		console->sendCommand(command);
	}
}
void CEditor::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	gameVar.cl_mapAuthorName.set("%.24s", txt_authorName->text.s);
}


#endif

