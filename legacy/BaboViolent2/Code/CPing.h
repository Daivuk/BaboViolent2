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

#ifndef CPING_H_INCLUDED
#define CPING_H_INCLUDED


#include "Zeven.h"


class CPing
{
private:
	//--- Unique peer ID
	int uniquePeerID;

	//--- Current time elapsed
	float m_ping;

public:
	//--- Constructor
	CPing();

	//--- Destructor
	virtual ~CPing();

	//--- Send the ping
	int ping(const CString & in_ip, const unsigned short & in_port);

	//--- Update (return the ping result)
	int update(float delay);

	//--- Result from a ping request
	int pingResult(int in_id);

	int GetPeerId() const { return uniquePeerID; }
};


#endif

