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

#include "dkoInner.h"
#include "CdkoMesh.h"

#ifndef WIN32
	#include "LinuxHeader.h"
#endif
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

//
// Les trucs static
//
char *CDko::lastErrorString = 0;
CDkoModel *CDko::modelArray[MAX_MODEL];
bool CDko::inited = false;
unsigned int CDko::renderStateBitField = 0;
_typBitFieldPile *CDko::bitFieldPile = 0;
INT4 CDko::globalFrameID = 0;



//
// On update la derni�e erreur
//
void CDko::updateLastError(char *error)
{
	if (lastErrorString) delete [] lastErrorString;
	lastErrorString = new char [strlen(error)+1];
	strcpy(lastErrorString, error);
}



//
// Pour ajouter une animation �cet objet �partir d'un fichier 3ds
//
int			dkoAddAnimationFromFile(unsigned int modelID, char* filename, char* animationName)
{
	(void)modelID; (void)filename; (void)animationName;
	return 0;
	/*
	if (CDko::modelArray[modelID])
	{
		return CDko::modelArray[modelID]->addAnimationFromFile(modelID, filename, animationName);
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return 0;
	}*/
}



//
// Pour ajouter une lumi�e �un model
//
void			dkoAddLight(unsigned int modelID, float *position, float *diffuse, float *specular, float range)
{
	(void)modelID; (void)position; (void)diffuse; (void)specular; (void)range;
}



//
// Pour construire l'arbre octal pour g�er rapidement les collisions avec ses faces
//
void			dkoBuildOctree(unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		if (!CDko::modelArray[modelID]->faceArray)
		{
			CDko::modelArray[modelID]->buildFaceList();
		}
		if (!CDko::modelArray[modelID]->octree && CDko::modelArray[modelID]->nbFace)
		{
			CDko::modelArray[modelID]->buildOctree();
		}
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}



//
// Calculer le color array pour le static lighting
//
void			dkoComputeStaticLighting(unsigned int modelID)
{
	(void)modelID;
}



//
// Enabeler un flag
//
void			dkoEnable(unsigned int renderFlags)
{
	CDko::renderStateBitField |= renderFlags;
}



//
// Pour �facer un model de la ram
//
void			dkoDeleteModel(unsigned int *modelID)
{
	if (CDko::modelArray[*modelID])
	{
		delete CDko::modelArray[*modelID];
		CDko::modelArray[*modelID] = 0;
	}

	*modelID = 0;
}



//
// Disabeler un flag
//
void			dkoDisable(unsigned int renderFlags)
{
	CDko::renderStateBitField &= ~renderFlags;
}



//
// Pour obtenir les infos sur un dummy
//
unsigned int	dkoGetDummy(char * dummyName, unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			_typDummy * dummy = CDko::modelArray[modelID]->dummies[i];
			if (stricmp(dummy->name, dummyName) == 0)
			{
				return i+1;
			}
		}
		CDko::updateLastError("Invalide dummy name");
		return 0;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return 0;
	}
}

