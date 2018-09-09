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

#ifdef WIN32
#define _WIN32_WINNT 0x0400
#endif

#ifndef WIN32
	#include "LinuxHeader.h"
#endif

#include "Zeven.h"
#include "Scene.h"
#include "Console.h"
#include <exception>
#include "CMaster.h"
#ifndef DEDICATED_SERVER
	#include "CStatus.h"
	#include "CLobby.h"
#endif


#ifdef WIN32
	#if defined(_DEBUG) && defined(USE_VLD) && !defined(DEDICATED_SERVER)
		#include <vld.h>
	#endif
#endif


// notre scene
Scene * scene = 0;


int resW = 800;
int resH = 600;
bool fullscreen = false;

char * bbNetVersion;

#ifdef DEDICATED_SERVER
bool quit = false;
void dkwForceQuit()
{
	quit = true;
}
#else
CVector2i mousePos_xbox;
CVector2f mousePos_xboxVel;
void updateXBoxMouse_main(float delay)
{
	CVector3f joy = dkiGetJoyR();

	joy[2] = 0;

	if (fabsf(joy[0]) < .25f) joy[0] = 0;
	if (fabsf(joy[1]) < .25f) joy[1] = 0;

	CVector2i res = dkwGetResolution();

	mousePos_xboxVel[0] += joy[0] * (30 + fabsf(mousePos_xboxVel[0]) * 0.0f) * delay;
	mousePos_xboxVel[1] += joy[1] * (30 + fabsf(mousePos_xboxVel[1]) * 0.0f) * delay;
	if (mousePos_xboxVel[0] > 2.5f) mousePos_xboxVel[0] = 2.5f;
	if (mousePos_xboxVel[0] < -2.5f) mousePos_xboxVel[0] = -2.5f;
	if (mousePos_xboxVel[1] > 2.5f) mousePos_xboxVel[1] = 2.5f;
	if (mousePos_xboxVel[1] < -2.5f) mousePos_xboxVel[1] = -2.5f;

	mousePos_xbox[0] += (int)(mousePos_xboxVel[0] * 600 * delay);
	mousePos_xbox[1] += (int)(mousePos_xboxVel[1] * 600 * delay);
	if (mousePos_xbox[0] < 0)
	{
		mousePos_xbox[0] = 0;
		mousePos_xboxVel[0] = 0;
	}
	if (mousePos_xbox[1] < 0)
	{
		mousePos_xbox[1] = 0;
		mousePos_xboxVel[1] = 0;
	}
	if (mousePos_xbox[0] > res[0])
	{
		mousePos_xbox[0] = res[0] - 1;
		mousePos_xboxVel[0] = 0;
	}
	if (mousePos_xbox[1] > res[1])
	{
		mousePos_xbox[1] = res[1] - 1;
		mousePos_xboxVel[1] = 0;
	}

	if (mousePos_xboxVel[0] > 0)
	{
		mousePos_xboxVel[0] -= delay * 25;
		if (mousePos_xboxVel[0] < 0) mousePos_xboxVel[0] = 0;
	}
	if (mousePos_xboxVel[0] < 0)
	{
		mousePos_xboxVel[0] += delay * 25;
		if (mousePos_xboxVel[0] > 0) mousePos_xboxVel[0] = 0;
	}
	if (mousePos_xboxVel[1] > 0)
	{
		mousePos_xboxVel[1] -= delay * 25;
		if (mousePos_xboxVel[1] < 0) mousePos_xboxVel[1] = 0;
	}
	if (mousePos_xboxVel[1] < 0)
	{
		mousePos_xboxVel[1] += delay * 25;
		if (mousePos_xboxVel[1] > 0) mousePos_xboxVel[1] = 0;
	}
}

CVector2i dkwGetCursorPos_main()
{
	if (gameVar.cl_enableXBox360Controller)
	{
		return mousePos_xbox;
	}
	else
	{
		return dkwGetCursorPos();
	}
}
#endif


#ifndef DEDICATED_SERVER

// lil function to update the launcher on boot up
void UpdateLauncher()
{

	//test
	//CreateDirectory("testing", 0 );

	//CreateDirectory("test", 0 );
	//CreateDirectory("test/testing", 0 );


	// try to open the file, if not present, then we dont need to update the launcher!
	FILE * file = fopen("_Bv2Launcher.exe" ,"rb");

	//no update needed
	if(!file)
	{
		return;
	}

	fseek (file , 0 , SEEK_END);
	long Size = ftell(file);
	rewind(file);

		//open the current laucher so we can overwrite it!
	FILE * ufile = fopen("Bv2Launcher.exe","wb");

	// problem opening the file for writing, no update will occur
	if(!ufile)
	{
		fclose(file);
        return;
	}

	char *buffer = new char[Size];

	fread( buffer , Size , 1 , file );
	fwrite( buffer , Size , 1 , ufile );

	//finished with the files
	fclose( file );
	fclose( ufile );

	//let's kill the unusefull file
	remove( "_Bv2Launcher.exe" );




}


