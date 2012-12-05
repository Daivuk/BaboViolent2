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
#include "Choice.h"



//
// Constructeur
//
Choice::Choice(CVector2i screenPosition, 
			   CString caption, 
			   unsigned int m_font, 
			   float textSize, 
			   control_justify justify)
: Control(screenPosition, caption, m_font, textSize, justify)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_choice = 0;
}



//
// Destructeur
//
Choice::~Choice()
{
	dksDeleteSound(m_sfxClic);
	m_choices.clear();
}



//
// Pour quand on clic dessus, le child saura quoi faire
//
void Choice::onClic()
{
	dksPlaySound(m_sfxClic, -1, 200);
	
	// On loop les choix
	m_choice++;
	if (m_choice >= (int)m_choices.size()) m_choice = 0;
	changeCaption(getChoice());
}



//
// Pour quand on clic dessus, le child saura quoi faire
//
void Choice::onRightClic()
{
	dksPlaySound(m_sfxClic, -1, 200);
	
	// On loop les choix
	m_choice--;
	if (m_choice < 0) m_choice = (int)m_choices.size() - 1;
	changeCaption(getChoice());
}



//
// Pour ajouter un choix
//
void Choice::addChoice(CString choice)
{
	m_choices.push_back(choice);
	if (m_choices.size() == 1)
	{
		changeCaption(m_choices[0]);
	}
}



//
// sets
//
void Choice::setChoice(int choice)
{
	m_choice = choice;
	if (m_choice < 0) m_choice = 0;
	if (m_choice >= (int)m_choices.size()) m_choice = 0;
	changeCaption(getChoice());
}
#endif

