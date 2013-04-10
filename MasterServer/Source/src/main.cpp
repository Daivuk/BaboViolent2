
#ifndef WIN32
	#include "sys/times.h"

	#define stricmp strcasecmp //helper define
#else
	#include <windows.h>
#endif

#include "cNetManager.h"
#include "dkc.h"


int main()
{

	//creating the net Manager
	cNetManager	NetManager;
	NetManager.Init();


	//main program here
	bool isRunning = true;
	
	dkcInit(30);

	//linux time struct
	#ifndef WIN32
		timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = 1000000;
	#endif


	while(isRunning)
	{
		// On va updater notre timer
		int nbFrameElapsed = dkcUpdateTimer();

		// On va chercher notre delay
		float delay = dkcGetElapsedf();
		
		while (nbFrameElapsed)
		{ 
			//let's update our NetManager
			isRunning	=	NetManager.Update(delay);

			nbFrameElapsed--;
		}
		//linux sleep
		#ifndef WIN32
			nanosleep(&ts,0);
			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		//windows sleep
		#else
			Sleep(1);
		#endif
		
	}
  
    return 0; 
}
