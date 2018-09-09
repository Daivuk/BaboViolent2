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

#ifndef CCONTROL_H_INCLUDED
#define CCONTROL_H_INCLUDED


#include "Zeven.h"
#include "Writting.h"
#include <vector>

#define CONTROL_TEXTALIGN_TOPLEFT 0
#define CONTROL_TEXTALIGN_TOPCENTER 1
#define CONTROL_TEXTALIGN_TOPRIGHT 2
#define CONTROL_TEXTALIGN_MIDDLELEFT 3
#define CONTROL_TEXTALIGN_MIDDLECENTER 4
#define CONTROL_TEXTALIGN_MIDDLERIGHT 5
#define CONTROL_TEXTALIGN_BOTTOMLEFT 6
#define CONTROL_TEXTALIGN_BOTTOMCENTER 7
#define CONTROL_TEXTALIGN_BOTTOMRIGHT 8

#define CONTROL_SNAP_TOP 0
#define CONTROL_SNAP_RIGHT 1
#define CONTROL_SNAP_BOTTOM 2
#define CONTROL_SNAP_LEFT 3


#include "CListener.h"


class CControl : public Writting, CListener
{
public:
	CVector2i pos;
	CVector2i localPos;
	CVector2i size;
	CVector2i lastPos;
	CVector2i lastSize;
//	CVector2i realPos;
	CVector2i downPos;

	CString text;

	float textSize;

	float anim;

	CVector3f backColor;
	CVector3f foreColor;
	CVector3f borderColor;
	CVector3f imgColor;

	bool frame3D;
	bool buttonPush;
	bool noFrame;

	CVector2i scrollPos;
	CVector2i scrollOrigin;
	CVector2i scrollSize; // Must be > size

	CListener * listener;

	CVector2i localMousePos;
	CVector2i mouseDownPos;
	CVector2i lastMousePosD;

	CControl * parent;

	std::vector<CControl*> children;

	bool enable;

	bool isHoverable;

	bool visible;

	unsigned int font;

	int textAlign;


	int selectedIndex;
	bool mouseOver;

	bool textShadow;

	CString style;

	bool editable;

	bool isFocusable;

	bool frame2D;

	bool password;

	bool superPanel;

	bool perfectFitTexture;

	float hoverFreq;

	bool check;

	bool isCheckBox;
	bool slider;

	bool selectableChildren;
	CControl * selectedChild;
	CControl * hoveringChild;

	bool noFill;
	//--- Texture (custom babo rendering!)
//	unsigned int tex_baboTex;

	unsigned int texture;
	CVector2f textureCorner;
	bool useTextureCorner;

	int Rect[4];

	bool partOfScroll;

	int value;
	int valueMin;
	int valueMax;

	CControl * vscroll;
	CControl * hscroll;
	CControl * vscrollB1;
	CControl * vscrollB2;
	CControl * hscrollB1;
	CControl * hscrollB2;
	CControl * vscrollBar;
	CControl * hscrollBar;

	CControl * spinU;
	CControl * spinD;

	CString toolTips;

	void* customData;

	std::vector<CControl *> m_radios;

public:
	CControl();
	CControl(
		CControl * in_parent, 
		CVector2i in_pos, 
		CVector2i in_size, 
		CString in_text, 
		CListener * in_listener, 
		CString in_style, 
		CControl * snapTo=0, 
		int snapFlag=CONTROL_SNAP_RIGHT, 
		int spacer=5,
		bool in_partOfScroll=false);

	virtual ~CControl();

	//--- Update
	void update(float delay);

	//--- Render
	void render();

	void updateScrollRect();
	void updateScrollBarPos();

	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
	void DbClick(CControl * control);
	void Hold(CControl * control);

	void registerRadio(CControl * radio)
	{
		check = true;
		int i;
		for (i=0;i<(int)m_radios.size();++i)
		{
			m_radios[i]->m_radios.push_back(radio);
			radio->m_radios.push_back(m_radios[i]);
		}
		m_radios.push_back(radio);
		radio->m_radios.push_back(this);
	}

	void addChild(CControl * child)
	{
		children.push_back(child);
		updateScrollRect();
	}

	void selectChild(int index)
	{
		selectedIndex = index;
		if(index >= 0)
		{
			selectedChild = children[index + 2];
		}
		else
			selectedChild = 0;
	}

	void clearList()
	{
		for (int i=0; i<(int)children.size(); ++i)
		{
			if (!children[i]->partOfScroll)
			{
				delete children[i];
			//	children.erase(children.begin() + i); // The child unregister itself
				--i;
			}
		}
	}
};


#endif

