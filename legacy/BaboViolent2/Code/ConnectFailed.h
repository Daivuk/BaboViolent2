#ifndef CONNECTFAILED_H
#define CONNECTFAILED_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class ConnectFailed : public ControlListener
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
	ConnectFailed(Menu * pRootMenu);

	// Destructor
	virtual ~ConnectFailed();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

