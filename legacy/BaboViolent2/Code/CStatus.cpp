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

#include <vector>
#include "Zeven.h"
#include "CStatus.h"
#include "CCurl.h"
#include "GameVar.h"
#include "Console.h"

CStatus * status = 0;

CStatus::CStatus(): m_status(ONLINE)
{
	update();
}

CStatus::~CStatus()
{
	// Send one last update
	if(m_status != OFFLINE)
	{
		set(OFFLINE);
		while(m_requests.size() > 0) 
		{
			processQueue(); 
		}
	}
}

void CStatus::set(int status, CString serverName /* =  */, CString serverIP /* =/* =  */, int port /* = 0 */)
{
	m_status = status;
	m_server = serverName;
	m_ip = serverIP;
	m_port = port;
	update();
}

int CStatus::get()
{
	return m_status;
}

CString CStatus::getText(int in_status)
{
	// Allow this to be used to get status text for other users
	if(in_status < OFFLINE || in_status > INGAME)
		in_status = m_status;

	switch(in_status)
	{
	case ONLINE:
		return CString("%s", "\x2Online");
	case WEBSITE:
		return CString("%s", "\x3Website");
	case INGAME:
		return CString("%s", "\x9In Game");
	case OFFLINE:
	default:
		return CString("%s", "\x7Offline");
	}
}

void CStatus::update()
{
	// Send updated status
	CUrlData data;
	data.add("action", "updatestatus");
	data.add("username",gameVar.cl_accountUsername.s);
	data.add("password", gameVar.cl_accountPassword.s, CUrlData::MD5);
	data.add("location", m_status);
	data.add("ip", bb_getMyIP());

	if(m_status == CStatus::INGAME)
	{
		data.add("server_name", m_server.s);
		data.add("server_ip", m_ip.s);
		data.add("server_port", m_port);
	}
	
	CCurl* request = new CCurl(gameVar.db_accountServer, data.get());
	request->start();
	m_requests.push_back(request);

	if (console) console->add(CString("\x8> Sending Status Update: %s", getText().s));
}


void CStatus::processQueue()
{
	if(m_requests.size() > 0)
	{
		// Iterate through requests
		int i;
		for (i=0;i<(int)m_requests.size();++i)	
		{
			if (m_requests[i])
			{
				if(!m_requests[i]->isRunning())
				{
					delete m_requests[i];
					m_requests.erase(m_requests.begin() + i);
					--i;
				}
			}
		}
	/*	for(std::vector<CCurl*>::iterator i = m_requests.begin(); i != m_requests.end(); ++i)
		{
			if(!((*i)->isRunning()))
			{
				delete (*i);
				i = m_requests.erase(i);
			}
		}*/
	}
}