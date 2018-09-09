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


#include "dkwi.h"
#include <string.h>
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif
#ifndef WIN32
#include "linux_types.h"
#ifdef __MACOSX__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <time.h>
#include <signal.h>
#endif


//
// Les trucs static
//
char *CDkw::lastErrorString = 0;
int CDkw::w = 640;
int CDkw::h = 480;
int CDkw::colorDepth = 16;
#ifndef WIN32
unsigned long CDkw::flags = SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF | SDL_ASYNCBLIT  | SDL_OPENGLBLIT | SDL_HWACCEL;
#endif

char *CDkw::title = 0;
HINSTANCE CDkw::hInstance;
HWND CDkw::hWnd;
HDC CDkw::dc;
bool CDkw::running = true;
CMainLoopInterface *CDkw::mainLoopObject = 0;
CVector2i CDkw::cursorPos;
bool done = false;
#ifndef WIN32
static unsigned long clear_counter = 0;
DIMOUSESTATE2 CDkw::mouse_state;
unsigned char CDkw::keys_state[256+128+8];
static unsigned int shift_char_map[21][2] = 
{
    {SDLK_0, SDLK_RIGHTPAREN},
    {SDLK_1, SDLK_EXCLAIM},
    {SDLK_2, 64},
    {SDLK_3, SDLK_HASH},
    {SDLK_4, SDLK_DOLLAR},
    {SDLK_5, 37},
    {SDLK_6, SDLK_CARET},
    {SDLK_7, SDLK_AMPERSAND},
    {SDLK_8, SDLK_ASTERISK},
    {SDLK_9, SDLK_LEFTPAREN},
    {SDLK_MINUS, SDLK_UNDERSCORE},
    {SDLK_EQUALS, SDLK_PLUS},
    {SDLK_BACKQUOTE, 126},
    {SDLK_COMMA, SDLK_LESS},
    {SDLK_PERIOD, SDLK_GREATER},
    {SDLK_SLASH, SDLK_QUESTION},
    {SDLK_SEMICOLON, SDLK_COLON},
    {SDLK_BACKSLASH, 124},
    {SDLK_LEFTBRACKET, 123},
    {SDLK_RIGHTBRACKET, 125},
    {SDLK_QUOTE, SDLK_QUOTEDBL}
};
#endif




/// does window has the focus
static bool HasFocus = true;

///////////////////////////////////////////////////////////////////////////////////////
/// \brief Mise à jour de l'erreur
///
/// Fonction qui met à jour le message d'erreur (lastErrorString) relatif à la fenêtre.
///
/// \param error : Message d'erreur
///
/// \return Aucune
///////////////////////////////////////////////////////////////////////////////////////
void CDkw::updateLastError(char *error)
{
	//Si le message n'est pas null (il y a déja eu une erreur)
	if(lastErrorString)
	{
		delete [] lastErrorString;
	}
	if(error)
	{
	  lastErrorString = new char [strlen(error)+1];
	  strcpy(lastErrorString, error); //E.P memcpy plus efficace...
	}
}



