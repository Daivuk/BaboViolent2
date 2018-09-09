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

#ifndef EHIERARCHIC_H
#define EHIERARCHIC_H

#include "CFace.h"


class eHierarchic
{
public:
	// Ici la liste de ses enfant
	eHierarchic* childList;

	// Un pointeur vers le suivant
	eHierarchic* next;

	// Un pointeur vers son parent (il ne peut y en avoir qu'un)
	eHierarchic* parent;

	// Si il est enable ou pas
	bool enable;

	// Sa position
	float position[3];  // Pas utile mais bon

	// sa matrice
	float matrix[9];  // Pas utile non plus mais bon

public:
	// Constructeur / Destructeur
	eHierarchic(); virtual ~eHierarchic();

	// On ajoute un enfant
	void addChild(eHierarchic* child);

	// On enlève un enfant
	void removeChild(eHierarchic* child);

	// On détach du parent
	void detach();

	// Pour le dessiner
	virtual void drawIt() {}
	void drawAll();

	// Pour les animer
	virtual void doIt() {}
	void doAll();

	// Un set enable
	void setEnable(bool value);

	// On construit la liste des faces
	int _buildFaceList(CFace *faceArray, int index);
	virtual int _buildFaceListIt(CFace *faceArray, int index) { (void)faceArray; (void)index; return 0; }
	int _buildVertexArray(float *vertexArray, int index);
	virtual int _buildVertexArrayIt(float * vertexArray, int index) { (void)vertexArray; (void)index; return 0; }
};



#endif
