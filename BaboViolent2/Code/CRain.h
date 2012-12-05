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
#ifndef CRAIN_H_INCLUDED
#define CRAIN_H_INCLUDED


#include "CWeather.h"

#include "Zeven.h"


struct SRain
{
public:
	CVector3f pos;
public:
	SRain()
	{
	}
	void update(float delay)
	{
		pos[2] -= 15 * delay;
	}
	void render()
	{
#ifndef _DX_
		glColor4f(.25f, .7f, .3f,((pos[2] > 2)?2:pos[2]) / 2.0f * .3f);
		glVertex3fv(pos.s);
		glVertex3f(pos[0],pos[1],pos[2]-.5f);
#endif
	}
};


class CRain : public CWeather
{
public:
	//--- Weather sound
	FSOUND_SAMPLE * m_sfxRain;
	int channel;

	//--- La rain
	SRain rains[100];
	int nextRain;

public:
	//--- Constructor
	CRain();

	//--- Destructor
	virtual ~CRain();

	//--- Update
	void update(float delay, Map* map);

	//--- Render
	void render();
};


#endif
#endif

