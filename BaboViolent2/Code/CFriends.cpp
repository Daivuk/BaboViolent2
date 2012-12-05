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

#include "CFriends.h"
#include "Console.h"
#include "GameVar.h"
#include "CMenuManager.h"
#include "CCurl.h"
#include "CStatus.h"


#define TIXML_USE_STL
#include "tinyxml.h"

CFriends::CFriends(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(in_parent->size.s[0]-20, 450), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->borderColor.set(1,.5f,.25f);

	// Server join controls
	CControl * label_pw = new CControl(instance, CVector2i(10,10), CVector2i(80,25),"Password : ", this, "LABEL");
	label_pw->toolTips = "Enter the password and join here or click a game below.";

	txt_pw = new CControl(instance, CVector2i(10,10), CVector2i(100,25),"", this, "EDIT", label_pw, CONTROL_SNAP_RIGHT);
	txt_pw->password = true;

	btn_join = new CControl(instance, CVector2i(32,32),CVector2i(60,25),"Join",this,"BUTTON", txt_pw, CONTROL_SNAP_RIGHT, 5);
	btn_join->toolTips = "Click to join the selected game/ip !";

	// Friends list controls
	//CControl * label1 = new CControl(instance, CVector2i(10,10), CVector2i(40,25),"Name: ", this, "LABEL", btn_join, CONTROL_SNAP_RIGHT, 40);
	//label1->toolTips = "Name of the friend you'd like to search for.";

	//txt_name = new CControl(instance, CVector2i(10,10), CVector2i(150,25),"", this, "EDIT", label1, CONTROL_SNAP_RIGHT);

	//btn_search = new CControl(instance, CVector2i(32,32),CVector2i(60,25),"Search",this,"BUTTON", txt_name, CONTROL_SNAP_RIGHT, 5);
	//btn_search->toolTips = "Click here to search for a friend";

	btn_refresh = new CControl(instance, CVector2i(32,32),CVector2i(120,25),"Refresh",this,"BUTTON", txt_pw, CONTROL_SNAP_RIGHT, 390);
	btn_refresh->toolTips = "Refresh the friends";

	lst_browseList = new CControl(instance, CVector2i(0,0), CVector2i(716,460), "", this, "LISTBOX", label_pw, CONTROL_SNAP_BOTTOM, 5);
	lst_browseList->texture = dktCreateTextureFromFile("main/textures/Menu2Back.tga", DKT_FILTER_LINEAR);

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	lst_browseList->backColor.set(0,.3f,.7f);
	lst_browseList->imgColor = lst_browseList->backColor;

	originalY = instance->localPos[1];

	clickInterval = 0;
	lastClicked = 0;
	request = 0;
}

CFriends::~CFriends()
{
	while(request->isRunning()) {};
	delete request;
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
}


