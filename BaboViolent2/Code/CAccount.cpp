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

#include "CAccount.h"
#include "GameVar.h"
#include "CMaster.h"
#include "ConfirmPass.h"
#include "MessageDialog.h"
#include "Scene.h"

extern CMaster* master;
extern Scene* scene;

CAccount::CAccount(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(in_parent->size.s[0]-20, 450), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	//instance->texture = dktCreateTextureFromFile("main/textures/Menu3Back.tga", DKT_FILTER_LINEAR);
	//instance->borderColor.set(1,.5f,.25f);

	CControl * label1 = new CControl(instance, CVector2i(10,20), CVector2i(90,25),"Login:", this, "LABEL");
	label1->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label1->toolTips = "Login has to be unique.";
	txt_login = new CControl(instance, CVector2i(10,10), CVector2i(150,25),gameVar.cl_accountUsername, this, "EDIT", label1, CONTROL_SNAP_RIGHT);

	CControl * label2 = new CControl(instance, CVector2i(10,20), CVector2i(90,25),"Password:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM);
	label2->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label2->toolTips = "Password to your account.";
	txt_loginPassword = new CControl(instance, CVector2i(10,10), CVector2i(150,25),gameVar.cl_accountPassword, this, "EDIT", label2, CONTROL_SNAP_RIGHT);
	txt_loginPassword->password = true;

	btn_login = new CControl(instance, CVector2i(20,20),CVector2i(110,55),"Login",this,"BUTTON", txt_login, CONTROL_SNAP_RIGHT, 5);
	btn_create = new CControl(instance, CVector2i(20,20),CVector2i(110,25),"Create Account",this,"BUTTON", btn_login, CONTROL_SNAP_RIGHT);
	btn_delete = new CControl(instance, CVector2i(20,20),CVector2i(110,25),"Delete Account",this,"BUTTON", btn_create, CONTROL_SNAP_BOTTOM);

	CControl * label3 = new CControl(instance, CVector2i(10,20), CVector2i(90,25),"Nick:", this, "LABEL", label2, CONTROL_SNAP_BOTTOM);
	label3->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label3->toolTips = "Your in-game nick.";
	txt_nick = new CControl(instance, CVector2i(10,10), CVector2i(150,25),gameVar.cl_playerName, this, "EDIT", label3, CONTROL_SNAP_RIGHT);

	CControl * label4 = new CControl(instance, CVector2i(10,20), CVector2i(90,25),"Email:", this, "LABEL", label3, CONTROL_SNAP_BOTTOM);
	label4->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label4->toolTips = "Your e-mail address. Only way to recover password to your account.";
	txt_email = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"", this, "EDIT", label4, CONTROL_SNAP_RIGHT);

	CControl * separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Statistics", this, "SEPARATOR", label4, CONTROL_SNAP_BOTTOM);

	CControl * separator1 = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Clan", this, "SEPARATOR", separator, CONTROL_SNAP_BOTTOM);

	CControl * labelcn = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Clan name:", this, "LABEL", separator1, CONTROL_SNAP_BOTTOM);
	labelcn->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_clanName = new CControl(instance, CVector2i(10,10), CVector2i(400,25),"Friends eXclusive 123456789123456789123456789", this, "LABEL", labelcn, CONTROL_SNAP_RIGHT);

	CControl * labelct = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Clan tag:", this, "LABEL", labelcn, CONTROL_SNAP_BOTTOM);
	labelct->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_clanTag = new CControl(instance, CVector2i(10,10), CVector2i(400,25),"", this, "LABEL", labelct, CONTROL_SNAP_RIGHT);

	CControl * labelce = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Contact email:", this, "LABEL", labelct, CONTROL_SNAP_BOTTOM);
	labelce->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_email = new CControl(instance, CVector2i(10,10), CVector2i(400,25),"", this, "LABEL", labelce, CONTROL_SNAP_RIGHT);

	CControl * labelw = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Website:", this, "LABEL", labelce, CONTROL_SNAP_BOTTOM);
	labelw->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	lbl_website = new CControl(instance, CVector2i(10,10), CVector2i(400,25),"", this, "LABEL", labelw, CONTROL_SNAP_RIGHT);

	CControl * separator2 = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Change password", this, "SEPARATOR", labelw, CONTROL_SNAP_BOTTOM);

	CControl * label = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Old password:", this, "LABEL", separator2, CONTROL_SNAP_BOTTOM);
	label->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	txt_oldPassword = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label, CONTROL_SNAP_RIGHT);
	txt_oldPassword->password = true;

	label = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"New Password:", this, "LABEL", label, CONTROL_SNAP_BOTTOM);
	label->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label->toolTips = "Password to your account.";
	txt_newPassword = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label, CONTROL_SNAP_RIGHT);
	txt_newPassword->password = true;

	label = new CControl(instance, CVector2i(10,20), CVector2i(150,25),"Confirm password:", this, "LABEL", label, CONTROL_SNAP_BOTTOM);
	label->textAlign = CONTROL_TEXTALIGN_MIDDLERIGHT;
	label->toolTips = "Confirm password.";
	txt_password1 = new CControl(instance, CVector2i(10,10), CVector2i(125,25),"", this, "EDIT", label, CONTROL_SNAP_RIGHT);
	txt_password1->password = true;

	btn_changePass = new CControl(instance, CVector2i(20,30),CVector2i(120,25),"Change password",this,"BUTTON", label, CONTROL_SNAP_BOTTOM);
	btn_changePass->toolTips = "Change password";

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];
}

