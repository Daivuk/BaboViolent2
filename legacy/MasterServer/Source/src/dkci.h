/* RndLabs (c) All rights reserved */
#ifndef DKCI_H
#define DKCI_H


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
DLL_API(long)			dkcGetFrame(); // Pour obtenir le nb de frame o on est rendu
DLL_API(void)			dkcInit(int framePerSecond); // Init the timer (do at your program start)
DLL_API(void)			dkcJumpToFrame(int frame); // To step a couple of frame or to init it to 0
DLL_API(long)			dkcUpdateTimer(); // Will return the current frame count
DLL_API(void)			dkcSleep(long ms);



// La class static pour contenir le tout
class CDkc
{
public:
	static long frame; // Le frame qu'on est rendu
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
