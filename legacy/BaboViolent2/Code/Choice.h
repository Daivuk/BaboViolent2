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

#ifndef CHOICE_H
#define CHOICE_H


#include "Control.h"
#include <vector>


class Menu;


class Choice : public Control
{
private:
	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Nos choix
	std::vector<CString> m_choices;

	// Notre choix courant
	int m_choice;

public:
	// Constructeur
	Choice(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify);

	// Destructeur
	virtual ~Choice();

	// Pour quand on clic dessus, le child saura quoi faire
	void onClic();
	void onRightClic();

	// Pour ajouter un choix
	void addChoice(CString choice);

	// gets
	CString getChoice() {return (m_choices.size())?m_choices[m_choice]:CString();}
	int getChoiceIndex() {return m_choice;}

	// sets
	void setChoice(int choice);
};


#endif

