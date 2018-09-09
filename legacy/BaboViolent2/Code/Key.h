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

#ifndef KEY_H
#define KEY_H


#include "Control.h"


class Key : public Control
{
private:
	// La valeur de la touche
	int m_keyValue;

	// Si on est en attente de presser une touche !
	bool m_isWaitingForKey;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

public:
	// Constructeur
	Key(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify);

	// Destructeur
	virtual ~Key();

	// sets
	void setKeyValue(int keyValue);

	// gets
	int getKeyValue() {return m_keyValue;}

	// L'updater le gèrer
	virtual void update(float delay);

	// Pour quand on clic dessus, il faut activer le wait for key hehe (SICK)
	void onClic();
};


#endif

