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

#ifndef CDKOMATERIAL_H
#define CDKOMATERIAL_H


#include "ePTexture.h"


class CdkoMaterial
{
public:
	// Son nom
	char *matName;

	// Sa texture dkt
	ePTexture *textureMat;

	// Sinon on a les unsigned int directement ;)
	unsigned int texDiffuse;
	unsigned int texBump;
	unsigned int texSpecular;
	unsigned int texSelfIll;

/*	const short CHUNK_DKO_TEX_DIFFUSE = 0x02C0; // char*
	const short CHUNK_DKO_TEX_BUMP = 0x02D0; // char*
	const short CHUNK_DKO_TEX_SPECULAR = 0x02E0; // char*
	const short CHUNK_DKO_TEX_SELFILL = 0x02F0; // char*
*/
	// Les couleurs
	float ambient[4];
 	float diffuse[4];
	float specular[4];
	float emissive[4];

	// Le shininess
	short shininess;

	// La transparence
	float transparency;

	// Si le mat on voit des deux côté
	bool twoSided;

	// Son wireframe
	bool wire;
	float wireSize;

public:
	// Constructeur / Destructeur
	CdkoMaterial(); virtual ~CdkoMaterial();

	// On set les pass
	void setDiffusePass();
	void setSpecularPass();
	void setSelfIllPass();
	void setDetailPass();

	// Pour loader le materiel
	int loadFromFile(FILE *ficIn, char *path);
};


#endif