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


#ifndef DKSI_H
#define DKSI_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DKS_DLL_API __declspec(dllexport)
#else
#define DKS_DLL_API __declspec(dllimport)
#endif
#else
#define DKS_DLL_API
#endif


#ifdef WIN32
#include <windows.h>


// Link

#endif

#include "dks.h"

// Les includes pour FMod
#include "CString.h"
#include "CVector.h"


#ifdef USE_FMODEX
void dksSet3DListenerAttributes(const CVector3f * pos, const CVector3f * vel, const CVector3f * forward, const CVector3f * up);
void dksUpdate();
void dksSetSfxMasterVolume(float volume);
void dksStopSound(FMOD_SOUND * s);
FMOD_SYSTEM * dksGetSystem();
FMOD_CHANNEL * dksGetChannel(FMOD_SOUND * s);
#endif




#endif
