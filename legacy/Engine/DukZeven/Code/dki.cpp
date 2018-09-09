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


#include "dkii.h"
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

#ifndef WIN32
#include "dkw.h"
//#include <SDL/SDL.h>
#endif


#ifdef WIN32
// Les trucs static de notre class
LPDIRECTINPUT8 CDki::diObject;
LPDIRECTINPUTDEVICE8 CDki::diKeyboard;
LPDIRECTINPUTDEVICE8 CDki::diMouse;
LPDIRECTINPUTDEVICE8 CDki::diJoypad;
#endif

char CDki::keyboardStateDI[256];


int CDki::allState[256 + 8 + 128];


DIMOUSESTATE2 CDki::mouseStateDI;


_typMousePos CDki::mousePos;

#ifdef WIN32
DIDEVCAPS CDki::JoyCaps;
DIPROPRANGE CDki::diprg;
DIJOYSTATE2 CDki::EtatJoy;
#endif

int CDki::lastDown = 0;
float CDki::downTimer = 0;



#ifdef WIN32
//
// Call back pour énumérer les axes du joystick
//
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	(void)pContext; // Disable Warnings
    HRESULT hr;
    CDki::diprg.diph.dwSize = sizeof(DIPROPRANGE);
    CDki::diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    CDki::diprg.diph.dwHow = DIPH_BYID;
    CDki::diprg.diph.dwObj = pdidoi->dwType;
    CDki::diprg.lMin = -1000;
    CDki::diprg.lMax = +1000;
    hr = CDki::diJoypad->SetProperty(DIPROP_RANGE, &(CDki::diprg.diph));
    if (FAILED(hr)) return DIENUM_STOP;
    return DIENUM_CONTINUE;
}



#endif

#ifdef WIN32
//
// Call back pour énumérer les game pad
//
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	(void)pContext; // Disable Warnings
	HRESULT hr;

	hr = CDki::diObject->CreateDevice(pdidInstance->guidInstance, &(CDki::diJoypad), NULL);
	if (FAILED(hr)) return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

#endif


//
// Pour obtenir la première touche pressé (utile pour setter les touche dans les options)
//
int				dkiGetFirstDown()
{
	for (int i=0;i<256+8+128;i++)
	{
		if (CDki::allState[i] == DKI_DOWN)
		{
			return i;
		}
	}

	return DKI_NOKEY;
}



//
// On pogne la wheel mouse
//
int				dkiGetMouseWheelVel()
{
    return CDki::mouseStateDI.lZ;
}



//
// On capte la mouse
//
CVector2i		dkiGetMouse()
{
	return CVector2i(CDki::mousePos.x, CDki::mousePos.y);
}

CVector2i		dkiGetMouseVel()
{
	return CVector2i(CDki::mouseStateDI.lX, CDki::mouseStateDI.lY);
}



//
// Pour obtenir l'état d'une touche ou d'un bouton
//
int				dkiGetState(int inputID)
{
  int r = DKI_NOTHING;

	if(inputID != DKI_NOKEY)
	  r = CDki::allState[inputID];
	return r;
}


#ifdef WIN32

//
// On capte le joystick
//
CVector3f		dkiGetJoy()
{
	return CVector3f((float)CDki::EtatJoy.lX/1000.0f, (float)CDki::EtatJoy.lY/1000.0f, (float)CDki::EtatJoy.lZ/1000.0f);
}

CVector3f		dkiGetJoyR()
{
	return CVector3f((float)CDki::EtatJoy.lRx/1000.0f, (float)CDki::EtatJoy.lRy/1000.0f, (float)CDki::EtatJoy.lRz/1000.0f);
}

CVector3f		dkiGetJoyVel()
{
	return CVector3f((float)CDki::EtatJoy.lVX/1000.0f, (float)CDki::EtatJoy.lVY/1000.0f, (float)CDki::EtatJoy.lVZ/1000.0f);
}

#endif