void			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat)
{
	if (CDko::modelArray[modelID])
	{
		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			_typDummy * dummy = CDko::modelArray[modelID]->dummies[i];
			if (stricmp(dummy->name, dummyName) == 0)
			{
				for (int j=0;j<9;j++) mat[j] = dummy->matrix[0].m[j];
				return;
			}
		}
		CDko::updateLastError("Invalide dummy name");
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat)
{
	if (CDko::modelArray[modelID])
	{
		if (dummyID == 0)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		dummyID--;
		if ((int)dummyID >= CDko::modelArray[modelID]->dumSize)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		_typDummy * dummy = CDko::modelArray[modelID]->dummies[dummyID];
		for (int j=0;j<9;j++) mat[j] = dummy->matrix[0].m[j];
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat, short frameID)
{
	if (CDko::modelArray[modelID])
	{
		if (frameID < 0) frameID = 0;
		if (frameID >= CDko::modelArray[modelID]->timeInfo[2]) frameID = frameID % (CDko::modelArray[modelID]->timeInfo[2]);
		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			_typDummy * dummy = CDko::modelArray[modelID]->dummies[i];
			if (stricmp(dummy->name, dummyName) == 0)
			{
				for (int j=0;j<9;j++) mat[j] = dummy->matrix[frameID].m[j];
				return;
			}
		}
		CDko::updateLastError("Invalide dummy name");
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat, short frameID)
{
	if (CDko::modelArray[modelID])
	{
		if (frameID < 0) frameID = 0;
		if (frameID >= CDko::modelArray[modelID]->timeInfo[2]) frameID = frameID % (CDko::modelArray[modelID]->timeInfo[2]);
		if (dummyID == 0)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		dummyID--;
		if ((int)dummyID >= CDko::modelArray[modelID]->dumSize)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		_typDummy * dummy = CDko::modelArray[modelID]->dummies[dummyID];
		for (int j=0;j<9;j++) mat[j] = dummy->matrix[frameID].m[j];
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos)
{
	if (CDko::modelArray[modelID])
	{
		if (CDko::modelArray[modelID]->dumSize == 0)
		{
			CDko::updateLastError("There is no dummy");
			return;
		}
		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			_typDummy * dummy = CDko::modelArray[modelID]->dummies[i];
			if (stricmp(dummy->name, dummyName) == 0)
			{
				for (int j=0;j<3;j++) pos[j] = dummy->position[0].v[j];
				return;
			}
		}
		CDko::updateLastError("Invalide dummy name");
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos)
{
	if (CDko::modelArray[modelID])
	{
		if (dummyID == 0)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		dummyID--;
		if ((int)dummyID >= CDko::modelArray[modelID]->dumSize)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		_typDummy * dummy = CDko::modelArray[modelID]->dummies[dummyID];
		for (int j=0;j<3;j++) pos[j] = dummy->position[0].v[j];
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos, short frameID)
{
	if (CDko::modelArray[modelID])
	{
		if (frameID < 0) frameID = 0;
		if (frameID >= CDko::modelArray[modelID]->timeInfo[2]) frameID = frameID % (CDko::modelArray[modelID]->timeInfo[2]);
		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			_typDummy * dummy = CDko::modelArray[modelID]->dummies[i];
			if (stricmp(dummy->name, dummyName) == 0)
			{
				for (int j=0;j<3;j++) pos[j] = dummy->position[frameID].v[j];
				return;
			}
		}
		CDko::updateLastError("Invalide dummy name");
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}

void			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos, short frameID)
{
	if (CDko::modelArray[modelID])
	{
		if (frameID < 0) frameID = 0;
		if (frameID >= CDko::modelArray[modelID]->timeInfo[2]) frameID = frameID % (CDko::modelArray[modelID]->timeInfo[2]);
		if (dummyID == 0)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		dummyID--;
		if ((int)dummyID >= CDko::modelArray[modelID]->dumSize)
		{
			CDko::updateLastError("Invalide dummy ID");
			return;
		}
		_typDummy * dummy = CDko::modelArray[modelID]->dummies[dummyID];
		for (int j=0;j<3;j++) pos[j] = dummy->position[frameID].v[j];
		return;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}



//
// Obtenir le nom d'un dummy (Permet aussi de savoir si ce dummy existe
//
char*			dkoGetDummyName(unsigned int dummyID, unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		if ((int)dummyID >= CDko::modelArray[modelID]->dumSize)
		{
			CDko::updateLastError("Invalide dummy ID");
			return 0;
		}
		_typDummy * dummy = CDko::modelArray[modelID]->dummies[dummyID];
		return dummy->name;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return 0;
	}
}



//
// Obtenir la derni�e �reur
//
char*			dkoGetLastError()
{
	return CDko::lastErrorString;
}



//
// Pour obtenir le nb de vertex total sur un model
//
int				dkoGetNbVertex(unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		if (!CDko::modelArray[modelID]->faceArray)
		{
			CDko::modelArray[modelID]->buildFaceList();
		}

		return CDko::modelArray[modelID]->nbFace * 3;
	}

	return 0;
}



//
// On obtiens le object aligned bounding box du model
//
void			dkoGetOABB(unsigned int modelID, float *OABB)
{
	if (CDko::modelArray[modelID])
	{
		for (int i=0;i<6;i++) OABB[i] = CDko::modelArray[modelID]->OABB[i];
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return;
	}
}



//
// On obtien le radius du model
//
float			dkoGetRadius(unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		return CDko::modelArray[modelID]->radius;
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return -1;
	}
}



//
// Obtenir le nb de frame dans une animation
//
short			dkoGetTotalFrame(unsigned int modelID)
{
	if (CDko::modelArray[modelID])
	{
		return CDko::modelArray[modelID]->timeInfo[2];
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return -1;
	}
}



