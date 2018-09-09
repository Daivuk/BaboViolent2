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
#include "CLobby.h"
#include "Zeven.h"


CLobby* lobby = 0;



//
//--- Constructor
//
CLobby::CLobby()
{
	lastSent = 0;
}



//
//--- Destructor
//
CLobby::~CLobby()
{
	clearLobby();
	ZEVEN_SAFE_DELETE(lastSent);
}



//
//--- Push a game in
//
void CLobby::pushGame(void* in_game)
{
	m_games.push_back(in_game);
}



//
//--- Retrieve a game in front
//    If there is no game, this function return null.
//
void* CLobby::getNext()
{
	ZEVEN_SAFE_DELETE(lastSent);

	if (m_games.size() > 0)
	{
		lastSent = m_games[0];
		m_games.erase(m_games.begin());
		return lastSent;
	}
	else
	{
		return 0;
	}
}



//
//--- Clear all games
//
void CLobby::clearLobby()
{
	int i;
	ZEVEN_DELETE_VECTOR(m_games, i);
	ZEVEN_SAFE_DELETE(lastSent);
}

#endif