class MainLoopInterface : public CMainLoopInterface
{
public:



	// Les fonctions obligatoire du MainLoopInterface de la dll dkw
	void paint()
	{

		// On va updater notre timer
		int nbFrameElapsed = dkcUpdateTimer();

#ifdef _DEBUG
		// LAG GENERATOR , use it to bind a key and test in lag conditions
		if (dkiGetState(DIK_BACK) == DKI_DOWN)
		{
			Sleep(300);
		}
#endif


		// On va chercher notre delay
		float delay = dkcGetElapsedf();

		// On passe le nombre de frame �animer
		while (nbFrameElapsed)
		{
			// On update nos input
			dkiUpdate(delay, resW, resH);

			// Xbox mouse pos
			updateXBoxMouse_main(delay);

			// On update le writing
			if (writting) writting->updateWritting(delay);

			// Update la console
			console->update(delay);

			// On appel nos fonction pour animer ici
			scene->update(delay);

			// On d�r�ente pour le prochain frame
			nbFrameElapsed--;
		}

#ifdef _DX_
		if (dkglGetDXDevice())
		{
			dkglGetDXDevice()->BeginScene();
#endif
		// On render le tout
		scene->render();
#ifdef _DX_
		dkglGetDXDevice()->EndScene();
#endif

		// Swap buffers if valid context is found
#ifndef _DX_
		if( dkwGetDC() )
		{
			SwapBuffers( dkwGetDC() );
		}
#else
		dkglGetDXDevice()->Present(	NULL,
									NULL,
									NULL,
									NULL);
		}
#endif

      #ifdef NDEBUG
      #ifdef WIN32
      #ifdef _PRO_
  //    if (IsDebuggerPresent() == TRUE)
  //       {
  //       throw(0);
  //       }
      #endif
      #endif  
      #endif
	}

	void textWrite(unsigned int caracter)
	{
		// Voil�juste un writting peut avoir le focus �la fois
		if (writting) writting->writeText(caracter);
	}
} mainLoopInterface;
#endif



class StringInterface : public CStringInterface
{
public:
	virtual void updateString(CString* string, char * newValue)
	{
		*string = newValue;
	}
    StringInterface()
        {
            //printf("Constructor: 0x%x\n", this);
        }
    virtual ~StringInterface()
        {
        }
} stringInterface;



#ifdef DEDICATED_SERVER

#include "CThread.h"
	bool s_locked = false;
	bool s_internalLock = false;

class CMainLoopConsole : public CThread
{
public:
	bool locked;
	bool internalLock;

public:
	CMainLoopConsole()
	{
		locked = false;
		internalLock = false;
	}

	void execute(void* pArg)
	{
		#ifndef WIN32 // linux timestruct for nanosleep
			timespec ts;

			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		#endif
	

		while (!quit)
		{
			// On va updater notre timer
			int nbFrameElapsed = dkcUpdateTimer();

			// On va chercher notre delay
			float delay = dkcGetElapsedf();

			// On passe le nombre de frame �animer
			while (nbFrameElapsed)
			{
				// Update la console
				console->update(delay);

				// On appel nos fonction pour animer ici
				scene->update(delay);

				// On d�r�ente pour le prochain frame
				nbFrameElapsed--;

				//printf("FPS: %f\n", dkcGetFPS());
			}

			//--- On check si on n'est pas lock�avant de continuer
			if (locked)
			{
				internalLock = true;
			}

			while (internalLock)
			{
				#ifdef WIN32
					Sleep(1);
				#else
					if(nanosleep(&ts,0))
					{
						printf("problem nanosleep internal lock\n");
					}
				#endif
			//	printf("--- internalLock (execute)\n");
			}

			#ifdef WIN32
				Sleep(1);
			#else
			if(nanosleep(&ts,0))
			{
				printf("problem nanosleep main loop\n");
			}
			#endif
		}

		//printf(" game main loop has quit \n");

	/*	char input[256];
		while (!quit)
		{
			std::cin.getline(input,256);


			lock();
			console->sendCommand(input);//CString("Execute CTF"));
			unlock();

			#ifdef WIN32
				Sleep(1);
			#else
				if(nanosleep(&ts,0))
				{
					printf("problem nanosleep console loop\n");
				}
				ts.tv_sec = 0;
				ts.tv_nsec = 1000000;
			#endif

			//cin.ignore( 10000 , '\n');
			//input[0] = 0;
			//fflush(stdin);
		};*/
	}

