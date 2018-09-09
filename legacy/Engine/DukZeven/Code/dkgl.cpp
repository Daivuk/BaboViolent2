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


#include "dkgli.h"
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

int CDkgl::colorDepth=16;

#ifndef _DX_
	HGLRC CDkgl::renderingContext;
	HDC CDkgl::deviceContext;
#else
	IDirect3D9 *g_D3D=NULL;
	IDirect3DDevice9 *g_device=NULL;
#endif
/*
IDirect3D9 *g_D3D=NULL;

   g_D3D = Direct3DCreate9( D3D_SDK_VERSION);
   if(!g_D3D){
      //Handle error
   }

   //Then at the end of your application
   if(g_D3D){
      g_D3D->Release();
      g_D3D=NULL;
   }*/


void dkglEnableVsync(bool vsync)
{
#ifndef _DX_
	// if we want vsync, check if video card supports the extension
	if( vsync )
	{

		if( dkglCheckExtension( "WGL_EXT_swap_control" ) )
		{
#ifdef WIN32
			typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
			PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
			if( wglSwapIntervalEXT )
			{
				wglSwapIntervalEXT( 1 );
			}
#else
			//dlopen() ? libGLX.so
			//glXSwapIntervalSGI(1);
#endif
		}
	}
#endif
}

bool CheckExtension( char *m_szSupportedGLExtensions, char* szExtensionName )
{
#ifndef _DX_
	unsigned int uiNextExtension;
	char*		 szSupExt= m_szSupportedGLExtensions;
	char*		 cEndExtensions;

	//find the end of the extension list
	cEndExtensions= szSupExt+strlen( m_szSupportedGLExtensions );

	//search through the entire list
	while( szSupExt<cEndExtensions )
	{
		//find the next extension in the list
		uiNextExtension= (unsigned int)(strcspn( szSupExt, " " ));

		//check the extension to the one given in the argument list
		if( ( strlen( szExtensionName )==uiNextExtension ) && 
			( strncmp( szExtensionName, szSupExt, uiNextExtension )==0 ) )
		{
			return true;
		}

		//move to the nexte extension in the list
		szSupExt+= ( uiNextExtension+1 );
	}
#endif
	return false;
}


//
// Pour vérifier une extension de la carte
//
bool			 dkglCheckExtension(char * extension)
{
#ifndef _DX_
	char * exts = (char*)glGetString(GL_EXTENSIONS);
	return !(strstr(exts, extension) == 0);
#else
	return false;
#endif
}



//
// Pour créer le context openGL (rendering context)
//
int				 dkglCreateContext(
#ifndef _DX_
								   HDC mDC, int colorDepth
#else
								   HWND wnd, bool fullScreen, int width, int height
#endif
								   )
{
#ifndef _DX_
	CDkgl::colorDepth = colorDepth;
	CDkgl::deviceContext = mDC;

	// On init notre pixel format
	if (initPixelFormat(CDkgl::deviceContext, CDkgl::colorDepth)==0) return 0;

	// On cré un rendering context
	CDkgl::renderingContext = 
#ifdef WIN32
	  wglCreateContext(CDkgl::deviceContext);
#else
	mDC;
#endif

	// On le met comme courant
#ifdef WIN32
	wglMakeCurrent(CDkgl::deviceContext, CDkgl::renderingContext); // On le met comme device courant

#else
	//	SDL_Init(SDL_INIT_VIDEO);
#endif
	// On check le extensions
//	CDkgl::extensions = (char*)glGetString(GL_EXTENSIONS);
#else

   g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
   if (!g_D3D)
   {
	   return 0;
   }

   D3DFORMAT format = D3DFMT_A8R8G8B8;
   D3DPRESENT_PARAMETERS pp;
   HRESULT hr;

	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));

	pp.BackBufferCount = 1;
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pp.MultiSampleQuality = 0;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.hDeviceWindow = wnd;
	pp.Flags = 0;
	pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	pp.BackBufferFormat = format;
	pp.AutoDepthStencilFormat = D3DFMT_D16;
	pp.EnableAutoDepthStencil = TRUE;

	if (fullScreen)
	{
		pp.Windowed          = FALSE;
		pp.BackBufferWidth   = width;
		pp.BackBufferHeight  = height;
	}
	else
	{
		pp.Windowed          = TRUE;
	}

	hr = g_D3D->CreateDevice(
						D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						wnd,
						D3DCREATE_HARDWARE_VERTEXPROCESSING,
						&pp,
						&g_device);
	if(FAILED(hr))
	{
		return 0;
	}


#endif
	return 1;
}

#ifdef _DX_
IDirect3DDevice9* dkglGetDXDevice()
{
	return g_device;
}
#endif


