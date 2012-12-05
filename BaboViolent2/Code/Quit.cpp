#ifndef CONSOLE
#include "Quit.h"
#include "GameVar.h"



// 
// Constructor
//
Quit::Quit(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(new Label(CVector2i(400,250),gameVar.lang_areYouSureYouWantToQuit, font, 25, JUSTIFY_CENTER));
	add(btn_yes = new Button(CVector2i(300,300),gameVar.lang_yesQ,font,50,JUSTIFY_CENTER,this));
	add(btn_no = new Button(CVector2i(500,300),gameVar.lang_noQ,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
Quit::~Quit()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void Quit::onClick(Control * control)
{
	if (control == btn_yes)
	{
		dkwForceQuit();
	}
	if (control == btn_no)
	{
		rootMenu->currentMenu = MENU_MAIN;
	}
}
#endif

