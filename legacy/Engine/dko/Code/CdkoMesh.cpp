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

#include "CdkoModel.h"
#include "CdkoMesh.h"
#include "dkoInner.h"

#include <math.h>




//
// Constructeur / Destructeur
//
CdkoMesh::CdkoMesh()
{
	name = 0;
	nbMatGroup = 0;
	matGroupArray = 0;
}

CdkoMesh::~CdkoMesh()
{
	if (name) delete [] name;
	if (matGroupArray) delete [] matGroupArray;
}



//
// pour loader du fichier
//
int CdkoMesh::loadFromFile(FILE *ficIn, char *path)
{
	(void)path;
	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_NAME:
			{
				if (name) delete [] name;
				name = readString(ficIn);
				break;
			}
		case CHUNK_DKO_POSITION:
			{
				fread(position, 3, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_MATRIX:
			{
				fread(matrix, 9, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_NB_MAT_GROUP:
			{
				fread(&nbMatGroup, 1, sizeof(short), ficIn);

				// On load les sous-objet
				if (matGroupArray) delete [] matGroupArray;
				matGroupArray = new _typMatGroup[nbMatGroup];
				for (int i=0;i<nbMatGroup;i++)
				{
					if (matGroupArray[i].meshAtFrame) delete [] matGroupArray[i].meshAtFrame;
					matGroupArray[i].meshAtFrame = new _typMeshAtFrame[parentModel->timeInfo[2]];
					matGroupArray[i].parentModel = parentModel;
					loadMatGroup(ficIn, &(matGroupArray[i]));
				}

				break;
			}
		}

		chunkID = readChunk(ficIn);
	}

	// Tout est beau
	return 1;
}



//
// On load un sous-objet (sous-sous objet en fait :P)
//
int CdkoMesh::loadMatGroup(FILE *ficIn, _typMatGroup *matGroup)
{
	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_MAT_ID:
			{
				short MatID;
				fread(&MatID, 1, sizeof(short), ficIn);
				matGroup->material = &(((CDkoModel*)parent)->materialArray[MatID]);
				break;
			}
		case CHUNK_DKO_NB_VERTEX:
			{
				fread(&(matGroup->nbVertex), 1, sizeof(matGroup->nbVertex), ficIn);
                printf("nbVertex: %ld\n", matGroup->nbVertex);
				break;
			}
		case CHUNK_DKO_VERTEX_ARRAY:
			{
				// On load pour tout les frames
				if (matGroup->interpolatedVA) delete [] matGroup->interpolatedVA;
				matGroup->interpolatedVA = new float [matGroup->nbVertex*3];
				for (int f=0;f<parentModel->timeInfo[2];f++)
				{
					if (matGroup->meshAtFrame[f].vertexArray) delete [] matGroup->meshAtFrame[f].vertexArray;
					matGroup->meshAtFrame[f].vertexArray = new float [matGroup->nbVertex*3];
					fread(matGroup->meshAtFrame[f].vertexArray, matGroup->nbVertex*3, sizeof(float), ficIn);
				}

				// On pogne son parent
				CDkoModel* owner = (CDkoModel*)parent;
				owner->nbFace += matGroup->nbVertex/2; // On incrémente son nombre de face (triangle)

				// On passe tout les vertex pour créer notre OABB
				for (int i=0;i<matGroup->nbVertex;i++)
				{
					float tmp[3] = {
					matGroup->meshAtFrame[0].vertexArray[i*3+0],
					matGroup->meshAtFrame[0].vertexArray[i*3+1],
					matGroup->meshAtFrame[0].vertexArray[i*3+2]};

					// On le passe dans la matrice
					CVector right(matrix[0] , matrix[1] , matrix[2]);
					CVector front(matrix[3] , matrix[4] , matrix[5]);
					CVector up(matrix[6] , matrix[7] , matrix[8]);
					CVector current = right*tmp[0] + front*tmp[1] + up*tmp[2] + CVector(position[0], position[1], position[2]);

					if (owner->firstVertex)
					{
						owner->min[0] = current[0];
						owner->min[1] = current[1];
						owner->min[2] = current[2];
						owner->max[0] = current[0];
						owner->max[1] = current[1];
						owner->max[2] = current[2];
						owner->firstVertex = false;
					}

					for (int j=0;j<3;j++)
					{
						if (owner->min[j] > current[j]) owner->min[j] = current[j];
						if (owner->max[j] < current[j]) owner->max[j] = current[j];
					}

					// On en profite pour calculer le rayon
					float dis = sqrtf(current[0]*current[0] + current[1]*current[1] + current[2]*current[2]);

					// Si il est plus grand que le rayon qu'on a déjà on le update
					if (dis > owner->radius) owner->radius = dis;
				}
				break;
			}
		case CHUNK_DKO_NORMAL_ARRAY:
			{
				// On load pour tout les frames
				if (matGroup->interpolatedNA) delete [] matGroup->interpolatedNA;
				matGroup->interpolatedNA = new float [matGroup->nbVertex*3];
				for (int f=0;f<parentModel->timeInfo[2];f++)
				{
					if (matGroup->meshAtFrame[f].normalArray) delete [] matGroup->meshAtFrame[f].normalArray;
					matGroup->meshAtFrame[f].normalArray = new float [matGroup->nbVertex*3];
					fread(matGroup->meshAtFrame[f].normalArray, matGroup->nbVertex*3, sizeof(float), ficIn);
				}
				break;
			}
		case CHUNK_DKO_TEXCOORD_ARRAY:
			{
				matGroup->animatedUV = false;
			//	matGroup->texCoordArray = new float [matGroup->nbVertex*2];
			//	fread(matGroup->texCoordArray, matGroup->nbVertex*2, sizeof(float), ficIn);
				if (matGroup->meshAtFrame[0].texCoordArray) delete [] matGroup->meshAtFrame[0].texCoordArray;
				matGroup->meshAtFrame[0].texCoordArray = new float [matGroup->nbVertex*2];
				fread(matGroup->meshAtFrame[0].texCoordArray, matGroup->nbVertex*2, sizeof(float), ficIn);
				break;
			}
		case CHUNK_DKO_TEXCOORD_ARRAY_ANIM:
			{
				// On load pour tout les frames
				matGroup->animatedUV = true;
				if (matGroup->interpolatedUV) delete [] matGroup->interpolatedUV;
				matGroup->interpolatedUV = new float [matGroup->nbVertex*2];
				for (int f=0;f<parentModel->timeInfo[2];f++)
				{
					if (matGroup->meshAtFrame[f].texCoordArray) delete [] matGroup->meshAtFrame[f].texCoordArray;
					matGroup->meshAtFrame[f].texCoordArray = new float [matGroup->nbVertex*2];
					fread(matGroup->meshAtFrame[f].texCoordArray, matGroup->nbVertex*2, sizeof(float), ficIn);
				}
				break;
			}
		}

		chunkID = readChunk(ficIn);
	}

	// Tout est beau
	return 1;
}



