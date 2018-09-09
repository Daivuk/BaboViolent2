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


#include "CListener.h"
#include "CControl.h"
#include "Helper.h"
#include "CMenuManager.h"
#include "KeyManager.h"
#include "GameVar.h"




CControl::CControl()
{
	isCheckBox = false;
	customData = 0;
	backColor.set(.7f,.7f,.7f);
	foreColor.set(0,0,0);
	borderColor.set(0,0,0);
	font = 0; //--- Should be set
	frame3D = false;
	buttonPush = false;
	listener = 0;
	parent = 0;
	textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	textSize = 25;
	mouseOver = false;
	isHoverable = false;
	textShadow = false;
	anim = 0;
	style = "NONE";
	editable = false;
	isFocusable = false;
	frame2D = false;
	password = false;
	noFill = false;
	texture = 0;
	textureCorner = CVector2f(1.0f, 1.0f);
	useTextureCorner = false;
	perfectFitTexture = false;
	partOfScroll = false;
	vscroll = 0;
	hscroll = 0;
	vscrollB1 = 0;
	vscrollB2 = 0;
	hscrollB1 = 0;
	hscrollB2 = 0;
	vscrollBar = 0;
	hscrollBar = 0;
	hoverFreq = .5f;
	selectableChildren = false;
	selectedChild = 0;
	hoveringChild = 0;
	value = 0;
	valueMin = 0;
	valueMax = 10;
	spinU = 0;
	spinD = 0;
	check = false;
	superPanel = false;
	slider = false;
	noFrame = false;
	selectedIndex = 0;
	visible = true;
	enable = true;
}

