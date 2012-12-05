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

#include "CCredit.h"



CCredit::CCredit(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->texture = dktCreateTextureFromFile("main/textures/Menu5Back.tga", DKT_FILTER_LINEAR);
	instance->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	instance->borderColor.set(1,.5f,.25f);

	CControl * label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,64),"BaboViolent 2 | Team", this, "LABEL");
	label1->textSize = 64;
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- game Designer
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Game Designer", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"David St-Louis \"\x3RndLabs\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- Programers
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Programmers", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"David St-Louis \"\x3RndLabs\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Marc Durocher \"\x3RndLabs\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// nuvem
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Jason \"nuvem\" Kozak", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// cnik
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Dominik \"cnik\" Kornaus", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// PM
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Paulius \"PM\" Maruska", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Lordlou
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Louis \"Lordlou\" Poirier", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- Graphics
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Modeling / Graphics", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Adam
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Adam Pilkington \"\x3HeadGames Art Lead\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Daivuk
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"David St-Louis \"\x3RndLabs\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Louis
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Louis-Nicolas Dozois \"\x3HeadGames\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Pacifist
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Michal \"Pacifist\" Mojzik", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- Sounds
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Sound designers", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Dominic \"Doimuk\" Valiquette", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- Music
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Music samples", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Lamb of God", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//--- Testers
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Quality Control", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 30);
	label1->foreColor.set(1,1,.5f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Sunita
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Sunita Kollipara \"\x3HeadGames\"", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	// Community
	label1 = new CControl(instance, CVector2i(10,10), CVector2i(700,25),"Whole www.baboviolent.net community - Thanks guys!", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label1->foreColor.set(.5f,1,.75f);
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;


	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}


CCredit::~CCredit()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CCredit::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CCredit::MouseLeave(CControl * control)
{
}
void CCredit::MouseDown(CControl * control)
{
}
void CCredit::MouseUp(CControl * control)
{
}
void CCredit::MouseMove(CControl * control)
{
}
void CCredit::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}
void CCredit::Validate(CControl * control)
{
	dksPlaySound(m_sfxClic, -1, 200);
}


#endif