//
// Pour obtenir le mesh interpol��un certain frame
//
float*			dkoGetVertexArray(unsigned int modelID, float frameID, int & nbVertex, float *vertexArray)
{
//	float * vertexArray = 0;

	if (CDko::modelArray[modelID])
	{
		if (!CDko::modelArray[modelID]->faceArray)
		{
			CDko::modelArray[modelID]->buildFaceList();
		}
	}

	if (CDko::modelArray[modelID]) 
	{
		CDko::modelArray[modelID]->framef = frameID;
		if (CDko::modelArray[modelID]->framef > CDko::modelArray[modelID]->timeInfo[2]-1)
			CDko::modelArray[modelID]->framef = -1;
		if (CDko::modelArray[modelID]->framef <= 0)
			CDko::modelArray[modelID]->framef = -1;
		CDko::modelArray[modelID]->currentFrame = short((int)frameID % (CDko::modelArray[modelID]->timeInfo[2]));

		nbVertex = CDko::modelArray[modelID]->nbFace * 3;
	//	vertexArray = new float [nbVertex * 3];
		CDko::modelArray[modelID]->buildVertexArray(vertexArray);
	}

	return vertexArray; // L'utilisateur devra penser �l'effacer lui m�e
}



//
// Initialiser le engine dko
//
void			dkoInit()
{
	CDko::lastErrorString = 0;

	for (int i=0;i<MAX_MODEL;i++) CDko::modelArray[i] = 0;

	CDko::inited = true;

	CDko::renderStateBitField = 0;
	dkoEnable(
		DKO_TEXTURE_MAP | 
		DKO_DETAIL_MAP | 
		DKO_BUMP_MAP | 
		DKO_SPECULAR_MAP | 
		DKO_SELFILL_MAP |
		DKO_DYNAMIC_LIGHTING |
		DKO_SPECULAR |
		DKO_MULTIPASS);

	CDko::bitFieldPile = 0;
#ifndef CONSOLE
	// Init les textures
	dktInit();
#endif
}



//
// Initialiser et effacer la liste des lumi�es d'un objet
//
void			dkoInitLightList(unsigned int modelID)
{
	(void)modelID;
}



//
// Loader un fichier .dko ou .3ds
//
unsigned int	dkoLoadFile(char* filename)
{

	FILE *ficIn = fopen(filename, "rb");

	// On check la validit�du fichier
	if (!ficIn)
	{
		CDko::updateLastError("Invalide filename");
		return 0;
	}

	// On une case dans le array vite avant tout
	unsigned int ID;
	for (ID=1;ID<MAX_MODEL;ID++)
	{
		if (!CDko::modelArray[ID]) break;
	}

	// Oups, il n'y a plus de place
	if (ID == MAX_MODEL)
	{
		CDko::updateLastError("Max number of models reached : 1024");
		fclose(ficIn);
		return 0;
	}

	// Parfait, on cr�un nouveau Model dko ici
	CDko::modelArray[ID] = new CDkoModel();

	// On efface le string jusqu'au /
	size_t len = strlen(filename);
	size_t i;
	for (i = len;filename[i]!='/' && filename[i]!='\\' && i>0;i--);
	i++;
	char *path = new char [i+1];
	strncpy(path, filename, i);
	path[i] = '\0';

	// On le load du fichier
	bool _3dsFile = (stricmp(&(filename[strlen(filename)-4]), "3ds") == 0);
	if (_3dsFile)
	{
		if (!CDko::modelArray[ID]->loadFromFile3DS(ficIn, path))
		{
			delete [] path;
			fclose(ficIn);
			return 0;
		}
	}
	else
	{
		if (!CDko::modelArray[ID]->loadFromFile(ficIn, path))
		{
			delete [] path;
			fclose(ficIn);
			return 0;
		}
	}

	// On efface les cochoneries
	delete [] path;

	// On a fini dle loader, on close et retourne son ID
	fclose(ficIn);
	return ID;
}



//
// Instancier un model �partir d'un autre
//
unsigned int	dkoLoadFile(unsigned int modelID)
{
	return modelID;
}



