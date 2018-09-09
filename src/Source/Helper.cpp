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

#include "Helper.h"


//
// Pour enlever la couleur d'un message, pour le shadow par exemple
//
CString textColorLess(const CString & text)
{
	CString rval;
	char	result[MAX_CARAC];
	int		ri = 0;

	for (int i=0;i<text.len();++i)
	{
		char c = text[i];
		if ((unsigned char)c >= '\x10' || (unsigned char)c == '\n')
		{
			result[ri] = c;
			ri++;
		}
	}

	result[ri] = '\0';

	return CString("%s", result);
}


#ifndef DEDICATED_SERVER
bool enableShadow = true;

//
// Pour �rire du texte centr�
//
void printCenterText(float x, float y, float size, const CString & text)
{

	float width = dkfGetStringWidth(size, text.s);

	if ((enableShadow) && (gameVar.r_simpleText != true))
	{
		int shadowDis = (size > 32) ? 2 : 1;
		float curColor[4];

#ifndef _DX_
		glGetFloatv(GL_CURRENT_COLOR, curColor);
		glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,0,0, curColor[3]);
			dkfPrint(size,x-width/2+shadowDis,y+shadowDis,0,textColorLess(text).s);
		glPopAttrib();
		glColor4fv(curColor);
#endif
	}
	dkfPrint(size,x-width/2,y,0,text.s);
}

void printLeftText(float x, float y, float size, const CString & text)
{

	
	if ((enableShadow) && (gameVar.r_simpleText != true))
	{
		int shadowDis = (size > 32) ? 2 : 1;
		float curColor[4];


#ifndef _DX_
		glGetFloatv(GL_CURRENT_COLOR, curColor);
		glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,0,0, curColor[3]);
			dkfPrint(size,x/*+shadowDis*/,y+1,0,textColorLess(text).s);
		glPopAttrib();
		glColor4fv(curColor);
#endif
	}
	dkfPrint(size,x,y,0,text.s);
}

void printRightText(float x, float y, float size, const CString & text)
{
	
	float width = dkfGetStringWidth(size, text.s);
	
	
	if ((enableShadow) && (gameVar.r_simpleText != true))
	{
		float curColor[4];
		int shadowDis = (size > 32) ? 2 : 1;

#ifndef _DX_
		glGetFloatv(GL_CURRENT_COLOR, curColor);
		glPushAttrib(GL_CURRENT_BIT);
			glColor4f(0,0,0, curColor[3]);
			dkfPrint(size,x-width+shadowDis,y+shadowDis,0,textColorLess(text).s);
		glPopAttrib();
		glColor4fv(curColor);
#endif
	}
	
	dkfPrint(size,x-width,y,0,text.s);
}



//
// Pour afficher un quad truc simple de meme
//
void renderTexturedQuad(int x, int y, int w, int h, unsigned int texture)
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
			glTexCoord2i(0,1);
			glVertex2i(x,y);
			glTexCoord2i(0,0);
			glVertex2i(x,y+h);
			glTexCoord2i(1,0);
			glVertex2i(x+w,y+h);
			glTexCoord2i(1,1);
			glVertex2i(x+w,y);
		glEnd();
	glPopAttrib();
#endif
}

