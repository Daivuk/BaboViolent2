#ifndef INCORRECTPASSWORD_H
#define INCORRECTPASSWORD_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class IncorrectPassword : public ControlListener
{
public:
	// The main buttons
	Label * lbl_reason;
	Button * btn_ok;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	IncorrectPassword(Menu * pRootMenu);

	// Destructor
	virtual ~IncorrectPassword();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

