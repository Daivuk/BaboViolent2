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
#include "IntroScreen.h"
#include "GameVar.h"



//
// Constructeur
//
IntroScreen::IntroScreen()
{
	showDelay = 6;
	tex_rndLogo = dktCreateTextureFromFile("main/textures/RnDLabs.tga", DKT_FILTER_LINEAR);
//	tex_glowLogo = dktCreateTextureFromFile("main/textures/RnDLabsGlow.tga", DKT_FILTER_LINEAR);
	tex_hgLogo = dktCreateTextureFromFile("main/textures/HeadGames.tga", DKT_FILTER_LINEAR);
//	sfx_intro = dksCreateSoundFromFile("main/Sounds/IntroScreen.mp3", false);

//	FSOUND_SetSFXMasterVolume((int)(255.0f*gameVar.s_masterVolume));
/*	if (gameVar.s_masterVolume > 0)
	{
		dksPlayMusic("main/sounds/IntroScreen.mp3", -1);
	}*/
}



//
// Destructeur
//
IntroScreen::~IntroScreen()
{
	dktDeleteTexture(&tex_rndLogo);
	dktDeleteTexture(&tex_hgLogo);
//	dksDeleteSound(sfx_intro);
}



//
// Update
//
void IntroScreen::update(float delay)
{
	if (dkiGetFirstDown() != DKI_NOKEY) showDelay = 0;
	if (showDelay == 6)
	{
	//	dksPlaySound(sfx_intro, -1, 255);
	}
	showDelay -= delay;
}



//
// Render
//
void IntroScreen::render()
{
#ifndef _DX_
	dkglPushOrtho(1,1);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_TEXTURE_2D);
			if (showDelay > 5)
			{
				glBindTexture(GL_TEXTURE_2D, tex_hgLogo);
				glColor3f(1-(showDelay-5),1-(showDelay-5),1-(showDelay-5));
			}
			else if (showDelay > 4)
			{
				glBindTexture(GL_TEXTURE_2D, tex_hgLogo);
				glColor3f(1,1,1);
			}
			else if (showDelay > 3)
			{
				glBindTexture(GL_TEXTURE_2D, tex_hgLogo);
				glColor3f(showDelay - 3,showDelay - 3,showDelay - 3);
			}

			else if (showDelay > 2)
			{
				glBindTexture(GL_TEXTURE_2D, tex_rndLogo);
				glColor3f(1-(showDelay-2),1-(showDelay-2),1-(showDelay-2));
			}
			else if (showDelay > 1)
			{
				glBindTexture(GL_TEXTURE_2D, tex_rndLogo);
				glColor3f(1,1,1);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, tex_rndLogo);
				glColor3f(showDelay, showDelay, showDelay);
			}
			glBegin(GL_QUADS);
				glTexCoord2i(0,1);
				glVertex2i(0,0);
				glTexCoord2i(0,0);
				glVertex2i(0,1);
				glTexCoord2i(1,0);
				glVertex2i(1,1);
				glTexCoord2i(1,1);
				glVertex2i(1,0);
			glEnd();
		/*	glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBindTexture(GL_TEXTURE_2D, tex_glowLogo);
			if (showDelay > 5) glColor4f(0,0,0,.1f);
			else if (showDelay > 4) glColor4f(1-(showDelay-4),1-(showDelay-4),1-(showDelay-4),.1f);
			else if (showDelay > 2) glColor4f(1,1,1,.1f);
			else glColor4f(showDelay,showDelay,showDelay,.1f);

			float scale = 0;
			if (showDelay < 3) scale = (3 - showDelay) / 3.0f * .025f;

			for (float i=0;i<10;++i)
			{
				glPushMatrix();
					glTranslatef(.5f,.5f,0);
					glScalef(i*scale+.5f,i*scale+.5f,1);
					glRotatef(scale*1000*(i/20.0f), 0, 0, 1);
					glBegin(GL_QUADS);
						glTexCoord2i(0,1);
						glVertex2i(-1,-1);
						glTexCoord2i(0,0);
						glVertex2i(-1,1);
						glTexCoord2i(1,0);
						glVertex2i(1,1);
						glTexCoord2i(1,1);
						glVertex2i(1,-1);
					glEnd();
				glPopMatrix();
			}*/
		glPopAttrib();
	dkglPopOrtho();
#endif
}
#endif

