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

#include "Scene.h"

// Creating menu
void Scene::createMenu()
{
	//--- Overall control
	menuManager.root = new CControl();
	menuManager.root->font = font;
	menuManager.root->size.set(800,600);
	menuManager.root->backColor.set(.3f,.5f,.8f);
	menuManager.root->foreColor.set(1,1,1);
	menuManager.root->textShadow = true;
	menuManager.root->noFill = true;
	if(gameVar.r_animatedMenu)
	{
		menuManager.root->superPanel = true;
		menuManager.root->texture = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_NEAREST);
	}

	//--- Create the main tab container
	mainTab = new CMainTab(menuManager.root);
}


void Scene::createSurvey()
{
	//--- Overall control
	menuManager.root = new CControl();
	menuManager.root->font = font;
	menuManager.root->size.set(800,600);
	menuManager.root->backColor.set(.3f,.5f,.8f);
	menuManager.root->foreColor.set(1,1,1);
	menuManager.root->textShadow = true;
	menuManager.root->noFill = true;

	//--- Create the survey container
	survey = new CSurvey(menuManager.root);
}


#endif

