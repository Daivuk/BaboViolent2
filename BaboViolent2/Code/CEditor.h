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
#ifndef CEDITOR_H_INCLUDED
#define CEDITOR_H_INCLUDED


#include "CPanel.h"


class CEditor : public CPanel
{
public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	CControl * btn_edit;

	//--- Some controls (Login and create acount)
	CControl * txt_authorName;
	CControl * txt_mapName;
	CControl * txt_width; CControl * btn_widthPreset[5];
	CControl * txt_height; CControl * btn_heightPreset[5];

public:
	CEditor(CControl * in_parent, CControl * in_alignTo);
	virtual ~CEditor();

	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
	void Validate(CControl * control);
};


#endif
#endif