//
// Pour initialiser le API
//
int				dkiInit(HINSTANCE appInstance, HWND appHandle)
{
	// On fou nos input à 0
	int i;
	for (i=0;i<256;i++)
	{
		CDki::keyboardStateDI[i] = 0;
	}
	for (i=0;i<256+8+128;i++)
	{
		CDki::allState[i] = DKI_NOTHING;
	}

#ifdef WIN32
	// On initialise l'objet direct input
	DirectInput8Create(appInstance, DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**)&(CDki::diObject), NULL);
	if (!CDki::diObject) return 0;

	// On va initialiser le clavier
	CDki::diObject->CreateDevice(GUID_SysKeyboard, &(CDki::diKeyboard), NULL);
	if (!CDki::diKeyboard) return 0;
	CDki::diKeyboard->SetCooperativeLevel(appHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) ;
	CDki::diKeyboard->SetDataFormat(&c_dfDIKeyboard);
	CDki::diKeyboard->Acquire();

	// Maintenant on va initialiser la mouse
	CDki::diObject->CreateDevice(GUID_SysMouse, &(CDki::diMouse), NULL);
	if (!CDki::diMouse) return 0;
    CDki::diMouse->SetCooperativeLevel(appHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND/*DISCL_BACKGROUND*/);
    CDki::diMouse->SetDataFormat(&c_dfDIMouse2);
    CDki::diMouse->Acquire();

	// Maintenant on va initialiser le joystick, s'il y en a un
	CDki::diObject->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (CDki::diJoypad)
	{
		CDki::diJoypad->SetDataFormat(&c_dfDIJoystick2) ;
		CDki::diJoypad->SetCooperativeLevel(appHandle, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		CDki::JoyCaps.dwSize = sizeof(DIDEVCAPS);
		CDki::diJoypad->GetCapabilities(&(CDki::JoyCaps));
		CDki::diJoypad->EnumObjects(EnumAxesCallback, (VOID*)appHandle, DIDFT_AXIS);
		CDki::diJoypad->Acquire();
	}
#else
//        SDL_WM_GrabInput(SDL_GRAB_ON);
#endif

	return 1;
}



//
// Pour bien fermer le tout
//
void			dkiShutDown()
{
#ifdef WIN32
	if (CDki::diJoypad)
	{
		CDki::diJoypad->Unacquire();
		CDki::diJoypad->Release();
		CDki::diJoypad = NULL;
	}

	CDki::diMouse->Unacquire();
	CDki::diMouse->Release();
	CDki::diMouse = NULL;

	CDki::diKeyboard->Unacquire();
	CDki::diKeyboard->Release();
	CDki::diKeyboard = NULL;

	CDki::diObject->Release();
	CDki::diObject = NULL;
#else
//        SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
}



//
// On update le tout
//
#ifdef WIN32
void			dkiUpdate(float elapsef, int width, int height)
{
	(void)elapsef; // Disable warnings
	int i;


	// On capte le clavier
	HRESULT hr; 
	hr = CDki::diKeyboard->GetDeviceState(256,(LPVOID)&(CDki::keyboardStateDI));

	// Bon, si on a perdu le focus probablement qu'on a perdu le clavier, on le repogne de même dans un while
	if (FAILED(hr))
	{
		hr = CDki::diKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST)
			hr = CDki::diKeyboard->Acquire();
	}

	// On update notre clavier
	for (i=0;i<256;i++)
	{
		if (INPUTDOWN(CDki::keyboardStateDI[i]))
		{
			if (CDki::allState[i] == DKI_NOTHING)
			{
				CDki::allState[i] = DKI_DOWN;
				CDki::lastDown = i;
				CDki::downTimer = 0;
			}
			else
			{
				CDki::allState[i] = DKI_HOLD;
			}
		}
		else
		{
			if (CDki::allState[i] == DKI_DOWN || CDki::allState[i] == DKI_HOLD)
			{
				CDki::allState[i] = DKI_UP;
			}
			else
			{
				CDki::allState[i] = DKI_NOTHING;
			}
		}
	}

	// Au fais jouer notre timer pour le down
