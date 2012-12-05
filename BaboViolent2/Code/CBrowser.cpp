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

#include "CBrowser.h"
#include "GameVar.h"
#include "Console.h"
#include "CLobby.h"
#include "CMaster.h"
#include "CMenuManager.h"
#include <algorithm>


// Sorting functor
struct SPingSort
{
	bool operator()(CControl* const & start, CControl* const & end)
	{
		SBrowsableGame* gstart = (SBrowsableGame*)(start->customData);
		SBrowsableGame* gend = (SBrowsableGame*)(end->customData);

		// Items with no customData are not servers and go first
		if ((gstart == 0) && (gend == 0))
			return (start < end);
		else if (gstart == 0)
			return true;
		else if (gend == 0)
			return false;

		// If ping is the same, sort by # of players
		if (gstart->ping == gend->ping)
			return gstart->bv2Row->nbPlayer > gend->bv2Row->nbPlayer;
		else
			return gstart->ping < gend->ping;

	}
};



CBrowser::CBrowser(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->borderColor.set(1,.5f,.25f);

	//--- Labels and controls
	CControl * label1 = new CControl(instance, CVector2i(10,10), CVector2i(90,25),"IP address : ", this, "LABEL");
	label1->toolTips = "Enter the IP to join here or click a game below.";

	txt_ip = new CControl(instance, CVector2i(10,10), CVector2i(150,25),(gameVar.cl_lastUsedIP != "0.0.0.0")?gameVar.cl_lastUsedIP:"", this, "EDIT", label1, CONTROL_SNAP_RIGHT);

	CControl * label_pw = new CControl(instance, CVector2i(10,10), CVector2i(80,25),"Password : ", this, "LABEL", txt_ip, CONTROL_SNAP_RIGHT);
	label_pw->toolTips = "Enter the password and join here or click a game below.";

	txt_pw = new CControl(instance, CVector2i(10,10), CVector2i(100,25),"", this, "EDIT", label_pw, CONTROL_SNAP_RIGHT);
	txt_pw->password = true;

	btn_join = new CControl(instance, CVector2i(32,32),CVector2i(120,25),"Join",this,"BUTTON", txt_pw, CONTROL_SNAP_RIGHT, 15);
	btn_join->toolTips = "Click to join the selected game/ip !";

	btn_refresh = new CControl(instance, CVector2i(32,32),CVector2i(120,25),"Refresh",this,"BUTTON", btn_join, CONTROL_SNAP_RIGHT, 15);
	btn_refresh->toolTips = "Refresh the games";

	lst_browseList = new CControl(instance, CVector2i(0,0), CVector2i(716,460), "", this, "LISTBOX", label1, CONTROL_SNAP_BOTTOM, 5);
	lst_browseList->texture = dktCreateTextureFromFile("main/textures/Menu2Back.tga", DKT_FILTER_LINEAR);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	lst_browseList->backColor.set(0,.3f,.7f);
	lst_browseList->imgColor = lst_browseList->backColor;

	originalY = instance->localPos[1];

	clickInterval = 0;
	lastClicked = 0;
}