//
// On construit la liste des faces
//
int CdkoMesh::_buildFaceListIt(CFace *faceArray, int index)
{
	int nbFace = 0;

	for (int i=0;i<nbMatGroup; i++)
	{
		// On cré ses face
		for (int j=0;j<matGroupArray[i].nbVertex / 3;j++)
		{
			CFace *face = &(faceArray[index + nbFace + j]);

			for (int k=0;k<3;k++)
			{
				float tmp[3] = {
					matGroupArray[i].meshAtFrame[0].vertexArray[j*9+k*3+0],
					matGroupArray[i].meshAtFrame[0].vertexArray[j*9+k*3+1],
					matGroupArray[i].meshAtFrame[0].vertexArray[j*9+k*3+2]};

				// On le passe dans la matrice
				CVector right(matrix[0] , matrix[1] , matrix[2]);
				CVector front(matrix[3] , matrix[4] , matrix[5]);
				CVector up(matrix[6] , matrix[7] , matrix[8]);
				CVector current = right*tmp[0] + front*tmp[1] + up*tmp[2] + CVector(position[0], position[1], position[2]);

				face->point[k] = current;
			}

			// On calcul d'abords la normal
			CVector normal = cross(face->point[1]-face->point[0],face->point[2]-face->point[0]);
			normalize(normal);

			// La normal
			face->a = normal[0];
			face->b = normal[1];
			face->c = normal[2];

			// Produit scalaire
			face->d = dot(normal, face->point[0]);

			// For fast collisions
			face->createNormals();

			// Voilà!
		}

		nbFace += matGroupArray[i].nbVertex / 3;
	}

	return nbFace;
}



