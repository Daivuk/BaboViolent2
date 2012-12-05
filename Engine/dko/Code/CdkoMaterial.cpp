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

#include "CdkoMaterial.h"
#include "dkoInner.h"



//
// Constructeur / Destructeur
//
CdkoMaterial::CdkoMaterial()
{
	textureMat = 0;
	matName = 0;
	shininess = 0;
	transparency = 0;
	twoSided = false;
	wire = false;
	wireSize = 1;
	texDiffuse =0;
	texBump =0;
	texSpecular =0;
	texSelfIll =0;

	for (int i=0;i<3;i++)
	{
		ambient[i]=0;
 		diffuse[i]=1;
		specular[i]=0;
		emissive[i]=0;
	}
	ambient[3]=1;
 	diffuse[3]=1;
	specular[3]=1;
	emissive[3]=1;
}

CdkoMaterial::~CdkoMaterial()
{
	if (textureMat) delete textureMat;
	if (matName) delete matName;

#ifndef CONSOLE
	dktDeleteTexture(&texDiffuse);
	dktDeleteTexture(&texBump);
	dktDeleteTexture(&texSpecular);
	dktDeleteTexture(&texSelfIll);
#endif
}



//
// Les pass pour les différentes couches du rendu
//
void CdkoMaterial::setDiffusePass()
{
#ifndef CONSOLE
#ifndef _DX_
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient); // L'ambient
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse); // La couleur diffuse
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular); // La couleur et intensité spécular
	glMaterialfv(GL_FRONT,GL_EMISSION,emissive); // Le seft illumination
	glMateriali(GL_FRONT,GL_SHININESS, shininess); // L'intensité du spécular : reflet de lumière

	glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if ((textureMat || texDiffuse) && CDko::renderStateBitField & DKO_TEXTURE_MAP)
	{
		glEnable(GL_TEXTURE_2D);
		if (textureMat)
		{
			glBindTexture(GL_TEXTURE_2D, textureMat->baseMap.textureID);
			glScalef(1/textureMat->baseMap.scale,1/textureMat->baseMap.scale,1);
		}
		else if (texDiffuse) glBindTexture(GL_TEXTURE_2D, texDiffuse);
		glColor3f(1,1,1); // La texture remplace la diffuse
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glColor4fv(diffuse);
	}


	// Si on force le clamping
	if (CDko::renderStateBitField & DKO_CLAMP_TEXTURE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}


	if (twoSided) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
	if (wire || CDko::renderStateBitField & DKO_FORCE_WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(wireSize);
	}
#endif
#endif
}

void CdkoMaterial::setDetailPass()
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureMat->detailMap.textureID);
		glScalef(1/textureMat->detailMap.scale,1/textureMat->detailMap.scale,1);
	glColor3f(1,1,1); // La texture remplace la diffuse
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR,GL_ZERO);

	if (twoSided) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
	if (wire || CDko::renderStateBitField & DKO_FORCE_WIREFRAME) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(wireSize);
	}

//	glDepthMask(GL_TRUE);
#endif
#endif
}

void CdkoMaterial::setSpecularPass()
{
#ifndef CONSOLE
#ifndef _DX_
	float zero[] = {0,0,0,1};
	glMaterialfv(GL_FRONT,GL_AMBIENT,zero); // L'ambient
	glMaterialfv(GL_FRONT,GL_DIFFUSE,zero); // La couleur diffuse
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular); // La couleur et intensité spécular
	glMaterialfv(GL_FRONT,GL_EMISSION,zero); // Le seft illumination
	glMateriali(GL_FRONT,GL_SHININESS, shininess); // L'intensité du spécular : reflet de lumière

	glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if ((textureMat || texSpecular) && CDko::renderStateBitField & DKO_SPECULAR_MAP)
	{
	//	float one[] = {1,1,1,1};
	//	glMaterialfv(GL_FRONT,GL_SPECULAR,one); // La texture remplace la couleur spéculaire
		glEnable(GL_TEXTURE_2D);
		if (textureMat)
		{
			glBindTexture(GL_TEXTURE_2D, textureMat->specularMap.textureID);
			glScalef(1/textureMat->specularMap.scale,1/textureMat->specularMap.scale,1);
		}
		else if (texSpecular) glBindTexture(GL_TEXTURE_2D, texSpecular);
		glColor3f(1,1,1); // La texture remplace la diffuse
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glColor4fv(specular); // La couleur du specular
	}
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE);

	if (twoSided) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
	if (wire || CDko::renderStateBitField & DKO_FORCE_WIREFRAME) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(wireSize);
	}

