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
#include "Dialog.h"
#include "CMenuManager.h"

IDialog::IDialog(unsigned int font, CString dialogTitle, CVector2i size, DialogButtons buttons)
	:	state(StateNothing)
	,	m_sfxClic(0)
	,	m_sfxOver(0)
	,	dialogRoot(0)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	dialogRoot = new CControl();
	dialogRoot->font = font;
	dialogRoot->size.set(800, 600);
	dialogRoot->backColor.set(.3f,.5f,.8f);
	dialogRoot->foreColor.set(1,1,1);
	dialogRoot->textShadow = true;
	dialogRoot->noFill = true;

	CVector2i pos = (dialogRoot->size - size) / 2;
	dialogFrameHeader = new CControl(dialogRoot,  pos - CVector2i(0, 35), CVector2i(size[0], 35), "", this, "FRAME");

	CControl * temp = 0;
	temp = new CControl(dialogFrameHeader, CVector2i(5, 5), CVector2i(size[0] - 10, 25), dialogTitle, this, "LABEL");
	temp->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;

	//dialogFrameHeader->backColor.set(0.25f, 0.25f, 0.25f);

	dialogFrame = new CControl(dialogRoot, pos, size, "", this, "FRAME", dialogFrameHeader, CONTROL_SNAP_BOTTOM, 0);

	dialogFrameButtons = new CControl(dialogRoot, CVector2i(pos[0], pos[1] + size[1]), CVector2i(size[0], 35), "", this, "FRAME", dialogFrame, CONTROL_SNAP_BOTTOM, 0);

	int margin = 5;
	int ctrlWidth = 0;
	if (buttons == ButtonsOK)
	{
		ctrlWidth	= GetControlWidth(1, margin, size[0], 100);
		btn_ok		= new CControl(dialogFrameButtons, CVector2i(size[0] - ctrlWidth - 2 * margin, 5), CVector2i(ctrlWidth, 25), "OK", this, "BUTTON");
		btn_yes		= 0;
		btn_no		= 0;
		btn_cancel	= 0;
	}
	else if (buttons == ButtonsOKCancel)
	{
		ctrlWidth	= GetControlWidth(2, margin, size[0], 100);
		btn_ok		= new CControl(dialogFrameButtons, CVector2i(size[0] - ctrlWidth * 2 - 3 * margin, 5),	CVector2i(ctrlWidth, 25), "OK", this, "BUTTON");
		btn_yes		= 0;
		btn_no		= 0;
		btn_cancel	= new CControl(dialogFrameButtons, CVector2i(5, 5),	CVector2i(ctrlWidth, 25), "Cancel", this, "BUTTON", btn_ok, CONTROL_SNAP_RIGHT);
	}
	else if (buttons == ButtonsYesNo)
	{
		ctrlWidth	= GetControlWidth(2, margin, size[0], 100);
		btn_ok		= 0;
		btn_yes		= new CControl(dialogFrameButtons, CVector2i(size[0] - ctrlWidth * 2 - 3 * margin, 5),	CVector2i(ctrlWidth, 25), "Yes", this, "BUTTON");
		btn_no		= new CControl(dialogFrameButtons, CVector2i(5, 5),	CVector2i(ctrlWidth, 25), "No", this, "BUTTON", btn_yes, CONTROL_SNAP_RIGHT);
		btn_cancel	= 0;
	}
	else// if (buttons == ButtonsYesNoCancel)
	{
		ctrlWidth	= GetControlWidth(3, margin, size[0], 100);
		btn_ok		= 0;
		btn_yes		= new CControl(dialogFrameButtons, CVector2i(size[0] - ctrlWidth * 3 - 4 * margin, 5),	CVector2i(ctrlWidth, 25), "Yes", this, "BUTTON");
		btn_no		= new CControl(dialogFrameButtons, CVector2i(5, 5),	CVector2i(ctrlWidth, 25), "No", this, "BUTTON", btn_yes, CONTROL_SNAP_RIGHT);
		btn_cancel	= new CControl(dialogFrameButtons, CVector2i(5, 5),	CVector2i(ctrlWidth, 25), "Cancel", this, "BUTTON", btn_no, CONTROL_SNAP_RIGHT);
	}

	dialogFrameButtons->backColor.set(0.25f, 0.25f, 0.25f);
	menuManager.showDialog(this);
}

IDialog::~IDialog()
{
	delete dialogRoot;
}

void IDialog::update(float delay)
{
	menuManager.update(delay, dialogRoot);
}

void IDialog::render()
{
	menuManager.render(dialogRoot);
}

void IDialog::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}

void IDialog::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (btn_ok != 0 && control == btn_ok)
	{
		state = StateOK;
		Finish();
		return;
	}
	if (btn_cancel != 0 && control == btn_cancel)
	{
		state = StateCancel;
		Finish();
		return;
	}
	if (btn_yes != 0 && control == btn_yes)
	{
		state = StateYes;
		Finish();
		return;
	}
	if (btn_no != 0 && control == btn_no)
	{
		state = StateNo;
		Finish();
		return;
	}
}

void IDialog::OnOk()
{
}

void IDialog::OnCancel()
{
}

void IDialog::OnYes()
{
}

void IDialog::OnNo()
{
}

void IDialog::Finish()
{
	if(state == StateOK)
	{
		OnOk();
		if (state == StateOK && !onOkEvent.empty())
			onOkEvent();
	}
	else if(state == StateCancel)
	{
		OnCancel();
		if (state == StateCancel && !onCancelEvent.empty())
			onCancelEvent();
	}
	else if(state == StateYes)
	{
		OnYes();
		if (state == StateYes && !onYesEvent.empty())
			onYesEvent();
	}
	else if(state == StateNo)
	{
		OnNo();
		if (state == StateNo && !onNoEvent.empty())
			onNoEvent();
	}
}

int IDialog::GetControlWidth(int numBtns, int margin, int width, int defCtrlWidth)
{
	int sumMargin = (numBtns + 1) * margin;

	if (width >= (numBtns * defCtrlWidth) + sumMargin)
		return defCtrlWidth;
	else
		return (int)((width - sumMargin) / (float)numBtns);
}

#endif //DEDICATED_SERVER