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

#ifndef CSTATUS_H_INCLUDED
#define CSTATUS_H_INCLUDED

#include "CString.h"


class CCurl;


class CStatus
{
public:

	// States
	enum {
		OFFLINE = 0,
		WEBSITE = 1,
		ONLINE = 2,
		INGAME = 3
	};

	CStatus();
	virtual ~CStatus();

	void	set(int status, CString serverName = "", CString serverIP = "", int port = 0);
	int		get();
	CString	getText(int in_status = -1);
	void	update();
	void	processQueue();


private:
	std::vector<CCurl*>	m_requests;
	int		m_status;

	CString	m_server;
	CString m_ip;
	int		m_port;
};


extern CStatus* status;

#endif

