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


/******************************************************************

Class : CFont
Auteur : David St-Louis
Compatibilité : <all>

< Description >

Construction d'un font, et permet d'imprimer à l'écran dans cette
police.
Voici quelque spécificité :
\n = retour de chariot (automatique)
^0 = texte noir (user)
^1 = texte bleu (user)
^2 = texte vert (user)
^3 = texte cyan (user)
^4 = texte rouge (user)
^5 = texte mauve (user)
^6 = texte brun (user)
^7 = texte gris (user)
^8 = texte jaune (user)
^9 = texte blanc (user)

******************************************************************/

#ifndef CFONT_H
#define CFONT_H


#include "CString.h"
#include <fstream>

using namespace std;


#ifdef WIN32

#ifndef _DX_
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "dkt.lib" )
#else
#pragma comment( lib, "dkt_dx.lib" )

#endif


#include <windows.h>
// Les includes pour opengl
#ifndef _DX_
#include <gl/gl.h> 
#include <gl/glu.h> 
#endif
//#include <gl/glext.h>

#elif __MACOSX__
#include <SDL_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
// pour pouvoir loader une texture
#include "dkt.h"



struct typ_characterProp{
	float u1,v1;
	float u2,v2;
	float w;

	typ_characterProp(){
		u1=0;v1=0;
		u2=0;v2=0;
		w=0;
	}
};



class CFont
{
public:
	// Le fichier Font .fnt
	CString filename;

	// La texture utilisé (ogl)
	unsigned int textureID;

	// La grosseur de chaque lettre
	int kerning[256];
	float finalCaracKerning[256];

	// La hauteur des lettres
	int height;

	// La liste de display list contenant les caractère individuel
	unsigned int baseFont;

	// Son ID du dkf
	unsigned int fontID;

	// Les propriétées de chaque lettre
	typ_characterProp characterProp[256];

	// Si cette font est tga ou fnt
	bool fntFont;

	// Le nb d'instance
	int nbInstance;

public:
	// Constructeurs
	CFont();

	// Destructeur
	virtual ~CFont();

public:

	// Pour la détruire
	void destroy();

	// Pour loader les propriétées de la police d'un fichier
	int loadFontFile(ifstream &fntFile);
	int loadTGAFile(char * tgaFile);

	// Pour créer la police
	int create(CString filename); // attention, ceci ne cré PAS les display list
	void reloadIt(); // Ceci va la créer (parce que ça nous prend le bon context avec le bon renderer)

	// Pour imprimer du texte à l'aide de cette police
	void printText(float size, float x, float y, float z, char *text);
};


#endif
