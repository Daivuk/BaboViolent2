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

#ifndef CLISTENER_H_INCLUDED
#define CLISTENER_H_INCLUDED


#include "Zeven.h"

class CControl;


class CListener
{
public:
public:
	virtual void MouseEnter(CControl * control) {}
	virtual void MouseLeave(CControl * control) {}
	virtual void MouseDown(CControl * control) {}
	virtual void MouseUp(CControl * control) {}
	virtual void MouseMove(CControl * control) {}
	virtual void Click(CControl * control) {}
	virtual void Validate(CControl * control) {}
	virtual void Move(CControl * control) {}
	virtual void Resize(CControl * control) {}
	virtual void Hold(CControl * control) {}
	virtual void Paint(CControl * control) {}
	virtual void DbClick(CControl * control) {}

	virtual void setVisible(bool value) {}
	virtual void update(float delay) {}
};


#endif

