#ifndef CONSOLE
#include "IncorrectName.h"
#include "GameVar.h"



// 
// Constructor
//
IncorrectName::IncorrectName(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(new Label(CVector2i(400,200),gameVar.lang_incorrectName, font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
IncorrectName::~IncorrectName()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void IncorrectName::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_OPTION;
		return;
	}
}
#endif