////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui reçoit tous les inputs dirigés à la fenêtre
///
/// Fonction de Callback utilisée par Windows pour gérer les différents inputs
///	dirigés à la fenêtre. Un pointeur vers cette fontion est passé en paramètre lors de la
///	création de la "windows class" (WNDCLASS).
///
/// \param hWnd : Handle unique de la fenêtre.
/// \param uMsg : Message que la fenêtre a reçu.
/// \param wParam : Variable utilisée pour passer des paramètres.
/// \param lParam : Variable utilisée pour passer des paramètres.
///
/// \return LRESULT : Est égal à un long
///
/// \note
/// CALLBACK est égal à _stdcall
///
/// \par 
/// Il faut se rappeler que plusieurs fenêtre pourraient être créées de la même definition 
/// de "classe window". Donc en créant deux fenêtres de la même classe, chacune va posséder son 
/// propre handle mais chacune va appeler cette fontion avec des messages. En général, une seule
/// fenêtre est créée à partir de la classe window.
///
///	\see
/// http://www.toymaker.info/Games/html/wndproc.html# \n
/// http://www.newty.de/fpt/index.html
////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
LRESULT CALLBACK CDkw::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}
	case WM_CLOSE:
		// On envoit le message pour quitter
		PostQuitMessage(0);
		return 0;
		break;
	case WM_PAINT:
		// On effectue le rendu
		if (mainLoopObject) mainLoopObject->paint();
		return 0;
		break;
	case WM_KEYDOWN:
		// Une touche est pesée
		return 0;
		break;
	case WM_KEYUP:
		// Une touche est releasée
		return 0;
		break;
	case WM_MOUSEMOVE:
		// On pogne la position de la mouse sur la fenêtre
		cursorPos[0] = LOWORD(lParam);
		cursorPos[1] = HIWORD(lParam);
		return 0;
		break;
	case WM_SIZE:

		// On resize la fenêtre
		w = LOWORD(lParam);
		h = HIWORD(lParam);

		// on reajuste les infos sur le width pi height
		POINT res;
		res.x = GetSystemMetrics(SM_CXSCREEN);
		res.y = GetSystemMetrics(SM_CYSCREEN);

		m_WindowRect.left = res.x/2-w/2;
		m_WindowRect.top = res.y/2-h/2;
		m_WindowRect.right = res.x/2-w/2 + w;
		m_WindowRect.bottom = res.y/2-h/2 + h;

		return 0;
		break;
	case WM_CHAR:
		// Entrées texte
		if (mainLoopObject) mainLoopObject->textWrite(unsigned int(wParam));
		return 0;
		break;
	case WM_MOVE:
		
		m_WindowRect.left = (int)(short) LOWORD(lParam);   // horizontal position 
		m_WindowRect.top = (int)(short) HIWORD(lParam);   // vertical position 

		m_WindowRect.right = m_WindowRect.left + w;
		m_WindowRect.bottom = m_WindowRect.top + h;

		return 0;
		break;
	case WM_ACTIVATE:
		
		if( wParam == WA_INACTIVE )
		{
			dkwClipMouse( false );
			HasFocus = false;
		}
		else
		{
			HasFocus = true;
		}
		return 0;
		break;
/*	default:
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
		break;*/
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);



/*	switch (uMsg)
	{
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_CLOSE:
		{
		PostQuitMessage(0);
		return 0;
		}
		case WM_MOUSEMOVE:
		{
		gkConsole.MouseMove(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); //on dit a la cosnoel qu'il y a deplacement de la souris
		return 0;
		}
		case WM_MBUTTONDOWN:
		{
		gkConsole.Mouse->SetMouseState(1,true); //on dit a la souris que le piton du milieu est peser
		return 0;
		}
		case WM_MBUTTONUP:
		{
		gkConsole.Mouse->SetMouseState(7,false); //certainement qu'on n'est pas entrain de tenir le middle click
		gkConsole.Mouse->SetMouseState(4,true); //on vien de lever le piton middle de la souris
		return 0;
		}
		case WM_LBUTTONDOWN:
		{
		gkConsole.Mouse->SetMouseState(0,true); //on dit a la souris que le piton du milieu est peser
		return 0;
		}
		case WM_LBUTTONUP:
		{
		gkConsole.Mouse->SetMouseState(6,false); //certainement qu'on n'est pas entrain de tenir le left click
		gkConsole.Mouse->SetMouseState(3,true); //on vien de lever le piton gauche de la souris

		return 0;
		}
		case WM_RBUTTONDOWN:
		{
		gkConsole.Mouse->SetMouseState(2,true); //on est on mouse button down
		return 0;
		}
		case WM_RBUTTONUP:
		{
		gkConsole.Mouse->SetMouseState(8,false); //certainement qu'on n'est pas entrain de tenir le left click
		gkConsole.Mouse->SetMouseState(5,true); //on vien de lever le piton gauche de la souris

		return 0;
		}
		case 0x020A: //MouseWheel
		{
		gkConsole.MouseWheel((int)wParam > 0 ? true : false);
		return 0;
		}
		case WM_KEYDOWN:
		{
		fprintf(gkConsole.fp,"Key Ascii DOWN : %i \n",(int)wParam); // on print le keydown dans le fichier debug.txt
		gkConsole.changeKeyState(1,(int)wParam);
		return 0;
		}

		case WM_KEYUP:
		{
		fprintf(gkConsole.fp,"Key Ascii UP : %i \n",(int)wParam); // pour le debogage seulement
		gkConsole.changeKeyState(0,(int)wParam);
		return 0;
		}

		case WM_SIZE:
		{
		gkGraphics.ReSize(LOWORD(lParam),HIWORD(lParam));
		return 0;
		}
		}
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
		*/
}

