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

#ifndef CDKOMESH_H
#define CDKOMESH_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "platform_types.h"

#include "eHierarchic.h"

#include "CdkoMaterial.h"
class CDkoModel;


// On a ça pour chaque frame
struct _typMeshAtFrame
{
	float* vertexArray;
	float* normalArray;
	float* texCoordArray;

	_typMeshAtFrame()
	{
		vertexArray = 0;
		normalArray = 0;
		texCoordArray = 0;
	}
	virtual ~_typMeshAtFrame()
	{
		if (vertexArray) delete [] vertexArray;
		if (normalArray) delete [] normalArray;
		if (texCoordArray) delete [] texCoordArray;
	}
};


class _typMatGroup
{
public:
	// Le nb de vertex
	INT4 nbVertex;

	// Les array
	_typMeshAtFrame *meshAtFrame;
//	float* vertexArray;
//	float* texCoordArray;
	bool animatedUV;
//	float* normalArray;
	float* colorArray; // Pour tenir le tangent space

	float* interpolatedVA;
	float* interpolatedNA;
	float* interpolatedUV;
	float* ptrVA;
	float* ptrNA;
	float* ptrUV;


	CDkoModel* parentModel;

	// Le pointeur vers son material
	CdkoMaterial *material;

	// Constructeur / Destructeur
	_typMatGroup()
	{
	//	vertexArray = 0;
	//	texCoordArray = 0;
	//	normalArray = 0;
		colorArray = 0;
		material = 0;
		interpolatedVA=0;
		interpolatedNA=0;
		interpolatedUV=0;
		ptrVA=0;
		ptrNA=0;
		ptrUV=0;
		animatedUV = false;
		meshAtFrame = 0;
	}

	virtual ~_typMatGroup()
	{
	//	if (vertexArray) delete [] vertexArray;
	//	if (texCoordArray) delete [] texCoordArray;
	//	if (normalArray) delete [] normalArray;
		if (colorArray) delete [] colorArray;
	//	if (material) delete material;
		if (interpolatedVA) delete [] interpolatedVA;
		if (interpolatedNA) delete [] interpolatedNA;
		if (interpolatedUV) delete [] interpolatedUV;
		if (meshAtFrame) delete [] meshAtFrame;
	}
};



class CdkoMesh : public eHierarchic
{
public:
	// Son nom
	char *name;

	// Le nb de material group
	short nbMatGroup;

	CDkoModel* parentModel;

	// Son array de material group
	_typMatGroup *matGroupArray;

public:
	// Constructeur / Destructeur
	CdkoMesh(); virtual ~CdkoMesh();

	// pour loader du fichier
	int loadFromFile(FILE *ficIn, char *path);

	// pour loader un matgroup
	int loadMatGroup(FILE *ficIn, _typMatGroup *matGroup);

	// Pour le dessiner
	void drawIt();
	void drawBumpFull(int i);

	// On construit la liste des faces
	int _buildFaceListIt(CFace *faceArray, int index);
	int _buildVertexArrayIt(float * vertexArray, int index);
};



#endif
