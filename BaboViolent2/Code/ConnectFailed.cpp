#ifndef CONSOLE
#include "ConnectFailed.h"
#include "GameVar.h"



// 
// Constructor
//
ConnectFailed::ConnectFailed(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(lbl_reason = new Label(CVector2i(400,250), "Connection failed, try again"/*gameVar.lang_connectionFailed*/, font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
ConnectFailed::~ConnectFailed()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void ConnectFailed::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_JOIN_GAME;
	}
}
#endif