	void lock()
	{
		#ifndef WIN32 // linux timestruct for nanosleep

			timespec ts;

			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		#endif

		locked = true;
		while (!internalLock)
		{
			#ifdef WIN32
				Sleep(1);
			#else
				if(nanosleep(&ts,0))
				{
					printf("problem nanosleep lock\n");
				}
				ts.tv_sec = 0;
				ts.tv_nsec = 1000000;
				
				
			#endif
		//	printf("--- internalLock (lock)\n");
		}
	}

	void unlock()
	{
		locked = false;
		internalLock = false;
	}
};


int main(int argc, const char* argv[])
{

	// lil print out so that people now know that its working

	printf("***************************************\n");
	printf("*   Babo Violent 2 Dedicated Server   *\n");
	printf("*   Version 2.11d                     *\n");
	printf("*                                     *\n");
	printf("* check the /main/LaunchScript files  *\n");
	printf("* to configure your server            *\n");
	printf("***************************************\n\n\n");

	
	// Example on how to get the mac adress
	//unsigned char mac[8];		// unsigned here is very important
	//bb_getMyMAC( mac );

	//char macString[20];
	//sprintf( macString , "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x" , (int)mac[0], (int)mac[1],(int)mac[2],(int)mac[3],(int)mac[4],(int)mac[5] );
	//printf( " mac addr : %s " , macString );





	#ifndef WIN32 // linux timestruct for nanosleep
		timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = 1000000; // 1 ms
	#endif

	// PREMI�E CHOSE �FAIRE, on load les config
	dksvarInit(&stringInterface);
	dksvarLoadConfig("main/bv2.cfg");
	dksvarSaveConfig("main/bv2.cfg"); // On cre8 le config file aussi

	// On init nos DLL qui vont �re utilis�dans ce jeu
	// On initialise quelque cossin important avant tout
	dkcInit(30); // 30 frame par seconde (m�e que 15 serait le best)

	// On init la network
	if (bb_init() == 1)
	{
		#ifdef WIN32
			MessageBox(NULL, "Error initiating baboNet", "Error", 0);
		#endif
		return 0;
	}
	bbNetVersion = bb_getVersion();
	if (CString("%s", bbNetVersion) != "4.0")
	{
		// Error
		bb_peerShutdown();
		bb_shutdown();
		#ifdef WIN32
			MessageBox(NULL, "Wrong version of BaboNet\nReinstalling the game may resolve this prolem", "Error", 0);
		#endif
		return 0;
	}

	// On init la console
	console = new Console();
	console->init();

	//--- On cr�le master
	master = new CMaster();

	// On cr�notre scene
	scene = new Scene();

	// La loop principal
	CMainLoopConsole mainLoopConsole;

	//--- On start la thread
	mainLoopConsole.start();


/*	{
		#ifndef WIN32 // linux timestruct for nanosleep
			timespec ts;

			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		#endif
	

		float tim = 0;

		while (!quit)
		{
			// On va updater notre timer
			int nbFrameElapsed = dkcUpdateTimer();

			// On va chercher notre delay
			float delay = dkcGetElapsedf();

			//printf("nbFrameElapsed : %i\n",nbFrameElapsed);

			// On passe le nombre de frame �animer
			while (nbFrameElapsed)
			{
				// Update la console
				console->update(delay);

				// On appel nos fonction pour animer ici
				scene->update(delay);

				// On d�r�ente pour le prochain frame
				nbFrameElapsed--;

				//tim += delay;
				//printf("elapsed : %f\n",tim);

				//printf("FPS: %f\n", dkcGetFPS());
			}

			//--- On check si on n'est pas lock�avant de continuer
			if (s_locked)
			{
				s_internalLock = true;
			}

			while (s_internalLock)
			{
				#ifdef WIN32
					Sleep(1);
				#else
					if(nanosleep(&ts,0))
					{
						printf("problem nanosleep internal lock\n");
					}
					ts.tv_sec = 0;
					ts.tv_nsec = 1000000;
				#endif
			//	printf("--- internalLock (execute)\n");
			}

			#ifdef WIN32
				Sleep(1);
			#else
			if(nanosleep(&ts,0))
			{
				printf("problem nanosleep main loop\n");
			}
			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
			#endif
		}
	}*/

	//--- Get the arguments and send that to console
//	int argc, const char* argv[]

	if (argc > 1)
	{
		CString executeCmd = "execute ";
		executeCmd += (char*)(argv[1]);
		console->sendCommand(executeCmd);
	}


	char input[256];
	while (!quit)
	{
		std::cin.getline(input,256);


		mainLoopConsole.lock();
		if(std::cin.gcount())
			console->sendCommand(input);//CString("Execute CTF"));
		mainLoopConsole.unlock();


		#ifdef WIN32
			Sleep(1);

		#else
			if(nanosleep(&ts,0))
			{
				printf("problem nanosleep console loop\n");
			}
			ts.tv_sec = 0;
			ts.tv_nsec = 1000000;
		#endif

		//cin.ignore( 10000 , '\n');
		//input[0] = 0;
		//fflush(stdin);
	};

	//printf(" main console loop has quit \n");


//	while (dkwMainLoop());

	// On efface la scene et ses amis
	delete scene;
	delete master;
	delete console;
	console = 0;
	master = 0;
	scene = 0;

	dksvarSaveConfig("main/bv2.cfg");

	// On shutdown le tout (L'ordre est assez important ici)
	bb_peerShutdown();
	bb_shutdown();

	// Tout c'est bien pass� on retourne 0
	return 0;
}

