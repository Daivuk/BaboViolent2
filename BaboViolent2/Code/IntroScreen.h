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

#ifndef INTROSCREEN_H
#define INTROSCREEN_H


#include "Zeven.h"


class IntroScreen
{
public:
	// Le delay qui reste à l'afficher
	float showDelay;

	// Notre logo
	unsigned int tex_rndLogo;
	unsigned int tex_hgLogo;
//	unsigned int tex_glowLogo;

	// Le super son mistic du logo
	FSOUND_SAMPLE * sfx_intro;

public:
	// Constructeur
	IntroScreen();

	// Destructeur
	virtual ~IntroScreen();

	// Update
	void update(float delay);

	// Render
	void render();
};


#endif

