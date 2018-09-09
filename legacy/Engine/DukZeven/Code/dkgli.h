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


#ifndef DKGLI_H
#define DKGLI_H



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
	#ifndef _DX_
		#pragma comment( lib, "opengl32.lib" )
		#pragma comment( lib, "glu32.lib" )
	#else
		#pragma comment (lib, "d3d9.lib" )
	#endif


	#include <windows.h>

	// Les includes pour opengl
	#ifndef _DX_
		#include <gl/gl.h> 
		#include <gl/glu.h> 
		#include "glext.h" 
	#else
		#include "d3d9.h"
	#endif
#else
	#include "linux_types.h"
	#ifdef __MACOSX__
	#include <SDL.h>
	#else
	#include <SDL/SDL.h>
	#endif
	#include "LinuxHeader.h"
	#ifdef __MACOSX__
	#include <SDL_opengl.h>
	#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
	#endif
#endif

#include "CString.h"
#include "CVector.h"



const int DKGL_BLENDING_ADD_SATURATE = 0;
const int DKGL_BLENDING_ADD = 3;
const int DKGL_BLENDING_MULTIPLY = 1;
const int DKGL_BLENDING_ALPHA = 2;



// Les fonction du DKGL
DLL_API void			 dkglEnableVsync(bool enabled = true);
DLL_API bool			 dkglCheckExtension(char * extension);
DLL_API int				 dkglCreateContext(
#ifndef _DX_
								   HDC mDC, int colorDepth
#else
								   HWND wnd, bool fullScreen, int width, int height
#endif
	);
DLL_API void			 dkglDrawCoordSystem();
DLL_API void			 dkglDrawWireCube();
DLL_API void			 dkglPopOrtho();
DLL_API void			 dkglPushOrtho(float mWidth, float mHeight);
DLL_API void			 dkglSetBlendingFunc(int blending);
DLL_API void			 dkglSetPointLight(int ID, float x, float y, float z, float r, float g, float b);
DLL_API void			 dkglSetProjection(float mFieldOfView, float mNear, float mFar, float mWidth, float mHeight);
DLL_API void			 dkglShutDown();
DLL_API CVector3f		 dkglUnProject(CVector2i & pos2D, float zRange);
DLL_API CVector3f		 dkglProject(CVector3f & pos3D);

#ifdef _DX_
DLL_API IDirect3DDevice9* dkglGetDXDevice();
#endif



// Pour initialiser le format des pixel
int initPixelFormat(HDC mDC, int colorDepth);


// Pour tenir certaines infos
class CDkgl
{
public:
	// Notre rendering context
	static HGLRC renderingContext;

	// La profondeur des couleur
	static int colorDepth;

	// Le device context
	static HDC deviceContext;

public:
};


#endif
