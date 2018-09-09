#ifndef NOMAPSELECTED_H
#define NOMAPSELECTED_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class NoMapSelected : public ControlListener
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
	NoMapSelected(Menu * pRootMenu);

	// Destructor
	virtual ~NoMapSelected();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

