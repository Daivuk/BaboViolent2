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

/* TCE (c) All rights reserved */


#include "dkfi.h"
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif


//
// Pour binder le font pour dire que c lui qu'on utilise
//
void			dkfBindFont(unsigned int ID)
{
	for (int i=0;i<(int)fonts.size();i++)
	{
		CFont *font = fonts.at(i);
		if (ID == font->fontID)
		{
			currentBind = font;
			return;
		}
	}
}



//
// Pour créer une police de charactère
//
unsigned int	dkfCreateFont(char *filename)
{
	// Bon avant là, on check si il l'a pas loadé 2 fois
	for (int i=0;i<(int)fonts.size();i++)
	{
		CFont *font = fonts.at(i);
		if (font->filename == filename)
		{
			font->nbInstance++;
			return font->fontID;
		}
	}

	// Sinon on cré la font
	CFont *font = new CFont();
	font->fontID = ++currentIDCount;
	if (!font->create(filename))
	{
		delete font;
		return 0;
	}

	// On construit le tout
	font->reloadIt();

	fonts.push_back(font);
	return font->fontID;
}



//
// Pour effacer une fonct
//
void			dkfDeleteFont(unsigned int *ID)
{
	for (int i=0;i<(int)fonts.size();i++)
	{
		CFont *font = fonts.at(i);
		if (font->fontID == *ID)
		{
			font->nbInstance--;
			if (font->nbInstance == 0)
			{
				fonts.erase(fonts.begin() + i);
				if (currentBind == font) currentBind = 0;
				delete font;
			}
			*ID = 0;
			return;
		}
	}
}



//
// Pour obtenir la position d'un caracter dans le string en vecteur 2 flota
//
CPoint2f		dkfGetCaracterPos(float size, char *text, int caracter)
{
	if (currentBind)
	{
		float posX = 0;
		float posY = 0;
		size_t len = strlen(text);
		for (size_t i=0;i<len;i++)
		{
			if (text[i] == '\n')
			{
				posX = 0;
				posY += size;
				continue;
			}

			if (i == size_t(caracter)) return CPoint2f(posX, posY);

			posX += currentBind->finalCaracKerning[(unsigned char)text[i]] * size;
		}

		return CPoint2f(posX, posY);
	}

	return CPoint2f(0,0);
}



//
// Obtenir le caractère où on est au dessus avec la sourie
//
int				dkfGetOverStringCaracter(float size, char *text, CPoint2f & onStringPos)
{
	if (currentBind)
	{
		if (onStringPos[1] < 0) return 0;
		float posX = 0;
		float posY = 0;
		size_t len = strlen(text);
		for (size_t i=0;i<len;i++)
		{
			if (text[i] == '\n')
			{
				posX = 0;
				posY += size;
				continue;
			}

			if (onStringPos[0] <= posX + currentBind->finalCaracKerning[(unsigned char)text[i]] * size &&
				onStringPos[1] >= posY &&
				onStringPos[1] < posY+size) return int(i);

			posX += currentBind->finalCaracKerning[(unsigned char)text[i]] * size;
		}

		return int(strlen(text));
	}

	return -1;
}



//
// retourne le height et width en pixel d'un string utilisant cette police
//
float			dkfGetStringHeight(float size, char *text)
{
	// Ici on compte les retour de chariot
	size_t len = strlen(text);
	int nbChariot = 1;
	for (size_t i=0;i<len;i++)
	{
		if (text[i] == '\n') nbChariot++;
	}
	return (float)nbChariot*size; // Pour le height c'est aussi simple que ça
}

float			dkfGetStringWidth(float size, char *text)
{
	if (currentBind)
	{
		float bestScore = 0;
		float currentScore = 0;
		size_t len = strlen(text);
		for (size_t i=0;i<len;i++)
		{
			currentScore += currentBind->finalCaracKerning[(unsigned char)text[i]];

			if (text[i] == '\n' || i == len-1)
			{
				if (currentScore > bestScore) bestScore = currentScore;
				currentScore = 0;
				continue;
			}
		}

		if (bestScore > 0)
			return bestScore * size;
		else
			return currentScore * size;
	}
	else
		return 0;
}



//
// Pour afficher du text à l'écran avec cette font
//
void			dkfPrint(float size, float x, float y, float z, char *text)
{
	if (currentBind)
	{
		currentBind->printText(size, x, y, z, text);
	}
}



//
// Pour shutdowner et effacer les polices
//
void			dkfShutDown()
{
	for (int i=0;i<(int)fonts.size();i++)
	{
		CFont *font = fonts.at(i);
		if (font) delete font;
	}
	fonts.clear();
	currentBind = 0;
	currentIDCount = 0;
}