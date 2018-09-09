#ifndef CREDITS_H
#define CREDITS_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class Credits : public ControlListener
{
public:
	// The main buttons
	Button * btn_back;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	Credits(Menu * pRootMenu);

	// Destructor
	virtual ~Credits();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