void renderTexturedQuadSmooth(int x, int y, int w, int h, unsigned int texture)
{
	float curColor[4];
#ifndef _DX_
	glGetFloatv(GL_CURRENT_COLOR, curColor);

	glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUAD_STRIP);
			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,1);
			glVertex2i(x,y);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,1);
			glVertex2i(x,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,1);
			glVertex2i(x+10,y);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(0,1);
			glVertex2i(x+10,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,1);
			glVertex2i(x+w-10,y);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(1,1);
			glVertex2i(x+w-10,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,1);
			glVertex2i(x+w,y);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,1);
			glVertex2i(x+w,y+10);
		glEnd();
		glBegin(GL_QUAD_STRIP);
			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,1);
			glVertex2i(x,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,0);
			glVertex2i(x,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(0,1);
			glVertex2i(x+10,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(0,0);
			glVertex2i(x+10,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(1,1);
			glVertex2i(x+w-10,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(1,0);
			glVertex2i(x+w-10,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,1);
			glVertex2i(x+w,y+10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,0);
			glVertex2i(x+w,y+h-10);
		glEnd();
		glBegin(GL_QUAD_STRIP);
			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,0);
			glVertex2i(x,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,0);
			glVertex2i(x,y+h);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(0,0);
			glVertex2i(x+10,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(0,0);
			glVertex2i(x+10,y+h);

			glColor4f(curColor[0],curColor[1],curColor[2],1);
			glTexCoord2i(1,0);
			glVertex2i(x+w-10,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,0);
			glVertex2i(x+w-10,y+h);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,0);
			glVertex2i(x+w,y+h-10);

			glColor4f(curColor[0],curColor[1],curColor[2],0);
			glTexCoord2i(1,0);
			glVertex2i(x+w,y+h);
		glEnd();
	glPopAttrib();
#endif
}

void renderMenuQuad(int x, int y, int w, int h)
{
#ifndef _DX_
	glPushAttrib(GL_CURRENT_BIT);

		//--- Round corner of 5 units
		CVector4f color4;
		glGetFloatv(GL_CURRENT_COLOR, color4.s);

	/*	if (gameVar.r_highDetailMenu)
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
		}*/
		if (gameVar.r_highDetailMenu)
		{
			glBegin(GL_QUADS);
				glColor4f(0, 0, 0, .25f);
				
				glVertex2i(x - 1, y);
				glVertex2i(x - 1, y + h);
				glVertex2i(x + w, y + h);
				glVertex2i(x + w, y);

				glVertex2i(x, y - 1);
				glVertex2i(x, y + h);
				glVertex2i(x + w, y + h);
				glVertex2i(x + w, y - 1);

				glVertex2i(x, y);
				glVertex2i(x, y + h + 1);
				glVertex2i(x + w, y + h + 1);
				glVertex2i(x + w, y);

				glVertex2i(x, y);
				glVertex2i(x, y + h);
				glVertex2i(x + w + 1, y + h);
				glVertex2i(x + w + 1, y);
			glEnd();

			glBegin(GL_QUADS);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x,y);
				glColor4fv((color4 * 1).s);
				glVertex2i(x,y+h / 2);
				glColor4fv((color4 * 1).s);
				glVertex2i(x+w,y+h / 2);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w,y);

				glColor4fv((color4*.9f).s);
				glVertex2i(x,y + h / 2);
				glColor4fv((color4*0.6f).s);
				glVertex2i(x,y+h);
				glColor4fv((color4*0.6f).s);
				glVertex2i(x+w,y+h);
				glColor4fv((color4*.9f).s);
				glVertex2i(x+w,y + h / 2);
			glEnd();
		}
		else
		{
			glBegin(GL_QUADS);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x,y);
				glColor4fv((color4 * 0.6f).s);
				glVertex2i(x,y+h);
				glColor4fv((color4 * 0.6f).s);
				glVertex2i(x+w,y+h);
				glColor4fv((color4*1.2f).s);
				glVertex2i(x+w,y);
			glEnd();
		}
	glPopAttrib();
#endif
}

/*void renderMenuQuad(int x, int y, int w, int h)
{
	//--- Round corner of 5 units
	CVector4f color4;
	glGetFloatv(GL_CURRENT_COLOR, color4.s);
	CVector3f color(color4[0], color4[1], color4[2]);

/*	glBegin(GL_QUAD_STRIP);
		glColor3fv((color*.8f).s);
		glVertex2i(x,y+5);
		glColor3fv((color*1.2f).s);
		glVertex2i(x,y+h-5);
		glColor3fv((color*.8f).s);
		glVertex2i(x+1,y+3);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+1,y+h-3);
		glColor3fv((color*.8f).s);
		glVertex2i(x+3,y+1);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+3,y+h-1);
		glColor3fv((color*.8f).s);
		glVertex2i(x+5,y);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+5,y+h);
		glColor3fv((color*.8f).s);
		glVertex2i(x+w-5,y);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+w-5,y+h);
		glColor3fv((color*.8f).s);
		glVertex2i(x+w-3,y+1);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+w-3,y+h-1);
		glColor3fv((color*.8f).s);
		glVertex2i(x+w-1,y+3);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+w-1,y+h-3);
		glColor3fv((color*.8f).s);
		glVertex2i(x+w,y+5);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+w,y+h-5);
	glEnd();*/
