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
#include "Write.h"
#include "Menu.h"



//
// Constructeur
//
Write::Write(CVector2i screenPosition, 
			   CString caption, 
			   unsigned int m_font, 
			   float textSize, 
			   control_justify justify, 
			   int flag)
: Control(screenPosition, caption, m_font, textSize, justify)
{
	validationType = flag;
	set("%s", caption.s);
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
}



//
// Destructeur
//
Write::~Write()
{
	loseFocus();
	dksDeleteSound(m_sfxClic);
}



//
// L'updater le gèrer
//
void Write::update(float delay)
{
	if (m_leftOffset < 0)
	{
		m_leftOffset += delay * fabsf(m_leftOffset) * 4;
		if (m_leftOffset > 0) m_leftOffset = 0;
	}

	if (m_clicDelay > 0)
	{
		m_clicDelay -= delay;
		if (m_clicDelay < 0)
		{
			m_clicDelay = 0;
			m_state = CONTROL_NOTHING;
		}
	}

	// On check si la souri est dans notre Rectangle
	CVector2i mousePos = dkwGetCursorPos_main();
	CVector2i res = dkwGetResolution();

	// Il faut maintenant ajuster la mousePos dans notre 800x600
	mousePos[0] = (int)((float)mousePos[0] / (float)res[0] * 800.0f);
	mousePos[1] = (int)((float)mousePos[1] / (float)res[1] * 600.0f);

	if (isActivated())
	{
		loseFocus();
		dksPlaySound(m_sfxClic, -1, 200);

		if (validationType == VALIDATE_INT)
		{
			int value = toInt();
			set("%i", value);
		}
		if (validationType == VALIDATE_FLOAT)
		{
			float value = toFloat();
			set("%.02f", value);
		}

		if (len() != 0) changeCaption(*this);
		else set("%s", m_caption.s);
	}

	if (haveFocus())
	{
		changeCaption(*this); // On change la caption en tout temps pour updater le sizing du control
		if (dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN)
		{
			if (len() != 0) changeCaption(*this);
			else set("%s", m_caption.s);
			loseFocus();
			dksPlaySound(m_sfxClic, -1, 200);
		}
	}
	else
	{
		// On test maintenant avec le rectange du bouton
		if (mousePos[0] >= (int)m_rect[0]-10 &&
			mousePos[0] <= (int)m_rect[2]+10 &&
			mousePos[1] >= (int)m_rect[1] &&
			mousePos[1] <= (int)m_rect[3])
		{
			// Si on n'avait pas de state, on tombe over
			if (m_state == CONTROL_NOTHING)
			{
				dksPlaySound(m_sfxOver, -1, 150);
				m_state = CONTROL_OVER;
			}
			if (dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN)
			{
				m_state = CONTROL_DOWN;
				m_clicDelay = .1f;
				onClic();
			}
		}
		else if (m_state == CONTROL_OVER)
		{
			m_state = CONTROL_NOTHING;
		}
	}
}



//
// Pour l'afficher
//
void Write::render()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		// Le contour
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_menu);
		float offset = 0;
		switch (m_state)
		{
		case CONTROL_DOWN: 
		case CONTROL_NOTHING: 
			glColor3f(.5f,1,0);
			break;
		case CONTROL_OVER:
			glColor3f(0,1,.5f); 
			offset = 8;
			break;
		}
		glBegin(GL_QUAD_STRIP);
			glTexCoord2f(0,1);
			glVertex2f(m_rect[0]+m_leftOffset-m_textSize*.5f-offset,m_rect[1]);
			glTexCoord2f(0,0);
			glVertex2f(m_rect[0]+m_leftOffset-m_textSize*.5f-offset,m_rect[3]);
			glTexCoord2f(.5f,1);
			glVertex2f(m_rect[0]+m_leftOffset-offset,m_rect[1]);
			glTexCoord2f(.5f,0);
			glVertex2f(m_rect[0]+m_leftOffset-offset,m_rect[3]);
			glTexCoord2f(.5f,1);
			glVertex2f(m_rect[2]+m_leftOffset+offset,m_rect[1]);
			glTexCoord2f(.5f,0);
			glVertex2f(m_rect[2]+m_leftOffset+offset,m_rect[3]);
			glTexCoord2f(0,1);
			glVertex2f(m_rect[2]+m_leftOffset+m_textSize*.5f+offset,m_rect[1]);
			glTexCoord2f(0,0);
			glVertex2f(m_rect[2]+m_leftOffset+m_textSize*.5f+offset,m_rect[3]);
		glEnd();

		if (haveFocus())
		{
			// Le standard pour les bouton
			switch (m_state)
			{
			case CONTROL_NOTHING: 
				glColor3f(1,1,1);
				print(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0);
				break;
			case CONTROL_OVER:
				glColor3f(.5f,.5f,1); 
				print(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0);
				break;
			case CONTROL_DOWN: 
				glColor3f(1,.5f,.5f); 
				print(m_textSize, m_rect[0]+m_leftOffset+2, m_rect[1]+2, 0);
				break;
			}
		}
		else
		{
			// Le standard pour les bouton
			switch (m_state)
			{
			case CONTROL_NOTHING: 
				glColor3f(1,1,1);
				print(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0);
				break;
			case CONTROL_OVER:
				glColor3f(.5f,.5f,1); 
				print(m_textSize, m_rect[0]+m_leftOffset, m_rect[1], 0);
				break;
			case CONTROL_DOWN: 
				glColor3f(1,.5f,.5f); 
				print(m_textSize, m_rect[0]+m_leftOffset+2, m_rect[1]+2, 0);
				break;
			}
		}
	glPopAttrib();
#endif
}



//
// Pour quand on clic dessus, le child saura quoi faire
//
void Write::onClic()
{
	giveFocus();
	dksPlaySound(m_sfxClic, -1, 200);
}
#endif