//
//--- Update it
//
void CFriends::updatePerso(float delay)
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

	if(request && !request->isRunning() && request->recieved() > 0 && requestProcessed == false)
	{
		requestProcessed = true;
		console->add(CString("\x8> Friends request recieved (%i bytes)",request->recieved()));

		if(request->response()[0] == '<')
		{
			TiXmlDocument doc;
			doc.Parse(request->response().c_str());

			if(!doc.Error())
			{
				console->add("\x8> Friends Parse successful");

				TiXmlHandle docHandle(&doc);
				TiXmlElement* item = docHandle.FirstChild("friends").FirstChild().ToElement();

				for(item; item; item = item->NextSiblingElement() )
				{
					const char* name = item->Attribute("name");
					const char* sname = item->Attribute("server_name");
					int stat = 0;
					item->QueryIntAttribute("location", &stat);
					int lastSeen = 0;
					item->QueryIntAttribute("last_update", &lastSeen);
					const char* ip = item->Attribute("server_ip");
					const char* port = item->Attribute("server_port");

					CString statusText =status->getText(stat);
					
					CControl* friendRow = new CControl(lst_browseList, CVector2i(10, 10 + nbGames*30), CVector2i(676,30), "", this, "LABEL");
					CControl* Name = new CControl(friendRow, CVector2i(0, 5), CVector2i(150,20), CString("%s",name), this, "LABEL");
					CControl* serverOn = new CControl(friendRow, CVector2i(150, 5), CVector2i(300,20), CString("%s", sname), this, "LABEL");
					CControl* friendStatus = new CControl(friendRow, CVector2i(450, 5), CVector2i(100,20), statusText, this, "LABEL");

					CControl* since;

					if (lastSeen < 60)
					{
						since = new CControl(friendRow, CVector2i(550, 5), CVector2i(100,20), CString("%i mins ago", lastSeen % 60), this, "LABEL");
					}
					else if (lastSeen < 60 * 24)
					{
						since = new CControl(friendRow, CVector2i(550, 5), CVector2i(100,20), CString("%i hours ago", (lastSeen / 60) % 24), this, "LABEL");
					}
					else if (lastSeen < 60 * 24 * 7)
					{
						since = new CControl(friendRow, CVector2i(550, 5), CVector2i(100,20), CString("%i days ago", (lastSeen / 60 / 24) % 7), this, "LABEL");
					}
					else
					{
						since = new CControl(friendRow, CVector2i(550, 5), CVector2i(100,20), CString("%i weeks ago", (lastSeen / 60 / 24 / 7)), this, "LABEL");
					}

					CString* ipport = new CString("%s %s", ip, port);
					friendRow->customData = ipport;

					nbGames++;
				}
			}
		}
	}
}

void CFriends::updateList()
{
	lst_browseList->clearList();
	nbGames = 0;

	if(request == 0 || !request->isRunning())
	{
		if(request != 0)
			delete request;

		CUrlData data;
		data.add("action", "getfriends");
		data.add("username",gameVar.cl_accountUsername.s);
		data.add("password", gameVar.cl_accountPassword.s, CUrlData::MD5);
		data.add("ip", bb_getMyIP());

		request = new CCurl(gameVar.db_accountServer, data.get());
		request->start();
		requestProcessed = false;
		console->add("\x9> Starting friends request");
	}
}

void CFriends::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CFriends::MouseLeave(CControl * control)
{
}
void CFriends::MouseDown(CControl * control)
{
}
void CFriends::MouseUp(CControl * control)
{
}
void CFriends::MouseMove(CControl * control)
{
}
void CFriends::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_join)
	{
		if (lastClicked && lastClicked->customData)
		{
			CString* server = ((CString*)(lastClicked->customData));
			if(server->len() > 1)
			{
				command = "connect ";
				command += *server;
				command += " ";
				command += txt_pw->text;
				console->sendCommand(command);
			}
		}
	}
	if (control == btn_refresh)
	{
		updateList();
	}
}
void CFriends::Validate(CControl * control)
{
	dksPlaySound(m_sfxClic, -1, 200);
}
void CFriends::DbClick(CControl * control)
{
	if(menuManager.mousePos[1] < 100)
		return;

	// on check si y a un custom data
	//if (control->customData)
	//{
	//	txt_ip->text = ((SBrowsableGame*)(control->customData))->bv2Row->ip;
	//	txt_ip->text += CString(" " ) + CString("%i", ((SBrowsableGame*)(control->customData))->bv2Row->port);
	//}
	//// on check si y a un custom data
	if (control->customData)
	{
		//--- Bon, on check pour le double click!
		if (lastClicked == control && clickInterval < 1)
		{
			//	menuManager.activeControl = 0;
			//	menuManager.focusControl = 0;
			//	menuManager.hoveringControl = 0;

			//--- Bah, on launch ste game là !!!
			CString* server = ((CString*)(control->customData));
			if(server->len() > 1)
			{
				command = "connect ";
				command += *server;
				command += " ";
				command += txt_pw->text;
				console->sendCommand(command);
			}
		}
		else
		{
			clickInterval = 0;
			lastClicked = control;
		}
	}
}


#endif

