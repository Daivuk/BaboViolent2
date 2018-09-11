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


#include "BrebisGL.h"
#include "dkwi.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <string>
#include "dki.h"

static std::string last_error = "";
static SDL_Window* window = nullptr;
static SDL_GLContext gl_context = nullptr;
static bool done = true;
static CMainLoopInterface *mainLoopObject = nullptr;

//
// La plus importante. Cré la fenêtre et init les cossin
//
int dkwInit(int width, int height, int mcolorDepth, char* mTitle, CMainLoopInterface *mMainLoopObject, bool fullScreen, int refreshRate)
{
    mainLoopObject = mMainLoopObject;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        last_error = std::string("Error: ") + SDL_GetError() + "\n";
        return 0;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullScreen) flags |= SDL_WINDOW_FULLSCREEN;
    window = SDL_CreateWindow(mTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if (brebisGLInit() == KHRONOS_FALSE)
    {
        last_error = "Failed to initialize BrebisGL\n";
        return 0;
    }

    done = false;

    return 1;
}

//
// Pour forcer l'application à fermer
//
void dkwForceQuit()
{
    done = true;
}

//
// On obtien le Device Context de la fenetre
//
SDL_GLContext dkwGetDC()
{
    return gl_context;
}

//
// On obtien le handle de la fenêtre
//
SDL_Window* dkwGetHandle()
{
    return window;
}

//
// Obtenir la dernière erreur
//
char* dkwGetLastError()
{
    return (char*)last_error.c_str();
}

//
// Pour retourner la position de la sourie sur l'écran
//
CVector2i dkwGetCursorPos()
{
    return dkiGetMouse();
}

//
// On retourne la résolution de la fenêtre
//
CVector2i dkwGetResolution()
{
    SDL_Window* pWindow = SDL_GL_GetCurrentWindow();
    int w, h;
    SDL_GetWindowSize(pWindow, &w, &h);

    return { w, h };
}

// On clip la mouse au window rect
void dkwClipMouse( bool abEnabled )
{
}

//
// On effectu le loop principal de l'application
//
int dkwMainLoop()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;

    int(*sdlEventCall)(SDL_Event * event) = SDL_WaitEvent;

    while (SDL_PollEvent(&event))
    {
        //ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            done = true;
        //if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        //{
        //    width = event.window.data1;
        //    height = event.window.data2;
        //}
    }

    mainLoopObject->paint();

	return done ? 0 : 1;
}

//
// Pour shutdowner le tout
//
void dkwShutDown()
{
}

//
// Pour forcer un update des messages (meton pendant un loading)
//
void dkwUpdate()
{
}
