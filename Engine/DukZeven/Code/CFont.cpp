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


#include "CFont.h"



//
// Constructeur / Destructeur
//
CFont::CFont()
{
	nbInstance = 1;
	textureID = 0;
	baseFont = 0;
	fontID = 0;
	fntFont = false;
	for (int i=0;i<256;i++) 
	{
		kerning[i] = 0;
		finalCaracKerning[i] = 0;
	}
}

CFont::~CFont()
{
	destroy();
	dktDeleteTexture(&textureID);
}



//
// On détruir la police et la réinit
//
void CFont::destroy()
{
#ifndef _DX_
	if (baseFont) glDeleteLists(baseFont, 128);
#endif
	baseFont = 0;
}



//
// Pour loader les propriétées de la police d'un fichier
//
int CFont::loadFontFile(ifstream &fntFile)
{
	char variable[256];
	fntFile >> variable;

	while (!fntFile.eof())
	{
		if (variable[0] == '/' && variable[0] == '/')
		{
			fntFile.ignore(512, '\n');
			fntFile >> variable;
			continue;
		}

		if (stricmp(variable, "TEXTURE") == 0)
		{
			fntFile >> variable;
			// On cré notre texture
			textureID = dktCreateTextureFromFile(variable, DKT_FILTER_BILINEAR);
		}

		if (stricmp(variable, "HEIGHT") == 0)
		{
			fntFile >> height;
		}

		if (stricmp(variable, "KERNING") == 0)
		{
			int current = 0;
			fntFile >> variable;
			while (!fntFile.eof() && !(stricmp(variable, ";") == 0))
			{
				if (stricmp(variable, "OFFSET") == 0)
				{
					kerning[current++] = -1;
					fntFile >> kerning[current++];
					fntFile >> variable;
					continue;
				}
				if (stricmp(variable, "NEWLINE") == 0)
				{
					kerning[current++] = -2;
					fntFile >> variable;
					continue;
				}

				// Sinon on load le charactère
				fntFile >> kerning[current++];
				fntFile >> variable;
			}
		}
		
		fntFile >> variable;
	}

	fntFont = true;
	return 1;
}



//
// On cré une font, mais uniquement à partir d'un TGA
//
int CFont::loadTGAFile(char * tgaFile)
{
	// On cré notre texture
	textureID = dktCreateTextureFromFile(tgaFile, DKT_FILTER_BILINEAR);

	// Bon, ben on cré notre font avec cette texture là
	if (textureID)
	{
		fntFont = false;
		unsigned char * alphaData = new unsigned char [512 * 512];
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alphaData);
		glPopAttrib();
#endif

		// Chaque caracter fait 64 de haut
		characterProp[32].u1 = 0;
		characterProp[32].v1 = 0;
		characterProp[32].u2 = 0;
		characterProp[32].v2 = 0;
		characterProp[32].w = .25f;
		finalCaracKerning[32] = characterProp[32].w;

		int charH = 64;
		int curX = 0;
		int curY = 0;
		for (int c=33;c<128+32;c++){
			// Bon, on compte les pixel yééé
			bool nextCarac = false;
			while (!nextCarac){
				for (int j=0;j<charH;j++){
					if (alphaData[(512 - (j+curY)-1)*512 + (curX)] > 0){
						int from = curX-1;
						int to;
						if (from < 0) from = 0;

						// Maintenant on check pour la fin du caractère
						while (true)
						{
							bool allFalse = true;
							for (int jj=0;jj<charH;jj++)
							{
								if (alphaData[(512 - (jj+curY)-1)*512 + (curX)] > 0)
								{
									allFalse = false;
									break;
								}
							}
							if (allFalse || curX >= 512-1)
							{
								to = curX+1;
								if (to >= 512) to = 512;
								break;
							}
							curX++;
						}

						// Bon, maintenant on peut créer ce character // 25, 34
						characterProp[c].w = (float)(to - from) / (float)charH;
						characterProp[c].u1 = (float)from / (float)512;
						characterProp[c].u2 = (float)to / (float)512;
						characterProp[c].v1 = 1-(float)curY / (float)512;
						characterProp[c].v2 = 1-(float)(curY+charH) / (float)512;
						finalCaracKerning[c] = characterProp[c].w;

						nextCarac = true;
						break;
					}
				}
				curX++;
				if (curX >= 512){
					curX = 0;
					curY += charH;
					if (curY >= 512){
						c = 128+32; // On a fini
						nextCarac = true;
						break;
					}
				}
			}
		}

		delete [] alphaData;
	}

	return textureID;
}



