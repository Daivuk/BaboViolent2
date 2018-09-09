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

#ifndef CDKOMODEL_H
#define CDKOMODEL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eHierarchic.h"

#include "CdkoMaterial.h"
#include "COctree.h"

//#include <vector>  // WTF CALISS JPEUX PAS L'INCLURE ÇA CHI DUR




const short DKO_VERSION = 0x0002;

const short CHUNK_DKO_VERSION = 0x0000; // short
const short CHUNK_DKO_TIME_INFO = 0x0001; // short[3]

const short CHUNK_DKO_PROPERTIES = 0x0100; // ...
	const short CHUNK_DKO_NAME = 0x0110; // char*
	const short CHUNK_DKO_POSITION = 0x0120; // float[3]
	const short CHUNK_DKO_MATRIX = 0x0130; // float[9]

const short CHUNK_DKO_MATLIST = 0x0200; // short  (nb de material)
	const short CHUNK_DKO_MATNAME = 0x0210; // char*
	const short CHUNK_DKO_TEX_DKT = 0x0220; // char*
	const short CHUNK_DKO_AMBIENT = 0x0230; // float[4]
	const short CHUNK_DKO_DIFFUSE = 0x0240; // float[4]
	const short CHUNK_DKO_SPECULAR = 0x0250; // float[4]
	const short CHUNK_DKO_EMISSIVE = 0x0260; // float[4]
	const short CHUNK_DKO_SHININESS = 0x0270; // short
	const short CHUNK_DKO_TRANSPARENCY = 0x0280; // float
	const short CHUNK_DKO_TWO_SIDED = 0x0290; // char
	const short CHUNK_DKO_WIRE_FRAME = 0x02A0; // char
	const short CHUNK_DKO_WIRE_WIDTH = 0x02B0; // float
	const short CHUNK_DKO_TEX_DIFFUSE = 0x02C0; // char*
	const short CHUNK_DKO_TEX_BUMP = 0x02D0; // char*
	const short CHUNK_DKO_TEX_SPECULAR = 0x02E0; // char*
	const short CHUNK_DKO_TEX_SELFILL = 0x02F0; // char*

const short CHUNK_DKO_TRI_MESH = 0x0300; // ...
//	const short CHUNK_DKO_NAME = 0x0110; // char*
//	const short CHUNK_DKO_POSITION = 0x0120; // float[3]
//	const short CHUNK_DKO_MATRIX = 0x0130; // float[9]
	const short CHUNK_DKO_NB_MAT_GROUP = 0x0340; // short
		const short CHUNK_DKO_MAT_ID = 0x0341; // short
		const short CHUNK_DKO_NB_VERTEX = 0x0342; // long
		const short CHUNK_DKO_VERTEX_ARRAY = 0x0343; // float* x NbFrame
		const short CHUNK_DKO_NORMAL_ARRAY = 0x0344; // float* x NbFrame
		const short CHUNK_DKO_TEXCOORD_ARRAY = 0x0345; // float*
		const short CHUNK_DKO_TEXCOORD_ARRAY_ANIM = 0x0346; // float* x NbFrame

const short CHUNK_DKO_DUMMY = 0x0400;
//	const short CHUNK_DKO_NAME = 0x0110; // char*
//	const short CHUNK_DKO_POSITION = 0x0120; // float[3]
//	const short CHUNK_DKO_MATRIX = 0x0130; // float[9]

const short CHUNK_DKO_END = 0x0900; // ...




struct _typPos
{
	float v[3];
};
struct _typMat
{
	float m[9];
};
struct _typDummy
{
	// Son nom
	char *name;
	_typPos *position;
	_typMat *matrix;

	_typDummy(int duration)
	{
		name=0;
		position=new _typPos[duration];
		matrix=new _typMat[duration];
	}
	virtual ~_typDummy()
	{
		if (name) delete [] name;
		if (position) delete [] position;
		if (matrix) delete [] matrix;
	}
};



class CDkoModel : public eHierarchic
{
public:
	// Son nom
	char *name;

	// Le nb de material
	short nbMat;

	// La liste des materiels
	CdkoMaterial *materialArray;

	// Son radius
	float radius;

	// Son bounding box
	float OABB[6];
	float min[3];
	float max[3];
	bool firstVertex;

	// Le array de ses faces (pour les collision)
	int nbFace;
	CFace *faceArray;

	// Son octree pour les collisions
	COctreeNode *octree;

	// L'animation courante
	short timeInfo[3];
	short currentFrame;
	float framef;

	// ses dummies
	int dumSize;
	_typDummy* dummies[64]; // On a un max de 64 dummy là

public:
	// Constructeur / Destructeur
	CDkoModel(); virtual ~CDkoModel();

	// pour loader du fichier
	int loadFromFile(FILE *ficIn, char *path);
	int loadFromFile3DS(FILE *ficIn, char *path);

	// Pour loader ses propriétées
	void loadProperties(FILE *ficIn);

	// Pour loader un dummy
	void loadDummy(FILE *ficIn);

	// On va créer sa facelist
	void buildFaceList();
	void buildVertexArray(float * vertexArray);

	// On va créer un octree à partir de la facelist
	void buildOctree();

	// Pour loader une nouvelle animation dans ce fichier
//	int addAnimationFromFile(unsigned int modelID, char* filename, char* animationName);

	// Pour les truc 3ds
//	void readMainBlockAnim(FILE* fic3DS, typChunk &previousChunk, char* filename);
};



// pour lire un chunk
short readChunk(FILE *ficIn);

// Pour lire un string
char *readString(FILE *ficIn);


#endif