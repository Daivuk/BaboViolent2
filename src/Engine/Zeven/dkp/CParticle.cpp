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

/* TCE (c) All rights reserved */


#include "CParticle.h"
#include "dkpi.h"



//
// Constructeur / Destructeur
//
CParticle::CParticle(
		float *mposition,
		float *mvel,
		float *mstartColor,
		float *mendColor,
		float mstartSize,
		float mendSize,
		float mduration,
		float mdensity,
		float mairResistanceInfluence,
		float mrotationSpeed,
		unsigned int mtexture,
		unsigned int msrcBlend,
		unsigned int mdstBlend,
		int mtransitionFunc)
{
	texture = mtexture;
	position = CPoint3f(mposition);
	lastPosition = position;
	startColor = CColor4f(mstartColor);
	endColor = CColor4f(mendColor);
	vel = CVector3f(mvel);
	life = 0;
	fadeSpeed = 1.0f / mduration;
	startSize = mstartSize;
	endSize = mendSize;
	density = mdensity;
	angle = (float)(rand()%36000)/100.0f;
	rotationSpeed = mrotationSpeed;
	srcBlend = msrcBlend;
	dstBlend = mdstBlend;
//	transitionFunc = mtransitionFunc;  // Pas utilisé
	(void)mtransitionFunc;// No need to show the warning about unreferenced parameter
	airResistanceInfluence = mairResistanceInfluence;
	billboard = false;
	billboardOpacity = 0;
	billboardFadeDis = 16;
	billboardFadeDelay = 1;
	toDelete = false;

	isMultipleTexture = false;
	textureArray = 0;
	nbFrame = 1;

	// Ici on la calcul tout suite pour pouvoir la placer tout suite à bonne place dans le array
	if (CDkp::sorting) camDis = dot((CDkp::camPos - position),(CDkp::camPos - position));
}

CParticle::~CParticle()
{
}



//
// Pour l'updater
//
int CParticle::update()
{
	if (billboard)
	{
		// On pogne sa distance
		camDis = distanceSquared(CDkp::camPos, position);

		if (toDelete)
		{
			billboardOpacity -= CDkp::delay * billboardFadeDelay;
			if (billboardOpacity <= 0) return 0;
		}
		else
		{
			if (camDis >= billboardFadeDis*billboardFadeDis)
			{
				toDelete = true;
			}
			else
			{
				if (billboardOpacity < 1)
				{
					billboardOpacity += CDkp::delay * billboardFadeDelay;
					if (billboardOpacity > 1) billboardOpacity = 1;
				}
			}
		}
	}
	else
	{
		// On diminu sa vie
		life +=	fadeSpeed * CDkp::delay;

		// On check si elle est encore vivante
		if (life >= 1) return 0;

		// Sa rotation
		angle += rotationSpeed * CDkp::delay;

		// On affecte la gravité
		vel += CDkp::gravity * density * CDkp::delay;

		// On l'affecte à la densité de l'air

		// On anim finalement sa position
		position += vel * CDkp::delay;

		// Sa distance avec notre camera
		if (CDkp::sorting) camDis = distanceSquared(CDkp::camPos, position);
	}

	return 1;
}



//
// Pour le rendre
//
void CParticle::render()
{
#ifndef _DX_
	// On la positionne
	glPushMatrix();

		glBlendFunc(srcBlend, dstBlend);

		CColor4f curColor = startColor + (endColor - startColor) * life;

		if (billboard) curColor[3] *= billboardOpacity;

		float sizeTime = startSize + (endSize - startSize) * life;
		glTranslatef(position[0], position[1], position[2]);
		glMultMatrixf(CDkp::modelView);
		glScalef(sizeTime, sizeTime, sizeTime);
		glRotatef(angle, 0, 0, 1);

		if (isMultipleTexture)
		{
			int currentFrame = (int)(nbFrame*life);
			int nextFrame = currentFrame+1;
			float midFrame = (float)nbFrame * life - (float)currentFrame;
			if (nextFrame >= nbFrame) nextFrame = currentFrame;

			glBindTexture(GL_TEXTURE_2D, textureArray[currentFrame]);
			glColor4fv((curColor*CColor4f(1,1,1,1-midFrame)).s);
			glPushAttrib(GL_ENABLE_BIT);
			//	glDisable(GL_LIGHTING);
				if (dstBlend == DKP_ONE || dstBlend == DKP_ZERO) 
				{
					glDisable(GL_FOG);
					glDisable(GL_LIGHTING);
				}

				//--- Render
		//		glCallList(CDkp::dpSprite);
			//	glEnableClientState(GL_VERTEX_ARRAY);
			//	glEnableClientState(GL_NORMAL_ARRAY);
			//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			//		glVertexPointer(3, GL_FLOAT, 0, CDkp::vertexArray);
			//		glNormalPointer(GL_FLOAT, 0, CDkp::normalArray);
			//		glTexCoordPointer(2, GL_FLOAT, 0, CDkp::texCoordArray);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
			//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			//	glDisableClientState(GL_NORMAL_ARRAY);
			//	glDisableClientState(GL_VERTEX_ARRAY);

				glBindTexture(GL_TEXTURE_2D, textureArray[nextFrame]);
				glColor4fv((curColor*CColor4f(1,1,1,midFrame)).s);

				//--- Render here
		//		glCallList(CDkp::dpSprite);
		//		glEnableClientState(GL_VERTEX_ARRAY);
		//		glEnableClientState(GL_NORMAL_ARRAY);
		//		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//			glVertexPointer(3, GL_FLOAT, 0, CDkp::vertexArray);
		//			glNormalPointer(GL_FLOAT, 0, CDkp::normalArray);
		//			glTexCoordPointer(2, GL_FLOAT, 0, CDkp::texCoordArray);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		//		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//		glDisableClientState(GL_NORMAL_ARRAY);
		//		glDisableClientState(GL_VERTEX_ARRAY);
		}
		else 
		{
			if (CDkp::lastTexture != texture) glBindTexture(GL_TEXTURE_2D, texture);
			CDkp::lastTexture = texture;
			glColor4fv(curColor.s);
			glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_LIGHTING);
				if (dstBlend == DKP_ONE || dstBlend == DKP_ZERO) 
				{
					glDisable(GL_FOG);
					glDisable(GL_LIGHTING);
				}
	//			glCallList(CDkp::dpSprite);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
			/*	glBegin(GL_TRIANGLE_FAN);
					glTexCoord2f(.5f, .5f);
					glNormal3f(0,0,1);
					glVertex3f(0,0,0);

					glTexCoord2f(0, 1);
					glNormal3f(-1,1,0);
					glVertex3f(-1,1,0);

					glTexCoord2f(0, 0);
					glNormal3f(-1,-1,0);
					glVertex3f(-1,-1,0);

					glTexCoord2f(1, 0);
					glNormal3f(1,-1,0);
					glVertex3f(1,-1,0);

					glTexCoord2f(1, 1);
					glNormal3f(1,1,0);
					glVertex3f(1,1,0);

					glTexCoord2f(0, 1);
					glNormal3f(-1,1,0);
					glVertex3f(-1,1,0);
				glEnd();*/
			glPopAttrib();
		}
	glPopMatrix();
#endif
}