/*
	glBegin(GL_QUADS);
		glColor3fv((color*.8f).s);
		glVertex2i(x,y);
		glColor3fv((color*1.2f).s);
		glVertex2i(x,y+h);
		glColor3fv((color*.8f).s);
		glVertex2i(x+w,y+h);
		glColor3fv((color*1.2f).s);
		glVertex2i(x+w,y);
	glEnd();
}*/

#endif


//
// Pour utiliser une fonction cubic spline pour pr�enir le lag appearance ;)
//
float cubicSpline(float x0, float x1, float x2, float x3, float t)
{
	return x0 * ((1 - t)*(1 - t)*(1 - t)) + x1 * 3 * t * ((1 - t)*(1 - t)) + x2 * 3 * (t*t) * (1 - t) + x3 * (t*t*t);
}

CVector3f cubicSpline(const CVector3f & x0, const CVector3f & x1, const CVector3f & x2, const CVector3f & x3, float t)
{
	return x0 * ((1 - t)*(1 - t)*(1 - t)) + x1 * 3 * t * ((1 - t)*(1 - t)) + x2 * 3 * (t*t) * (1 - t) + x3 * (t*t*t);
}



//
// On check sphere-to-box
//
bool sphereToBox(CVector3f& p1, CVector3f& p2, CVector3f& omin, CVector3f& omax, CVector3f & radius)
{
 	CVector3f min = omin - radius;
	CVector3f max = omax + radius;
    CVector3f d = (p2 - p1) * 0.5f;
    CVector3f e = (max - min) * 0.5f;
    CVector3f c = p1 + d - (min + max) * 0.5f;
    CVector3f ad = CVector3f(fabsf(d[0]),fabsf(d[1]),fabsf(d[2])); // Returns same vector with all components positive

    if (fabsf(c[0]) > e[0] + ad[0])
        return false;
    if (fabsf(c[1]) > e[1] + ad[1])
        return false;
    if (fabsf(c[2]) > e[2] + ad[2])
        return false;
  
    if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1]/* + EPSILON*/)
        return false;
    if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2]/* + EPSILON*/)
        return false;
    if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0]/* + EPSILON*/)
        return false;
            
    return true;
}



//
// Distance entre un point et une droite (2 sqrtf() dans fonction)
//
bool segmentToSphere(CVector3f & p1, CVector3f & p2, CVector3f & c, float radius)
{
	// Vecteur directeur
	CVector3f u = p2 - p1;
	float l = u.length();
	u /= l;

	// Vecteur vers les points
	CVector3f p = c - p1;

	// Projection distance
	float d = dot(p, u);

	// On trouve le point le plus proche sur le segment
	CVector3f r;
	if (d < 0)
	{
		r = p1;
	}
	else if (d > l)
	{
		r = p2;
	}
	else
	{
		r = p1 + u * d;
	}

	// On check la distance du point �l'axe
	if (distanceSquared(r, c) <= radius*radius)
	{
		// On ajuste notre p2
		p2 = r;
		return true;
	}

/*	;
	CVector3f D = H - P0;

	// On projecte D sur A
	CVector3f d = projection(D, A);

	// On clamp pour pas qu'on le d�asse
	if (dot(d, A) < 0) d = P0;
	if (distanceSquared(CVector3f(), d) > distanceSquared(P0, P1)) d = P1;

	// On pogne le point sur l'axe (notre seul sqrt de la fonction
	CVector3f R = P0 + A * d.length();

	// On check la distance du point �l'axe
	if (distanceSquared(R, H) <= r1*r1)
	{
		// On ajuste notre p2 (ici appel�P1)
		P1 = R;
		return true;
	}*/

	return false;
}

