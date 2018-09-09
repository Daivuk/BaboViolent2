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

#ifndef WRITTING_H
#define WRITTING_H


#include "Zeven.h"


class Writting : public CString
{
protected:
	// La position de son cursor
	int m_cursorPos;

	// Son animation pour le cursor
	float m_cursorAnim;

	// Si il a le focus
	bool m_haveFocus;

	// Pour dire qu'on a perdu le focus
	bool m_isActivated;

	// Le nb de caractère max
	int m_maxCarac;

	// Le son pour quand on tape
	FSOUND_SAMPLE * m_sfxWrite;

public:
	// Constructeur
	Writting();

	// Destructeur
	virtual ~Writting();

	// Pour écrire dedans
	void writeText(unsigned int caracter);

	// Replace text with new string
	void replaceText(CString str);

	// Pour updater
	void updateWritting(float delay);

	// pour lui donner le focus du writing
	void giveFocus();

	// Pour lui enlever le focus du writing
	void loseFocus();

	// Pour savoir si on a le focus
	bool haveFocus() {return m_haveFocus;}
	bool isActivated() {if (m_isActivated) {m_isActivated=false;return true;} else return false;}

	// pour l'afficher
	void print(float size, float x, float y, float z);

	// pour le reseter
	void clear();

	//--- Set max number of characters
	void SetMaxCarac(int in_maxCarac){m_maxCarac = in_maxCarac;}
};


// Le pointeur global sur le writting qui a présentment le focus
extern Writting * writting;


#endif

