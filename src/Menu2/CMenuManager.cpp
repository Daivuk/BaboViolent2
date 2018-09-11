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

#include "CControl.h"
#include "CMenuManager.h"
#include "Helper.h"
#include "Scene.h"

CMenuManager menuManager;
extern Scene *scene;


CMenuManager::CMenuManager()
{
	activeControl = 0;
	focusControl = 0;
	hoveringControl = 0;
	root = 0;
	removeTop = 0;
}


CMenuManager::~CMenuManager()
{
	ZEVEN_SAFE_DELETE(root);
}



void CMenuManager::update(float delay, CControl * toUpdate)
{
	hoveringControl = 0;
	CVector2i res = dkwGetResolution();
	mousePos = dkwGetCursorPos_main();

	//--- On criss la mouse pos sur 800x600
	mousePos[0] = (int)(((float)mousePos[0] / (float)res[0]) * 800.0f);
	mousePos[1] = (int)(((float)mousePos[1] / (float)res[1]) * 600.0f);

//	if (dialogs.empty() == false)
//		return;

	if (toUpdate)
	{
		toUpdate->update(delay);
	}
	else if (root != NULL && dialogs.empty()) 
	{
		root->update(delay);
	}
}

void CMenuManager::updateDialogs(float delay)
{
	if (dialogs.empty())
		return;
	hoveringControl = 0;
	CVector2i res = dkwGetResolution();
	mousePos = dkwGetCursorPos_main();

	//--- On criss la mouse pos sur 800x600
	mousePos[0] = (int)(((float)mousePos[0] / (float)res[0]) * 800.0f);
	mousePos[1] = (int)(((float)mousePos[1] / (float)res[1]) * 600.0f);

	/*while (dialogs.empty() == false)
	{
		if (dialogs.top().done || dialogs.top().canceled)
			dialogs.pop();
		else
			break;
	}*/

//	int startSize = dialogs.c.size();// [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
	int startSize = dialogs.size();
	for (int i = startSize - 1; i >= 0; )
	{
	//	dialogs.c.at(i)->update(delay); // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
		dialogs[i]->update(delay);	

	/*	if (startSize != dialogs.c.size()) // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
		{
			startSize = dialogs.c.size();
			i = startSize - 1;
		}*/
		if (startSize != dialogs.size())
		{
			startSize = dialogs.size();
			i = startSize - 1;
		}
		else
			i--;
	}
/*	if (removeTop == 1) // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
	{
		dialogs.pop();
	}
	else if (removeTop == 2)
	{
		delete dialogs.top();
		dialogs.pop();
	}*/
	if (removeTop == 1)
	{
		dialogs.pop_back();
	}
	else if (removeTop == 2)
	{
		delete dialogs.back();
		dialogs.pop_back();
	}
	removeTop = 0;
}

extern bool enableShadow;

void CMenuManager::render(CControl * toRender)
{
#ifndef _DX_
	glClear(GL_DEPTH_BUFFER_BIT);
#endif
	CVector2i res = dkwGetResolution();
	if(gameVar.r_widescreen > 1) res[0] = static_cast<int>(res[1]*1.333f);

#ifndef _DX_
	dkglPushOrtho(800, 600);
		glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (toRender) toRender->render();
			else if (root) root->render();
		glPopAttrib();
	dkglPopOrtho();
#endif

	//--- Head games logo bottom right // Temporarly disabled until 2.07
	/*dkglPushOrtho(800, 600);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor3f(1,1,1);
			glBindTexture(GL_TEXTURE_2D, scene->tex_miniHeadGames);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
				glTexCoord2i(0,1);
				glVertex2i(800-128,600-128);
				glTexCoord2i(0,0);
				glVertex2i(800-128,600-64);
				glTexCoord2i(1,0);
				glVertex2i(800-64,600-64);
				glTexCoord2i(1,1);
				glVertex2i(800-64,600-128);
			glEnd();
		glPopAttrib();
	dkglPopOrtho();*/

	renderTooltip(hoveringControl, res);
}

void CMenuManager::renderDialogs()
{
	if (dialogs.empty() == false)
	{
#ifndef _DX_
		glClear(GL_DEPTH_BUFFER_BIT);
#endif
		CVector2i res = dkwGetResolution();
		if(gameVar.r_widescreen > 1) res[0] = static_cast<int>(res[1]*1.333f);

#ifndef _DX_
		dkglPushOrtho(800, 600);
			glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//for (int i = dialogs.c.size() - 1; i >= 0; i--)
				//	dialogs.c.at(i)->render();
			//	dialogs.top()->render(); // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
				dialogs.back()->render();
			glPopAttrib();
		dkglPopOrtho();
#endif

		renderTooltip(hoveringControl, res);
	}
}

void CMenuManager::renderTooltip(CControl * control, const CVector2i& res)
{
#ifndef _DX_
	//--- We render the tooltips text if the mouse if over that control
	dkglPushOrtho((float)res[0], (float)res[1]);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (control)
			{
				if (!control->toolTips.isNull())
				{
					int w = (int)dkfGetStringWidth(25, control->toolTips.s) + 10;
					int h = (int)dkfGetStringHeight(25, control->toolTips.s) + 10;

					CVector2i realMousePos = dkwGetCursorPos_main();

					int x = realMousePos[0] + 16;
					int y = realMousePos[1] + 16;

					if (x + w > res[0]) x = res[0] - w;
					if (y + h > res[1]) y = res[1] - h;

					glColor3f(0, 0, 0);
					renderMenuQuad(x-1, y-1, w+2, h+2);
					glColor3f(.5f, .5f, .38f);
					renderMenuQuad(x, y, w, h);

				//	enableShadow = false;
					glColor3f(1,1,1);
					printLeftText((float)x+5,(float)y+5,25,control->toolTips);
				//	enableShadow = true;
				}
			}
		glPopAttrib();
	dkglPopOrtho();
#endif
}

void CMenuManager::showDialog(IDialog* dialog)
{
//	dialogs.push(dialog); // [dsl] Changed that to vector.. Wasn't used as a stack wasn't compiling on 2008...
	dialogs.push_back(dialog);
}

void CMenuManager::hideDialog(bool autoMemFree/*IDialog* dialog*/)
{
	//dialogs.pop();
	if (!autoMemFree)
		removeTop = 1;
	else
		removeTop = 2;
}

#endif

