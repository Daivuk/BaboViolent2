#ifndef CONSOLE
#include "ServerClose.h"
#include "GameVar.h"



// 
// Constructor
//
ServerClose::ServerClose(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(new Label(CVector2i(400,250),gameVar.lang_serverHasClosedTheConnection, font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
ServerClose::~ServerClose()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void ServerClose::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_JOIN_GAME;
		return;
	}
}
#endif

