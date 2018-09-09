#ifndef WRONGVERSION_H
#define WRONGVERSION_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class WrongVersion : public ControlListener
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
	WrongVersion(Menu * pRootMenu);

	// Destructor
	virtual ~WrongVersion();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

