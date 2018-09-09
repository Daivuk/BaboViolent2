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

#include "ePTexture.h"



//
// Constructeur
//
ePTexture::ePTexture()
{
	name = 0;
}



//
// Pour enregistrer en .dkt
//

// La struct pour les chunks
struct _typChunkDKT
{
	unsigned short chunkID;
	INT4 lenght;
	_typChunkDKT()
	{
		chunkID = 0;
		lenght = 0;
	}
};

// Une strucs pour saver plus rapidement les données
struct _typDimension
{
	short width;
	short height;
	short bpp;
	_typDimension()
	{
		width=0;
		height=0;
		bpp=0;
	}
};

// Une strucs pour saver les transformations appliqué sur la texture
struct _typTransformation
{
	float scaleU;
	float scaleV;
	float posU;
	float posV;
	_typTransformation()
	{
		scaleU=0;
		scaleV=0;
		posU=0;
		posV=0;
	}
};

// La définition des chunks
const unsigned short CHUNK_VERSION = 0x0000;
	const unsigned short CHUNK_MAP = 0x1000;
		const unsigned short CHUNK_MAP_NAME = 0x1100;
		const unsigned short CHUNK_MAP_TEXTURE = 0x1200;
			const unsigned short CHUNK_MAP_TEXTURE_DIMENSION = 0x1210;
			const unsigned short CHUNK_MAP_TEXTURE_TRANSFORMATION = 0x1220;
			const unsigned short CHUNK_MAP_TEXTURE_DATA = 0x1230;
			const unsigned short CHUNK_MAP_TEXTURE_END = 0x1290;
	const unsigned short CHUNK_MAP_END = 0x1900;
const unsigned short CHUNK_VERSION_END = 0x9000;




//
// Pour le loader
//
void ePTexture::loadIt(char* filename)
{
	// On ouvre le fichier
	FILE* ficIn = fopen(filename, "rb");

	// Si l'ouverture a échoué
	if (!ficIn) return;

	// On check la version (ceci doit être le premier chunk, sinon ce n'est pas un fichier valide
	_typChunkDKT chunk;
    fread(&chunk, 1, sizeof(chunk), ficIn);

		if (chunk.chunkID == CHUNK_VERSION)
		{
			// La première version
			short version;
			fread(&version, 1, 2, ficIn);
			if (version != 0x0001) return;
		}
		else
		{
			return;
		}

	// On li le chunk
       fread(&chunk, 1, sizeof(chunk), ficIn);

	// Ensuite on passe tout les chunk jusqu'à fin
	while (chunk.chunkID != CHUNK_VERSION_END)
	{
		// On load une map
		if (chunk.chunkID == CHUNK_MAP)
		{
			loadMap(ficIn);
		}

		// On li le chunk
        fread(&chunk, 1, sizeof(chunk), ficIn);
	}

	// On ferme le fichier
	fclose(ficIn);
}

void ePTexture::loadMap(FILE* ficIn)
{
	_typLayer* ptrLayer = 0;

	// On li le premier chunk
	_typChunkDKT chunk;
    fread(&chunk, 1, sizeof(chunk), ficIn);

	while (chunk.chunkID != CHUNK_MAP_END)
	{
		switch (chunk.chunkID)
		{
		case CHUNK_MAP_NAME:
			{
				char tmp[256];
				loadString(tmp, ficIn);
				if (stricmp(tmp, "basemap") == 0) ptrLayer = &baseMap;
				if (stricmp(tmp, "detailmap") == 0) ptrLayer = &detailMap;
				if (stricmp(tmp, "bumpmap") == 0) ptrLayer = &bumpMap;
				if (stricmp(tmp, "specmap") == 0) ptrLayer = &specularMap;
				if (stricmp(tmp, "selfillmap") == 0) ptrLayer = &selfIlluminationMap;
				break;
			}
		case CHUNK_MAP_TEXTURE:
			{
				loadTexture(ptrLayer, ficIn);
				break;
			}
		}

		// On li le chunk suivant
        fread(&chunk, 1, sizeof(chunk), ficIn);
	}
}