//
//--- Update it
//
void CBrowser::updatePerso(float delay)
{
	if (!command.isNull())
	{
		console->sendCommand(command.s);
		command = "";
		menuManager.activeControl = 0;
		menuManager.focusControl = 0;
		menuManager.hoveringControl = 0;
	}
	clickInterval += delay;
	if (lobby)
	{
		SBrowsableGame* bv2Row = 0;
		while (bv2Row = (SBrowsableGame*)lobby->getNext())
		{
			SBrowsableGame* holdRow = new SBrowsableGame();
			memcpy(holdRow, bv2Row, sizeof(SBrowsableGame));
			CString gameType;
			switch (holdRow->bv2Row->gameType)
			{
			case 0:
				gameType = "Babomatch";
				break;
			case 1:
				gameType = "Team Babomatch";
				break;
			case 2:
				gameType = "Capture the Flag";
				break;
			case 3:
#ifdef _PRO_
            gameType = "Champion";
#else
				gameType = "Counter Baborist";
#endif
				break;
			}
			CControl* gameRow = new CControl(lst_browseList, CVector2i(10, 10 + nbGames*30), CVector2i(676,30), "", this, "LABEL");
			gameRow->customData = holdRow; // Voilà pour ça
	
			CControl* Name = new CControl(gameRow, CVector2i(0, 5), CVector2i(350,20), holdRow->bv2Row->serverName, this, "LABEL");
			CControl* Type = new CControl(gameRow, CVector2i(350, 5), CVector2i(150,20), gameType, this, "LABEL");
		
			CString players("%2i/%2i", holdRow->bv2Row->nbPlayer, holdRow->bv2Row->maxPlayer);
			CControl* Players = new CControl(gameRow, CVector2i(500, 5), CVector2i(88,20), players, this, "LABEL");

			CString ping;
			if (holdRow->ping >= 1000)
			{
				ping = "\x5???";
			}
			else if (holdRow->ping > 200)
			{
				ping = CString("\x4") + holdRow->ping;
			}
			else if (holdRow->ping > 100)
			{
				ping = CString("\x9") + holdRow->ping;
			}
			else
			{
				ping = CString("\x2") + holdRow->ping;
			}
			CControl* Ping = new CControl(gameRow, CVector2i(588, 5), CVector2i(88,20), ping, this, "LABEL");

			//--- Build the tooltip
			Name->toolTips = "";
			Name->toolTips += CString("\x6Map name:\x8 %s\n", holdRow->bv2Row->map);
			Name->toolTips += CString("\x6Passworded:\x8 %s\n", (holdRow->bv2Row->password[0] == 0)?"\x2No":"\x4Yes");
			Name->toolTips += CString("\x6IP:\x8 %s\n", holdRow->bv2Row->ip);
			Name->toolTips += CString("\x6Port:\x8 %i", holdRow->bv2Row->port);

			nbGames++;
		}

		// Sort	
		std::sort(lst_browseList->children.begin(), lst_browseList->children.end(), SPingSort());

		// Set positions
		for(unsigned int i = 2; i < lst_browseList->children.size(); ++i)
		for(int i = 2; i < (int)lst_browseList->children.size(); ++i)
		{
			lst_browseList->children[i]->localPos.y() = 10 + (i-2)*30;
		}

	}
}



CBrowser::~CBrowser()
{
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


void CBrowser::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CBrowser::MouseLeave(CControl * control)
{
}
void CBrowser::MouseDown(CControl * control)
{
}
void CBrowser::MouseUp(CControl * control)
{
}
void CBrowser::MouseMove(CControl * control)
{
}
void CBrowser::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_join)
	{
		if (!txt_ip->text.isNull())
		{
			gameVar.cl_lastUsedIP = txt_ip->text;

			CString copy = txt_ip->text;
			CString ip  = copy.getFirstToken(' ');
			CString port = copy.getNextToken(' ');

			if (port != "")
				console->sendCommand(CString("connect %s %i %s", ip.s, port.toInt(), txt_pw->text.s));
			else
				console->sendCommand(CString("connect %s %i %s", ip.s, gameVar.cl_port, txt_pw->text.s));
		}
	}
	if (control == btn_refresh)
	{
		//--- On clear la liste avant
		lst_browseList->clearList();
		nbGames = 0;
		if (master) master->requestGames();
	}
}
void CBrowser::Validate(CControl * control)
{
	dksPlaySound(m_sfxClic, -1, 200);
}
void CBrowser::DbClick(CControl * control)
{
	if(menuManager.mousePos[1] < 100)
		return;

	// on check si y a un custom data
	if (control->customData)
	{
		txt_ip->text = ((SBrowsableGame*)(control->customData))->bv2Row->ip;
		txt_ip->text += CString(" " ) + CString("%i", ((SBrowsableGame*)(control->customData))->bv2Row->port);
	}
	// on check si y a un custom data
	if (control->customData)
	{
		//--- Bon, on check pour le double click!
		if (lastClicked == control && clickInterval < 1)
		{
		//	menuManager.activeControl = 0;
		//	menuManager.focusControl = 0;
		//	menuManager.hoveringControl = 0;

			//--- Bah, on launch ste game là !!!
			command = "connect ";
			command += ((SBrowsableGame*)(control->customData))->bv2Row->ip;
			command += " ";
			command += CString("%i", ((SBrowsableGame*)(control->customData))->bv2Row->port);
			command += " ";
			command += txt_pw->text;
		//	console->sendCommand(command);
		}
		else
		{
			clickInterval = 0;
			lastClicked = control;
		}
	}
}


#endif


