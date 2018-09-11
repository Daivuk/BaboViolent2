/* TCE (c) All rights reserved */


#ifndef DKTI_H
#define DKTI_H



#include "platform_types.h"


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

//#pragma comment( lib, "DevIL.lib" )


#include <windows.h>

// Les includes pour opengl

#include "dkgl.h"
#include "dkt.h"

// pour devil
//#include <IL/il.h>

#include <vector>
#include "CString.h"
#include "CVector.h"



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
