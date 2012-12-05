#ifndef MAINMENU_H
#define MAINMENU_H


#include "ControlListener.h"
#include "Button.h"
#include "Menu.h"


class MainMenu : public ControlListener
{
public:
	// The main buttons
	Button * btn_startServer;
	Button * btn_joinServer;
	Button * btn_option;
	Button * btn_credits;
	Button * btn_quit;
	Button * btn_resume;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	MainMenu(Menu * pRootMenu);

	// Destructor
	virtual ~MainMenu();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

