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

#include "CPing.h"
#include "Console.h"



//
//--- Constructor
//
CPing::CPing()
{
	uniquePeerID = -1;
	m_ping = 0;
}



//
//--- Destructor
//
CPing::~CPing()
{
	if (uniquePeerID != -1)
	{
		//--- On delete ça instant
		bb_peerDelete(uniquePeerID, true);
	}
}



//
//--- Send the ping
//
int CPing::ping(const CString & in_ip, const unsigned short & in_port)
{
	m_ping = 0;
	uniquePeerID = bb_peerSend(in_ip.s, in_port, 0, 0, 0, true);
	if (uniquePeerID != -1)
	{
		bb_peerDelete(uniquePeerID);
	}
	return uniquePeerID;
}



//
//--- Result from a ping request
//
int CPing::pingResult(int in_id)
{
	if (in_id == uniquePeerID)
	{
		return (int)(m_ping * 1000);
	}

	return -1;
}



//
//--- Update (return the ping result)
//
int CPing::update(float delay)
{
	if (uniquePeerID != -1)
	{
		m_ping += delay;
		if (m_ping >= 1)
		{
			bb_peerDelete(uniquePeerID, true);
			m_ping = 1;
			return (int)(m_ping * 1000);
		}
	}

	return -1;
}

