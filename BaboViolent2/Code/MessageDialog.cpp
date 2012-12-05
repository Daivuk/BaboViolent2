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
#include "MessageDialog.h"
#include "CMenuManager.h"

MessageDialog::MessageDialog(unsigned int font, DialogButtons buttons, CString caption, CString line1,
							 CString line2, CString line3, CString line4, CString line5)
							 :	IDialog(font, caption, CVector2i(300, 145), buttons)
	,	lbl_infoLine1(0)
	,	lbl_infoLine2(0)
	,	lbl_infoLine3(0)
	,	lbl_infoLine4(0)
	,	lbl_infoLine5(0)
{
	lbl_infoLine1 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280, 25), line1, this, "LABEL");
	lbl_infoLine1->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
	if(line2 != "")
	{
		lbl_infoLine2 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280, 25), line2, this, "LABEL", lbl_infoLine1, CONTROL_SNAP_BOTTOM);
		lbl_infoLine2->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
		if(line3 != "")
		{
			lbl_infoLine3 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280, 25), line3, this, "LABEL", lbl_infoLine2, CONTROL_SNAP_BOTTOM);
			lbl_infoLine3->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
			if(line4 != "")
			{
				lbl_infoLine4 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280, 25), line4, this, "LABEL", lbl_infoLine3, CONTROL_SNAP_BOTTOM);
				lbl_infoLine4->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
				if(line5 != "")
				{
					lbl_infoLine5 = new CControl(dialogFrame, CVector2i(10, 10), CVector2i(280, 25), line5, this, "LABEL", lbl_infoLine4, CONTROL_SNAP_BOTTOM);
					lbl_infoLine5->textAlign = CONTROL_TEXTALIGN_MIDDLECENTER;
				}
			}
		}
	}
	dialogFrame->backColor.set(0.25f, 0.25f, 0.25f);
}

MessageDialog::~MessageDialog()
{
}

#endif