/*	if (CDki::allState[CDki::lastDown] == DKI_HOLD)
	{
		CDki::downTimer += elapsef;
		while (CDki::downTimer > .35f)
		{
			CDki::downTimer -= .05f;
			CDki::allState[CDki::lastDown] = DKI_DOWN;
		}
	}*/

	// On capte la mouse
	hr = CDki::diMouse->GetDeviceState(sizeof(DIMOUSESTATE2),(LPVOID)&(CDki::mouseStateDI));

	// Bon, si on a perdu le focus probablement qu'on a perdu la mouse, on la repogne de même dans un while
	if (FAILED(hr))
	{
		hr = CDki::diMouse->Acquire();
        while( hr == DIERR_INPUTLOST)
			hr = CDki::diMouse->Acquire();
	}

	// On update les boutons de notre mouse
	for (i=0;i<8;i++)
	{
		if (INPUTDOWN(CDki::mouseStateDI.rgbButtons[i]))
		{
			if (CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_NOTHING)
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_DOWN;
			}
			else
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_HOLD;
			}
		}
		else
		{
			if (CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_DOWN || CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_HOLD)
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_UP;
			}
			else
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_NOTHING;
			}
		}
	}

	// La position de la sourie
	CDki::mousePos.x += CDki::mouseStateDI.lX;
	CDki::mousePos.y += CDki::mouseStateDI.lY;

	if (CDki::mousePos.x > width-1) CDki::mousePos.x = width-1;
	if (CDki::mousePos.x < 0) CDki::mousePos.x = 0;
	if (CDki::mousePos.y > height-1) CDki::mousePos.y = height-1;
	if (CDki::mousePos.y < 0) CDki::mousePos.y = 0;

	// On capte le joystick
	if (CDki::diJoypad)
	{
		hr = CDki::diJoypad->Poll() ;
		if (FAILED(hr))
		{
			hr = CDki::diJoypad->Acquire() ;
			while(hr == DIERR_INPUTLOST)
				hr = CDki::diJoypad->Acquire() ;
		}
		CDki::diJoypad->GetDeviceState(sizeof(DIJOYSTATE2), &CDki::EtatJoy) ;
	}

	// On update les boutons du joystick
	for (i=0;i<128;i++)
	{
		if (INPUTDOWN(CDki::EtatJoy.rgbButtons[i]))
		{
			if (CDki::allState[DKI_JOY_BUTTON1+i] == DKI_NOTHING)
			{
				CDki::allState[DKI_JOY_BUTTON1+i] = DKI_DOWN;
			}
			else
			{
				CDki::allState[DKI_JOY_BUTTON1+i] = DKI_HOLD;
			}
		}
		else
		{
			if (CDki::allState[DKI_JOY_BUTTON1+i] == DKI_DOWN || CDki::allState[DKI_JOY_BUTTON1+i] == DKI_HOLD)
			{
				CDki::allState[DKI_JOY_BUTTON1+i] = DKI_UP;
			}
			else
			{
				CDki::allState[DKI_JOY_BUTTON1+i] = DKI_NOTHING;
			}
		}
	}
}
#else

void dkiUpdate(float elapsef, int width, int height)
{
	int i;

	// On capte le clavier
	dkwGetKeysState((unsigned char *) CDki::keyboardStateDI, sizeof(CDki::keyboardStateDI));

	// On update notre clavier
	for (i=0;i<256;i++)
	{
		if (INPUTDOWN(CDki::keyboardStateDI[i]))
		{
			if (CDki::allState[i] == DKI_NOTHING)
			{
				CDki::allState[i] = DKI_DOWN;
				CDki::lastDown = i;
				CDki::downTimer = 0;
			}
			else
			{
				CDki::allState[i] = DKI_HOLD;
			}
		}
		else
		{
			if (CDki::allState[i] == DKI_DOWN || CDki::allState[i] == DKI_HOLD)
			{
				CDki::allState[i] = DKI_UP;
			}
			else
			{
				CDki::allState[i] = DKI_NOTHING;
			}
		}
	}

	// On capte la mouse

	dkwGetMouseState(& CDki::mouseStateDI);

	// On update les boutons de notre mouse
	for (i=0;i<8;i++)
	{
		if (INPUTDOWN(CDki::mouseStateDI.rgbButtons[i]))
		{
			if (CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_NOTHING)
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_DOWN;
			}
			else
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_HOLD;
			}
		}
		else
		{
			if (CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_DOWN || CDki::allState[DKI_MOUSE_BUTTON1+i] == DKI_HOLD)
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_UP;
			}
			else
			{
				CDki::allState[DKI_MOUSE_BUTTON1+i] = DKI_NOTHING;
			}
		}
	}

	// La position de la sourie
	CDki::mousePos.x += CDki::mouseStateDI.lX;
	CDki::mousePos.y += CDki::mouseStateDI.lY;

	if (CDki::mousePos.x > width-1) CDki::mousePos.x = width-1;
	if (CDki::mousePos.x < 0) CDki::mousePos.x = 0;
	if (CDki::mousePos.y > height-1) CDki::mousePos.y = height-1;
	if (CDki::mousePos.y < 0) CDki::mousePos.y = 0;

}

#endif



//
// Setter manuellement la position du curseur
//
void			dkiSetMouse(CVector2i & mousePos)
{
	CDki::mousePos.x = mousePos[0];
	CDki::mousePos.y = mousePos[1];
}