//
// Pour savoir ce que contient un model DKO
//
void			dkoOutputDebugInfo(unsigned int modelID, char *filename)
{
	if (CDko::modelArray[modelID])
	{
		FILE *ficout = fopen(filename, "w");
		if (!ficout) return;

		for (eHierarchic* ptrObject = CDko::modelArray[modelID]->childList; ptrObject; ptrObject = ptrObject->next)
		{
			CdkoMesh * mesh = (CdkoMesh*)ptrObject;
			fprintf(ficout, "Mesh  : %s\n", mesh->name);
		}

		for (int i=0;i<CDko::modelArray[modelID]->dumSize;i++)
		{
			fprintf(ficout, "Dummy : %s\n", CDko::modelArray[modelID]->dummies[i]->name);
		}

		fclose(ficout);
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
	}
}



//
// Poper le bitfield
//
void			dkoPopRenderState()
{
	if (CDko::bitFieldPile)
	{
		_typBitFieldPile* old = CDko::bitFieldPile;
		CDko::renderStateBitField = old->bitField;
		CDko::bitFieldPile = old->previous;
		delete old;
	}
}



//
// Pusher le bitfield
//
void			dkoPushRenderState()
{
	CDko::bitFieldPile = new _typBitFieldPile(CDko::bitFieldPile, CDko::renderStateBitField);
}



//
// Pour performer une d�ection d'intersection avec un ray
//
bool			dkoRayIntersection(unsigned int modelID, float *mp1, float *mp2, float *intersect, float *normal, int &n)
{
	if (CDko::modelArray[modelID]) 
	{
		if (CDko::modelArray[modelID]->octree)
		{
			CVector mintersection;
			CVector mnormal;
			CVector p1(mp1);
			CVector p2(mp2);
			float dis = distanceFast(p1, p2);

			// Voil�on est pret, on test � dans le octree
			CDko::globalFrameID++;
			if (CDko::modelArray[modelID]->octree->findRayIntersection(p1, p2, dis, mintersection, mnormal, n))
			{
				intersect[0] = mintersection[0];
				intersect[1] = mintersection[1];
				intersect[2] = mintersection[2];
				normal[0] = mnormal[0];
				normal[1] = mnormal[1];
				normal[2] = mnormal[2];
				return true;
			}
			else
				return false;
		}
		else
		{
			CDko::updateLastError("Octree not built for that model");
			return false;
		}
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return false;
	}
}



//
// Rendre un model
//
void			dkoRender(unsigned int modelID)
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
		glEnable(GL_RESCALE_NORMAL);
		if (CDko::modelArray[modelID]) 
		{
			CDko::modelArray[modelID]->framef=-1;
			CDko::modelArray[modelID]->currentFrame = 0; // Le frame par default
			CDko::modelArray[modelID]->drawAll();

			if (CDko::modelArray[modelID]->faceArray)
			{
				if (CDko::renderStateBitField & DKO_RENDER_FACE)
				{
					glEnable(GL_CULL_FACE);

					// On dessine sa faceList pour le fun
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT, GL_LINE);
					glLineWidth(1);

					for (int i=0;i<CDko::modelArray[modelID]->nbFace;i++)
					{
						glColor3f(0,1,0);
						glBegin(GL_TRIANGLES);
							for (int j=0;j<3;j++)
							{
								glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[j].s);
							}
						glEnd();

						glColor3f(1,0,1);
						glBegin(GL_LINES);
							glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[0].s);
							glVertex3f(
								CDko::modelArray[modelID]->faceArray[i].point[0][0] + CDko::modelArray[modelID]->faceArray[i].a*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][1] + CDko::modelArray[modelID]->faceArray[i].b*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][2] + CDko::modelArray[modelID]->faceArray[i].c*10);
						glEnd();
					}
				}

				// � c TELLEMENT temporaire
				if (CDko::modelArray[modelID]->octree && CDko::renderStateBitField & DKO_RENDER_NODE) CDko::modelArray[modelID]->octree->render();
			}

			// On affiche sont BB
			if (CDko::renderStateBitField & DKO_RENDER_BB)
			{
				glPushAttrib(GL_POLYGON_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
					glDisable(GL_TEXTURE_2D);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_LIGHTING);
					glDisable(GL_CULL_FACE);
					glLineWidth(1);
					glColor3f(0,1,1);
					glPushMatrix();
						glTranslatef(CDko::modelArray[modelID]->OABB[0], CDko::modelArray[modelID]->OABB[1], CDko::modelArray[modelID]->OABB[2]);
						glScalef(CDko::modelArray[modelID]->OABB[3], CDko::modelArray[modelID]->OABB[4], CDko::modelArray[modelID]->OABB[5]);
						glBegin(GL_QUADS);
							glNormal3f(0, -1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, 1);

							glNormal3f(1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, 1, 1);

							glNormal3f(-1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, -1, 1);

							glNormal3f(0, 0, 1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, 1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 0, -1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, -1);
						glEnd();
					glPopMatrix();
				glPopAttrib();
			}
		}
	glPopAttrib();
