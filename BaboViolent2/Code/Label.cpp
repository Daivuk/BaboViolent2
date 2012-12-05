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
#include "Label.h"
#include "Helper.h"



//
// Constructeur
//
Label::Label(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify)
: Control(screenPosition, caption, m_font, textSize, justify)
{
	m_isMouseHandle = false;
}



//
// Pour l'afficher
//
void Label::render()
{
#ifndef _DX_
	glPushAttrib(GL_CURRENT_BIT);

		dkfBindFont(m_font);

		// Shadow
		glColor3f(0,0,0);
		dkfPrint(m_textSize, m_rect[0]+m_leftOffset+1, m_rect[1]+1, 0, textColorLess(m_caption).s);

		// Le standard pour les bouton
		switch (m_state)
		{
		case CONTROL_DOWN: 
		case CONTROL_NOTHING: 
			glColor3f(1,1,1);
			dkfPrint(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0, m_caption.s);
			break;
		case CONTROL_OVER:
			glColor3f(.5f,.5f,1); 
			dkfPrint(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0, m_caption.s);
			break;
		}
	glPopAttrib();
#endif
}
#endif

