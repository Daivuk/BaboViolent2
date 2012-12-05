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
#include "Writting.h"
#include "Helper.h"


// Le pointeur sur le writting qui a présentemnt le focus
Writting * writting;


//
// Constructeur
//
Writting::Writting()
{
	m_cursorPos = 0;
	m_cursorAnim = 0;
	m_haveFocus = false;
	m_isActivated = false;
	m_sfxWrite = dksCreateSoundFromFile("main/sounds/Write.wav", false);
	m_maxCarac = 79;
}



//
// Destructeur
//
Writting::~Writting()
{
	loseFocus();
	dksDeleteSound(m_sfxWrite);
}



//
// Pour écrire dedans
//
void Writting::writeText(unsigned int caracter)
{
	// Bon y a des fucks là avec les caractères qui ont des accents
	if (caracter > 127)
	{
		for (int i=0;i<1;++i) // Juste pour faire fonctionner le break là
		{
			if (caracter == 199) {caracter = 128;break;}
			if (caracter == 252) {caracter = 129;break;}
			if (caracter == 233) {caracter = 130;break;}
			if (caracter == 226) {caracter = 131;break;}
			if (caracter == 228) {caracter = 132;break;}
			if (caracter == 224) {caracter = 133;break;}
			if (caracter == 229) {caracter = 134;break;}
			if (caracter == 231) {caracter = 135;break;}
			if (caracter == 234) {caracter = 136;break;}
			if (caracter == 235) {caracter = 137;break;}
			if (caracter == 232) {caracter = 138;break;}
			if (caracter == 239) {caracter = 139;break;}
			if (caracter == 238) {caracter = 140;break;}
			if (caracter == 236) {caracter = 141;break;}
			if (caracter == 196) {caracter = 142;break;}
			if (caracter == 197) {caracter = 143;break;}
			if (caracter == 201) {caracter = 144;break;}
			if (caracter == 230) {caracter = 145;break;}
			if (caracter == 198) {caracter = 146;break;}
			if (caracter == 244) {caracter = 147;break;}
			if (caracter == 246) {caracter = 148;break;}
			if (caracter == 242) {caracter = 149;break;}
			if (caracter == 251) {caracter = 150;break;}
			if (caracter == 249) {caracter = 151;break;}
			if (caracter == 255) {caracter = 152;break;}
			if (caracter == 214) {caracter = 153;break;}
			if (caracter == 220) {caracter = 154;break;}
			if (caracter == 248) {caracter = 155;break;}
			if (caracter == 163) {caracter = 156;break;}
			if (caracter == 216) {caracter = 157;break;}
			if (caracter == 215) {caracter = 158;break;}
			if (caracter == 170) {caracter = 159;break;}
		}
	}

	// On reset notre anim bien sur
	m_cursorAnim = 0;
	dksPlaySound(m_sfxWrite, -1, 200);

	// Si on fait backspace
	if (caracter == 8)
	{
		if (m_cursorPos > 0)
		{
			remove(m_cursorPos-1);
			m_cursorPos--;
		}
		return;
	}
	
	// Enter
	if (caracter == 13)
	{
		m_isActivated = true;
		return;
	}

	// On check pour changer la couleur
	if (caracter >= '1' && caracter <= '9')
	{
		if (m_cursorPos > 0)
		{
			if (s[m_cursorPos-1] == '^')
			{
				remove(m_cursorPos-1);
				m_cursorPos--;

				if (caracter == '1') insert(CString("\x1"), m_cursorPos);
				if (caracter == '2') insert(CString("\x2"), m_cursorPos);
				if (caracter == '3') insert(CString("\x3"), m_cursorPos);
				if (caracter == '4') insert(CString("\x4"), m_cursorPos);
				if (caracter == '5') insert(CString("\x5"), m_cursorPos);
				if (caracter == '6') insert(CString("\x6"), m_cursorPos);
				if (caracter == '7') insert(CString("\x7"), m_cursorPos);
				if (caracter == '8') insert(CString("\x8"), m_cursorPos);
				if (caracter == '9') insert(CString("\x9"), m_cursorPos);
				m_cursorPos++;
				return;
			}
		}
	}

	// On écris notre caractère
	if (caracter >= 32 && caracter <= 159)
	{
		insert(CString("%c", caracter), m_cursorPos);
		m_cursorPos++;
	}

	if (len() > m_maxCarac) 
	{
		m_cursorPos = m_maxCarac;
		resize(m_maxCarac);
	}
}


//
// Replace text with new string
//
void Writting::replaceText(CString str)
{
	clear();
	set("%s", str.s);
	m_cursorPos = len();
}



//
// Pour updater
//
void Writting::updateWritting(float delay)
{
	// On anim le curseur
	m_cursorAnim += delay;
	while (m_cursorAnim >= 1) m_cursorAnim -= 1;

	// Si on déplace avec les flèches
	if (dkiGetState(DIK_LEFT) == DKI_DOWN)
	{
		dksPlaySound(m_sfxWrite, -1, 200);
		m_cursorAnim = 0;
		m_cursorPos--;
		if (m_cursorPos < 0) m_cursorPos = 0;
	}
	if (dkiGetState(DIK_RIGHT) == DKI_DOWN)
	{
		dksPlaySound(m_sfxWrite, -1, 200);
		m_cursorAnim = 0;
		m_cursorPos++;
		if (m_cursorPos > len()) m_cursorPos = len();
	}
	if (dkiGetState(DIK_HOME) == DKI_DOWN)
	{
		dksPlaySound(m_sfxWrite, -1, 200);
		m_cursorAnim = 0;
		m_cursorPos = 0;
	}
	if (dkiGetState(DIK_END) == DKI_DOWN)
	{
		dksPlaySound(m_sfxWrite, -1, 200);
		m_cursorAnim = 0;
		m_cursorPos = len();
	}
	if (dkiGetState(DIK_DELETE) == DKI_DOWN)
	{
		dksPlaySound(m_sfxWrite, -1, 200);
		m_cursorAnim = 0;
		if (m_cursorPos < len()) 
		{
			remove(m_cursorPos);
			if (m_cursorPos > len()) m_cursorPos = len();
		}
	}
}



//
// pour lui donner le focus du writing
//
void Writting::giveFocus()
{
	if (writting) writting->loseFocus();
	writting = this;
	m_cursorPos = len();
	m_cursorAnim = 0;
	m_haveFocus = true;
}



//
// Pour lui enlever le focus du writing
//
void Writting::loseFocus()
{
	if (writting == this) writting = 0;
	m_haveFocus = false;
}



//
// Pour l'afficher
//
void Writting::print(float size, float x, float y, float z)
{
	// On écris le text normalement
	printLeftText(x,y,size,CString("%s", s));
//	dkfPrint(size, x, y, z, s);

	// On dessine le curseur
	if (m_haveFocus && m_cursorAnim < .5f)
	{
		CString tmp("%s", s);
		tmp.resize(m_cursorPos);
		float posX = dkfGetStringWidth(size, tmp.s);
		dkfPrint(size, x + posX, y, z, "_");
	}
}



//
// pour le reseter
//
void Writting::clear()
{
	reset();
	m_cursorPos = 0;
	m_cursorAnim = 0;
}
#endif