void ePTexture::loadTexture(_typLayer* ptrLayer, FILE* ficIn)
{
	// On li le premier chunk
	_typChunkDKT chunk;
    fread(&chunk, 1, sizeof(chunk), ficIn);

	// On passe les autres apres
	while (chunk.chunkID != CHUNK_MAP_TEXTURE_END)
	{
		switch (chunk.chunkID)
		{
		case CHUNK_MAP_TEXTURE_DIMENSION:
			{
				_typDimension dim;
				fread(&dim, 1, sizeof(_typDimension), ficIn);
				ptrLayer->w = dim.width;
				ptrLayer->h = dim.height;
				ptrLayer->bpp = dim.bpp;	
				break;
			}
		case CHUNK_MAP_TEXTURE_TRANSFORMATION:
			{
				_typTransformation trans;
				fread(&trans, 1, sizeof(_typTransformation), ficIn);
				ptrLayer->scale = trans.scaleU;
				break;
			}
		case CHUNK_MAP_TEXTURE_DATA:
			{
#ifndef DEDICATED_SERVER
	#ifndef _DX_
					GLubyte* imageData = new GLubyte [ptrLayer->w*ptrLayer->h*ptrLayer->bpp];
	#else
					unsigned char* imageData = new unsigned char [ptrLayer->w*ptrLayer->h*ptrLayer->bpp];
	#endif
#else
				unsigned char* imageData = new unsigned char [ptrLayer->w*ptrLayer->h*ptrLayer->bpp];
#endif
				fread(imageData, 1, ptrLayer->w*ptrLayer->h*ptrLayer->bpp, ficIn);
				if (ptrLayer->bpp == 4)
					ptrLayer->textureID = createTextureFromBuffer(imageData, ptrLayer->w, ptrLayer->h, ptrLayer->bpp, 1, false);
				else
					ptrLayer->textureID = createTextureFromBuffer(imageData, ptrLayer->w, ptrLayer->h, ptrLayer->bpp, 2, false);
				break;
			}
		}

		// On li le chunk suivant
        fread(&chunk, 1, sizeof(chunk), ficIn);
	}
}



//
// Loader une string
//
void ePTexture::loadString(char* string, FILE* ficIn)
{
	// On load le premier caractère
	int i=0;
	fread(&(string[i++]), 1, 1, ficIn);

	// On load le reste
	for (; string[i-1]; fread(&(string[i++]), 1, 1, ficIn));
}





//
// Créer une texture ogl à partir du buffer
//
//
// On cré une texture à partir d'un buffer
//
unsigned int createTextureFromBuffer(unsigned char *Buffer, int Width, int Height, int BytePerPixel, int Filter, bool inverse)
{
#ifndef DEDICATED_SERVER
#ifndef _DX_
	// On cré la texture
	unsigned int Texture=0;
	GLint Level = (BytePerPixel == 3) ? GL_RGB : GL_RGBA;

	// On génère une texture
	glGenTextures(1, &Texture);

	// On pogne la dernière erreur
	if (Texture == 0)
	{
	}

	// On bind cette texture au context
	glBindTexture(GL_TEXTURE_2D, Texture);

	// On met ça en bilinear (pixel et mipmap)
	switch (Filter)
	{
	case 0: // Nearest
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			break;
		}
	case 1: // Linear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			break;
		}
	case 2: // Bilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			break;
		}
	case 3: // Trilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	default: // Nearest default
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
			break;
		}
	}

	if (inverse)
	{
		unsigned char temp;

		// Ici c'est con, mais faut switcher le rouge avec le bleu
		for(INT4 i=0; i<(INT4)(Width*Height*BytePerPixel); i+=BytePerPixel)
		{
			temp=Buffer[i];
			Buffer[i] = Buffer[i + 2];
			Buffer[i + 2] = temp;
		}
	}

	// On construit les mipmap maintenant
	gluBuild2DMipmaps(GL_TEXTURE_2D, BytePerPixel, Width, Height,
					  Level, GL_UNSIGNED_BYTE, Buffer);

	return Texture;
#endif
	return 0;
#else
	(void)Buffer; (void)Width; (void)Height; (void)BytePerPixel; (void)Filter; (void)inverse;
	return 0;
#endif
}
