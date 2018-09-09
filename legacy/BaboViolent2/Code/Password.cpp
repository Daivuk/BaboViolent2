#ifndef CONSOLE
#include "Password.h"
#include "GameVar.h"
#include "Console.h"



// 
// Constructor
//
Password::Password(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(lbl_reason = new Label(CVector2i(400,250),"This game requires a password", font, 25, JUSTIFY_CENTER));
	add(btn_back = new Button(CVector2i(360,350),gameVar.lang_back,font,50,JUSTIFY_RIGHT,this));
	add(btn_join = new Button(CVector2i(440,350),gameVar.lang_join,font,50,JUSTIFY_LEFT,this));
	add(txt_password = new Write(CVector2i(400,300), "pass", font, 30, JUSTIFY_CENTER));
}



//
// Destructor
//
Password::~Password()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void Password::onClick(Control * control)
{
	if (control == btn_back)
	{
		rootMenu->currentMenu = MENU_JOIN_GAME;
	}
	if (control == btn_join)
	{
		// Join da game here
		if (txt_password->getCaption() == gameVar.cl_password)
		{
			console->sendCommand(CString("connect %s %i", gameVar.cl_lastUsedIP.s, gameVar.cl_port));
		}
		else
		{
			rootMenu->currentMenu = MENU_INCORRECT_PASSWORD;
		}
	}
}
#endif

