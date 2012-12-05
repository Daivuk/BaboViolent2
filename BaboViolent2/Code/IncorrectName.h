#ifndef INCORRECTNAME_H
#define INCORRECTNAME_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Label.h"


class IncorrectName : public ControlListener
{
public:
	// The main buttons
	Button * btn_ok;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	IncorrectName(Menu * pRootMenu);

	// Destructor
	virtual ~IncorrectName();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

