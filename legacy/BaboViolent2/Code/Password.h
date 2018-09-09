#ifndef PASSWORD_H
#define PASSWORD_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class Password : public ControlListener
{
public:
	// The main buttons
	Label * lbl_reason;
	Button * btn_back;
	Button * btn_join;
	Write * txt_password;

	CString password;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	Password(Menu * pRootMenu);

	// Destructor
	virtual ~Password();

	// Si on clic sur un bouton
	void onClick(Control * control);
};


#endif

