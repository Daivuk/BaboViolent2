/*
	Copyright 2012 bitHeads inc.

	This file is part of the BaboViolent 2 source code.

	The BaboViolent 2 source code is free software: you can redistribute it and/or 
	modify it under the terms of the GNU General Public License as published by the 
	Free Software Foundation, either version 3 of the License, or (at your option) 
	any later version.

	The BaboViolent 2 source code is distributed in the hope that it will be useful, 
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with the 
	BaboViolent 2 source code. If not, see http://www.gnu.org/licenses/.
*/

#ifndef CONSOLE
#ifndef CMENUMANAGER_H_INCLUDED
#define CMENUMANAGER_H_INCLUDED

// #include <stack> // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
#include <vector>
#include "Dialog.h"
#include "CControl.h"

class CMenuManager
{
private:
//	std::stack<IDialog*> dialogs;// [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
	std::vector<IDialog*> dialogs;
	int removeTop; //0-nop, 1-yup, 2-yup&clean stuff

public:
	CControl * activeControl;
	CControl * focusControl;
	CControl * hoveringControl;

	CControl * root;

	CVector2i mousePos;

public:
	CMenuManager();
	virtual ~CMenuManager();

	void update(float delay, CControl * toUpdate = 0);
	void updateDialogs(float delay);
	void render(CControl * toRender = 0);
	void renderDialogs();

	void showDialog(IDialog* dialog);
	//hide top dialog window
	//if you want to close window during update process of controls then set
	//autoMemFree to true so there is no access violation
	void hideDialog(bool autoMemFree = false/*IDialog* dialog*/);

private:
	void renderTooltip(CControl * control, const CVector2i& res);
};


extern CMenuManager menuManager;


#endif
#endif

