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
#include "Control.h"
#include "Helper.h"



//
// Constructeur
//
Control::Control(CVector2i screenPosition, CString caption, unsigned int font, float textSize, control_justify justify)
{
	m_isMouseHandle = true;
	m_screenPosition = screenPosition;
	m_leftOffset = 0;//-(float)m_screenPosition[0] - (float)m_screenPosition[1];
	m_font = font;
	m_textSize = textSize;
	m_justify = justify;
	m_state = CONTROL_NOTHING;
	m_clicDelay = 0;
	anim = 0;
	changeCaption(caption);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);
	tex_menu = dktCreateTextureFromFile("main/textures/Menu.tga", DKT_FILTER_LINEAR);
}



//
// Destructeur
//
Control::~Control()
{
	dksDeleteSound(m_sfxOver);
	dktDeleteTexture(&tex_menu);
}



//
// L'updater le gèrer
//
void Control::update(float delay)
{
	anim += delay * rand(.5f, 2.0f);
	while (anim >= PI*2) anim -= PI*2;

	if (m_leftOffset < 0)
	{
		m_leftOffset += delay * fabsf(m_leftOffset) * 4;
		if (m_leftOffset > 0) m_leftOffset = 0;
	}

	if (m_isMouseHandle)
	{
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
			else if (dkiGetState(DKI_MOUSE_BUTTON2) == DKI_DOWN)
			{
				m_state = CONTROL_DOWN;
				m_clicDelay = .1f;
				onRightClic();
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
void Control::render()
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
			glColor3f(.95f,.3f,.25f);
			break;
		case CONTROL_OVER:
			glColor3f(.95f,1,.25f); 
			offset = 8;
			break;
		}
		glBegin(GL_QUAD_STRIP);
			glColor3f(sinf(anim)*.25f + .75f, .5f, .5f);
			glTexCoord2f(0,1);
			glVertex2f(m_rect[0]+m_leftOffset-m_textSize*.5f-offset,m_rect[1]);

			glTexCoord2f(0,0);
			glColor3f(sinf(anim+1)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[0]+m_leftOffset-m_textSize*.5f-offset,m_rect[3]);

			glTexCoord2f(.5f,1);
			glColor3f(sinf(anim+2)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[0]+m_leftOffset-offset,m_rect[1]);

			glTexCoord2f(.5f,0);
			glColor3f(sinf(anim+3)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[0]+m_leftOffset-offset,m_rect[3]);

			glTexCoord2f(.5f,1);
			glColor3f(sinf(anim+4)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[2]+m_leftOffset+offset,m_rect[1]);

			glTexCoord2f(.5f,0);
			glColor3f(sinf(anim+5)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[2]+m_leftOffset+offset,m_rect[3]);

			glTexCoord2f(0,1);
			glColor3f(sinf(anim+6)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[2]+m_leftOffset+m_textSize*.5f+offset,m_rect[1]);

			glTexCoord2f(0,0);
			glColor3f(sinf(anim+7)*.25f + .75f, .5f, .5f);
			glVertex2f(m_rect[2]+m_leftOffset+m_textSize*.5f+offset,m_rect[3]);
		glEnd();

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



//
// Pour changer la caption
//
void Control::changeCaption(CString caption)
{
	m_caption = caption;

	// On va chercher les dimension du string
	dkfBindFont(m_font);
	float w = dkfGetStringWidth(m_textSize, m_caption.s);
	float h = dkfGetStringHeight(m_textSize, m_caption.s);

	// On met à jour notre Rect avec ça
	switch (m_justify)
	{
	case JUSTIFY_LEFT:
		m_rect[0] = (float)m_screenPosition[0];
		m_rect[1] = (float)m_screenPosition[1];
		m_rect[2] = (float)m_screenPosition[0] + w;
		m_rect[3] = (float)m_screenPosition[1] + h;
		break;
	case JUSTIFY_CENTER:
		m_rect[0] = (float)m_screenPosition[0] - w/2;
		m_rect[1] = (float)m_screenPosition[1];
		m_rect[2] = (float)m_screenPosition[0] + w/2;
		m_rect[3] = (float)m_screenPosition[1] + h;
		break;
	case JUSTIFY_RIGHT:
		m_rect[0] = (float)m_screenPosition[0] - w;
		m_rect[1] = (float)m_screenPosition[1];
		m_rect[2] = (float)m_screenPosition[0];
		m_rect[3] = (float)m_screenPosition[1] + h;
		break;
	}
}
#endif