//
// Pour trouver les vertex à un point donné
//
int CdkoMesh::_buildVertexArrayIt(float * vertexArray, int index)
{
	int nbFloat = 0;
	for (int i=0;i<nbMatGroup; i++)
	{
		// Ha ah!! on interpolate
		if (parentModel->framef > -1)
		{
			int frameFrom = (int)parentModel->framef;
			int frameTo = (int)parentModel->framef + 1;
			if (frameTo >= parentModel->timeInfo[2]) frameTo = 0;
			float percent = parentModel->framef - (float)(int)parentModel->framef;
			int nbFloat = matGroupArray[i].nbVertex*3;

			for (int n=0;n<nbFloat;n++)
			{
				matGroupArray[i].interpolatedVA[n] = 
					matGroupArray[i].meshAtFrame[frameFrom].vertexArray[n] +
					(matGroupArray[i].meshAtFrame[frameTo].vertexArray[n] -
					matGroupArray[i].meshAtFrame[frameFrom].vertexArray[n]) * percent;
			}

			matGroupArray[i].ptrVA = matGroupArray[i].interpolatedVA;
		}
		else
		{
			matGroupArray[i].ptrVA = matGroupArray[i].meshAtFrame[parentModel->currentFrame].vertexArray;
		}

		// On cré ses face
		for (int j=0;j<matGroupArray[i].nbVertex;j++)
		{
			float * ptrArray = &(vertexArray[index + nbFloat + j * 3]);

			float tmp[3] = {
				matGroupArray[i].ptrVA[j*3+0],
				matGroupArray[i].ptrVA[j*3+1],
				matGroupArray[i].ptrVA[j*3+2]};

			// On le passe dans la matrice
			CVector right(matrix[0] , matrix[1] , matrix[2]);
			CVector front(matrix[3] , matrix[4] , matrix[5]);
			CVector up(matrix[6] , matrix[7] , matrix[8]);
			CVector current = right*tmp[0] + front*tmp[1] + up*tmp[2] + CVector(position[0], position[1], position[2]);

			ptrArray[0] = current[0];
			ptrArray[1] = current[1];
			ptrArray[2] = current[2];
		}

		nbFloat += matGroupArray[i].nbVertex * 3;
	}

	return nbFloat;
}