//
// Signifie que l'on vient de créer l'objet font
//
int CFont::create(CString filename)
{
	if (strnicmp(&(filename.s[strlen(filename.s)-3]), "TGA", 3) == 0)
	{
		return loadTGAFile(filename.s);
	}

	// On ouvre le fichier de définition
	ifstream fntFile(filename.s, ios::in);

	// On check si ça fonctionné
	if (fntFile.fail())
	{
		return 0;
	}

	// On load le tout!
	if (!loadFontFile(fntFile)) {fntFile.close(); return 0;}

	// On start l'application et on run
	fntFile.close();

	return 1;
}



//
// On la construit :
// On construit la display list à l'aide de notre tableau de kerning
//
void CFont::reloadIt()
{
	// On détruit l'encien avant
	destroy();

	// On génère 256 display list pour stocker tout les caractères
#ifndef _DX_
	baseFont = glGenLists(256);
#endif


	if (fntFont)
	{
		// Variable utilent pour faire le bon pourcentage avec le kerning
		int x = 0;
		int y = 0;

		// On passe nos 128 en loop
		int i = 0, k = 0;
		while (i<128)
		{
			// On pogne le kerning
			if (kerning[k] == -1)
			{
				k++; x += kerning[k++];
				continue;
			}
			if (kerning[k] == -2)
			{
				x = 0;
				y += height;
				k++;
				continue;
			}

			// On les fou en pourcentage sur 512
			float txf = (float)x / 512.0f;
			float tyf = (float)y / 512.0f;
			float twidthf = (float)(kerning[k]) / 512.0f;
			float theightf = (float)height / 512.0f;
			float widthf = twidthf / theightf;
			x+=kerning[k];
			k++;
			finalCaracKerning[i+32] = widthf;

			// Ensuite on crée notre display list pour ce caractère
#ifndef _DX_
			glNewList(baseFont+i+32 /* on commence au charatère 32, "Space" */, GL_COMPILE);

				// On dessine un quad de 16x16 avec la lettre affiché dedans
				glBegin(GL_QUADS);
					glTexCoord2f(txf,1-tyf);
					glVertex2f(0,0);

					glTexCoord2f(txf,1-(tyf+theightf));
					glVertex2f(0,1);

					glTexCoord2f(txf+twidthf,1-(tyf+theightf));
					glVertex2f(widthf,1);

					glTexCoord2f(txf+twidthf,1-tyf);
					glVertex2f(widthf,0);
				glEnd();

				// On le déplace de 10 pour prévoir l'espace entre deux lettres
				glTranslatef(widthf,0,0);
			glEndList();
#endif

			// On incrémente pour le charactère suivant
			i++;
		}

		// On cré les charatères spéciaux (le enter)
#ifndef _DX_
		glNewList(baseFont+1, GL_COMPILE);
			glColor3f(0,0,1);
		glEndList();
		glNewList(baseFont+2, GL_COMPILE);
			glColor3f(0,1,0);
		glEndList();
		glNewList(baseFont+3, GL_COMPILE);
			glColor3f(0,1,1);
		glEndList();
		glNewList(baseFont+4, GL_COMPILE);
			glColor3f(1,0,0);
		glEndList();
		glNewList(baseFont+5, GL_COMPILE);
			glColor3f(1,0,1);
		glEndList();
		glNewList(baseFont+6, GL_COMPILE);
			glColor3f(1,.7f,0);
		glEndList();
		glNewList(baseFont+7, GL_COMPILE);
			glColor3f(.5f,.5f,.5f);
		glEndList();
		glNewList(baseFont+8, GL_COMPILE);
			glColor3f(1,1,1);
		glEndList();
		glNewList(baseFont+9, GL_COMPILE);
			glColor3f(1,1,0);
		glEndList();
		glNewList(baseFont+10, GL_COMPILE);
			glPopMatrix();
			glTranslatef(0,1,0);
			glPushMatrix();
		glEndList();
#endif
	}
	else
	{
		// On passe nos 128 en loop
		int i = 0;
		while (i<128)
		{
			// Ensuite on crée notre display list pour ce caractère
#ifndef _DX_
			glNewList(baseFont+i+32 /* on commence au charatère 32, "Space" */, GL_COMPILE);

				// On dessine un quad de 16x16 avec la lettre affiché dedans
				glBegin(GL_QUADS);
					glTexCoord2f(characterProp[i+32].u1,characterProp[i+32].v1);
					glVertex2f(0,0);

					glTexCoord2f(characterProp[i+32].u1,characterProp[i+32].v2);
					glVertex2f(0,1);

					glTexCoord2f(characterProp[i+32].u2,characterProp[i+32].v2);
					glVertex2f(characterProp[i+32].w,1);

					glTexCoord2f(characterProp[i+32].u2,characterProp[i+32].v1);
					glVertex2f(characterProp[i+32].w,0);
				glEnd();

				// On le déplace de 10 pour prévoir l'espace entre deux lettres
				glTranslatef(characterProp[i+32].w,0,0);
			glEndList();
#endif

			// On incrémente pour le charactère suivant
			i++;
		}

#ifndef _DX_
		// On cré les charatères spéciaux (le enter)
		glNewList(baseFont+1, GL_COMPILE);
			glColor3f(.25f,.25f,1);
		glEndList();
		glNewList(baseFont+2, GL_COMPILE);
			glColor3f(.25f,1,.25f);
		glEndList();
		glNewList(baseFont+3, GL_COMPILE);
			glColor3f(.25f,1,1);
		glEndList();
		glNewList(baseFont+4, GL_COMPILE);
			glColor3f(1,.25f,.25f);
		glEndList();
		glNewList(baseFont+5, GL_COMPILE);
			glColor3f(1,.25f,1);
		glEndList();
		glNewList(baseFont+6, GL_COMPILE);
			glColor3f(1,.7f,0);
		glEndList();
		glNewList(baseFont+7, GL_COMPILE);
			glColor3f(.5f,.5f,.5f);
		glEndList();
		glNewList(baseFont+8, GL_COMPILE);
			glColor3f(1,1,1);
		glEndList();
		glNewList(baseFont+9, GL_COMPILE);
			glColor3f(1,1,0);
		glEndList();
		glNewList(baseFont+10, GL_COMPILE);
			glPopMatrix();
			glTranslatef(0,1,0);
			glPushMatrix();
		glEndList();
#endif
	}
}



//
// On imprime le text à l'écran en utilisant cette police
//
void CFont::printText(float size, float x, float y, float z, char *text)
{
#ifndef _DX_
	glPushMatrix();
		glTranslatef(x,y,z);
		glScalef(size,size,size);
		glPushMatrix();
			// On dessine ici la chaine avec CallLists
			glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIST_BIT | GL_TEXTURE_BIT);
				glBindTexture(GL_TEXTURE_2D, textureID);
				glEnable(GL_TEXTURE_2D);
				glListBase(baseFont);
				glCallLists(GLsizei(strlen(text)), GL_UNSIGNED_BYTE, text);
			glPopAttrib();
		glPopMatrix();
	glPopMatrix();
#endif
}
