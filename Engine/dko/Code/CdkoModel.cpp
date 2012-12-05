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
#include "dkoInner.h" // Avoir acces au mother static
#include "CdkoMesh.h"
//#include "CHUNKINF.H"



//
// Constructeur / Destructeur
//

CDkoModel::CDkoModel()
{
	materialArray = 0;
	nbMat = 0;
	name = 0;
	radius = 0;
	int i;
	for (i=0;i<6;OABB[i++] = 0.0f);
	firstVertex = true;
	nbFace = 0;
	faceArray = 0;
	octree = 0;
	timeInfo[0] = 0;
	timeInfo[1] = 0;
	timeInfo[2] = 1;
	currentFrame = 0;
	dumSize = 0;
	framef=0;
	for (i=0;i<64;dummies[i++] = 0);
}

CDkoModel::~CDkoModel()
{
	if (name) delete [] name;
	if (materialArray) delete [] materialArray;
	if (octree) delete octree;

	// On efface les dummy
	for (int i=0;i<64;i++) if (dummies[i]) delete dummies[i];

	if (faceArray) delete [] faceArray;
}



//
// pour loader un fichier 3ds
//
int CDkoModel::loadFromFile3DS(FILE *ficIn, char *path)
{
	(void)ficIn; (void)path;
	return 0;
}



//
// pour loader du fichier
//
int CDkoModel::loadFromFile(FILE *ficIn, char *path)
{
	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_VERSION:
			{
				// On check si on a la bonne version
				short version;
				fread(&version, 1, sizeof(short), ficIn);
				if (version > DKO_VERSION)
				{
					CDko::updateLastError("Incorrect version of file");
					return 0;
				}
				break;
			}
		case CHUNK_DKO_TIME_INFO:
			{
				// On importe le start frame, le end frame et la duration dans un ti array de short
				fread(timeInfo, 1, sizeof(short)*3, ficIn);

				// Ça va nous servir pour allouer l'espace pour nos meshes
				break;
			}
		case CHUNK_DKO_PROPERTIES:
			{
				loadProperties(ficIn);
				break;
			}
		case CHUNK_DKO_MATLIST:
			{
				fread(&nbMat, 1, sizeof(short), ficIn);
				if (nbMat)
				{
					materialArray = new CdkoMaterial[nbMat];

					// On load tout les materiaux
					for (int i=0;i<nbMat;i++)
					{
						if (!materialArray[nbMat-i-1].loadFromFile(ficIn, path))
						{
							return 0;
						}
					}
				}
				else
				{
					CDko::updateLastError("There is no material set in the file");
					return 0;
				}
				break;
			}
		case CHUNK_DKO_TRI_MESH:
			{
				CdkoMesh *newMesh = new CdkoMesh();
				addChild(newMesh);
				newMesh->parentModel = this;
				newMesh->loadFromFile(ficIn, path);
				break;
			}
		case CHUNK_DKO_DUMMY:
			{
				loadDummy(ficIn);
				break;
			}
		}

		chunkID = readChunk(ficIn);
	}

	// On calcul son OABB
	for (int i=0;i<3;i++)
	{
		OABB[0+i] = (max[i] + min[i]) / 2;
		OABB[3+i] = (max[i] - min[i]) / 2;
	}

	// Tout est beau
	return 1;
}



//
// Pour loader un dummy
//
void CDkoModel::loadDummy(FILE *ficIn)
{
	// On cré notre dummy
	_typDummy *newDum = new _typDummy(timeInfo[2]);

	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_NAME:
			{
				newDum->name = readString(ficIn);
				break;
			}
		case CHUNK_DKO_POSITION:
			{
				for (int i=0;i<timeInfo[2];i++)
				{
					fread(newDum->position[i].v, 3, sizeof(float), ficIn);
				}
				break;
			}
		case CHUNK_DKO_MATRIX:
			{
				for (int i=0;i<timeInfo[2];i++)
				{
					fread(newDum->matrix[i].m, 9, sizeof(float), ficIn);
				}
				break;
			}
		}

		chunkID = readChunk(ficIn);
	}

	// On l'ajoute à notre liste
	dummies[dumSize] = newDum;
	dumSize++;
}



