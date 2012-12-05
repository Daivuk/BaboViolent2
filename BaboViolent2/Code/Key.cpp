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
#include "Key.h"
#include "KeyManager.h"



//
// Constructeur
//
Key::Key(CVector2i screenPosition, CString caption, unsigned int m_font, float textSize, control_justify justify)
: Control(screenPosition, caption, m_font, textSize, justify)
{
	m_isWaitingForKey = false;
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
}



//
// Destructeur
//
Key::~Key()
{
	dksDeleteSound(m_sfxClic);
}



//
// sets
//
void Key::setKeyValue(int keyValue)
{
	m_keyValue = keyValue;
	changeCaption(CString("[ ") + keyManager.getKeyName(m_keyValue) + " ]");
	m_isWaitingForKey = false;
}



//
// L'updater le gèrer
//
void Key::update(float delay)
{
	if (m_leftOffset < 0)
	{
		m_leftOffset += delay * fabsf(m_leftOffset) * 4;
		if (m_leftOffset > 0) m_leftOffset = 0;
	}

	if (m_clicDelay > 0)
	{
		m_clicDelay -= delay;
		if (m_clicDelay < 0)
		{
			m_clicDelay = 0;
			m_state = CONTROL_NOTHING;
		}
	}

	// On check si la souri est dans notre Rectangle
	CVector2i mousePos = dkwGetCursorPos_main();
	CVector2i res = dkwGetResolution();

	// Il faut maintenant ajuster la mousePos dans notre 800x600
	mousePos[0] = (int)((float)mousePos[0] / (float)res[0] * 800.0f);
	mousePos[1] = (int)((float)mousePos[1] / (float)res[1] * 600.0f);

	// On check si on ne doit pas attendre pour une touche
	if (m_isWaitingForKey)
	{
		int key = dkiGetFirstDown();
		if (key != DKI_NOKEY)
		{
			dksPlaySound(m_sfxClic, -1, 200);
			setKeyValue(key);
		}
	}
	else
	{
		// On test maintenant avec le rectange du bouton
		if (mousePos[0] >= (int)m_rect[0]-10 &&
			mousePos[0] <= (int)m_rect[2]+10 &&
			mousePos[1] >= (int)m_rect[1] &&
			mousePos[1] <= (int)m_rect[3])
		{
			// Si on n'avait pas de state, on tombe over
			if (m_state == CONTROL_NOTHING)
			{
				dksPlaySound(m_sfxOver, -1, 150);
				m_state = CONTROL_OVER;
			}
			if (dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN)
			{
				m_state = CONTROL_DOWN;
				m_clicDelay = .1f;
				onClic();
			}
		}
		else if (m_state == CONTROL_OVER)
		{
			m_state = CONTROL_NOTHING;
		}
	}
}



//
// Pour quand on clic dessus, il faut activer le wait for key hehe (SICK)
//
void Key::onClic()
{
	if (m_isWaitingForKey == false) 
	{
		dksPlaySound(m_sfxClic, -1, 200);
		m_isWaitingForKey = true;
		changeCaption("\x4HIT KEY");
	}
}
#endif

