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

#include "eHierarchic.h"
#ifdef WIN32
#ifndef DEDICATED_SERVER
#ifndef DEDICATED_SERVER
	#include <windows.h>
#ifndef _DX_
	#include <gl/gl.h>
#endif
#endif
#endif
#else
#include "LinuxHeader.h"
#ifdef __MACOSX__
#include <SDL_Opengl.h>
#else
#include <GL/gl.h>
#endif
#endif



//
// Constructeur / Destructeur
//
eHierarchic::eHierarchic()
{
	next = 0;
	childList = 0;
	parent = 0;
	enable = true;
	position[0] = 0;
	position[1] = 0;
	position[2] = 0;
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 1;
	matrix[5] = 0;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 1;
}
eHierarchic::~eHierarchic()
{
	// On efface aussi tout ses enfants
	eHierarchic* ToKill;
	for (eHierarchic* ptrObject = childList; ptrObject; delete ToKill)
	{
		ToKill = ptrObject;
		ptrObject = ptrObject->next;
	}
}



//
// Pour ajouter un enfant
//
void eHierarchic::addChild(eHierarchic* child)
{
	// On le d�ache d'abords de son parent
	if (child->parent) child->detach();

	// Maintenant on l'ajoute �la fin de la liste
	if (!childList)
	{
		child->next = childList;
		childList = child;
	}
	else
	{
		for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
		{
			if (!ptrObject->next)
			{
				ptrObject->next = child;
				break;
			}
		}
	}

	child->parent = this;
}



//
// Pour setter si il est enable ou non
//
void eHierarchic::setEnable(bool value)
{
	// On le set lui en premier
	enable = value;

	// Ensuite ses childs
	for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
		ptrObject->setEnable(value);
}



//
// Pour le d�acher du parent
//
void eHierarchic::detach()
{
	if (parent)
	{
		if (this == parent->childList)
		{
			parent->childList = next;
		}
		else
		{
			for (eHierarchic* ptrObject = parent->childList; ptrObject; ptrObject = ptrObject->next)
			{
				if (ptrObject->next == this)// BUG: it was '=' instead of '=='.
				{
					ptrObject->next = next;
				}
			}
		}
		parent = 0;
		next = 0;
	}
}



//
// Pour retirer une enfant de sa liste
//
void eHierarchic::removeChild(eHierarchic* child)
{
	if (child)
	{
		child->detach();
	}
}



//
// Pour les dessiner tous
//
void eHierarchic::drawAll()
{
	if (enable) 
	{
		// On fait son push pop
#ifndef DEDICATED_SERVER
#ifndef _DX_
		glPushMatrix();
			glTranslatef(position[0], position[1], position[2]);
			float Matrix[16] = {
				matrix[0], matrix[1], matrix[2], 0,
				matrix[3], matrix[4], matrix[5], 0,
				matrix[6], matrix[7], matrix[8], 0,
				0,          0,          0,          1};
			glMultMatrixf(Matrix);
#endif
#endif

			// On le dessine lui en premier
			drawIt();

			// Ensuite ses childs
			for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
				ptrObject->drawAll(); // hum pas vraiment d'hierarchi �l'interieur d'un dko mais bon
#ifndef DEDICATED_SERVER
#ifndef _DX_
		glPopMatrix();
#endif
#endif
	}
}



//
// Pour les effectuer tous
//
void eHierarchic::doAll()
{
	if (enable)
	{
		// Les childs en premier
		for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
			ptrObject->doAll();

		// Le parent en dernier
		doIt();
	}
}



//
// On trouve toute les faces
//
int eHierarchic::_buildFaceList(CFace *faceArray, int index)
{
	int nbAdded = _buildFaceListIt(faceArray, index);

	if (enable)
	{
		// Les childs en premier
		for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
		{
			nbAdded += ptrObject->_buildFaceList(faceArray, index + nbAdded);
		}
	}

	return nbAdded;
}



//
// On rempli son array de vertex array
//
int eHierarchic::_buildVertexArray(float *vertexArray, int index)
{
	int nbAdded = _buildVertexArrayIt(vertexArray, index);

	if (enable)
	{
		// Les childs en premier
		for (eHierarchic* ptrObject = childList; ptrObject; ptrObject = ptrObject->next)
		{
			nbAdded += ptrObject->_buildVertexArray(vertexArray, index + nbAdded);
		}
	}

	return nbAdded;
}