CAccount::~CAccount()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CAccount::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CAccount::MouseLeave(CControl * control)
{
}
void CAccount::MouseDown(CControl * control)
{
}
void CAccount::MouseUp(CControl * control)
{
}
void CAccount::MouseMove(CControl * control)
{
}
void CAccount::Click(CControl * control)
{
	if (control->style == "BUTTON" || control->style == "LISTBOX" || control->style == "CHECK")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}

	if (control == btn_create)
	{
		// [dsl] This stuff doesn't build, and not sure what it is. Was part of Pro version
	//	lastDialog = new ConfirmPass(scene->font, CVector2i(250, 70));
	//	lastDialog->onOkEvent.bind(this, confirmCreateAccountOkHandler);
	//	lastDialog->onCancelEvent.bind(this, confirmCreateAccountCancelHandler);
	}
	if (control == btn_login)
	{
		master->AccountManager.loginAccount(txt_login->text.s, txt_loginPassword->text.s);
	}
	if (control == btn_delete)
	{
		// [dsl] This part doesn't build. Part of pro-version I didn't code it. Need review.
	//	lastDialog = new MessageDialog(scene->font, ButtonsYesNo, "Are you sure you want to delete account?",
	//		"All data connected to it will be lost!");
	//	lastDialog->onYesEvent.bind(this, confirmRemoveAccountYesHandler);
	//	lastDialog->onNoEvent.bind(this, confirmRemoveAccountNoHandler);
	}
	if (control == btn_changePass)
	{
		master->AccountManager.changePassword(0, txt_oldPassword->text.s, txt_newPassword->text.s);
	}
}
void CAccount::Validate(CControl * control)
{
	if (control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
}

void CAccount::confirmCreateAccountOkHandler()
{
	master->AccountManager.createAccount(txt_login->text.s, txt_loginPassword->text.s, txt_nick->text.s, txt_email->text.s);
	menuManager.hideDialog(true);
	lastDialog = 0;
}

void CAccount::confirmCreateAccountCancelHandler()
{
	menuManager.hideDialog(true);
	lastDialog = 0;
}

void CAccount::confirmRemoveAccountYesHandler()
{
	master->AccountManager.deleteAccount(0, txt_loginPassword->text.s);
	menuManager.hideDialog(true);
	lastDialog = 0;
}

void CAccount::confirmRemoveAccountNoHandler()
{
	menuManager.hideDialog(true);
	lastDialog = 0;
}

#endif

