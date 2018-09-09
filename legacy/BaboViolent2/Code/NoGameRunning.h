#ifndef NOGAMERUNNING_H
#define NOGAMERUNNING_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class NoGameRunning : public ControlListener
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
	NoGameRunning(Menu * pRootMenu);

	// Destructor
	virtual ~NoGameRunning();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