#endif
#else
	(void)modelID;
#endif
}



//
// Dessiner le model sur un frame sp�ifique
//
void			dkoRender(unsigned int modelID, unsigned short frameID)
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
		glEnable(GL_RESCALE_NORMAL);
		if (CDko::modelArray[modelID]) 
		{
			CDko::modelArray[modelID]->framef=-1;
			CDko::modelArray[modelID]->currentFrame = frameID % (CDko::modelArray[modelID]->timeInfo[2]);
			CDko::modelArray[modelID]->drawAll();

			if (CDko::modelArray[modelID]->faceArray)
			{
				if (CDko::renderStateBitField & DKO_RENDER_FACE)
				{
					glEnable(GL_CULL_FACE);

					// On dessine sa faceList pour le fun
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT, GL_LINE);
					glLineWidth(1);

					for (int i=0;i<CDko::modelArray[modelID]->nbFace;i++)
					{
						glColor3f(0,1,0);
						glBegin(GL_TRIANGLES);
							for (int j=0;j<3;j++)
							{
								glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[j].s);
							}
						glEnd();

						glColor3f(1,0,1);
						glBegin(GL_LINES);
							glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[0].s);
							glVertex3f(
								CDko::modelArray[modelID]->faceArray[i].point[0][0] + CDko::modelArray[modelID]->faceArray[i].a*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][1] + CDko::modelArray[modelID]->faceArray[i].b*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][2] + CDko::modelArray[modelID]->faceArray[i].c*10);
						glEnd();
					}
				}

				// � c TELLEMENT temporaire
				if (CDko::modelArray[modelID]->octree && CDko::renderStateBitField & DKO_RENDER_NODE) CDko::modelArray[modelID]->octree->render();
			}

			// On affiche sont BB
			if (CDko::renderStateBitField & DKO_RENDER_BB)
			{
				glPushAttrib(GL_POLYGON_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
					glDisable(GL_TEXTURE_2D);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_LIGHTING);
					glDisable(GL_CULL_FACE);
					glLineWidth(1);
					glColor3f(0,1,1);
					glPushMatrix();
						glTranslatef(CDko::modelArray[modelID]->OABB[0], CDko::modelArray[modelID]->OABB[1], CDko::modelArray[modelID]->OABB[2]);
						glScalef(CDko::modelArray[modelID]->OABB[3], CDko::modelArray[modelID]->OABB[4], CDko::modelArray[modelID]->OABB[5]);
						glBegin(GL_QUADS);
							glNormal3f(0, -1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, 1);

							glNormal3f(1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, 1, 1);

							glNormal3f(-1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, -1, 1);

							glNormal3f(0, 0, 1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, 1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 0, -1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, -1);
						glEnd();
					glPopMatrix();
				glPopAttrib();
			}
		}
	glPopAttrib();
#endif
#else
	(void)modelID; (void)frameID;
#endif
}