#else



//
// Fonction principal
//
int WINAPI WinMain(	HINSTANCE	hInstance,				// Instance
			HINSTANCE	hPrevInstance,				// Previous Instance
			LPSTR		lpCmdLine,				// Command Line Parameters
			int		nCmdShow)				// Window Show State
{
	// PREMI�E CHOSE �FAIRE, on load les config
	dksvarInit(&stringInterface);
	dksvarLoadConfig("main/bv2.cfg");
	dksvarSaveConfig("main/bv2.cfg"); // On cre8 le config file aussi

	// On load tout suite le language utilis�par le joueur
	if (!gameVar.isLanguageLoaded())
	{
		MessageBox(NULL, "Can not load language file\nTry deleting the config file.", "Error", 0);
		return 0;
	}


	if (gameVar.r_bitdepth != 16 && gameVar.r_bitdepth != 32) gameVar.r_bitdepth = 32;

	// On init nos DLL qui vont �re utilis�dans ce jeu
	// On initialise quelque cossin important avant tout
	dkcInit(30); // 30 frame par seconde (m�e que 15 serait le best)
	   

	//--- Windowed mode requires special handling
	if(!gameVar.r_fullScreen)
	{
		// If we create a window at the specified resolution, the client area
		// will actually be smaller vertically, and players could potentially
		// manipulate the title-bar height to increase the viewable area further

		//--- So lets make a window
		WNDCLASS wclass;
		wclass.style = CS_OWNDC;
		wclass.cbClsExtra = 0;
		wclass.cbWndExtra = 0;
		wclass.hInstance = GetModuleHandle(0);
		wclass.hIcon = 0;
		wclass.hCursor = 0;
		wclass.lpszClassName = "bv2_res_test";
		wclass.lpszMenuName = 0;
		wclass.hbrBackground = 0;
		wclass.lpfnWndProc = &DefWindowProc;
		RegisterClass(&wclass);

		//--- We'll create a standard window, but we won't show it
		HWND hwnd = CreateWindow(wclass.lpszClassName, "", WS_CAPTION, 0, 0, gameVar.r_resolution[0], gameVar.r_resolution[1], 0, 0, GetModuleHandle(0), 0);

		//--- Handle errors
		if(!hwnd)
		{
			//--- Uh oohs
			DWORD	error = GetLastError();
			LPVOID	buffer;
	
			//--- Get the message
			FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL, error, 0, (LPTSTR) &buffer, 0, NULL);

			//--- Put that message in a box
			MessageBox(hwnd, (LPSTR)buffer, "Error", 0);

			//--- Release mem and exit
			LocalFree(buffer);
			return 0;
		}

		//--- We just want the client area resolution
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);

		//--- Great, now some cleanup
		DestroyWindow(hwnd);
		UnregisterClass(wclass.lpszClassName, GetModuleHandle(0));

		//--- Now calculate the window size necessary to match the requested resolution
		CVector2i adjustedRes;
		adjustedRes[0] = gameVar.r_resolution[0] + (gameVar.r_resolution[0] - clientRect.right);
		adjustedRes[1] = gameVar.r_resolution[1] + (gameVar.r_resolution[1] - clientRect.bottom);

		//--- Okay, now we're good to go
		if (!dkwInit(adjustedRes[0], adjustedRes[1], gameVar.r_bitdepth, gameVar.lang_gameName.s, &mainLoopInterface, gameVar.r_fullScreen, gameVar.r_refreshRate)) 
		{
			char * error = dkwGetLastError();
			MessageBox(NULL, error, "Error", 0);
			return 0;
		}
	}
	else if (!dkwInit(gameVar.r_resolution[0], gameVar.r_resolution[1], gameVar.r_bitdepth, gameVar.lang_gameName.s, &mainLoopInterface, gameVar.r_fullScreen, gameVar.r_refreshRate)) 
	{
		char * error = dkwGetLastError();
		MessageBox(NULL, error, "Error", 0);
		return 0;
	}

	// On init les input
	if (!dkiInit(dkwGetInstance(), dkwGetHandle()))
	{
		dkwShutDown();
		MessageBox(NULL, "Error creating Input", "Error", 0);
		return 0;
	}

	// Set single CPU usage
	if(gameVar.cl_affinityMode > 0)
	{
		::SetProcessAffinityMask(::GetCurrentProcess(), 0x1);
	}

	// On cr�notre API openGL
	if (!dkglCreateContext(
#ifndef _DX_
		dkwGetDC(), gameVar.r_bitdepth
#else
		dkwGetHandle(), gameVar.r_fullScreen, gameVar.r_resolution[0], gameVar.r_resolution[1]
#endif
		)) 
	{
		dkiShutDown();
		dkwShutDown();
		MessageBox(NULL, "Error creating openGL context", "Error", 0);
		return 0;
	}

	// Restore system settings
	if(gameVar.cl_affinityMode == 1)
	{
		DWORD procMask;
		DWORD sysMask;
		::GetProcessAffinityMask(::GetCurrentProcess(), &procMask, &sysMask);
		::SetProcessAffinityMask(::GetCurrentProcess(), sysMask);
	}

	// On init les textures
	dktInit();

	// On init les objets 3D
	dkoInit();

	// On init les particles
	dkpInit();

	// On init le son
	if (!dksInit(gameVar.s_mixRate, gameVar.s_maxSoftwareChannels))
	{
		dkpShutDown();
		dkoShutDown();
		dkiShutDown();
		dkglShutDown();
		dkwShutDown();
		MessageBox(NULL, "Error creating fmod", "Error", 0);
		return 0;
	}

	// On init la network
	if (bb_init() == 1)
	{
		dksShutDown();
		dkpShutDown();
		dkoShutDown();
		dkiShutDown();
		dkglShutDown();
		dkwShutDown();
		MessageBox(NULL, "Error initiating baboNet", "Error", 0);
		return 0;
	}
	bbNetVersion = bb_getVersion();
	if (CString("%s", bbNetVersion) != "4.0")
	{
		// Error
		bb_peerShutdown();
		bb_shutdown();
		dksShutDown();
		dkpShutDown();
		dkoShutDown();
		dkiShutDown();
		dkglShutDown();
		dkwShutDown();
		MessageBox(NULL, "Wrong version of BaboNet\nReinstalling the game may resolve this prolem", "Error", 0);
		return 0;
	}

	// On cr�le lobby
	lobby = new CLobby();

	// On init la console
	console = new Console();
	console->init();

	// Create the status manager
	status = new CStatus();

	//--- On cr�le master
	master = new CMaster();

	// On cr�notre scene
	scene = new Scene();

	ShowCursor(FALSE);



	#ifndef DEDICATED_SERVER
		//check if we need to update to launcher
		UpdateLauncher();
	#endif

	// check command line options
	CString str = lpCmdLine;
	if( str.len() > 1 )
	{
		console->sendCommand( str );
	}


	// La loop principal
/*	try
	{*/
		while (dkwMainLoop());
/*	}
	catch(...)
	{
		dkwShutDown(); // On close la fenetre au moins!
		MessageBox(NULL, "Babo Violent 2 has encountered an error and needs to close, sorry for the inconvenience", "Error", 0);
		return 0;
	}*/


	ShowCursor(TRUE);

	// On efface la scene
	delete scene;
	scene = 0;

	// Delete master
	delete master;
	master = 0;

	delete console;
	console = 0;

	delete lobby;
	lobby = 0;

	dksvarSaveConfig("main/bv2.cfg");

	// On shutdown le tout (L'ordre est assez important ici)
	bb_peerShutdown();
	bb_shutdown();
	dksShutDown();
	dkpShutDown();
	dkoShutDown();
	dkfShutDown();
	dktShutDown();
	dkglShutDown();
	dkiShutDown();
	dkwShutDown();

	// Do last update after window is closed
	delete status;

	// Tout c'est bien pass� on retourne 0
	return 0;
}

#endif

