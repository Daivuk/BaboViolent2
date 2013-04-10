/* RndLabs (c) All rights reserved */


#include "dkci.h"



//
// Les trucs static
//
long CDkc::frame=0; // Le frame qu'on est rendu
INT64 CDkc::lastFrameCount=0; // Le temps au dernier frame
float CDkc::elapsedf=0; // Le temps �oul�entre deux frames, pour les animations
float CDkc::currentFrameDelay=0; // Le temps qu'on a de fait sur ce frame
bool CDkc::usePerformanceTimer=false;
INT64 CDkc::frequency=1000;
float CDkc::perSeconde=40; // Par default 25 frames par seconde
int CDkc::framePerSeconde=25;
float CDkc::fps = 0;
int CDkc::oneSecondFrameCound = 0;
float CDkc::oneSecondElapsedcpt = 0;



//
// The elapsed time in seconde
//
float			dkcGetElapsedf()
{
	return CDkc::perSeconde;
}



//
// Obtenir le frame per second
//
float			dkcGetFPS()
{
	return CDkc::fps;
}



//
// Pour obtenir le nb de frame o on est rendu
//
long			dkcGetFrame()
{
	return CDkc::frame;
}



//
// Init the timer (do at your program start)
//
void			dkcInit(int framePerSecond)
{
	CDkc::framePerSeconde = framePerSecond;

	#ifdef WIN32
		// On check si on peut utiliser un timer de haute performance
		if (!QueryPerformanceFrequency((LARGE_INTEGER *) &(CDkc::frequency)))
		{
			CDkc::usePerformanceTimer = false;
			CDkc::frequency = 1000;
		}
		else
		{
			CDkc::usePerformanceTimer = true;
		}
	#else
		CDkc::frequency = sysconf(_SC_CLK_TCK);
	#endif

	// Pour savoir le bon delait pour le nb de frame par seconde
	CDkc::perSeconde = 1.0f / (float)(CDkc::framePerSeconde);
}



//
// To step a couple of frame or to init it to 0
//
void			dkcJumpToFrame(int frame)
{
	CDkc::frame = frame;
	CDkc::currentFrameDelay = 0;
	CDkc::lastFrameCount=0;
	CDkc::elapsedf=0;
	CDkc::currentFrameDelay=0;
	CDkc::fps = 0;
	CDkc::oneSecondFrameCound = 0;
	CDkc::oneSecondElapsedcpt = 0;
}



//
// Will return the current frame count
//
long			dkcUpdateTimer()
{
	// On prend le nombre de tick du CPU
	INT64 lGetTickCount;
	
	#ifdef WIN32
		if (CDkc::usePerformanceTimer)
		{
			QueryPerformanceCounter((LARGE_INTEGER *) &lGetTickCount);
		}
		else
		{
			lGetTickCount = GetTickCount();
		}
	#else
		//get the current number of microseconds since january 1st 1970
		
		tms tBuf;
		//int ct = times(&tBuf);

		//printf("ct = %i\n",ct);

		lGetTickCount = times(&tBuf);

	#endif

	// On update le timer
	double elapsedd = (double)((CDkc::lastFrameCount) ? lGetTickCount - CDkc::lastFrameCount : 0) / (double)CDkc::frequency;
	CDkc::lastFrameCount = lGetTickCount;
	CDkc::elapsedf = (float)elapsedd;
	CDkc::currentFrameDelay += CDkc::elapsedf;
	long nbFrameAdded=0;
	while (CDkc::currentFrameDelay >= CDkc::perSeconde)
	{
		CDkc::currentFrameDelay -= CDkc::perSeconde;
		CDkc::frame++;
		nbFrameAdded++;
	}

	// On update pour le fps
	CDkc::oneSecondElapsedcpt += CDkc::elapsedf;
	CDkc::oneSecondFrameCound++;
	while (CDkc::oneSecondElapsedcpt >= 1)
	{
		CDkc::oneSecondElapsedcpt -= 1;
		CDkc::fps = (float)CDkc::oneSecondFrameCound;
		CDkc::oneSecondFrameCound = 0;
	}

	// On retourne le nombre de frame �animer
	return nbFrameAdded;
}



void			dkcSleep(long ms)
{
	#ifdef WIN32
		long timeMem = GetTickCount();
		long currentTime = timeMem;
		while (currentTime - timeMem < ms)
		{
			currentTime = GetTickCount();
		}
	#endif
}