//
// Pour le dessiner
//
void CdkoMesh::drawIt()
{

	// On passe chaque material Group
	for (int i=0;i<nbMatGroup;i++)
	{
		// On interpolate si c'est nécéssaire
		if (parentModel->framef > -1)
		{
			// Ha ah!! on interpolate
			int frameFrom = (int)parentModel->framef;
			int frameTo = (int)parentModel->framef + 1;
			if (frameTo >= parentModel->timeInfo[2]) frameTo = 0;
			float percent = parentModel->framef - (float)(int)parentModel->framef;
			int nbFloat = matGroupArray[i].nbVertex*3;

			for (int n=0;n<nbFloat;n++)
			{
				matGroupArray[i].interpolatedVA[n] = 
					matGroupArray[i].meshAtFrame[frameFrom].vertexArray[n] +
					(matGroupArray[i].meshAtFrame[frameTo].vertexArray[n] -
					matGroupArray[i].meshAtFrame[frameFrom].vertexArray[n]) * percent;
				matGroupArray[i].interpolatedNA[n] = 
					matGroupArray[i].meshAtFrame[frameFrom].normalArray[n] +
					(matGroupArray[i].meshAtFrame[frameTo].normalArray[n] -
					matGroupArray[i].meshAtFrame[frameFrom].normalArray[n]) * percent;
			}
			if (matGroupArray[i].animatedUV)
			{
				nbFloat = matGroupArray[i].nbVertex*2;
				for (int n=0;n<nbFloat;n++)
				{
					matGroupArray[i].interpolatedUV[n] =
						matGroupArray[i].meshAtFrame[frameFrom].texCoordArray[n] +
						(matGroupArray[i].meshAtFrame[frameTo].texCoordArray[n] -
						matGroupArray[i].meshAtFrame[frameFrom].texCoordArray[n]) * percent;
				}
				matGroupArray[i].ptrUV = matGroupArray[i].interpolatedUV;
			}
			else
			{
				matGroupArray[i].ptrUV = matGroupArray[i].meshAtFrame[0].texCoordArray;
			}

			matGroupArray[i].ptrVA = matGroupArray[i].interpolatedVA;
			matGroupArray[i].ptrNA = matGroupArray[i].interpolatedNA;
		}
		else
		{
			matGroupArray[i].ptrVA = matGroupArray[i].meshAtFrame[parentModel->currentFrame].vertexArray;
			matGroupArray[i].ptrNA = matGroupArray[i].meshAtFrame[parentModel->currentFrame].normalArray;
			if (matGroupArray[i].animatedUV) matGroupArray[i].ptrUV = matGroupArray[i].meshAtFrame[parentModel->currentFrame].texCoordArray;
			else matGroupArray[i].ptrUV = matGroupArray[i].meshAtFrame[0].texCoordArray;
		}

		if (CDko::renderStateBitField & DKO_BUMP_MAP && matGroupArray[i].material &&
			CDko::renderStateBitField & DKO_MULTIPASS && CDko::renderStateBitField & DKO_DYNAMIC_LIGHTING)
		{
			if (matGroupArray[i].material->textureMat)
			{
				if (matGroupArray[i].material->textureMat->bumpMap.textureID)
				{
					drawBumpFull(i);
					continue;
				}
			}
		}

#ifndef DEDICATED_SERVER
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT);
			if (!(CDko::renderStateBitField & DKO_DYNAMIC_LIGHTING)) glDisable(GL_LIGHTING);

				glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, matGroupArray[i].ptrVA);
					if (matGroupArray[i].ptrNA)
					{
						glEnableClientState(GL_NORMAL_ARRAY);
						glNormalPointer(GL_FLOAT, 0, matGroupArray[i].ptrNA);
					}
						if (matGroupArray[i].ptrUV)
						{
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, matGroupArray[i].ptrUV);
						}

							if (matGroupArray[i].material) 
							{
								matGroupArray[i].material->setDiffusePass();
								glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
								glPopMatrix();
								glMatrixMode(GL_MODELVIEW);
							}
							else
							{
								glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
							}
							glPopAttrib();

							glDepthMask(GL_FALSE);

							// On mets le detail map
							if (matGroupArray[i].material && CDko::renderStateBitField & DKO_MULTIPASS)
							{
								if (CDko::renderStateBitField & DKO_DETAIL_MAP)
								{
									if (matGroupArray[i].material->textureMat)
									{
										if (matGroupArray[i].material->textureMat->detailMap.textureID)
										{
											matGroupArray[i].material->setDetailPass();
												glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
												glPopMatrix();
												glMatrixMode(GL_MODELVIEW);
											glPopAttrib();
										}
									}
								}

								if (CDko::renderStateBitField & DKO_SPECULAR &&
									CDko::renderStateBitField & DKO_DYNAMIC_LIGHTING)
								{
									matGroupArray[i].material->setSpecularPass();
										glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
										glPopMatrix();
										glMatrixMode(GL_MODELVIEW);
									glPopAttrib();
								}

								if (CDko::renderStateBitField & DKO_SELFILL_MAP)
								{
									if (matGroupArray[i].material->textureMat)
									{
										if (matGroupArray[i].material->textureMat->selfIlluminationMap.textureID)
										{
											matGroupArray[i].material->setSelfIllPass();
												glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
												glPopMatrix();
												glMatrixMode(GL_MODELVIEW);
											glPopAttrib();
										}
									}
									else if (matGroupArray[i].material->texSelfIll)
									{
										matGroupArray[i].material->setSelfIllPass();
											glDrawArrays(GL_TRIANGLES, 0, matGroupArray[i].nbVertex);
											glPopMatrix();
											glMatrixMode(GL_MODELVIEW);
										glPopAttrib();
									}
								}
							}
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDepthMask(GL_TRUE);
		glPopAttrib();
#endif
#endif
	}
}



//
// Pour dessiner multi-pass avec le bump pis toute
//
void CdkoMesh::drawBumpFull(int i)
{
	(void)i;
	// On fait lumière par lumière
/*	bool enabledLight[8];
	for (int j=0;j<8;j++)
	{
		enabledLight[j] = glIsEnabled(GL_LIGHT0+j);
		glDisable(GL_LIGHT0+j);
	}

	// On débute le array tout suite
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, matGroupArray[i].vertexArray);

	// Maintenant on enable les lights une par une et on fait les pass nécéssaire
	for (int l=0;l<8;l++)
	{
		if (enabledLight[l])
		{
			// Le tout premier pass doit être celui du bump (le plus toff quoi)
			// Ici on va avoir besoin du vertex shader hey oui...
	//		glDisable(GL_
		}
	}*/
}
