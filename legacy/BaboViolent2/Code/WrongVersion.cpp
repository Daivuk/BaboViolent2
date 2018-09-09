#ifndef CONSOLE
#include "WrongVersion.h"
#include "GameVar.h"



// 
// Constructor
//
WrongVersion::WrongVersion(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(new Label(CVector2i(400,250),"Wrong game Version.\nPlease download new version at www.rndlabs.ca", font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
WrongVersion::~WrongVersion()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void WrongVersion::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_JOIN_GAME;
		return;
	}
}
#endif

