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

#include "CRain.h"
#include "Map.h"



//
//--- Constructor
//
CRain::CRain()
{
	m_sfxRain = dksCreateSoundFromFile("main/sounds/rain2.wav", true);

	//--- Start the sound
	channel = dksPlaySound(m_sfxRain, -1, 50);

	nextRain = 0;
}



//
//--- Destructor
//
CRain::~CRain()
{
	FSOUND_StopSound(channel);
	dksDeleteSound(m_sfxRain);
}



//
//--- Update
//
void CRain::update(float delay, Map* map)
{
  int i;
	//--- On crée la pluit yé
	for (i=0;i<3;++i)
	{
		rains[nextRain].pos = rand(map->camPos + CVector3f(-3,-3,5), map->camPos + CVector3f(3,3,5));
		nextRain++;
		if (nextRain == 100) nextRain = 0;
	}

	//--- On anime la plus
	for (i=0;i<100;++i)
	{
		if (rains[i].pos[2] > 0)
		{
			rains[i].update(delay);
		}
	}
}



//
//--- Render
//
void CRain::render()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2);
		glBegin(GL_LINES);
			for (int i=0;i<100;++i)
			{
				if (rains[i].pos[2] > 0)
				{
					rains[i].render();
				}
			}
		glEnd();
	glPopAttrib();
#endif
}


#endif

