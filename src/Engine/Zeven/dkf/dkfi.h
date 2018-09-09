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

/* TCE (c) All rights reserved */


#ifndef DKFI_H
#define DKFI_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif




#include "CFont.h"
#include <vector>
#include "CVector.h"




// Les fonction du DKT
DLL_API void			dkfBindFont(unsigned int ID);
DLL_API unsigned int	dkfCreateFont(char *filename);
DLL_API void			dkfDeleteFont(unsigned int *ID);
DLL_API CPoint2f		dkfGetCaracterPos(float size, char *text, int caracter);
DLL_API int				dkfGetOverStringCaracter(float size, char *text, CPoint2f & onStringPos);
DLL_API float			dkfGetStringHeight(float size, char *text);
DLL_API float			dkfGetStringWidth(float size, char *text);
DLL_API void			dkfPrint(float size, float x, float y, float z, char *text);
DLL_API void			dkfShutDown();




//
// La liste global des font
//
std::vector<CFont*> fonts;
unsigned int currentIDCount = 0;
CFont *currentBind = 0;



#endif