//
// Dessiner un frame interpol�
//
void			dkoRender(unsigned int modelID, float frameID)
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
		glEnable(GL_RESCALE_NORMAL);
		if (CDko::modelArray[modelID]) 
		{
			CDko::modelArray[modelID]->framef = frameID;
			if (CDko::modelArray[modelID]->framef > CDko::modelArray[modelID]->timeInfo[2]-1)
				CDko::modelArray[modelID]->framef = -1;
			if (CDko::modelArray[modelID]->framef <= 0)
				CDko::modelArray[modelID]->framef = -1;
			CDko::modelArray[modelID]->currentFrame = short((int)frameID % (CDko::modelArray[modelID]->timeInfo[2]));
			CDko::modelArray[modelID]->drawAll();

			if (CDko::modelArray[modelID]->faceArray)
			{
				if (CDko::renderStateBitField & DKO_RENDER_FACE)
				{
					glEnable(GL_CULL_FACE);

					// On dessine sa faceList pour le fun
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT, GL_LINE);
					glLineWidth(1);

					for (int i=0;i<CDko::modelArray[modelID]->nbFace;i++)
					{
						glColor3f(0,1,0);
						glBegin(GL_TRIANGLES);
							for (int j=0;j<3;j++)
							{
								glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[j].s);
							}
						glEnd();

						glColor3f(1,0,1);
						glBegin(GL_LINES);
							glVertex3fv(CDko::modelArray[modelID]->faceArray[i].point[0].s);
							glVertex3f(
								CDko::modelArray[modelID]->faceArray[i].point[0][0] + CDko::modelArray[modelID]->faceArray[i].a*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][1] + CDko::modelArray[modelID]->faceArray[i].b*10,
								CDko::modelArray[modelID]->faceArray[i].point[0][2] + CDko::modelArray[modelID]->faceArray[i].c*10);
						glEnd();
					}
				}

				// � c TELLEMENT temporaire
				if (CDko::modelArray[modelID]->octree && CDko::renderStateBitField & DKO_RENDER_NODE) CDko::modelArray[modelID]->octree->render();
			}

			// On affiche sont BB
			if (CDko::renderStateBitField & DKO_RENDER_BB)
			{
				glPushAttrib(GL_POLYGON_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
					glDisable(GL_TEXTURE_2D);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_LIGHTING);
					glDisable(GL_CULL_FACE);
					glLineWidth(1);
					glColor3f(0,1,1);
					glPushMatrix();
						glTranslatef(CDko::modelArray[modelID]->OABB[0], CDko::modelArray[modelID]->OABB[1], CDko::modelArray[modelID]->OABB[2]);
						glScalef(CDko::modelArray[modelID]->OABB[3], CDko::modelArray[modelID]->OABB[4], CDko::modelArray[modelID]->OABB[5]);
						glBegin(GL_QUADS);
							glNormal3f(0, -1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, 1);

							glNormal3f(1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, -1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, -1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 1, 0);
							glTexCoord2i(0, 1);
							glVertex3i(1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, 1, 1);

							glNormal3f(-1, 0, 0);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(-1, -1, 1);

							glNormal3f(0, 0, 1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, 1, 1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, -1, 1);
							glTexCoord2i(1, 0);
							glVertex3i(1, -1, 1);
							glTexCoord2i(1, 1);
							glVertex3i(1, 1, 1);

							glNormal3f(0, 0, -1);
							glTexCoord2i(0, 1);
							glVertex3i(-1, -1, -1);
							glTexCoord2i(0, 0);
							glVertex3i(-1, 1, -1);
							glTexCoord2i(1, 0);
							glVertex3i(1, 1, -1);
							glTexCoord2i(1, 1);
							glVertex3i(1, -1, -1);
						glEnd();
					glPopMatrix();
				glPopAttrib();
			}
		}
	glPopAttrib();
#endif
#else
	(void)modelID; (void)frameID;
#endif
}



//
// Fermer le engine dko
//
void			dkoShutDown()
{
	if (CDko::lastErrorString) delete [] CDko::lastErrorString;

	for (int i=0;i<MAX_MODEL;i++) if (CDko::modelArray[i]) delete CDko::modelArray[i];

	_typBitFieldPile* toKill;
	for (_typBitFieldPile* ptrBitField = CDko::bitFieldPile; ptrBitField; delete toKill)
	{
		toKill = ptrBitField;
		ptrBitField = ptrBitField->previous;
	}

#ifndef CONSOLE
	dktShutDown();
#endif
}



//
// Pour performer un sphere intersection
//
bool			dkoSphereIntersection(unsigned int modelID, float *mp1, float *mp2, float rayon, float *intersect, float *normal, int &n)
{
	if (CDko::modelArray[modelID]) 
	{
		if (CDko::modelArray[modelID]->octree)
		{
			CVector mintersection;
			CVector mnormal;
			CVector p1(mp1);
			CVector p2(mp2);
			float dis = distanceFast(p1, p2);

			// Voil�on est pret, on test � dans le octree
			CDko::globalFrameID++;
			if (CDko::modelArray[modelID]->octree->findSphereIntersection(p1, p2, rayon, dis, mintersection, mnormal, n))
			{
				intersect[0] = mintersection[0];
				intersect[1] = mintersection[1];
				intersect[2] = mintersection[2];
				normal[0] = mnormal[0];
				normal[1] = mnormal[1];
				normal[2] = mnormal[2];
				return true;
			}
			else
				return false;
		}
		else
		{
			CDko::updateLastError("Octree not built for that model");
			return false;
		}
	}
	else
	{
		CDko::updateLastError("Invalide model ID");
		return false;
	}
}
