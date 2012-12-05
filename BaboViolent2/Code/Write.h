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

#ifndef WRITE_H
#define WRITE_H


#include "Control.h"
#include "Writting.h"


#define NO_VALIDATION 0
#define VALIDATE_INT 1
#define VALIDATE_FLOAT 2


class Menu;


class Write : public Control, public Writting
{
private:
	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	int validationType;

public:
	// Constructeur
	Write(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify, int flag = NO_VALIDATION);

	// Destructeur
	virtual ~Write();

	// L'updater le gèrer
	virtual void update(float delay);

	// Pour quand on clic dessus, le child saura quoi faire
	void onClic();

	// Pour l'afficher
	virtual void render();
};


#endif

