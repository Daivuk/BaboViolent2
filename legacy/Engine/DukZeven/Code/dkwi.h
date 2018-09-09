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


#ifndef DKWI_H
#define DKWI_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#define CALLBACK
#endif


#include "platform_types.h"

#ifdef WIN32
#include <windows.h>
#else
#include <SDL/SDL.h>
#include "linux_types.h"
#endif

#include "CVector.h"

// le rectangle que forme notre fenetre
static RECT m_WindowRect;


class CMainLoopInterface
{
public:
	virtual void paint() = 0;
	virtual void textWrite(unsigned int caracter) = 0;
};



// Les fonction du DKW
DLL_API int				dkwInit(int width, int height, int colorDepth, char *title, CMainLoopInterface *mMainLoopObject, bool fullScreen, int refreshRate = -1);
DLL_API void			dkwForceQuit();
DLL_API HDC				dkwGetDC();
DLL_API HWND			dkwGetHandle();
DLL_API HINSTANCE		dkwGetInstance();
DLL_API char*			dkwGetLastError();
DLL_API CVector2i		dkwGetCursorPos();
DLL_API CVector2i		dkwGetResolution();
DLL_API int				dkwMainLoop();
DLL_API void			dkwShutDown();
DLL_API void			dkwUpdate();
DLL_API void			dkwClipMouse( bool abEnabled = true );



// La class static pour contenir le tout
class CDkw
{
public:
	/// Dernier message d'erreur
	static char *lastErrorString;

	/// Le titre de la fenêtre
	static char *title;

	/// Largeur de la fenêtre
	static int w;

	///Hauteur de la fenêtre
	static int h;

	/// La profondeur des couleurs
	static int colorDepth;

	/// Device context
	static HDC dc;

	/// Le handle de l'instance de l'application
	static HINSTANCE hInstance;

	/// Le handle de la fenetre
	static HWND hWnd;

	/// Booleen servant à savoir si l'application roule
	static bool running;

	/// Le callback pour le main loop
	static CMainLoopInterface *mainLoopObject;

	/// La position de la souris
	static CVector2i cursorPos;

public:
	// Pour updater l'erreur
	static void updateLastError(char *error);

	// La windows procedure
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifndef WIN32
	static unsigned long flags;
	static DIMOUSESTATE2 mouse_state;
	static unsigned char keys_state[256+128+8];
#endif
};



#endif
