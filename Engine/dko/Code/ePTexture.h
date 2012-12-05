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

#ifndef EPTEXTURE_H
#define EPTEXTURE_H


#include "platform_types.h"
#ifdef WIN32
#ifndef CONSOLE
#ifndef _DX_
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
#endif
#endif
#else
#ifdef __MACOSX__
	#include <SDL_opengl.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif
#endif

#ifndef WIN32
	#include "LinuxHeader.h"
#endif

#include <stdio.h>
#include <string.h>


// La petite structure pour les layers
class _typLayer
{
public:
	// La texture ogl
	unsigned int textureID;

	// La grandeur w et h de cette texture
	int w,h;
	int bpp;

	// Le scale (nb de fois r���
	float scale;

public:
	// Le constructeur
	_typLayer()
	{
		textureID = 0;
		w = 128;
		h = 128;
		bpp = 3;
		scale = 1;
	}
	virtual ~_typLayer()
	{
#ifndef CONSOLE
#ifndef _DX_
		if (textureID == 0) glDeleteTextures(1, &textureID);
#endif
#endif
	}
};



class ePTexture
{
public:
	// Le nom de la texture
	char* name;

	// Ici nos layer
	_typLayer baseMap;
	_typLayer detailMap;
	_typLayer bumpMap;
	_typLayer specularMap;
	_typLayer selfIlluminationMap;

private:
	// Pour loader
	void loadMap(FILE* fic);
	void loadTexture(_typLayer* ptrLayer, FILE* fic);
	void loadString(char* string, FILE* fic);

public:
	// Constructeur / destructeur
	ePTexture();

	// On le save
	void saveIt(char* filename);
	void saveMap(_typLayer* map, char* mapName, FILE* fic);

	// Pour le loader
	void loadIt(char* filename);
};



unsigned int createTextureFromBuffer(unsigned char *Buffer, int Width, int Height, int BytePerPixel, int Filter, bool inverse);



#endif
