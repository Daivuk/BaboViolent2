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

#include "CNews.h"

CNews::CNews(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	//instance->texture = dktCreateTextureFromFile("main/textures/Menu3Back.tga", DKT_FILTER_LINEAR);
	//instance->borderColor.set(1,.5f,.25f);

	btn_refresh = new CControl(instance, CVector2i(10,10),CVector2i(120,25),"Refresh",this,"BUTTON");
	btn_refresh->toolTips = "Refresh the news";

	lst_news = new CControl(instance, CVector2i(0,0), CVector2i(716,460), "", this, "LISTBOX", btn_refresh, CONTROL_SNAP_BOTTOM, 5);
	lst_news->texture = dktCreateTextureFromFile("main/textures/Menu2Back.tga", DKT_FILTER_LINEAR);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	lst_news->backColor.set(0,.3f,.7f);
	lst_news->imgColor = lst_news->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
	AddNews("Testing News", "by cnik", "lalalal lala bah\nkamamamamamba", "10/08/2008");
}

CNews::~CNews()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CNews::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CNews::MouseLeave(CControl * control)
{
}
void CNews::MouseDown(CControl * control)
{
}
void CNews::MouseUp(CControl * control)
{
}
void CNews::MouseMove(CControl * control)
{
}
void CNews::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_refresh)
	{
		//refresh news
	}
}
void CNews::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}

void CNews::AddNews(const CString& title, const CString& author, const CString& content, const CString& date)
{
	CControl* newsRow = new CControl(lst_news, CVector2i(10, 10), CVector2i(676,200), "", this, "LABEL");

	CControl* Title = new CControl(newsRow, CVector2i(0, 5), CVector2i(350,35), title, this, "LABEL");
	Title->textSize = 35;
	Title->textAlign = CONTROL_TEXTALIGN_TOPCENTER;
	CControl* Content = new CControl(newsRow, CVector2i(0, 40), CVector2i(350,80), content, this, "LABEL");//, Title);
	CControl* Author = new CControl(newsRow, CVector2i(0, 135), CVector2i(350,15), author, this, "LABEL");//, Content);
	Author->textSize = 20;
	CControl* Date = new CControl(newsRow, CVector2i(0, 145), CVector2i(350,20), date, this, "LABEL");//, Content);
	Date->textSize = 20;
}

#endif