CControl::CControl(CControl * in_parent, 
				   CVector2i in_pos, 
				   CVector2i in_size, 
				   CString in_text, 
				   CListener * in_listener, 
				   CString in_style, 
				   CControl * snapTo, 
				   int snapFlag,
				   int spacer, 
				   bool in_partOfScroll)
{
	anim = 0;
	isCheckBox = false;
	customData = 0;
	parent = in_parent;
	backColor.set(.7f,.7f,.7f);
	foreColor.set(0,0,0);
	borderColor.set(0,0,0);
	font = 0; //--- Should be set
	frame3D = false;
	buttonPush = false;
	listener = 0;
	textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	textSize = 25;
	mouseOver = false;
	isHoverable = false;
	textShadow = false;
	style = in_style;
	editable = false;
	isFocusable = false;
	frame2D = false;
	password = false;
	noFill = false;
	noFrame = false;
	texture = 0;
	textureCorner = CVector2f(1.0f, 1.0f);
	useTextureCorner = false;
	perfectFitTexture = false;
	partOfScroll = in_partOfScroll;
	vscroll = 0;
	hscroll = 0;
	vscrollB1 = 0;
	vscrollB2 = 0;
	hscrollB1 = 0;
	hscrollB2 = 0;
	vscrollBar = 0;
	hscrollBar = 0;
	hoverFreq = .5f;
	selectableChildren = false;
	selectedChild = 0;
	hoveringChild = 0;
	value = 0;
	valueMin = 0;
	valueMax = 10;
	spinU = 0;
	spinD = 0;
	superPanel = false;
	check = false;
	slider = false;
	selectedIndex = 0;
	visible = true;
	enable = true;

	localPos = in_pos;
	size = in_size;
	text = in_text;

	if (parent)
	{
		//pos = localPos + parent->pos;
		backColor = parent->backColor;
		foreColor = parent->foreColor;
		borderColor = parent->borderColor;
		font = parent->font;
		listener = parent->listener;
		textAlign = parent->textAlign;
		parent->addChild(this);
		textShadow = parent->textShadow;
	}

	imgColor = backColor;

	if (in_listener) listener = in_listener;

	if (in_style == "SUPER_PANEL")
	{
		superPanel = true;
	}
	if (in_style == "BUTTON")
	{
		buttonPush = true;
		frame3D = true;
		isHoverable = true;
	}
	if (in_style == "EDIT")
	{
		editable = true;
		buttonPush = false;
		frame3D = false;
		frame2D = true;
		isHoverable = true;
		isFocusable = true;
		backColor.set(0,0,0);
		borderColor.set(1,1,1);
		textAlign = CONTROL_TEXTALIGN_MIDDLELEFT;
	}
	if (in_style == "KEY")
	{
		buttonPush = true;
		frame3D = true;
		isHoverable = true;
		editable = false;
		isHoverable = true;
		isFocusable = true;
	}
	if (in_style == "FRAME")
	{
		frame3D = true;
	}
	if (in_style == "LABEL")
	{
		textAlign = CONTROL_TEXTALIGN_MIDDLELEFT;
		noFill = true;
	}
	if (in_style == "LISTBOX")
	{
		selectableChildren = true;
		selectedChild = 0;
		frame2D = true;
		buttonPush = false;
		frame3D = false;
	}
	if (in_style == "SPIN")
	{
		editable = true;
		buttonPush = false;
		frame3D = false;
		frame2D = true;
		isHoverable = true;
		isFocusable = true;
		textAlign = CONTROL_TEXTALIGN_MIDDLELEFT;
	//	backColor.set(.25f,0,.5f);
	//	borderColor.set(1,1,1);
	}
	if (in_style == "CHECK")
	{
		isCheckBox = true;
		buttonPush = true;
		frame3D = true;
		isHoverable = true;
	//	backColor.set(0,0,0);
	//	borderColor.set(1,1,1);
	}
	if (in_style == "SLIDER")
	{
		slider = true;
	}

	if (snapTo)
	{
		switch (snapFlag)
		{
		case CONTROL_SNAP_TOP:
			localPos = snapTo->localPos;
			localPos[1] -= size[1] - spacer;
			break;
		case CONTROL_SNAP_RIGHT:
			localPos = snapTo->localPos;
			localPos[0] += snapTo->size[0] + spacer;
			break;
		case CONTROL_SNAP_BOTTOM:
			localPos = snapTo->localPos;
			localPos[1] += snapTo->size[1] + spacer;
			break;
		case CONTROL_SNAP_LEFT:
			localPos = snapTo->localPos;
			localPos[0] -= size[0] - spacer;
			break;
		}
	}

	if (in_style == "SEPARATOR")
	{
		if (parent)
		{
			localPos.set(20, localPos[1]);
			size.set(parent->size[0] - 40, 20);
			frame3D = false;
			frame2D = true;
		}
	}

	if (in_style == "SEPARATOR_THIN")
	{
		if (parent)
		{
			backColor * 0.3f;
			localPos.set(20, localPos[1]);
			size.set(parent->size[0] - 40, 5);
			frame3D = false;
			frame2D = true;
		}
	}

	lastPos = localPos;
	lastSize = size;

	//--- Now lets create his scroll bars
	if (!partOfScroll)
	{
		vscroll = new CControl(this, CVector2i(size[0] - 20, 0), CVector2i(20,size[1]-20),"",this,"",0,1,5,true);
		vscroll->backColor = backColor * 0.6f;
		hscroll = new CControl(this, CVector2i(0, size[1] - 20), CVector2i(size[0]-20,20),"",this,"",0,1,5,true);
		hscroll->backColor = backColor * 0.6f;

		vscrollB1 = new CControl(vscroll, CVector2i(0,0), CVector2i(20,20),"",this,"BUTTON",0,1,5,true);
		vscrollB1->backColor = backColor * 1.4f;
		vscrollB2 = new CControl(vscroll, CVector2i(0,vscroll->size[1]-20), CVector2i(20,20),"",this,"BUTTON",0,1,5,true);
		vscrollB2->backColor = backColor * 1.4f;

		hscrollB1 = new CControl(hscroll, CVector2i(0,0), CVector2i(20,20),"",this,"BUTTON",0,1,5,true);
		hscrollB1->backColor = backColor * 1.4f;
		hscrollB2 = new CControl(hscroll, CVector2i(hscroll->size[0]-20,0), CVector2i(20,20),"",this,"BUTTON",0,1,5,true);
		hscrollB2->backColor = backColor * 1.4f;

		vscrollBar = new CControl(vscroll, CVector2i(0,20), CVector2i(20,vscroll->size[1]-40),"",this,"BUTTON",0,1,5,true);
		vscrollBar->backColor = backColor * 1.4f;
		hscrollBar = new CControl(hscroll, CVector2i(20,0), CVector2i(hscroll->size[0]-40,20),"",this,"BUTTON",0,1,5,true);
		hscrollBar->backColor = backColor * 1.4f;

		vscroll->visible = false;
		hscroll->visible = false;

		vscrollB1->texture = dktCreateTextureFromFile("main/textures/ArrowU.tga", DKT_FILTER_LINEAR);
		vscrollB1->perfectFitTexture = true;
		vscrollB1->imgColor.set(0,0,0);

		vscrollB2->texture = dktCreateTextureFromFile("main/textures/ArrowD.tga", DKT_FILTER_LINEAR);
		vscrollB2->perfectFitTexture = true;
		vscrollB2->imgColor.set(0,0,0);

		hscrollB1->texture = dktCreateTextureFromFile("main/textures/ArrowL.tga", DKT_FILTER_LINEAR);
		hscrollB1->perfectFitTexture = true;
		hscrollB1->imgColor.set(0,0,0);

		hscrollB2->texture = dktCreateTextureFromFile("main/textures/ArrowR.tga", DKT_FILTER_LINEAR);
		hscrollB2->perfectFitTexture = true;
		hscrollB2->imgColor.set(0,0,0);
	}

	//--- Create now his spin buttons
	if (style == "SPIN")
	{
		spinU = new CControl(this, CVector2i(size[0] - 20, 0), CVector2i(20,size[1]/2),"",this,"BUTTON",0,1,5,false);
		spinU->backColor = backColor * 1.4f;
		spinU->texture = dktCreateTextureFromFile("main/textures/ArrowU.tga", DKT_FILTER_LINEAR);
		spinU->perfectFitTexture = true;
		spinU->imgColor.set(0,0,0);
		spinU->visible = true;

		spinD = new CControl(this, CVector2i(size[0] - 20, size[1]/2), CVector2i(20,size[1]/2),"",this,"BUTTON",0,1,5,false);
		spinD->backColor = backColor * 1.4f;
		spinD->texture = dktCreateTextureFromFile("main/textures/ArrowD.tga", DKT_FILTER_LINEAR);
		spinD->perfectFitTexture = true;
		spinD->imgColor.set(0,0,0);
		spinD->visible = true;
	}

//	render();
}