#else
LRESULT CALLBACK CDkw::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT r = 0;
  unsigned char * buttons;
  const unsigned int bmasks[3] = {SDL_BUTTON_LMASK, SDL_BUTTON_RMASK, SDL_BUTTON_MMASK};
  int i;
  unsigned char bvalue = 0;



  SDL_Event * e = (SDL_Event *) lParam;
  if(e)
	{
	  switch(e->type)
		{
		case SDL_QUIT:
		  r = 1;
		  break;
		case SDL_ACTIVEEVENT:
		  {
			SDL_ActiveEvent & ae = e->active;
			if((ae.gain == 0) && (ae.state & SDL_APPACTIVE))
			  {
				dkwClipMouse( false );
				HasFocus = false;
			  }
			else if(ae.gain && (ae.state & SDL_APPACTIVE))
			  {
				HasFocus = true;
			  }
		  }
		  break;
		case SDL_VIDEOEXPOSE:
            if(mainLoopObject)
                mainLoopObject->paint();

            if(!clear_counter)
             {
                 // a workaround. We do not read the mouse state as in DX so we must clear the buffers at some time.
                 CDkw::mouse_state.lZ = 0;
                 clear_counter = 10;
             }
             else
                 clear_counter --;
            //SDL_PumpEvents();
		  break;
		case SDL_VIDEORESIZE:
		  {
			SDL_ResizeEvent & re = e->resize;
			SDL_Surface * s = (SDL_Surface *) hWnd;
			w = re.w;
			h = re.h;

			// on reajuste les infos sur le width pi height
			POINT res;
			res.x = s->w;
			res.y = s->h;
			
			m_WindowRect.left = res.x/2 - w/2;
			m_WindowRect.top = res.y/2 - h/2;
			m_WindowRect.right = res.x/2 - w/2 + w;
			m_WindowRect.bottom = res.y/2 - h/2 + h;
		  }

		  break;
		case SDL_MOUSEMOTION:
		  {
			SDL_MouseMotionEvent & m = e->motion;
			cursorPos[0] = m.x; //x-coord
			cursorPos[1] = m.y; //y-coord


			buttons = CDkw::mouse_state.rgbButtons;

 			CDkw::mouse_state.lX = m.xrel;
			CDkw::mouse_state.lY = m.yrel;
			
			for(i = 0; i < sizeof(bmasks) / sizeof(bmasks[0]); i ++)
			  buttons[i] = (m.state & bmasks[i]) ? 0x80 : 0;

		  }
		  break;
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
		  {  
			long & zaxis = CDkw::mouse_state.lZ;
			buttons = CDkw::mouse_state.rgbButtons;
			SDL_MouseButtonEvent & b = e->button;
			
			if(b.state == SDL_PRESSED)
                bvalue = 0x80;
			else if(b.state == SDL_RELEASED)
                bvalue = 0;
			
			switch(b.button)
			  {
			  case SDL_BUTTON_WHEELUP:
				zaxis = 1;
				break;
			  case SDL_BUTTON_WHEELDOWN:
				zaxis = -1;
				break;
			  case SDL_BUTTON_MIDDLE:
				buttons = & buttons[2];
				break;
			  case SDL_BUTTON_RIGHT:
				buttons = & buttons[1];
				break;
			  default:
				// left button is here
				break;
			  }
			*buttons = bvalue;
			// we don't need absolute mouse coordinates at press/release time
		  }

		  break;

        case SDL_KEYDOWN:
         {
            SDL_KeyboardEvent & k = e->key;
            unsigned int key = k.keysym.sym;
            CDkw::keys_state[k.keysym.sym] = 0x80;

            if(CDkw::keys_state[SDLK_LSHIFT] || CDkw::keys_state[SDLK_RSHIFT])
            {
                if((key >= SDLK_a) && (key <= SDLK_z))
                    key &= ~0x20; 
                else
                {
                    for(i = 0; i < sizeof(shift_char_map) / sizeof(shift_char_map[0]); i ++)
                    {
                        if(shift_char_map[i][0] == key)
                        {
                            key = shift_char_map[i][1];
                            break;
                        }
                    }
                }
                
            }

            if(mainLoopObject) 
                mainLoopObject->textWrite(key);
            if(CDkw::keys_state[SDLK_c] && CDkw::keys_state[SDLK_LCTRL])
                raise(SIGINT);
          }
            break;

        case SDL_KEYUP:
		  {
			SDL_KeyboardEvent & k = e->key;
            CDkw::keys_state[k.keysym.sym] = 0;
		  }
		  break;

		default:
		  break;
		}
	}
  return r;
}
#endif


