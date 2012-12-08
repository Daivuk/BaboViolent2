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

#ifndef HELPER_H
#define HELPER_H


#include "Zeven.h"
#include "GameVar.h"

#ifndef CONSOLE
// Pour �rire du texte centr�
void printCenterText(float x, float y, float size, const CString & text);
void printLeftText(float x, float y, float size, const CString & text);
void printRightText(float x, float y, float size, const CString & text);

// Pour afficher un quad truc simple de meme
void renderTexturedQuad(int x, int y, int w, int h, unsigned int texture);
void renderTexturedQuadSmooth(int x, int y, int w, int h, unsigned int texture);
//void renderMenuQuad(int x, int y, int w, int h);
void renderMenuQuad(int x, int y, int w, int h);
/*inline void renderMenuQuad(int x, int y, int w, int h)
{
#ifndef _DX_
	glPushAttrib(GL_CURRENT_BIT);

		//--- Round corner of 5 units
		CVector4f color4;
		glGetFloatv(GL_CURRENT_COLOR, color4.s);

		if (gameVar.r_highDetailMenu)
		{
			glBegin(GL_QUAD_STRIP);
				glColor4fv((color4*.8f).s);
				glVertex2i(x,y+5);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x,y+h-5);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+1,y+3);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+1,y+h-3);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+3,y+1);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+3,y+h-1);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+5,y);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+5,y+h);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w-5,y);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w-5,y+h);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w-3,y+1);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w-3,y+h-1);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w-1,y+3);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w-1,y+h-3);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w,y+5);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w,y+h-5);
			glEnd();
		}
		else
		{
			glBegin(GL_QUADS);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x,y);
				glColor4fv((color4*.8f).s);
				glVertex2i(x,y+h / 2);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w,y+h / 2);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w,y);

				glColor4fv((color4*1.2f).s);
				glVertex2i(x,y);
				glColor4fv((color4*.8f).s);
				glVertex2i(x,y+h);
				glColor4fv((color4*.8f).s);
				glVertex2i(x+w,y+h);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w,y);
			glEnd();
	//	}
	glPopAttrib();
#endif
}*/

#endif

// Pour utiliser une fonction cubic spline pour pr�enir le lag appearance ;)
float cubicSpline(float x0, float x1, float x2, float x3, float t);
CVector3f cubicSpline(const CVector3f & x0,const CVector3f & x1, const CVector3f & x2, const CVector3f & x3, float t);

// On check sphere-to-box
bool sphereToBox(CVector3f& p1, CVector3f& p2, CVector3f& omin, CVector3f& omax, CVector3f & radius);

// Pour enlever la couleur d'un message, pour le shadow par exemple
CString textColorLess(const CString & text);

// Segment to sphere
bool segmentToSphere(CVector3f & P0, CVector3f & P1, CVector3f & H, float r1);


#endif