CControl::~CControl()
{
	m_radios.clear();
	if (customData) delete customData;
	loseFocus();
	int i=0;

	//--- Delete himself in the parent
	if (parent)
	{
		for (i=0; i<(int)parent->children.size(); ++i)
		{
			if (parent->children[i] == this)
			{
				parent->children.erase(parent->children.begin() + i);
				break;
			}
		}
	}

	while (children.size() > 0)
	{
		delete children[0];
	}

	if (menuManager.hoveringControl == this) menuManager.hoveringControl = 0;
	if (menuManager.activeControl == this) menuManager.activeControl = 0;
	if (menuManager.focusControl == this) menuManager.focusControl = 0;
}



//
//--- Update
//
void CControl::update(float delay)
{
	if (superPanel)
	{
		anim += delay * 30;
		while (anim >= 360) anim -= 360;
	}

  int i;
	if (parent)
	{
	//	pos = parent->pos + localPos - parent->scrollPos;
	//	localPos = pos - parent->pos;
	//	if (partOfScroll) pos = parent->pos + localPos;
		pos = localPos + parent->pos - parent->scrollPos;
		if (partOfScroll) pos = localPos + parent->pos;
	}
	else
	{
		pos = localPos;
	//	pos = pos;
	}

	if (!visible) return;

	//--- Childs dabords!
	for (i=0;i<(int)children.size();++i)
	{
	//	if (!children[i]->enable) continue;
		if (!children[i]->visible) continue;
		children[i]->update(delay);
	}

//	if (!enable) return;

	//--- Nous apres
	localMousePos = menuManager.mousePos - pos;

	//--- We clamp with the parent!!! Important while scrolling
	if (!menuManager.activeControl || menuManager.activeControl == this)
	{
		bool canBeOver = true;
		if (parent)
		{
			if (menuManager.mousePos[0] < parent->pos[0] ||
				menuManager.mousePos[1] < parent->pos[1] ||
				menuManager.mousePos[0] > parent->pos[0] + parent->size[0] ||
				menuManager.mousePos[1] > parent->pos[1] + parent->size[1])
			{
				canBeOver = false;
			}
		}
		if (localMousePos[0] >= 0 &&
			localMousePos[0] < size[0] &&
			localMousePos[1] >= 0 &&
			localMousePos[1] < size[1] && canBeOver)
		{
			if (!partOfScroll && enable)
			{
				if (dkiGetMouseWheelVel() < 0)
				{
					if (vscrollB2) Hold(vscrollB2);
				}
				else if (dkiGetMouseWheelVel() > 0)
				{
					if (vscrollB1) Hold(vscrollB1);
				}
			}
			if (menuManager.hoveringControl == 0) menuManager.hoveringControl = this;
			if (!mouseOver && enable)
			{
				if (listener) listener->MouseEnter(this);
				if (parent)
				{
					if (parent->selectableChildren && !partOfScroll)
					{
						if (parent->listener) parent->listener->MouseEnter(parent);
					}
				}
			}
			mouseOver = true;
			if (parent && enable)
			{
				if (parent->selectableChildren && !partOfScroll)
				{
					parent->hoveringChild = this;
				}
			}

			//--- Do we mouse down?
			if ((gameVar.cl_enableXBox360Controller &&
				dkiGetState(DKI_JOY_BUTTON1) == DKI_DOWN) ||

				(!gameVar.cl_enableXBox360Controller && 
				(dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN && enable && !haveFocus())))
			{
				if (parent)
				{
					if (parent->selectableChildren && !partOfScroll)
					{
						parent->selectedChild = this;
						parent->selectedIndex = 0;
						for (i=0;i<(int)parent->children.size();++i)
						{
							if (parent->children[i] == this)
							{
								parent->selectedIndex = i - 2;
								break;
							}
						}
						if (parent->listener) parent->listener->Click(parent);
						if (listener) listener->DbClick(this);
					}
					else if (parent->parent)
					{
						if (parent->parent->selectableChildren && !partOfScroll)
						{
							parent->parent->selectedChild = parent;
							parent->parent->selectedIndex = 0;
							for (i=0;i<(int)parent->parent->children.size();++i)
							{
								if (parent->parent->children[i] == parent)
								{
									parent->parent->selectedIndex = i - 2;
									break;
								}
							}
							if (parent->parent->listener) parent->parent->listener->Click(parent->parent);
							if (parent->listener) parent->listener->DbClick(parent);
						}
					}
				}
				hoverFreq = .4f;
				menuManager.activeControl = this;
				mouseDownPos = menuManager.mousePos;
				lastMousePosD = mouseDownPos;
				downPos = localPos;
				if (listener) listener->MouseDown(this);
				if (listener) listener->Hold(this);

				//--- If we are a EDIT box, we get into edit mode
				if (editable)
				{
					menuManager.activeControl = 0;
					set("%s", text.s);
					giveFocus();
				}
				return;
			}
		}
		else
		{
			hoveringChild = 0;
			if (mouseOver)
			{
				if (listener) listener->MouseLeave(this);
			}
			mouseOver = false;
		}
	}

	if (menuManager.activeControl == this && enable)
	{
		if (lastMousePosD != menuManager.mousePos)
		{
			lastMousePosD = menuManager.mousePos;
			if (listener) listener->MouseMove(this);

			if (slider)
			{
				int inSliderPos = menuManager.mousePos[0] - pos[0] - 5;
				float percent = (float)inSliderPos / (float)(size[0] - 10);
				if (percent < 0) percent = 0;
				if (percent > 1) percent = 1;
				value = valueMin + (int)((float)(valueMax - valueMin) * percent);
				if (listener) listener->Validate(this);
			}
		}

		if ((gameVar.cl_enableXBox360Controller &&
				dkiGetState(DKI_JOY_BUTTON1) == DKI_UP) ||

			(!gameVar.cl_enableXBox360Controller && 
				dkiGetState(DKI_MOUSE_BUTTON1) == DKI_UP))
		{
			hoverFreq = .1f;
			menuManager.activeControl = 0;
			if (mouseOver)
			{
				if (style == "CHECK")
				{
					if (m_radios.size() > 0)
					{
						check = true;
						int i;
						for (i=0;i<(int)m_radios.size();++i)
						{
							m_radios[i]->check = false;
						}
					}
					else
					{
						check = !check;
					}
				}
				if (listener) listener->MouseUp(this);
				if (listener) listener->Click(this);
				if (style == "KEY")
				{
					menuManager.activeControl = 0;
					set("%s", text.s);
					giveFocus();
				}
			}
			else
			{
				if (listener) listener->MouseUp(this);
			}
		}

		if (mouseOver)
		{
			hoverFreq -= delay;
			if (hoverFreq <= 0)
			{
				hoverFreq = .1f;
				if (listener) listener->Hold(this);
			}
		}
	}

	if (lastPos != localPos)
	{
		lastPos = localPos;
		if (listener) listener->Move(this);
		if (parent) parent->updateScrollRect();
	}

	if (lastSize != size)
	{
		lastSize = size;
		if (listener) listener->Resize(this);
		if (parent) parent->updateScrollRect();
	}

	//--- Si on a le focus
	if (haveFocus())
	{
		if (style == "KEY")
		{
			int key = 0;
			if ((key = dkiGetFirstDown()) != DKI_NOKEY)
			{
				text = keyManager.getKeyName(key);
				selectedIndex = key;
				menuManager.activeControl = 0;
				loseFocus();
				if (listener && enable) listener->Validate(this);	
			}
		}
	}

	//--- We are writting
	if (haveFocus() && 
		
		((dkiGetState(DKI_JOY_BUTTON1) == DKI_DOWN && gameVar.cl_enableXBox360Controller) ||
		(dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN && !gameVar.cl_enableXBox360Controller))
		
		|| isActivated())
	{
		loseFocus();
		text.set("%s", s);
		if (style == "SPIN")
		{
			value = text.toInt();
			if (value < valueMin) value = valueMin;
			if (value > valueMax) value = valueMax;
			text = CString() + value;
		}
		if (listener && enable) listener->Validate(this);	
	}
}


