#ifndef CONSOLE
#include "Credits.h"
#include "GameVar.h"



// 
// Constructor
//
Credits::Credits(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(btn_back = new Button(CVector2i(50,600-75),gameVar.lang_back,font,50,JUSTIFY_LEFT,this));

	// RndLabs Team
	add(new Label(CVector2i(25,25), CString("\x8 ") + gameVar.lang_RndLabsTeam, font, 64, JUSTIFY_LEFT));

	// Main Designer
	add(new Label(CVector2i(50,100), CString("\x9 ") + gameVar.lang_gameDesigner, font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,125), "David St-Louis", font, 30, JUSTIFY_LEFT));

	// Programeurs
	add(new Label(CVector2i(50,175), CString("\x9 ") + gameVar.lang_programmers, font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,200), "David St-Louis", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,225), "Marc Durocher", font, 30, JUSTIFY_LEFT));

	// Sound designer
	add(new Label(CVector2i(50,275), CString("\x9 ") + gameVar.lang_soundDesigners, font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,300), "Dominic Valiquette", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,325), "David St-Louis", font, 30, JUSTIFY_LEFT));

	// Music
	add(new Label(CVector2i(50,375), CString("\x9 ") + gameVar.lang_musics, font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,400), gameVar.lang_thanksTo + " \x3Lamb of God\x8!", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(50,425), CString("-> ") + gameVar.lang_buyAlbum, font, 30, JUSTIFY_LEFT));

	// Beta testers
	add(new Label(CVector2i(400,100), CString("\x9 ") + gameVar.lang_betaTesters, font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,125), "David Dupuis", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,150), "Thomas Jaillet", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,175), "Tristan Labelle", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,200), "Louis Poirier", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,225), "Maxime Thibeault-Leblanc", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,250), "Julien Verneuil", font, 30, JUSTIFY_LEFT));

	// Other
	add(new Label(CVector2i(400,300), CString("\x9 ") + "Translators", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,325), "Erno Matias Mikkil\x84", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,350), "Jeroen Arts", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,375), "Jens Turesson", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(400,400), "Peter Belanyi", font, 30, JUSTIFY_LEFT));
}



//
// Destructor
//
Credits::~Credits()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void Credits::onClick(Control * control)
{
	if (control == btn_back)
	{
		rootMenu->currentMenu = MENU_MAIN;
	}
}
#endif