//	glDepthMask(GL_TRUE);
#endif
#endif
}

void CdkoMaterial::setSelfIllPass()
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if (textureMat)
	{
		glBindTexture(GL_TEXTURE_2D, textureMat->selfIlluminationMap.textureID);
		glScalef(1/textureMat->selfIlluminationMap.scale,1/textureMat->selfIlluminationMap.scale,1);
	}
	else if (texSelfIll) glBindTexture(GL_TEXTURE_2D, texSelfIll);
	glColor3f(1,1,1); // La texture remplace la diffuse
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	if (twoSided) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
	if (wire || CDko::renderStateBitField & DKO_FORCE_WIREFRAME) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(wireSize);
	}

//	glDepthMask(GL_TRUE);
#endif
#endif
}



//
// Pour loader le materiel
//
int CdkoMaterial::loadFromFile(FILE *ficIn, char *path)
{
	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_MATNAME:
			{
				matName = readString(ficIn);
				break;
			}
		case CHUNK_DKO_TEX_DKT:
			{
				char *dktFilename = readString(ficIn);
				char strTMP[512];
				sprintf(strTMP, "%s%s", path, dktFilename);
				textureMat = new ePTexture();
				textureMat->loadIt(strTMP);
				delete [] dktFilename;
				break;
			}
		case CHUNK_DKO_TEX_DIFFUSE:
			{
				char *texFilename = readString(ficIn);
				char strTMP[512];
				sprintf(strTMP, "%s%s", path, texFilename);
#ifndef CONSOLE
				texDiffuse = dktCreateTextureFromFile(strTMP, DKT_FILTER_BILINEAR);
#endif
				delete [] texFilename;
				break;
			}
		case CHUNK_DKO_TEX_BUMP:
			{
				char *texFilename = readString(ficIn);
				char strTMP[512];
				sprintf(strTMP, "%s%s", path, texFilename);
#ifndef CONSOLE
				texBump = dktCreateTextureFromFile(strTMP, DKT_FILTER_BILINEAR);
#endif
				delete [] texFilename;
				break;
			}
		case CHUNK_DKO_TEX_SPECULAR:
			{
				char *texFilename = readString(ficIn);
				char strTMP[512];
				sprintf(strTMP, "%s%s", path, texFilename);
#ifndef CONSOLE
				texSpecular = dktCreateTextureFromFile(strTMP, DKT_FILTER_BILINEAR);
#endif
				delete [] texFilename;
				break;
			}
		case CHUNK_DKO_TEX_SELFILL:
			{
				char *texFilename = readString(ficIn);
				char strTMP[512];
				sprintf(strTMP, "%s%s", path, texFilename);
#ifndef CONSOLE
				texSelfIll = dktCreateTextureFromFile(strTMP, DKT_FILTER_BILINEAR);
#endif
				delete [] texFilename;
				break;
			}
		case CHUNK_DKO_AMBIENT:
			{
				fread(ambient, 4, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_DIFFUSE:
			{
				fread(diffuse, 4, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_SPECULAR:
			{
				fread(specular, 4, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_EMISSIVE:
			{
				fread(emissive, 4, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_SHININESS:
			{
				fread(&shininess, 1, sizeof(short), ficIn);
				break;
			}
		case CHUNK_DKO_TRANSPARENCY:
			{
				fread(&transparency, 1, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_TWO_SIDED:
			{
				char temp;
				fread(&temp, 1, sizeof(char), ficIn);
				twoSided = (temp) ? true : false;
				break;
			}
		case CHUNK_DKO_WIRE_FRAME:
			{
				char temp;
				fread(&temp, 1, sizeof(char), ficIn);
				wire = (temp) ? true : false;
				break;
			}
		case CHUNK_DKO_WIRE_WIDTH:
			{
				fread(&wireSize, 1, sizeof(float), ficIn);
				break;
			}
		}

		chunkID = readChunk(ficIn);
	}

	// Tout est beau
	return 1;
}