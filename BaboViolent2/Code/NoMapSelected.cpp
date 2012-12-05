#ifndef CONSOLE
#include "NoMapSelected.h"
#include "GameVar.h"



// 
// Constructor
//
NoMapSelected::NoMapSelected(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(new Label(CVector2i(400,250),gameVar.lang_selectAtLeastOneMap, font, 25, JUSTIFY_CENTER));
	add(btn_ok = new Button(CVector2i(400,300),gameVar.lang_back,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
NoMapSelected::~NoMapSelected()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void NoMapSelected::onClick(Control * control)
{
	if (control == btn_ok)
	{
		rootMenu->currentMenu = MENU_CREATE_SERVER;
	}
}
#endif