//
// La plus importante. Cré la fenêtre et init les cossin
//
#ifdef WIN32
int dkwInit(int width, int height, int mcolorDepth, char* mTitle, CMainLoopInterface *mMainLoopObject, bool fullScreen, int refreshRate)
{
	CDkw::mainLoopObject = mMainLoopObject;

	// On set ses propriétées de bases
	CDkw::w = width;
	CDkw::h = height;
	CDkw::colorDepth = mcolorDepth;
	if (mTitle)
	{
		CDkw::title = new char [strlen(mTitle)+1];
		strcpy(CDkw::title, mTitle);
	}

	// On se cré une instance
	CDkw::hInstance = GetModuleHandle(NULL);

	// On défini la windows class
	WNDCLASS wc = {CS_HREDRAW | CS_VREDRAW | CS_OWNDC, (WNDPROC) CDkw::WndProc, 0, 0,
		CDkw::hInstance, LoadIcon(NULL, IDI_WINLOGO), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(GRAY_BRUSH), NULL, "dkw"};
	

	// On enregistre la class
	if (!RegisterClass(&wc)) 
	{
		CDkw::updateLastError("Can not register window class");
		return 0;
	}

	// On va la centrer
	POINT res;
	res.x = GetSystemMetrics(SM_CXSCREEN);
	res.y = GetSystemMetrics(SM_CYSCREEN);

	// Le style de la fenetre en fonction de windows ou full screen
//	DWORD dwExStyle = WS_EX_APPWINDOW;
	DWORD dwStyle = WS_SYSMENU | WS_BORDER | /*WS_MINIMIZEBOX | */WS_MAXIMIZEBOX;
	m_WindowRect.left = res.x/2-width/2;
	m_WindowRect.top = res.y/2-height/2;
	m_WindowRect.right = res.x/2-width/2 + width;
	m_WindowRect.bottom = res.y/2-height/2 + height;
	if (fullScreen)
	{
		// On set les cossains pour le fullscreen
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth		= width;
		dmScreenSettings.dmPelsHeight		= height;
		dmScreenSettings.dmBitsPerPel		= mcolorDepth;
		if (refreshRate != -1 && fullScreen == true)
		{
			dmScreenSettings.dmDisplayFrequency	= refreshRate;
			dmScreenSettings.dmFields = DM_DISPLAYFREQUENCY;
		}
		else
			dmScreenSettings.dmFields = 0;
		dmScreenSettings.dmFields |= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// On switch en fullscreen
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			CDkw::updateLastError("Failled to switch in fullscreen mode");
			return 0;
		}

		// On set le style de fenetre
		//dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;// | WS_OVERLAPPEDWINDOW;

		m_WindowRect.left = 0;
		m_WindowRect.top = 0;
		m_WindowRect.right = width;
		m_WindowRect.bottom = height;
	}

	// Ajuster le rectangle de la fenetre (fuck it ça! Ça gâche toute)
	if (AdjustWindowRect(&m_WindowRect, dwStyle, FALSE) == 0) 
	{
		CDkw::updateLastError("Failled to adjust windows rect");
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
		return 0;
	}

	// Finalement on cré notre fenêtre
	CDkw::hWnd = CreateWindow("dkw", CDkw::title,
		dwStyle, m_WindowRect.left, m_WindowRect.top, width, height, 
		NULL, NULL, CDkw::hInstance, NULL);
	if (!(CDkw::hWnd))
	{
		CDkw::updateLastError("Failled to create window");
		return 0;
	}

	//--- Adjust the rect
	if (!fullScreen)
	{
		RECT clientRect;
		GetClientRect(CDkw::hWnd, &clientRect);
		SetWindowPos(	CDkw::hWnd,
						HWND_TOP,
						m_WindowRect.left,
						m_WindowRect.top,
						width + width - (clientRect.right - clientRect.left),
						height + height - (clientRect.bottom - clientRect.top),
						SWP_SHOWWINDOW);
	}

    // On montre notre fenêtre en premier plan
	ShowWindow(CDkw::hWnd, SW_SHOW);

	// On se cré finalement un device context
	CDkw::dc = GetDC(CDkw::hWnd);

	
	//if( fullScreen )
	//{
	//	SetWindowLongPtr(CDkw::hWnd, GWL_STYLE, WS_POPUP);
	//	SetWindowPos(CDkw::hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	//}

	return 1; // Ouff tout c'est bien passé ;)
}


