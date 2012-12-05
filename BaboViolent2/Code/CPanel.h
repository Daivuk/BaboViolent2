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
#ifndef CPANEL_H_INCLUDED
#define CPANEL_H_INCLUDED

#include "CListener.h"
#include "CControl.h"


class CPanel : public CListener
{
public:
	CControl * instance;
	FSOUND_SAMPLE * m_sfxImpact;

public:

	CPanel()
	{
		instance = 0;
		animY = 0;
		velY = 0;
		counterSound = 0;
		m_sfxImpact = dksCreateSoundFromFile("main/sounds/impact.wav", false);
	}
	virtual ~CPanel()
	{
		dksDeleteSound(m_sfxImpact);
	}

	float animY;
	float velY;
	int originalY;
	int counterSound;

	virtual void update(float delay)
	{
		if (velY != 0)
		{
			velY -= delay * 2000.0f;
			animY += velY * delay;
			if (animY <= 0)
			{
				animY = 0;
				velY *= -.25f;
				if (velY < .15f) velY = 0;
				else
				{
					if (counterSound > 0)
					{
						counterSound--;
						dksPlaySound(m_sfxImpact, -1, 150);
					}
				}
			}
			instance->localPos[1] = originalY + (int)animY;
		}

		updatePerso(delay);
	}

	void setVisible(bool value) 
	{
		if (instance)
		{
			instance->visible=value;
			if (value == true)
			{
				counterSound = 2;
				animY = 600;
				velY = -1200;
			}
		}
	}

	virtual void updatePerso(float delay) {}
};


#endif
#endif