void CControl::updateScrollRect()
{
	if (!partOfScroll)
	{
		scrollSize.set(0,0);
		scrollOrigin.set(0,0);
		CVector2i rect[2];
		for (int i=0;i<(int)children.size();++i)
		{
			if (!children[i]->partOfScroll)
			{
				if (rect[0][0] > children[i]->localPos[0]) rect[0][0] = children[i]->localPos[0];
				if (rect[0][1] > children[i]->localPos[1]) rect[0][1] = children[i]->localPos[1];
				if (rect[1][0] < children[i]->localPos[0] + children[i]->size[0]) rect[1][0] = children[i]->localPos[0] + children[i]->size[0];
				if (rect[1][1] < children[i]->localPos[1] + children[i]->size[1]) rect[1][1] = children[i]->localPos[1] + children[i]->size[1];
			}
		}

		scrollOrigin = rect[0];
	//	rect[1] -= size;
		scrollSize = (rect[1] - rect[0]) - size;

		if (scrollOrigin[0] > 0) scrollOrigin[0] = 0;
		if (scrollOrigin[1] > 0) scrollOrigin[1] = 0;

		if (scrollPos[0] > scrollOrigin[0] + scrollSize[0]) scrollPos[0] = scrollOrigin[0] + scrollSize[0];
		if (scrollPos[1] > scrollOrigin[1] + scrollSize[1]) scrollPos[1] = scrollOrigin[1] + scrollSize[1];
		if (scrollPos[0] < scrollOrigin[0]) scrollPos[0] = scrollOrigin[0];
		if (scrollPos[1] < scrollOrigin[1]) scrollPos[1] = scrollOrigin[1];

		if (scrollSize[0] > 0)
		{
			if (hscroll && hscrollBar) 
			{
				scrollSize[0] += 40;
				hscroll->visible = true;
				hscrollBar->size[0] = (int)(((float)size[0] / (float)(rect[1][0] - rect[0][0])) * (float)(hscroll->size[0] - 40));

				if (hscrollBar->size[0] < 20) hscrollBar->size[0] = 20;
			}
		}
		else
		{
			if (scrollSize[0] < 0) scrollSize[0] = 0;
			if (hscroll) hscroll->visible = false;
		}

		if (scrollSize[1] > 0) 
		{
			if (vscroll && vscrollBar) 
			{
				scrollSize[1] += 40;
				vscroll->visible = true;
				vscrollBar->size[1] = (int)(((float)size[1] / (float)(rect[1][1] - rect[0][1])) * (float)(vscroll->size[1] - 40));

				if (vscrollBar->size[1] < 20) vscrollBar->size[1] = 20;
			}
		}
		else
		{
			if (scrollSize[1] < 0) scrollSize[1] = 0;
			if (vscroll) vscroll->visible = false;
		}
	}
}