//
// Pour dessiner le system de coordonnée
//
void			 dkglDrawCoordSystem()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
		glLineWidth(2);
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(1,0,0);

			glVertex3f(1,0,0);
			glVertex3f(.9f,.1f,.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,-.1f,.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,-.1f,-.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,.1f,-.1f);
		glEnd();
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,1,0);

			glVertex3f(0,1,0);
			glVertex3f(.1f,.9f,.1f);
			glVertex3f(0,1,0);
			glVertex3f(-.1f,.9f,.1f);
			glVertex3f(0,1,0);
			glVertex3f(-.1f,.9f,-.1f);
			glVertex3f(0,1,0);
			glVertex3f(.1f,.9f,-.1f);
		glEnd();
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1);

			glVertex3f(0,0,1);
			glVertex3f(.1f,.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(-.1f,.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(-.1f,-.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(.1f,-.1f,.9f);
		glEnd();
	glPopAttrib();
#endif
}



//
// Draw wire cube
//
void			 dkglDrawWireCube()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
		glLineWidth(2);
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1,1,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(-1,-1,-1);
			glVertex3f(-1,1,-1);
			glVertex3f(1,1,-1);
			glVertex3f(1,-1,-1);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(-1,1,1);
			glVertex3f(-1,-1,1);
			glVertex3f(1,-1,1);
			glVertex3f(1,1,1);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(-1,1,1);
			glVertex3f(-1,1,-1);
			glVertex3f(-1,-1,1);
			glVertex3f(-1,-1,-1);
			glVertex3f(1,-1,1);
			glVertex3f(1,-1,-1);
			glVertex3f(1,1,1);
			glVertex3f(1,1,-1);
		glEnd();
	glPopAttrib();
#endif
}



//
// Pour revenir en vue 3D
//
void			 dkglPopOrtho()
{
#ifndef _DX_
	// On pop nos matrice
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// On pop nos attribs
	glPopAttrib();
#endif
}



//
// Pour setter la vue en 2D
//
void			 dkglPushOrtho(float mWidth, float mHeight)
{
#ifndef _DX_
	// On push les attribs pour certaines modifications
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
//	glCullFace(GL_BACK);

	// En mode 2D on ne veux pas de z-buffer
	glDisable(GL_DEPTH_TEST);

	// On push nos matrice et on set la matrice ortho
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0,mWidth,mHeight,0,-9999,9999);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
#endif
}



//
// Pour setter la fonction de blending
//
void			 dkglSetBlendingFunc(int blending)
{
#ifndef _DX_
	switch (blending)
	{
	case DKGL_BLENDING_ADD_SATURATE:
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case DKGL_BLENDING_ADD:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case DKGL_BLENDING_MULTIPLY:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;
	case DKGL_BLENDING_ALPHA:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	}
#endif
}



//
// Pour rapidement créer une lumière dans votre scene (très basic)
//
void			 dkglSetPointLight(int ID, float x, float y, float z, float r, float g, float b)
{
#ifndef _DX_
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0 + ID);
	float pos[] = {x,y,z,1};
	float amb[] = {r/6,g/6,b/6,1};
	float diff[] = {r,g,b,1};
	float spec[] = {r,g,b,1};

	glLightfv(GL_LIGHT0 + ID, GL_POSITION, pos);
	glLightfv(GL_LIGHT0 + ID, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0 + ID, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0 + ID, GL_SPECULAR, spec);
#endif
}



//
// On set la vue de la perspective là
//
void			 dkglSetProjection(float mFieldOfView, float mNear, float mFar, float mWidth, float mHeight)
{
#ifndef _DX_
	// On met la matrice de projection pour ce créer une vue perspective
	glMatrixMode(GL_PROJECTION);

	// On remet cette matrice à identity
	glLoadIdentity();

	// On ajuste la matrice de projection
	gluPerspective(mFieldOfView, mWidth/mHeight, mNear, mFar);

	// On remet cette de model view (qui est celle de la position et l'orientation)
	glMatrixMode(GL_MODELVIEW);

	// La model view à identity
	glLoadIdentity();
#endif
}



//
// Pour bien fermer tout ça
//
void			 dkglShutDown()
{
#ifdef WIN32
#ifndef _DX_
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(CDkgl::renderingContext);
#else

   if (g_D3D)
   {
      g_D3D->Release();
      g_D3D = NULL;
   }

#endif
#else
	//	SDL_Quit();
#endif

}



//
// Pour initialiser le format des pixel à l'écran
//
#ifndef _DX_
int initPixelFormat(HDC mDC, int colorDepth)
{
#ifdef WIN32
	// on défini le format des pixels
	PIXELFORMATDESCRIPTOR pfd=
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			(unsigned char)colorDepth,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24, // z-buffer
			0, // stencil-buffer
			PFD_MAIN_PLANE,
			0,
			0,
			0, 0, 0
		};	

	// On set le pixel
	return SetPixelFormat(mDC, ChoosePixelFormat(mDC, &pfd), &pfd);
#else
	return 1;
#endif
}
#endif



//
// Pour projeter la mouse ou un point 2D quelconque
//
CVector3f		 dkglUnProject(CVector2i & pos2D, float zRange)
{
	double x,y,z;
#ifndef _DX_
	CVector3f pos((float)pos2D[0], (float)pos2D[1], zRange);
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint    viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject (
		pos[0], 
		pos[1], 
		pos[2], 
		modelMatrix, 
		projMatrix, 
		viewport, 
		&x, 
		&y, 
		&z);
#endif

	return CVector3f((float)x,(float)y,(float)z);
}


CVector3f		 dkglProject(CVector3f & pos3D)
{
	double x,y,z;
#ifndef _DX_
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint    viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject (
		pos3D[0], 
		pos3D[1], 
		pos3D[2], 
		modelMatrix, 
		projMatrix, 
		viewport, 
		&x, 
		&y, 
		&z);
#endif
	return CVector3f((float)x,(float)y,(float)z);
}
