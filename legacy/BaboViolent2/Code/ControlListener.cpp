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
#include "ControlListener.h"
#include "Zeven.h"



//
// Constructeur
//
ControlListener::ControlListener()
{
}



//
// Destructeur
//
ControlListener::~ControlListener()
{
	int i;
	ZEVEN_DELETE_VECTOR(m_controls, i);
}



//
// Update
//
void ControlListener::updateMenu(float delay)
{
/*	if (dkiGetState(DIK_ESCAPE) == DKI_DOWN)
	{
		if (m_controls.size() > 0)
		{
			onClick(m_controls[0]);
		}
	}*/

	for (int i=0;i<(int)m_controls.size();m_controls[i++]->update(delay));
	updateUnique(delay);
}



//
// Render
//
void ControlListener::renderMenu()
{
#ifndef _DX_
	dkglPushOrtho(800,600);
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/*	glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex2f(0,0);
				glColor4f(0,0,0,0);
				glVertex2f(0,300);
				glVertex2f(800,300);
				glColor4f(0,0,0,1);
				glVertex2f(800,0);
			glEnd();
			glBegin(GL_QUADS);
				glColor4f(0,0,0,0);
				glVertex2f(0,300);
				glColor4f(0,0,0,1);
				glVertex2f(0,600);
				glVertex2f(800,600);
				glColor4f(0,0,0,0);
				glVertex2f(800,300);
			glEnd();
			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex2f(0,0);
				glVertex2f(0,600);
				glColor4f(0,0,0,0);
				glVertex2f(400,600);
				glVertex2f(400,0);
			glEnd();*/
			glBegin(GL_QUADS);
				glColor4f(0,0,0,.5f);
				glVertex2f(0,0);
				glVertex2f(0,600);
				glVertex2f(800,600);
				glVertex2f(800,0);
			glEnd();
			for (int i=0;i<(int)m_controls.size();m_controls[i++]->render());
			renderUnique();
		glPopAttrib();
	dkglPopOrtho();
#endif
}



//
// pour ajouter un control
//
void ControlListener::add(Control * control)
{
	m_controls.push_back(control);
}
#endif