extern bool enableShadow;
#include "Console.h"
//
//--- Render
//
void CControl::render()
{
	if (!visible) return;
//	console->add(CString("render control at : %i, %i, %i", this, pos[0], pos[1]));

	//--- Ouff, méchante job de clamping à faire icite
	Rect[0] = pos[0];
	Rect[1] = pos[1];
	Rect[2] = size[0];
	Rect[3] = size[1];

	if (parent)
	{
		if (Rect[0] < parent->Rect[0])
		{
			Rect[0] = parent->Rect[0];
		}
		if (Rect[1] < parent->Rect[1])
		{
			Rect[1] = parent->Rect[1];
		}
		if (Rect[0] + Rect[2] > parent->Rect[0] + parent->Rect[2])
		{
			Rect[2] = parent->Rect[0] + parent->Rect[2] - Rect[0];
		}
		if (Rect[1] + Rect[3] > parent->Rect[1] + parent->Rect[3])
		{
			Rect[3] = parent->Rect[1] + parent->Rect[3] - Rect[1];
		}
	}

	if (Rect[2] <= 0) return;
	if (Rect[3] <= 0) return;


	dkfBindFont(font);

	//--- Render it! Easy stuff
#ifndef _DX_
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_SCISSOR_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
	glEnable(GL_SCISSOR_TEST);
#endif


	CVector2i	res = dkwGetResolution();
	float		offset = 0;

	if(gameVar.r_widescreen > 1)
	{
		offset = (float)res[0];
		res[0] = (int)(res[1]*1.333f);
		offset = (offset - res[0])/2;	
	}

#ifndef _DX_
	glScissor(
		(GLint)((((float)Rect[0]/800.0f) * (float)res[0]) + offset), 
		res[1] - (int)(((float)(Rect[1])/600.0f) * (float)res[1]) - (int)(((float)Rect[3]/600.0f) * (float)res[1] + 1),
		(int)(((float)Rect[2]/800.0f) * (float)res[0]), 
		(int)(((float)Rect[3]/600.0f) * (float)res[1] + 1));
#endif

	CVector2i offsetText;

	CVector3f grayScale = backColor;
	if (!enable) grayScale.grayScale();
	if (!noFill)
	{
		if (frame3D)
		{
			if (mouseOver && menuManager.activeControl == this && isHoverable && enable || haveFocus())
			{
			/*	glColor3fv((backColor*1.3f).s);
				renderMenuQuad(pos[0]+2, pos[1]+2, size[0]-2, size[1]-2);
				glColor3fv((backColor*.7f).s);
				renderMenuQuad(pos[0], pos[1], size[0]-2, size[1]-2);*/
				offsetText.set(1,1);
#ifndef _DX_
				if ((menuManager.hoveringControl == this || menuManager.activeControl == this) && isHoverable) glColor3fv((grayScale * .7f).s);
				else glColor3fv(grayScale.s);
#endif
			}
			else
			{
			/*	glColor3fv((grayScale*.7f).s);
				renderMenuQuad(pos[0]+2, pos[1]+2, size[0]-2, size[1]-2);
				glColor3fv((grayScale*1.3f).s);
				renderMenuQuad(pos[0], pos[1], size[0]-2, size[1]-2);*/
#ifndef _DX_
				if ((menuManager.hoveringControl == this) && isHoverable) glColor3fv((grayScale * 1.3f).s);
				else glColor3fv(grayScale.s);
#endif
			}
			renderMenuQuad(pos[0]+2, pos[1]+2, size[0]-4, size[1]-4);
		}
		else if (frame2D)
		{
		/*	glColor3fv(borderColor.s);
			renderMenuQuad(pos[0], pos[1], size[0], size[1]);*/

#ifndef _DX_
			if ((menuManager.hoveringControl == this || menuManager.activeControl == this) && isHoverable) glColor3fv((grayScale * 1.2f).s);
			else glColor3fv(grayScale.s);
#endif
			renderMenuQuad(pos[0]+2, pos[1]+2, size[0]-4, size[1]-4);
		}
		else if (noFrame)
		{
		}
		else
		{
#ifndef _DX_
			if ((menuManager.hoveringControl == this || menuManager.activeControl == this) && isHoverable) glColor3fv((grayScale * 1.2f).s);
			else glColor3fv(grayScale.s);
#endif
			renderMenuQuad(pos[0], pos[1], size[0], size[1]);
		}
		if (isCheckBox)
		{
#ifndef _DX_
			glColor4f(0, 0, 0, .60f);
#endif
			renderMenuQuad(pos[0]+4, pos[1]+4, size[0]-8, size[1]-8);
		}
	}

	if (superPanel)
	{
		float x=(float)pos[0];
		float y=(float)pos[1];
		float w=(float)size[0];
		float h=(float)size[1];
		float ratio = h / w;
#ifndef _DX_

		glColor4f(1, 1, 1, .15f);
		
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2f(0,ratio - anim / 360.0f);
				glVertex2f(x,y);
				glTexCoord2f(0,-anim / 360.0f);
				glVertex2f(x,y+h);
				glTexCoord2f(1,-anim / 360.0f);
				glVertex2f(x+w,y+h);
				glTexCoord2f(1,ratio - anim / 360.0f);
				glVertex2f(x+w,y);
			glEnd();
		glPopAttrib();
		
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2f(0,ratio / 2 - anim / 180.0f);
				glVertex2f(x,y);
				glTexCoord2f(0,-anim / 180.0f);
				glVertex2f(x,y+h);
				glTexCoord2f(1,-anim / 180.0f);
				glVertex2f(x+w,y+h);
				glTexCoord2f(1,ratio / 2 - anim / 180.0f);
				glVertex2f(x+w,y);
			glEnd();
		glPopAttrib();
#endif
	}
	else if (texture)
	{
		CVector4f grayScaleImg(imgColor[0],imgColor[1],imgColor[2],1);
		if (!enable)
		{
			grayScaleImg[3] = .15f;
		}
		if(!useTextureCorner)
		{
			if (perfectFitTexture)
			{
#ifndef _DX_
				glColor4fv(grayScaleImg.s);
#endif
				renderTexturedQuad(pos[0], pos[1], size[0], size[1], texture);
			}
			else
			{
#ifndef _DX_
				glColor4fv(grayScaleImg.s);
#endif
				renderTexturedQuadSmooth(pos[0], pos[1], size[0], size[1], texture);
			}
		}
		else
		{
#ifndef _DX_
			glColor4fv(grayScaleImg.s);
			glPushAttrib(GL_ENABLE_BIT);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture);
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, textureCorner[1]);
					glVertex2i(pos[0],pos[1]);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2i(pos[0],pos[1]+size[1]);
					glTexCoord2f(textureCorner[0], 0.0f);
					glVertex2i(pos[0]+size[0],pos[1]+size[1]);
					glTexCoord2f(textureCorner[0], textureCorner[1]);
					glVertex2i(pos[0]+size[0],pos[1]);
				glEnd();
			glPopAttrib();
#endif
		}
	}

	if (listener) listener->Paint(this);

	if (check)
	{
#ifndef _DX_
		glColor3fv(borderColor.s);
#endif
	//	glColor4f(1, 1, 1, .60f);
		renderMenuQuad(pos[0]+6, pos[1]+6, size[0]-12, size[1]-12);
	}

	//--- Slider
	if (slider)
	{
		int sliderPos = (int)(((float)(value - valueMin) / (float)(valueMax - valueMin)) * (float)(size[0]-10));
#ifndef _DX_
		glColor3fv(borderColor.s);
#endif
		renderMenuQuad(pos[0] + sliderPos-5 + 5, pos[1], 10, size[1]);
		text = CString() + value;
	}


	//--- Show the text in it (Clamp)
	if (!text.isNull() || !isNull() || haveFocus())
	{
#ifndef _DX_
		if (!enable) glColor3f(1,1,1);//.5f, .5f, .5f);
		else glColor3fv(foreColor.s);
		glPushAttrib(GL_SCISSOR_BIT | GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
#endif
			if (!textShadow || !enable) enableShadow = false;
			CString _text = text;
			if (haveFocus())
			{
				_text = s;
				if (password)
				{
					_text = "";
					int _len = len();
					for (int i=0;i<_len;++i)
					{
						_text += "*";
					}
				}
			}
			else
			{
				if (password)
				{
					_text = "";
					int _len = text.len();
					for (int i=0;i<_len;++i)
					{
						_text += "*";
					}
				}
			}
			switch (textAlign)
			{
			case CONTROL_TEXTALIGN_TOPLEFT:
				printLeftText((float)pos[0] + 3 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_TOPCENTER:
				printCenterText((float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_TOPRIGHT:
				printRightText((float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_MIDDLELEFT:
				printLeftText((float)pos[0] + 3 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_MIDDLECENTER:
				printCenterText((float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_MIDDLERIGHT:
				printRightText((float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_BOTTOMLEFT:
				printLeftText((float)pos[0] + 3 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_BOTTOMCENTER:
				printCenterText((float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
				break;
			case CONTROL_TEXTALIGN_BOTTOMRIGHT:
				printRightText((float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
				break;
			}

			if (haveFocus() && m_cursorAnim < .5f)
			{
				CString tmp("%s", s);
				tmp.resize(m_cursorPos);
				float posX = dkfGetStringWidth(textSize, tmp.s);

				_text = "_";

				switch (textAlign)
				{
				case CONTROL_TEXTALIGN_TOPLEFT:
					printLeftText(posX + (float)pos[0] + 3 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_TOPCENTER:
					printCenterText(posX + (float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_TOPRIGHT:
					printRightText(posX + (float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] + 3 + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_MIDDLELEFT:
					printLeftText(posX + (float)pos[0] + 3 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_MIDDLECENTER:
					printCenterText(posX + (float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_MIDDLERIGHT:
					printRightText(posX + (float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] + (float)size[1] / 2 - ((float)textSize / 2) + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_BOTTOMLEFT:
					printLeftText(posX + (float)pos[0] + 3 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_BOTTOMCENTER:
					printCenterText(posX + (float)pos[0] + (float)size[0] / 2 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
					break;
				case CONTROL_TEXTALIGN_BOTTOMRIGHT:
					printRightText(posX + (float)pos[0] + (float)size[0] - 3 + offsetText[0], (float)pos[1] - textSize - 3 + (float)size[1] + offsetText[1], textSize, _text);
					break;
				}
			}

			enableShadow = true;

#ifndef _DX_
		glPopAttrib();
#endif
	}

	//--- Childs
	for (int i=(int)children.size() - 1;i>=0;--i)
	{
		if (!children[i]->visible) continue;
		children[i]->render();
	}

	if (parent)
	{
		if (parent->selectedChild == this)
		{
#ifndef _DX_
			glPushAttrib(GL_ENABLE_BIT);
				glEnable(GL_COLOR_LOGIC_OP);
				glLogicOp(GL_INVERT);
				renderMenuQuad(pos[0], pos[1], size[0], size[1]);
			glPopAttrib();
#endif
		}
		if (parent->hoveringChild == this)
		{
#ifndef _DX_
			glPushAttrib(GL_ENABLE_BIT);
				glColor4f(1, 1, 0, .3f);
				renderMenuQuad(pos[0], pos[1], size[0], size[1]);
			glPopAttrib();
#endif
		}
	}

#ifndef _DX_
	glPopAttrib();
#endif
}



void CControl::updateScrollBarPos()
{
//	vscrollBar->size[1] = (int)(((float)size[1] / (float)(rect[1][1] - rect[0][1])) * (float)(vscroll->size[1] - 40));
//	if (vscrollBar->size[1] < 20) vscrollBar->size[1] = 20;

	//--- Quel % on est
	float percent = (float)(scrollPos[1] - scrollOrigin[1]) / (float)scrollSize[1];
	vscrollBar->localPos[1] = (int)(((float)(vscroll->size[1] - 40) - (float)vscrollBar->size[1]) * percent) + 20;

	//--- Quel % on est
	percent = (float)(scrollPos[0] - scrollOrigin[0]) / (float)scrollSize[0];
	hscrollBar->localPos[0] = (int)(((float)(hscroll->size[0] - 40) - (float)hscrollBar->size[0]) * percent) + 20;
}




//
// Pour le scrolling
//
void CControl::MouseDown(CControl * control)
{
}
void CControl::MouseUp(CControl * control)
{
}
void CControl::MouseMove(CControl * control)
{
	if (control == vscrollBar)
	{
		//--- Get the movement
		int diff = menuManager.mousePos[1] - vscrollBar->mouseDownPos[1];
		vscrollBar->localPos[1] = vscrollBar->downPos[1] + diff;

		//--- Clamp up and down
		if (vscrollBar->localPos[1] < 20) vscrollBar->localPos[1] = 20;
		if (vscrollBar->localPos[1] + vscrollBar->size[1] > vscroll->size[1] - 20)
			vscrollBar->localPos[1] = vscroll->size[1] - 20 - vscrollBar->size[1];

		//--- Set the corresponding heigh percentage
		float percent = (float)(vscrollBar->pos[1] - vscroll->pos[1] - 20) / ((float)(vscroll->size[1] - 40) - vscrollBar->size[1]);
		scrollPos[1] = (int)((float)scrollSize[1] * percent) + scrollOrigin[1];
		if (scrollPos[1] < scrollOrigin[1]) scrollPos[1] = scrollOrigin[1];
		if (scrollPos[1] > scrollOrigin[1] + scrollSize[1]) scrollPos[1] = scrollOrigin[1] + scrollSize[1];
	}
	if (control == hscrollBar)
	{
		//--- Get the movement
		int diff = menuManager.mousePos[0] - hscrollBar->mouseDownPos[0];
		hscrollBar->localPos[0] = hscrollBar->downPos[0] + diff;

		//--- Clamp up and down
		if (hscrollBar->localPos[0] < 20) hscrollBar->localPos[0] = 20;
		if (hscrollBar->localPos[0] + hscrollBar->size[0] > hscroll->size[0] - 20)
			hscrollBar->localPos[0] = hscroll->size[0] - 20 - hscrollBar->size[0];

		//--- Set the corresponding heigh percentage
		float percent = (float)(hscrollBar->pos[0] - hscroll->pos[0] - 20) / ((float)(hscroll->size[0] - 40) - hscrollBar->size[0]);
		scrollPos[0] = (int)((float)scrollSize[0] * percent) + scrollOrigin[0];
		if (scrollPos[0] < scrollOrigin[0]) scrollPos[0] = scrollOrigin[0];
		if (scrollPos[0] > scrollOrigin[0] + scrollSize[0]) scrollPos[0] = scrollOrigin[0] + scrollSize[0];
	}
}
void CControl::Click(CControl * control)
{
}
void CControl::DbClick(CControl * control)
{
}
void CControl::Hold(CControl * control)
{
	if (control == spinU)
	{
		value++;
		if (value > valueMax) value = valueMax;
		text = CString() + value;
		if (listener) listener->Validate(this);
	}
	if (control == spinD)
	{
		value--;
		if (value < valueMin) value = valueMin;
		text = CString() + value;
		if (listener) listener->Validate(this);
	}
	if (control == hscrollB1)
	{
		scrollPos[0] -= 50;
		if (scrollPos[0] < scrollOrigin[0]) scrollPos[0] = scrollOrigin[0];
		updateScrollBarPos();
	}
	if (control == hscrollB2)
	{
		scrollPos[0] += 50;
		if (scrollPos[0] > scrollOrigin[0] + scrollSize[0]) scrollPos[0] = scrollOrigin[0] + scrollSize[0];
		updateScrollBarPos();
	}
	if (control == hscroll)
	{
		//--- Are we bellow or over the scroll bar
		if (menuManager.mousePos[0] < hscrollBar->pos[0])
		{
			scrollPos[0] -= size[0] - 40;
			if (scrollPos[0] < scrollOrigin[0]) scrollPos[0] = scrollOrigin[0];
		}
		else
		{
			scrollPos[0] += size[0] - 40;
			if (scrollPos[0] > scrollOrigin[0] + scrollSize[0]) scrollPos[0] = scrollOrigin[0] + scrollSize[0];
		}
		updateScrollBarPos();
	}
	if (control == vscrollB1)
	{
		scrollPos[1] -= 50;
		if (scrollPos[1] < scrollOrigin[1]) scrollPos[1] = scrollOrigin[1];
		updateScrollBarPos();
	}
	if (control == vscrollB2)
	{
		scrollPos[1] += 50;
		if (scrollPos[1] > scrollOrigin[1] + scrollSize[1]) scrollPos[1] = scrollOrigin[1] + scrollSize[1];
		updateScrollBarPos();
	}
	if (control == vscroll)
	{
		//--- Are we bellow or over the scroll bar
		if (menuManager.mousePos[1] < vscrollBar->pos[1])
		{
			scrollPos[1] -= size[1] - 40;
			if (scrollPos[1] < scrollOrigin[1]) scrollPos[1] = scrollOrigin[1];
		}
		else
		{
			scrollPos[1] += size[1] - 40;
			if (scrollPos[1] > scrollOrigin[1] + scrollSize[1]) scrollPos[1] = scrollOrigin[1] + scrollSize[1];
		}
		updateScrollBarPos();
	}
}


#endif

