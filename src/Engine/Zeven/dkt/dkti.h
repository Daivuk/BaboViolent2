/* TCE (c) All rights reserved */


#ifndef DKTI_H
#define DKTI_H



#include "platform_types.h"

#ifdef WIN32
#ifndef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifndef _DX_
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

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#endif
//#pragma comment( lib, "DevIL.lib" )


#include <windows.h>

// Les includes pour opengl
#ifndef _DX_
#include <gl/gl.h> 
#include <gl/glu.h> 
//#include <gl/glext.h>
#include "glext.h"
#endif
#else
#define DLL_API
#include "LinuxHeader.h"
#ifdef __MACOSX__
#include <SDL_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif>
#endif


// pour devil
//#include <IL/il.h>

#include <vector>
#include "CString.h"
#include "CVector.h"



const int DKT_FILTER_NEAREST = 0;
const int DKT_FILTER_LINEAR = 1;
const int DKT_FILTER_BILINEAR = 2;
const int DKT_FILTER_TRILINEAR = 3;




// Les fonction du DKT
DLL_API void			 dktBlurTexture(unsigned int textureID, int nbPass);
DLL_API void			 dktChangeFilter(int filter);
DLL_API unsigned int	 dktCreateEmptyTexture(int w, int h, int bpp, int filter);
DLL_API void			 dktCreateTextureFromBuffer(unsigned int *textureID, unsigned char *buffer, int w, int h, int bpp, int filter);
DLL_API unsigned int	 dktCreateTextureFromFile(char *filename, int filter);
DLL_API void			 dktDeleteTexture(unsigned int *textureID);
DLL_API char*		 dktGetLastError();
DLL_API int			 dktGetTextureBytePerPixel(unsigned int textureID);
DLL_API void			 dktGetTextureData(unsigned int textureID, unsigned char * data);
DLL_API CVector2i				 dktGetTextureSize(unsigned int textureID);
DLL_API void			 dktInit();
DLL_API void			 dktRenderToTexture(unsigned int textureID, int x, int y, int w, int h, unsigned int internalFormat);
DLL_API void			 dktShutDown();
DLL_API void			 dktUpdate();



// La class pour tenir une texture, son ID et son filename
class CTexture
{
public:
	// Le string pour le filename de la texture
	CString filename;

	// Son ID opengl
	unsigned int oglID;

	// La date de modification du fichier
	INT4 modifDate;

	// Sa résolution
	CVector2i size;

	// Le nombre de fois quelle a été loadé
	int nbInstance;

	// Le nombre de Byte per pixel de la texture
	int bpp;

public:
	// Constructeur / Destructeur
	CTexture()
	{
		oglID = 0;
		nbInstance = 0;
		modifDate = 0;
	}
	
	virtual ~CTexture()
	{
#ifndef _DX_
		glDeleteTextures(1, &oglID);
#endif
	}
};



// La class static pour contenir le tout
class CDkt
{
public:
	// Pour tenir la dernière erreur
	static char *lastErrorString;

	// Le vector de nos textures initialisé
	static std::vector<CTexture*> textures;

	// La texture qu'on check pour le auto updating
	static int checkingUpdate;

public:
	// Pour updater l'erreur
	static void updateLastError(char *error);
};



#endif
