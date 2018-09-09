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

/* RndLabs (c) All rights reserved */
#ifndef DKCI_H
#define DKCI_H



#include "platform_types.h"

#ifdef WIN32
	#ifndef DLL_EXPORTS
		#define DLL_API(p) __declspec(dllexport) p
	#else
		#define DLL_API(p) __declspec(dllimport) p
	#endif

	#include <windows.h>
	#define INT64 __int64
#else
	#define DLL_API(p) p
	#include "LinuxHeader.h"
	#define INT64 unsigned long long
#endif











// Les constantes


// Les fonction du DKC
DLL_API(float)			dkcGetElapsedf(); // The elapsed time in seconde
DLL_API(float)			dkcGetFPS(); // Obtenir le frame per second
DLL_API(INT4)			dkcGetFrame(); // Pour obtenir le nb de frame o on est rendu
DLL_API(void)			dkcInit(int framePerSecond); // Init the timer (do at your program start)
DLL_API(void)			dkcJumpToFrame(int frame); // To step a couple of frame or to init it to 0
DLL_API(INT4)			dkcUpdateTimer(); // Will return the current frame count
DLL_API(void)			dkcSleep(INT4 ms);



// La class static pour contenir le tout
class CDkc
{
public:
	static INT4 frame; // Le frame qu'on est rendu
	static INT64 lastFrameCount; // Le temps au dernier frame
	static float elapsedf; // Le temps �oul�entre deux frames, pour les animations
	static float currentFrameDelay; // Le temps qu'on a de fait sur ce frame
	static bool usePerformanceTimer; // Si on utilise ou pas le performance timer (plus pr�is)
	static INT64 frequency; // La fr�uence utilis�
	static float perSeconde; // C'est pour computer le nb de frame par seconde
	static int framePerSeconde;
	static float fps; // Le frame rate
	static int oneSecondFrameCound;
	static float oneSecondElapsedcpt;

public:
};



#endif
