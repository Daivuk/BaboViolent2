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
#include "dkw.h"

#include <memory.h>

static CVector2i mousePos(0, 0);
static SDL_Window* window;
static int allState[256 + 8 + 128];
static int lastDown = 0;
static float downTimer = 0;

//
// Pour obtenir la premi�re touche press� (utile pour setter les touche dans les options)
//
int dkiGetFirstDown()
{
	for (int i=0;i<256+8+128;i++)
	{
		if (allState[i] == DKI_DOWN)
		{
			return i;
		}
	}

	return DKI_NOKEY;
}



//
// On pogne la wheel mouse
//
int dkiGetMouseWheelVel()
{
    return 0;// CDki::mouseStateDI.lZ;
}



//
// On capte la mouse
//
CVector2i dkiGetMouse()
{
	return mousePos;
}

CVector2i dkiGetMouseVel()
{
	return CVector2i(0, 0);
}



//
// Pour obtenir l'�tat d'une touche ou d'un bouton
//
int dkiGetState(int inputID)
{
    int r = DKI_NOTHING;

	if(inputID != DKI_NOKEY)
	  r = allState[inputID];

	return r;
}


//
// On capte le joystick
//
CVector3f dkiGetJoy()
{
	return CVector3f(0, 0, 0);
}

CVector3f dkiGetJoyR()
{
	return CVector3f(0, 0, 0);
}

CVector3f dkiGetJoyVel()
{
	return CVector3f(0, 0, 0);
}

//
// Pour initialiser le API
//
int dkiInit(SDL_Window* appHandle)
{
    window = appHandle;
    memset(allState, 0, sizeof(allState));
	return 1;
}

//
// Pour bien fermer le tout
//
void dkiShutDown()
{
}

//
// On update le tout
//
void dkiUpdate(float elapsef, int width, int height)
{
    // SDL_GetMouseState() gives mouse position seemingly based on the last window entered/focused(?)
    // The creation of a new windows at runtime and SDL_CaptureMouse both seems to severely mess up with that, so we retrieve that position globally.
    int mx, my;
    Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);


    (void)elapsef; // Disable warnings
    int i;

    // On capte le clavier
    int numKeys;
    auto kbState = SDL_GetKeyboardState(&numKeys);

    // On update notre clavier
    for (i = 0; i<256 && i < numKeys; i++)
    {
        if (kbState[i])
        {
            if (allState[i] == DKI_NOTHING)
            {
                allState[i] = DKI_DOWN;
                lastDown = i;
                downTimer = 0;
            }
            else
            {
                allState[i] = DKI_HOLD;
            }
        }
        else
        {
            if (allState[i] == DKI_DOWN || allState[i] == DKI_HOLD)
            {
                allState[i] = DKI_UP;
            }
            else
            {
                allState[i] = DKI_NOTHING;
            }
        }
    }

    // On update les boutons de notre mouse
    bool mouseDown[3] = {
        (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
        (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
        (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0
    };
    for (i = 0; i<3; i++)
    {
        if (mouseDown[i])
        {
            if (allState[DKI_MOUSE_BUTTON1 + i] == DKI_NOTHING)
            {
                allState[DKI_MOUSE_BUTTON1 + i] = DKI_DOWN;
            }
            else
            {
                allState[DKI_MOUSE_BUTTON1 + i] = DKI_HOLD;
            }
        }
        else
        {
            if (allState[DKI_MOUSE_BUTTON1 + i] == DKI_DOWN || allState[DKI_MOUSE_BUTTON1 + i] == DKI_HOLD)
            {
                allState[DKI_MOUSE_BUTTON1 + i] = DKI_UP;
            }
            else
            {
                allState[DKI_MOUSE_BUTTON1 + i] = DKI_NOTHING;
            }
        }
    }

    // La position de la sourie
    int wx, wy;
    SDL_Window* focused_window = SDL_GetKeyboardFocus();
    if (window == focused_window)
    {
        SDL_GetWindowPosition(window, &wx, &wy);
        SDL_GetGlobalMouseState(&mx, &my);
        mx -= wx;
        my -= wy;
    }

    mousePos[0] = mx;
    mousePos[1] = my;

    //if (CDki::mousePos.x > width - 1) CDki::mousePos.x = width - 1;
    //if (CDki::mousePos.x < 0) CDki::mousePos.x = 0;
    //if (CDki::mousePos.y > height - 1) CDki::mousePos.y = height - 1;
    //if (CDki::mousePos.y < 0) CDki::mousePos.y = 0;

    // On capte le joystick
    //if (CDki::diJoypad)
    //{
    //    hr = CDki::diJoypad->Poll();
    //    if (FAILED(hr))
    //    {
    //        hr = CDki::diJoypad->Acquire();
    //        while (hr == DIERR_INPUTLOST)
    //            hr = CDki::diJoypad->Acquire();
    //    }
    //    CDki::diJoypad->GetDeviceState(sizeof(DIJOYSTATE2), &CDki::EtatJoy);
    //}

    // On update les boutons du joystick
    //for (i = 0; i<128; i++)
    //{
    //    if (INPUTDOWN(CDki::EtatJoy.rgbButtons[i]))
    //    {
    //        if (CDki::allState[DKI_JOY_BUTTON1 + i] == DKI_NOTHING)
    //        {
    //            CDki::allState[DKI_JOY_BUTTON1 + i] = DKI_DOWN;
    //        }
    //        else
    //        {
    //            CDki::allState[DKI_JOY_BUTTON1 + i] = DKI_HOLD;
    //        }
    //    }
    //    else
    //    {
    //        if (CDki::allState[DKI_JOY_BUTTON1 + i] == DKI_DOWN || CDki::allState[DKI_JOY_BUTTON1 + i] == DKI_HOLD)
    //        {
    //            CDki::allState[DKI_JOY_BUTTON1 + i] = DKI_UP;
    //        }
    //        else
    //        {
    //            CDki::allState[DKI_JOY_BUTTON1 + i] = DKI_NOTHING;
    //        }
    //    }
    //}
}

//
// Setter manuellement la position du curseur
//
void dkiSetMouse(CVector2i & mousePos)
{
}