//
// Pour loader ses propriétées
//
void CDkoModel::loadProperties(FILE *ficIn)
{
	// On load chunk par chunk jusqu'à ce qu'on pogne le chunk End
	short chunkID = readChunk(ficIn);

	while (chunkID != CHUNK_DKO_END)
	{
		switch(chunkID)
		{
		case CHUNK_DKO_NAME:
			{
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
		}

		chunkID = readChunk(ficIn);
	}
}



//
// Pour lire un chunk
//
short readChunk(FILE *ficIn)
{
	short chunkID;
	fread(&chunkID, 1, sizeof(short), ficIn);
	return chunkID;
}



//
// Pour lire un string
//
char *readString(FILE *ficIn)
{
	char tmp[256];
	int i = 0;
	fread(tmp, 1, 1, ficIn); // On li le premier caractère
	while (*(tmp + i++) != 0)  // Tant qu'on pogne pas le caractère 0, NULL, '\0', toute la même chose
	{
		fread(tmp + i, 1, 1, ficIn);	// On li le prochain caractère
	}
	
	// On cré un string dynamique et on retourne ça
	char *newStr = new char [i];
	strcpy(newStr, tmp);

	return newStr;
}



//
// On va créer sa facelist
//
void CDkoModel::buildFaceList()
{
	if (nbFace)
	{
		faceArray = new CFace[nbFace];

		// On passe tout ses child pis on construit la liste des faces pour les mesh
		_buildFaceList(faceArray, 0);
	}
}



//
// Pour se construire un vertex array
//
void CDkoModel::buildVertexArray(float * vertexArray)
{
	_buildVertexArray(vertexArray, 0);
}



//
// On va créer un octree à partir de la facelist
//
void CDkoModel::buildOctree()
{
	// On construit un array de pointeurs vers ses faces
	CFace **ptrFaceArray = new CFace*[nbFace];
	for (int i=0;i<nbFace;i++)
		ptrFaceArray[i] = &(faceArray[i]);

	// On trouve le côté le plus grand
	float size = (OABB[3]>OABB[4])?OABB[3]:OABB[4];
	size = (size>OABB[5])?size:OABB[5];

	// On construit le octree
	octree = new COctreeNode(ptrFaceArray, nbFace, CVector(OABB), size, 0);

	// On efface la liste temporaire
	delete [] ptrFaceArray;
}



//
// Pour loader une nouvelle animation dans ce fichier
//
/*
int CDkoModel::addAnimationFromFile(unsigned int modelID, char* filename, char* animationName)
{
	// Pour tenir le nb de byte lu dans tout le fichier
	int byteRead = 0;

	// On ouvre le fichier en lecteur binaire
	FILE *fic3DS = fopen(filename, "rb");
	if (!fic3DS) 
	{
		CDko::updateLastError("Invalide 3ds filename");
		return 0;	// Si le fichier n'a pas pus être ouvert, on retourne 0
	}

	// Ici on read le gros chunk, le premier
	typChunk currentChunk;
	ReadChunk(fic3DS, &currentChunk, filename);
	
	// Bon, est-ce que cest bien un fichier 3ds ça là?
	if (currentChunk.ID != M3DMAGIC)
	{
		CDko::updateLastError("Invalide 3ds file");
		fclose(fic3DS); // On ferme le fichier
		return 0; // Sinon on retourne 0 of course
	}

	// On peut maintenant processer les chunks suivant
//	dkPrintToConsole("M3DMAGIC");
	readMainBlock(fic3DS, currentChunk, filename);

	// Finalement on ferme le fichier
	fclose(fic3DS); // On ferme le fichier

	// On retourne le nombre de byte lu pendant le fichier (voir qui ça interresse :|)
	return currentChunk.mlength; // Voilà tout
}
*/


//
// Lire le block principal du fichier 3ds
//
/*
void CDkoModel::readMainBlockAnim(FILE* fic3DS, typChunk &previousChunk, char* filename)
{
	// Tant qu'on a pas lu le nb de bytes contenu dans le chunk
	while (previousChunk.bytesRead < previousChunk.mlength)
	{
		// On li maintenant le prochain chunk
		typChunk currentChunk;
		ReadChunk(fic3DS, &currentChunk, filename);

		// Check the chunk ID
		switch (currentChunk.ID)
		{
			// On load juste squi a rapport avec les animations
		case KFDATA:
			{
			//	dkPrintToConsole("KFDATA"); // Ici on va avoir un autre loader à part pour les animations
				readKFDATA(fic3DS, currentChunk, filename);
				break;
			}
		}

		// On clean ce qui reste dans le chunk
		cleanChunkByte(fic3DS, currentChunk, filename);
		previousChunk.bytesRead += currentChunk.bytesRead;
	}
}*/



//
// Pour loader nos pivot point
//
/*
void Cdko::readKFDATA(FILE* fic3DS, typChunk &previousChunk, char* filename)
{
	// Tant qu'on a pas lu le nb de bytes contenu dans le chunk
	while (previousChunk.bytesRead < previousChunk.mlength)
	{
		// On li maintenant le prochain chunk
		typChunk currentChunk;
		ReadChunk(fic3DS, &currentChunk, filename);

		// Check the chunk ID
		switch (currentChunk.ID)
		{
		case KFHDR:
			{	
				dkPrintToConsole("        KFHDR");

				short revision;
				currentChunk.bytesRead += fread(&revision, 1, sizeof(short), fic3DS);

				char *strTmp = GetString(fic3DS, currentChunk, filename);
				dkPrintToConsole("            %s", strTmp);
				delete [] strTmp;

				currentChunk.bytesRead += fread(&animLen, 1, sizeof(INT4), fic3DS);
				dkPrintToConsole("            Anim len : %i", animLen);
				break;
			}
		case KFSEG:
			{
				dkPrintToConsole("        KFSEG");

				currentChunk.bytesRead += fread(segFrame, 1, sizeof(INT4)*2, fic3DS);
				dkPrintToConsole("            Anim segment : %i, %i", segFrame[0], segFrame[1]);
				break;
			}
		case KFCURTIME:
			{
				dkPrintToConsole("        KFCURTIME");

				currentChunk.bytesRead += fread(&curFrame, 1, sizeof(INT4), fic3DS);
				dkPrintToConsole("            Current frame : %i", curFrame);
				break;
			}
		case OBJECT_NODE_TAG:
			{
				dkPrintToConsole("        OBJECT_NODE_TAG");
				// Ha ah! on va pogner les keyframes d'un objet
				readOBJECT_NODE_TAG(fic3DS,currentChunk,filename);
				break;
			}
		}
	
		// On clean ce qui reste dans le chunk
		cleanChunkByte(fic3DS, currentChunk, filename);
		previousChunk.bytesRead += currentChunk.bytesRead;
	}
}*/
