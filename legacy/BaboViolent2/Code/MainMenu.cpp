#ifndef CONSOLE
#include "MainMenu.h"
#include "Scene.h"

extern Scene * scene;



// 
// Constructor
//
MainMenu::MainMenu(Menu * pRootMenu)
{
	rootMenu = pRootMenu;

	font = dkfCreateFont("main/fonts/babo.tga");

	add(btn_resume = new Button(CVector2i(400,450),gameVar.lang_resumeCurrentGame,font,50,JUSTIFY_CENTER,this));
	add(btn_joinServer = new Button(CVector2i(400,175),gameVar.lang_joinGame,font,50,JUSTIFY_CENTER,this));
	add(btn_startServer = new Button(CVector2i(400,225),gameVar.lang_createServer,font,50,JUSTIFY_CENTER,this));
	add(btn_option = new Button(CVector2i(400,275),gameVar.lang_options,font,50,JUSTIFY_CENTER,this));
	add(btn_credits = new Button(CVector2i(400,325),gameVar.lang_credits,font,50,JUSTIFY_CENTER,this));
	add(btn_quit = new Button(CVector2i(400,375),gameVar.lang_quit,font,50,JUSTIFY_CENTER,this));
}



//
// Destructor
//
MainMenu::~MainMenu()
{
	dkfDeleteFont(&font);
}



//
// Si on clic sur un bouton
//
void MainMenu::onClick(Control * control)
{
	if (control == btn_resume)
	{
		if (scene->client)
		{
			if (gameVar.cl_playerName.len() < 2 || gameVar.cl_playerName == "Unnamed Babo")
			{
				rootMenu->currentMenu = MENU_INCORRECT_NAME;
			}
			else
			{
				rootMenu->isShowing = false;
				dkpReset();
				if (gameVar.s_inGameMusic)
				{
				/*	if (rand()%2 == 0)
					{*/
						dksPlayMusic("main/sounds/Music.ogg", -1, 60);
				/*	}
					else
					{
						dksPlayMusic("main/sounds/InGame02.ogg", -1);
					}*/
				}
			}
		}
		else
		{
			rootMenu->currentMenu = MENU_NO_GAME_RUNNING;
		}
		return;
	}
	if (control == btn_joinServer)
	{
		if (gameVar.cl_playerName.len() < 2 || gameVar.cl_playerName == "Unnamed Babo")
			rootMenu->currentMenu = MENU_INCORRECT_NAME;
		else
			rootMenu->currentMenu = MENU_JOIN_GAME;
		return;
	}
	if (control == btn_startServer)
	{
		if (gameVar.cl_playerName.len() < 2 || gameVar.cl_playerName == "Unnamed Babo")
			rootMenu->currentMenu = MENU_INCORRECT_NAME;
		else
			rootMenu->currentMenu = MENU_CREATE_SERVER;
		return;
	}
	if (control == btn_option)
	{
		rootMenu->currentMenu = MENU_OPTION;
		return;
	}
	if (control == btn_credits)
	{
		rootMenu->currentMenu = MENU_CREDITS;
		return;
	}
	if (control == btn_quit)
	{
		rootMenu->currentMenu = MENU_QUIT;
		return;
	}
}
#endif

