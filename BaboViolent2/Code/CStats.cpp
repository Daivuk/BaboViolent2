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

#include "CStats.h"

CStats::CStats(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(in_parent->size.s[0]-20, 450), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->borderColor.set(1,.5f,.25f);

	btn_refresh = new CControl(instance, CVector2i(10,10),CVector2i(60,25),"Refresh",this,"BUTTON");
	btn_refresh->toolTips = "Refresh the list of clans!";

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	originalY = instance->localPos[1];
}

CStats::~CStats()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


//
//--- Update it
//
void CStats::updatePerso(float delay)
{
}

void CStats::updateList()
{
}

void CStats::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CStats::MouseLeave(CControl * control)
{
}
void CStats::MouseDown(CControl * control)
{
}
void CStats::MouseUp(CControl * control)
{
}
void CStats::MouseMove(CControl * control)
{
}
void CStats::Click(CControl * control)
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
void CStats::Validate(CControl * control)
{
	dksPlaySound(m_sfxClic, -1, 200);
}
void CStats::DbClick(CControl * control)
{
}


#endif

