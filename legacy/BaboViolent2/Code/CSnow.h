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

#ifndef DEDICATED_SERVER
#ifndef CSNOW_H_INCLUDED
#define CSNOW_H_INCLUDED


#include "CWeather.h"

#include "Zeven.h"


struct SSnow
{
public:
	CVector3f pos;
public:
	SSnow()
	{
	}
	void update(float delay)
	{
		pos[2] -= 2 * delay;
		pos += rand(CVector3f(-1,-1,0), CVector3f(1,1,0)) * delay;
	}
	void render()
	{
#ifndef _DX_
		glColor4f(1, 1, 1,((pos[2] > 2)?2:pos[2]) / 2.0f);
		glTexCoord2f(0,1);
		glVertex3f(pos[0]-.05f,pos[1]+.05f,pos[2]);
		glTexCoord2f(0,0);
		glVertex3f(pos[0]-.05f,pos[1]-.05f,pos[2]);
		glTexCoord2f(1,0);
		glVertex3f(pos[0]+.05f,pos[1]-.05f,pos[2]);
		glTexCoord2f(1,1);
		glVertex3f(pos[0]+.05f,pos[1]+.05f,pos[2]);
#endif
	}
};


class CSnow : public CWeather
{
public:
	//--- Weather sound
	FSOUND_SAMPLE * m_sfxRain;
	int channel;

	//--- La rain
	SSnow rains[100];
	int nextRain;

	//--- Flocon
	unsigned int tex_snow;

	int nextIn;

public:
	//--- Constructor
	CSnow();

	//--- Destructor
	virtual ~CSnow();

	//--- Update
	void update(float delay, Map* map);

	//--- Render
	void render();
};


#endif
#endif