#else
int dkwInit(int width, int height, int mcolorDepth, char* mTitle, CMainLoopInterface *mMainLoopObject, bool fullScreen, int refreshRate)
{
  int e;


  CDkw::mainLoopObject = mMainLoopObject;

  // On set ses propriétées de bases
  CDkw::w = width;
  CDkw::h = height;
  CDkw::colorDepth = mcolorDepth;

//      if(CDkw::colorDepth > 24)
//          CDkw::colorDepth = 24;


  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
	{
	  if(fullScreen)
		CDkw::flags |= SDL_FULLSCREEN;

          SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
          SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
          SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
          SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
          SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
          SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

          SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 16);
          SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 16);
          SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 16);
          SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 16);
//          SDL_GL_SetAttribute(SDL_GL_STEREO, 1);
          //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, CDkw::colorDepth);
          
	  CDkw::hWnd = (HWND) SDL_SetVideoMode(CDkw::w, CDkw::h, CDkw::colorDepth, CDkw::flags);
          if(CDkw::hWnd)
          {
              SDL_SetClipRect((SDL_Surface *) CDkw::hWnd, 0);
//               SDL_GL_GetAttribute(SDL_GL_RED_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, & e);

//               SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, & e);
//               SDL_GL_GetAttribute(SDL_GL_STEREO, & e);
          }
	}


  if (mTitle)
    {
      CDkw::title = new char [strlen(mTitle)+1];
      strcpy(CDkw::title, mTitle);
    }

	// On se cré une instance
  m_WindowRect.left = 0;
  m_WindowRect.top = 0;
  m_WindowRect.right = width;
  m_WindowRect.bottom = height;
  

  CDkw::dc = CDkw::hWnd;

  return !!CDkw::hWnd;
}
#endif

//
// Pour forcer l'application à fermer
//
#ifdef WIN32
void dkwForceQuit()
{
	// On ne pose pas de question, on mets ça à false
	CDkw::running = false;
	PostQuitMessage(0);
	done = true;
	dkwClipMouse( false );
}

#else
void dkwForceQuit()
{
	// On ne pose pas de question, on mets ça à false
  SDL_Event e;
  CDkw::running = false;
  done = true;
  dkwClipMouse(false);
  e.type = SDL_QUIT;
  e.quit.type = SDL_QUIT;
  SDL_PushEvent(& e);
      printf("Quit!\n");
}
#endif


//
// On obtien le Device Context de la fenetre
//
HDC	dkwGetDC()
{
	return done ? 0 : CDkw::dc;
}



//
// On obtien le handle de la fenêtre
//
HWND dkwGetHandle()
{
	return CDkw::hWnd;
}



