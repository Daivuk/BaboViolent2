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
#include "ConfirmPass.h"
#include "CMenuManager.h"
#include "MessageDialog.h"

ConfirmPass::ConfirmPass(unsigned int font, CVector2i size)
	:	IDialog(font, "Confirm Password", CVector2i(450, 160), ButtonsOKCancel)
	,	subDialog(0)
{
	if(size[0] < 16) size[0] = 16;
	if(size[1] < 16) size[1] = 16;
	if(size[0] > 64) size[0] = 64;
	if(size[1] > 64) size[1] = 64;

	lbl_confirmPass = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(100,  25), "Password:", this, "LABEL");
	lbl_confirmPass->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	txt_pass        = new CControl(dialogFrame, CVector2i( 10,  10), CVector2i(325,  25), "", this, "EDIT", lbl_confirmPass, CONTROL_SNAP_RIGHT);

	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);

	if(subDialog != 0)
	{
		dialogRoot->visible = false;
	}
}

ConfirmPass::~ConfirmPass()
{
	if (subDialog != 0)
		delete subDialog;
}

void ConfirmPass::update(float delay)
{
	if(subDialog != 0)
	{
		//subDialog->update(delay);
		if(subDialog->state != StateNothing)
		{
			menuManager.hideDialog();
			delete subDialog;
			subDialog = 0;
			dialogRoot->visible = true;
		}
	}
	else
	{
		IDialog::update(delay);
	}
}

void ConfirmPass::render()
{
	if(subDialog != 0)
	{
		//subDialog->render();
	}
	else
	{
		IDialog::render();;
	}
}

void ConfirmPass::Click(CControl * control)
{
	// Notify parent
	IDialog::Click(control);

	//Validate(control);
}

void ConfirmPass::Validate(CControl * control)
{
}

void ConfirmPass::OnOk()
{
	if(txt_pass->text != "")
	{
		;
	}
	else
	{
		subDialog = new MessageDialog(dialogRoot->font, ButtonsOK, "Error!", "Passwords are different!");
		state = StateNothing;
	}
}

#endif
