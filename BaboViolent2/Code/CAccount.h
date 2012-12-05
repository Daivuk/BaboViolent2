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
#ifndef CACCOUNT_H_INCLUDED
#define CACCOUNT_H_INCLUDED

#include "CPanel.h"

class IDialog;

class CAccount : public CPanel
{
private:
	IDialog * lastDialog;

public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	CControl * btn_create;
	CControl * btn_delete;
	CControl * btn_login;
	CControl * btn_changePass;
	CControl * txt_login;
	CControl * txt_loginPassword;
	CControl * txt_oldPassword;
	CControl * txt_newPassword;
	CControl * txt_password1;
	CControl * txt_nick;
	CControl * txt_email;
	CControl * txt_status;

	CControl * lbl_clanName;
	CControl * lbl_clanTag;
	CControl * lbl_email;
	CControl * lbl_website;

public:
	CAccount(CControl * in_parent, CControl * in_alignTo);
	virtual ~CAccount();

	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
	void Validate(CControl * control);

private:
	void confirmCreateAccountOkHandler();
	void confirmCreateAccountCancelHandler();

	void confirmRemoveAccountYesHandler();
	void confirmRemoveAccountNoHandler();
};


#endif
#endif