//
// Pour obtenir l'instance de l'application
//
HINSTANCE dkwGetInstance()
{
	return CDkw::hInstance;
}



//
// Obtenir la dernière erreur
//
char* dkwGetLastError()
{
	return CDkw::lastErrorString;
}



//
// Pour retourner la position de la sourie sur l'écran
//
CVector2i dkwGetCursorPos()
{
	return CDkw::cursorPos;
}



//
// On retourne la résolution de la fenêtre
//
CVector2i dkwGetResolution()
{
	return CVector2i(CDkw::w, CDkw::h);
}

// On clip la mouse au window rect
void dkwClipMouse( bool abEnabled )
{
#ifdef WIN32
	if( !HasFocus )
		return;

	RECT r;
	SetRect(&r, m_WindowRect.left+5, m_WindowRect.top+10, m_WindowRect.right-4,m_WindowRect.bottom-5);
	ClipCursor( abEnabled ? &r : NULL );
#endif
}



//
// On effectu le loop principal de l'application
//
#ifdef WIN32
int dkwMainLoop()
{
	while(!done)
	{
		//Sleep(1);

		// Le mainloop
		MSG msg;

		// Le mainloop
		ZeroMemory (&msg, sizeof(MSG));

		// On peek les messages si c'est le cas
		if (!PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) return 0;
		//if (!GetMessage(&msg, NULL, NULL, NULL)) return 0;

		if (msg.message == WM_QUIT) return 0;

		// On transmet les messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Le main loop
//	MSG msg;
//	int result = GetMessage(&msg, CDkw::hWnd, 0, 0/*, PM_REMOVE*/);

	// On effectu les messages (affichage etc)
//	TranslateMessage(&msg);
//	DispatchMessage(&msg);

	return 0;
}
#else
int dkwMainLoop(bool * aExitFlag)
{
    //unsigned int c = 0;
  bool done = false;
  SDL_Event event;
      //struct timespec ts;
      while(!(*aExitFlag || done))
	{
        //if(!c)
        //  {
        //    c = 2;
            event.expose.type = SDL_VIDEOEXPOSE;
            event.type = SDL_VIDEOEXPOSE;
            SDL_PushEvent(& event);
                //  }
                //  else
                //  {
                // c --;
              //ts.tv_sec = 0;
                  //ts.tv_nsec = 5000000;
                  //nanosleep(& ts, 0);
                // }

        while(SDL_PollEvent(& event))
		{
		  done = (bool) CDkw::WndProc(CDkw::hWnd, 0, 0, (LRESULT) & event);
		}
	}
      printf("exit!\n");
  return 0;
}
#endif


#ifndef WIN32
void dkwGetMouseState(DIMOUSESTATE2 * aMouseState)
{
    (aMouseState && memcpy(aMouseState, & CDkw::mouse_state, sizeof(CDkw::mouse_state)), 0);
}

void dkwGetKeysState(unsigned char * aState, int aSize)
{
    if(aState && aSize && (aSize <= sizeof(CDkw::keys_state)))
        memcpy(aState, CDkw::keys_state, aSize);
}

#endif


//
// Pour shutdowner le tout
//
void			dkwShutDown()
{
	// On delete le buffer des erreurs
	if (CDkw::lastErrorString)  
	{
		delete [] CDkw::lastErrorString;
		CDkw::lastErrorString = 0;
	}

	// On delete le titre de la fenetre
	if (CDkw::title) 
	{
		delete [] CDkw::title;
		CDkw::title = 0;
	}

	dkwClipMouse( false );

#ifdef WIN32
	DestroyWindow(CDkw::hWnd);
#else
    
	SDL_Quit();
        printf("SDL shut down\n");
#endif
}



//
// Pour forcer un update des messages (meton pendant un loading)
//
void			dkwUpdate()
{
#ifdef WIN32
	// Le mainloop
	MSG msg;
	ZeroMemory (&msg, sizeof(MSG));

	// On peek les messages si c'est le cas
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		// On transmet les messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else

	SDL_PumpEvents();

#endif
}



