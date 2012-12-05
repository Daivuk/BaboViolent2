#ifndef CONSOLE
#include "IncorrectPassword.h"
#include "GameVar.h"



// 
// Constructor
//
IncorrectPassword::IncorrectPassword(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(lbl_reason = new Label(CVector2i(400,250),"Incorrect Password", font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
IncorrectPassword::~IncorrectPassword()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void IncorrectPassword::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_PASSWORD;
	}
}
#endif

