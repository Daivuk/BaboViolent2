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
#ifndef DIALOG_H
#define DIALOG_H

#include "CListener.h"
#include "fastdelegate.h"

enum DialogButtons
{
	ButtonsOK,
	ButtonsOKCancel,
	ButtonsYesNo,
	ButtonsYesNoCancel
};

enum DialogState
{
	StateOK,
	StateCancel,
	StateYes,
	StateNo,
	StateNothing
};

class IDialog : public CListener
{
public:
	DialogState state;

	// Sounds
	FSOUND_SAMPLE * m_sfxClic;
	FSOUND_SAMPLE * m_sfxOver;

	// Dialog root control
	CControl * dialogRoot;
	// Dialog frames
	CControl * dialogFrameHeader;
	CControl * dialogFrame;
	CControl * dialogFrameButtons;

	fastdelegate::FastDelegate0<> onOkEvent;
	fastdelegate::FastDelegate0<> onCancelEvent;
	fastdelegate::FastDelegate0<> onYesEvent;
	fastdelegate::FastDelegate0<> onNoEvent;

protected:
	// Dialog buttons
	CControl * btn_ok;
	CControl * btn_cancel;
	CControl * btn_yes;
	CControl * btn_no;

public:
	// Constructor/Destructor
	IDialog(unsigned int font, CString dialogTitle, CVector2i size, DialogButtons buttons);
	virtual ~IDialog();

	// update
	virtual void update(float delay);

	// render
	virtual void render();

	// CListener Events
	virtual void MouseEnter(CControl * control);
	virtual void Click(CControl * control);

protected:
	// Dialog events
	virtual void OnOk();
	virtual void OnCancel();
	virtual void OnYes();
	virtual void OnNo();

	void Finish();

	int GetControlWidth(int numBtns, int margin, int width, int defCtrlWidth);
};

#endif
#endif //DEDICATED_SERVER