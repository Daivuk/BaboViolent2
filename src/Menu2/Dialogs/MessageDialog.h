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

#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "Zeven.h"
#include "CControl.h"
#include "Dialog.h"

class MessageDialog : public IDialog
{
public:
	// Controls
	CControl * lbl_infoLine1;
	CControl * lbl_infoLine2;
	CControl * lbl_infoLine3;
	CControl * lbl_infoLine4;
	CControl * lbl_infoLine5;
public:
	// Constructor/Destructor
	MessageDialog(unsigned int font, DialogButtons buttons, CString caption, CString line1,
		CString line2 = "", CString line3 = "", CString line4 = "", CString line5 = "");
	~MessageDialog();
};


#endif
