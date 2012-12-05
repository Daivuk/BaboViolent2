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

#include "CRegisterClan.h"

CRegisterClan::CRegisterClan(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(in_parent->size.s[0]-20, 450), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	//instance->texture = dktCreateTextureFromFile("main/textures/Menu3Back.tga", DKT_FILTER_LINEAR);
	//instance->borderColor.set(1,.5f,.25f);

	CControl * label1 = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Clan name:", this, "LABEL");
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Clan name has to be unique.";
	txt_clanName = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label1, CONTROL_SNAP_RIGHT);
	CControl * req1 = new CControl(instance, CVector2i(10,0), CVector2i(25,25),"*", this, "LABEL", txt_clanName, CONTROL_SNAP_RIGHT);

	CControl * label2 = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Clan tag:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label2->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label2->toolTips = "Clan tag has to be unique.";
	txt_clanTag = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label2, CONTROL_SNAP_RIGHT);
	CControl * req2 = new CControl(instance, CVector2i(10,0), CVector2i(25,25),"*", this, "LABEL", txt_clanTag, CONTROL_SNAP_RIGHT);

	CControl * label3 = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Contact email:", this, "LABEL", label2, CONTROL_SNAP_BOTTOM);
	label3->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	txt_email = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label3, CONTROL_SNAP_RIGHT);

	CControl * label4 = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Website:", this, "LABEL", label3, CONTROL_SNAP_BOTTOM);
	label4->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	txt_website = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label4, CONTROL_SNAP_RIGHT);

	CControl * reqLabel = new CControl(instance, CVector2i(10,0), CVector2i(200,35),"(* - required fields)", this, "LABEL", label4, CONTROL_SNAP_BOTTOM);

	btn_register = new CControl(instance, CVector2i(20,30),CVector2i(120,25),"Register",this,"BUTTON", reqLabel, CONTROL_SNAP_BOTTOM);
	btn_register->toolTips = "Register clan!";

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}

CRegisterClan::~CRegisterClan()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CRegisterClan::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CRegisterClan::MouseLeave(CControl * control)
{
}
void CRegisterClan::MouseDown(CControl * control)
{
}
void CRegisterClan::MouseUp(CControl * control)
{
}
void CRegisterClan::MouseMove(CControl * control)
{
}
void CRegisterClan::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}
void CRegisterClan::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}

#endif

