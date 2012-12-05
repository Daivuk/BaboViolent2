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


#ifndef DKII_H
#define DKII_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif



#ifdef WIN32
// Et oui, on utilise directInput :(
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


// Les headers
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#else

#include "LinuxHeader.h"
#include "linux_types.h"

#endif

#include "CVector.h"

// Les fonction du DKI
DLL_API int				dkiGetFirstDown();
DLL_API int				dkiGetMouseWheelVel();
DLL_API CVector2i		dkiGetMouse();
DLL_API CVector2i		dkiGetMouseVel();
DLL_API int				dkiGetState(int inputID);
DLL_API CVector3f		dkiGetJoy();
DLL_API CVector3f		dkiGetJoyR();
DLL_API CVector3f		dkiGetJoyVel();
DLL_API int				dkiInit(HINSTANCE appInstance, HWND appHandle);
DLL_API void			dkiShutDown();
DLL_API void			dkiUpdate(float elapsef, int width, int height);
DLL_API void			dkiSetMouse(CVector2i & mousePos);



const int DKI_NOKEY = -1;
const int DKI_NOTHING = 0;
const int DKI_DOWN = 1;
const int DKI_HOLD = 2;
const int DKI_UP = 3;
const int DKI_CLIC = 3;
const int DKI_DBL_CLIC = 4;

const int DKI_MOUSE_BUTTON1 = 256;
const int DKI_MOUSE_BUTTON2 = 257;
const int DKI_MOUSE_BUTTON3 = 258;
const int DKI_MOUSE_BUTTON4 = 259;
const int DKI_MOUSE_BUTTON5 = 260;
const int DKI_MOUSE_BUTTON6 = 261;
const int DKI_MOUSE_BUTTON7 = 262;
const int DKI_MOUSE_BUTTON8 = 263;

const int DKI_JOY_BUTTON1 = 264; // 128 Buttons. DKI_JOY_BUTTON1 + n


#define INPUTDOWN(key) (key & 0x80)



struct _typMousePos
{
	int x;
	int y;
	_typMousePos()
	{
		x = 0;
		y = 0;
	}
};



// La class static pour contenir non status
class CDki
{
public:
#ifdef WIN32
	// L'objet DirectInput
	static LPDIRECTINPUT8 diObject;

	// L'objet pour le clavier
	static LPDIRECTINPUTDEVICE8 diKeyboard;

	// Celui pour la mouse
	static LPDIRECTINPUTDEVICE8 diMouse;

	// Et celui pour le joystick
	static LPDIRECTINPUTDEVICE8 diJoypad;

	// Le buffer pour les touches
#endif

	static char keyboardStateDI[256];

	// Les states de tout nos boutton clavier
	static int allState[256+8+128];

	// L'état de la sourie
	static DIMOUSESTATE2 mouseStateDI;
	static _typMousePos mousePos;

#ifdef WIN32
	// Pour le joystick
	static DIDEVCAPS JoyCaps;
	static DIPROPRANGE diprg;
	static DIJOYSTATE2 EtatJoy;

#endif
	// Pour savoir le dernier key pressé (pour le hold)
	static int lastDown;
	static float downTimer;

public:
};



#endif
