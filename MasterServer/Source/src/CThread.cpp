#include "CThread.h"

// OS dependant
#ifdef WIN32				//Win32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>		
#else
	#include <sched.h>		//Linux
	#include "LinuxHeader.h"
		
	typedef void* (*PTHREAD_START_ROUTINE)(

   		 void* lpThreadParameter

    	);

#endif



//
// Fonction pour cr�r un thread
//

int createThread(unsigned long (*pFuncter)(void*), void * pParam, unsigned long & pThreadID, int pPriority)
{

	#ifdef WIN32
		// We create the thread using win32 API
		HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)pFuncter, pParam, 0, &pThreadID);

		// If its not valid, just return false
		if (hThread == NULL) return 0;
		
		// We set the priority for the thread
		switch (pPriority)
		{
		case CTHREAD_PRIORITY_VERY_LOW:
			SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
			break;
		case CTHREAD_PRIORITY_LOW:
			SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case CTHREAD_PRIORITY_NORMAL:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case CTHREAD_PRIORITY_HIGH:
			SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case CTHREAD_PRIORITY_VERY_HIGH:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		}
	#else
		// We create the thread using POSIX thread system (Linux)
		pthread_t threadID = 0;

		//define thread attributes
		//pthread_attr_t *threadAttr = new pthread_attr_t;	//attribute for parameting the linux thread
		//pthread_attr_init(threadAttr);
		//pthread_attr_setdetachstate(threadAttr, PTHREAD_CREATE_JOINABLE);
		//pthread_attr_setinheritsched(threadAttr, PTHREAD_EXPLICIT_SCHED);
		//pthread_attr_setstacksize(threadAttr,4000);  //reserving a 4 meg stack for the game
		//pthread_attr_setschedpolicy(threadAttr, SCHED_OTHER);
		//sched_param schedparam;
 		//schedparam.sched_priority = DEFAULT_PRIO;
		//pthread_attr_setschedparam(threadAttr, &schedparam);
		
		int rc = pthread_create( &threadID,0,(PTHREAD_START_ROUTINE)pFuncter, pParam);
		if (rc)
		{
        		 printf("ERROR; return code from pthread_create() is %d\n", rc);
         		//exit(-1);
      		}
		pThreadID = threadID;
		//pthread_detach(threadID);
		
		
		return threadID;
	#endif

	

	

	// Everything is ok
	return 1;
}



//
// Constructor
//
CThread::CThread()
{
	mIsRunning = false;
}



//
// To start the thread
//
bool CThread::start(void * pArg, int pThreadPriority)
{
	arg(pArg); // store user data
	// Using OS specific call to create thread
	mIsRunning = true;

	int result = createThread(CThread::entryPoint, this, mThreadId, pThreadPriority);

	if (!result)
	{
		mIsRunning = false;
	}

	return (result) ? true : false;
}



//
// Run it
//
void CThread::run(void * pArg)
{
	setup();
	execute(pArg);
	mIsRunning = false;

#ifndef WIN32
	pthread_detach(mThreadId);
	pthread_exit(0);
#endif
}



//
// static
//
unsigned long CThread::entryPoint(void * pThis)
{
	CThread * pt = (CThread*)pThis;
	pt->run(pt->arg());
	return 0;
}



//
// Setuping thread
//
void CThread::setup()
{
	// Do any setup here
}
