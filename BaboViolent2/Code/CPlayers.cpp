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

#include "CPlayers.h"

CPlayers::CPlayers(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(in_parent->size.s[0]-20, 450), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->borderColor.set(1,.5f,.25f);

	CControl * label = new CControl(instance, CVector2i(10,10), CVector2i(80,25),"Filter: ", this, "LABEL");

	txt_filter = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"", this, "EDIT", label, CONTROL_SNAP_RIGHT);

	btn_refresh = new CControl(instance, CVector2i(32,32),CVector2i(60,25),"Refresh",this,"BUTTON", txt_filter, CONTROL_SNAP_RIGHT, 5);
	btn_refresh->toolTips = "Refresh the list of clans!";

	lst_browseList = new CControl(instance, CVector2i(0,0), CVector2i(716,460), "", this, "LISTBOX", label, CONTROL_SNAP_BOTTOM, 5);
	lst_browseList->texture = dktCreateTextureFromFile("main/textures/Menu2Back.tga", DKT_FILTER_LINEAR);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	lst_browseList->backColor.set(0,.3f,.7f);
	lst_browseList->imgColor = lst_browseList->backColor;

	originalY = instance->localPos[1];
}

CPlayers::~CPlayers()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


//
//--- Update it
//
void CPlayers::updatePerso(float delay)
{
}

void CPlayers::updateList()
{
}

void CPlayers::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CPlayers::MouseLeave(CControl * control)
{
}
void CPlayers::MouseDown(CControl * control)
{
}
void CPlayers::MouseUp(CControl * control)
{
}
void CPlayers::MouseMove(CControl * control)
{
}
void CPlayers::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_refresh)
	{
		updateList();
	}
}
void CPlayers::Validate(CControl * control)
{
	dksPlaySound(m_sfxClic, -1, 200);
}
void CPlayers::DbClick(CControl * control)
{
}


#endif

