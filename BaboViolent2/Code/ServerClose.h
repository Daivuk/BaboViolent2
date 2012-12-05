#ifndef SERVERCLOSE_H
#define SERVERCLOSE_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class ServerClose : public ControlListener
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
	ServerClose(Menu * pRootMenu);

	// Destructor
	virtual ~ServerClose();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

