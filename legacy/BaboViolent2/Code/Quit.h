#ifndef Quit_H
#define Quit_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class Quit : public ControlListener
{
public:
	// The main buttons
	Button * btn_yes;
	Button * btn_no;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	Quit(Menu * pRootMenu);

	// Destructor
	virtual ~Quit();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

