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

#ifndef REPORTGEN_H
#define REPORTGEN_H

#include <string>

class TiXmlElement;

class ReportGen
{
public:
	std::string genReport();

private:
	//TiXmlElement* genServerVersion();

	TiXmlElement* genServerInfo();

	void genGameInfo(TiXmlElement* element);

	void genInfo(TiXmlElement* element);

	void genTeams(TiXmlElement* element);

	void genPlayers(TiXmlElement* element);

	void genPlayer(TiXmlElement* parent, PlayerStats* stats);

/*	void genGameResultsCTF(TiXmlElement* results);
	void genGameResultsDM(TiXmlElement* results);
	void genGameResultsTDM(TiXmlElement* results);
	void genGameResultsSND(TiXmlElement* results);*/
};


#endif

