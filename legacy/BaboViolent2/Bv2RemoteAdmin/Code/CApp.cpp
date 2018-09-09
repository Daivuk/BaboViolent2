#include "CApp.h"
#include "CFrmMain.h"
#include "baboNet.h"

#ifdef WIN32
	#pragma comment (lib, "baboNet.lib")
#endif

#if !WIN32 && !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

//--- Execute that app yea
//	  This macro will launch the WinMain for us.
//	  Depending on which platform we compile :D
IMPLEMENT_APP(CApp)



//
//--- Constructor
//
CApp::CApp()
{
}



//
//--- Destructor
//
CApp::~CApp()
{
	bb_shutdown();
}



//
//--- On initialization of the app
//
bool CApp::OnInit()
{
	if( bb_init() == 1 )
	{
		wxMessageBox( wxT("Problem initializing baboNet lib, aborting"), wxT("Error"));
		return FALSE;
	}

	CFrmMain * frmMain = new CFrmMain();
	frmMain->CenterOnScreen();
	frmMain->Show(true);

	return TRUE;
}
