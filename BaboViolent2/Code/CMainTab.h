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
#ifndef CMAINTAB_H_INCLUDED
#define CMAINTAB_H_INCLUDED


#include "CListener.h"
#include "CNews.h"
#include "CProfile.h"
#include "CBrowser.h"
#include "CControl.h"
#include "CHost.h"
#include "CEditor.h"
#include "CCredit.h"
#include "COption.h"
#include "CMaster.h"




class CMainTab : public CListener
{
public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	//--- Top Buttons (tab)
	//CControl * btn_userLogin;
	CControl * btn_news;
	CControl * btn_profile;
	CControl * btn_browser;
	//CControl * btn_friends;
	CControl * btn_create;
	CControl * btn_editor;
	CControl * btn_option;
	CControl * btn_credits;
	CControl * btn_quit;

	//--- Nos menus
	CControl * lastButtonPress;
	CListener * current;
	CNews * news;
	CProfile * profile;
	CBrowser * browser;
	CHost * host;
	CEditor * editor;
	CCredit * credit;
	COption * option;

	int timeBrowseDelay;

public:
	CMainTab(CControl * in_parent);
	virtual ~CMainTab();

	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);

	void update(float delay)
	{
		if (timeBrowseDelay > 0)
		{
			timeBrowseDelay--;
			if (timeBrowseDelay == 0 && !master->isConnected())
			{
				//--- On clear la liste avant
				browser->lst_browseList->clearList();
				browser->nbGames = 0;
				master->requestGames();
			}
		}
		if (current) current->update(delay);
	}
};


#endif
#endif

