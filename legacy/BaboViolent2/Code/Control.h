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

#ifndef CONTROL_H
#define CONTROL_H


#include "Zeven.h"


enum control_state
{
	CONTROL_NOTHING,
	CONTROL_OVER,
	CONTROL_DOWN
};

enum control_justify
{
	JUSTIFY_LEFT,
	JUSTIFY_CENTER,
	JUSTIFY_RIGHT
};


class Control
{
protected:
	// Caption
	CString m_caption;

	// Son état
	control_state m_state;

	// Sa font
	unsigned int m_font;

	// Si on vient de le cliquer
	float m_clicDelay;

	// La grosseur de la font
	float m_textSize;

	// Son pourcent d'arrivé
	float m_leftOffset;

	float anim;

	// la position du text par rapport à sa screenPosition
	control_justify m_justify;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	// On ne gère pas les clic sur lui
	bool m_isMouseHandle;

	// La texture
	unsigned int tex_menu;

public:
	// Son RECT (avec la caption)
	CVector4f m_rect;

	// Constructeur
	Control(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify);

	// Sa position sur l'écran
	CVector2i m_screenPosition;

	// Destructeur
	virtual ~Control();

	// L'updater le gèrer
	virtual void update(float delay);

	// Pour quand on clic dessus, le child saura quoi faire
	virtual void onClic() {}
	virtual void onRightClic() {}

	// Pour changer la caption
	void changeCaption(CString caption);

	// gets
	CString getCaption() {return m_caption;}

	// Pour l'afficher
	virtual void render();
};


#endif

