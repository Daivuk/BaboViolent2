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


#include "Server.h"
#include "ReportGen.h"
#include "Scene.h"
#include <sstream>
#define TIXML_USE_STL
#include "tinyxml.h"

extern Scene * scene;

std::string ReportGen::genReport()
{
	TiXmlDocument doc;
	
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement* root = new TiXmlElement("bv2gamereport");
	doc.LinkEndChild(root);

	//game info
	genGameInfo(root);

	std::string filename = gameVar.sv_matchcode.s;
	if(filename.empty())
		filename = "report";

	doc.SaveFile("main/"+filename+".xml");

	TiXmlPrinter printer;
	printer.SetIndent("\t");

	doc.Accept(&printer);
	return printer.CStr();
}

/*TiXmlElement* ReportGen::genServerVersion()
{
	std::stringstream ss;
	ss << (int)GAME_VERSION_SV/10000 << ".";
	ss << (int)(GAME_VERSION_SV%1000)/100 << ".";
	ss << (int)GAME_VERSION_SV%100;

	TiXmlElement* element = new TiXmlElement("version");
	element->LinkEndChild(new TiXmlText(ss.str().c_str()));
	return element;
}
*/

TiXmlElement* ReportGen::genServerInfo()
{
	TiXmlElement* element = new TiXmlElement("servername");
	element->LinkEndChild(new TiXmlText(gameVar.sv_gameName.s));
	return element;
}

void ReportGen::genGameInfo(TiXmlElement* element)
{
	TiXmlElement* game = new TiXmlElement("game");

	//store game info
	genInfo(game);

	//store teams
	genTeams(game);

	//store results
	genPlayers(game);

	element->LinkEndChild(game);
}

void ReportGen::genInfo(TiXmlElement* element)
{
	//store game info
	TiXmlElement* gameInfo = new TiXmlElement("gameinfo");

		gameInfo->LinkEndChild(genServerInfo());

		//store game type
		TiXmlElement* gameMode = new TiXmlElement("gamemode");
		TiXmlText* type = 0;
		if (scene->server->game->gameType == GAME_TYPE_DM)
			type = new TiXmlText("DM");
		else if (scene->server->game->gameType == GAME_TYPE_TDM)
			type = new TiXmlText("TDM");
		else if (scene->server->game->gameType == GAME_TYPE_CTF)
			type = new TiXmlText("CTF");
		else if (scene->server->game->gameType == GAME_TYPE_SND)
			type = new TiXmlText("SND");
		if (type != 0)
			gameMode->LinkEndChild(type);
		gameInfo->LinkEndChild(gameMode);

		//store map
		TiXmlElement* map = new TiXmlElement("map");
		TiXmlText* mapName = new TiXmlText(scene->server->game->map->mapName.s);
		map->LinkEndChild(mapName);
		gameInfo->LinkEndChild(map);

		std::stringstream ss;
		//store match code
		ss << gameVar.sv_matchcode.s;
		TiXmlElement* matchCode = new TiXmlElement("matchcode");
		TiXmlText* mcode = new TiXmlText(ss.str().c_str());
		matchCode->LinkEndChild(mcode);
		gameInfo->LinkEndChild(matchCode);

		//store match mode
		ss.str(std::string());
		ss << gameVar.sv_matchmode;
		TiXmlElement* matchMode = new TiXmlElement("matchmode");
		TiXmlText* mmode = new TiXmlText(ss.str().c_str());
		matchMode->LinkEndChild(mmode);
		gameInfo->LinkEndChild(matchMode);

	element->LinkEndChild(gameInfo);
}

void ReportGen::genTeams(TiXmlElement* element)
{
	int gt = scene->server->game->gameType;
	if (gt == GAME_TYPE_CTF || gt == GAME_TYPE_TDM)
	{
		TiXmlElement* teams = new TiXmlElement("teams");

		TiXmlElement* team, * teamID, * score;
		TiXmlText* teamIDTxt, * teamScoreTxt;
		std::stringstream ss;

		//red team
		team = new TiXmlElement("team");

		ss.str(std::string());
		ss << PLAYER_TEAM_RED;
		teamIDTxt = new TiXmlText(ss.str().c_str());
		teamID = new TiXmlElement("teamid");
		teamID->LinkEndChild(teamIDTxt);

		ss.str(std::string());
		ss << scene->server->game->redScore;
		teamScoreTxt = new TiXmlText(ss.str().c_str());
		score = new TiXmlElement("score");
		score->LinkEndChild(teamScoreTxt);

		team->LinkEndChild(teamID);
		team->LinkEndChild(score);

		teams->LinkEndChild(team);

		//blue team
		team = new TiXmlElement("team");

		ss.str(std::string());
		ss << PLAYER_TEAM_BLUE;
		teamIDTxt = new TiXmlText(ss.str().c_str());
		teamID = new TiXmlElement("teamid");
		teamID->LinkEndChild(teamIDTxt);

		ss.str(std::string());
		ss << scene->server->game->blueScore;
		teamScoreTxt = new TiXmlText(ss.str().c_str());
		score = new TiXmlElement("score");
		score->LinkEndChild(teamScoreTxt);

		team->LinkEndChild(teamID);
		team->LinkEndChild(score);

		teams->LinkEndChild(team);

		element->LinkEndChild(teams);
	}
}

void ReportGen::genPlayers(TiXmlElement* element)
{
	TiXmlElement* players = new TiXmlElement("players");
	Player* p = 0;

	/*for (int i = 0; i < MAX_PLAYER; i++)
	{
		p = scene->server->game->players[i];
		if (p == 0 || p->timePlayedCurGame < 0.001f ||
			(p->teamID != PLAYER_TEAM_BLUE && p->teamID != PLAYER_TEAM_RED))
			continue;
		PlayerStats* ps = new PlayerStats(p);
		genPlayer(players, ps);
		delete ps;
	}*/

	Server::StatsCache cache = scene->server->getCachedStats();
	Server::StatsCache::iterator it = cache.begin();
	for (; it != cache.end(); it++)
	{
		genPlayer(players, it->second);
	}

	element->LinkEndChild(players);
}

void ReportGen::genPlayer(TiXmlElement* parent, PlayerStats* stats)
{
	std::stringstream ss;
	TiXmlText* value;
	TiXmlElement* player = new TiXmlElement("player");

	ss << stats->userID;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* playerID = new TiXmlElement("playerid");
	playerID->LinkEndChild(value);

	TiXmlElement* name = new TiXmlElement("name");
	value = new TiXmlText(stats->name.s);
	value->SetCDATA(true);
	name->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->teamID;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* team = new TiXmlElement("teamid");
	team->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->timePlayedCurGame;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* time = new TiXmlElement("time");
	time->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->kills;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* kills = new TiXmlElement("kills");
	kills->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->deaths;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* deaths = new TiXmlElement("deaths");
	deaths->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->dmg;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* dmg = new TiXmlElement("damage");
	dmg->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->score;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* caps = new TiXmlElement("caps");
	caps->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->flagAttempts;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* attempts = new TiXmlElement("attempts");
	attempts->LinkEndChild(value);

	ss.str(std::string());
	ss << stats->returns;
	value = new TiXmlText(ss.str().c_str());
	TiXmlElement* returns = new TiXmlElement("returns");
	returns->LinkEndChild(value);

	player->LinkEndChild(playerID);
	player->LinkEndChild(name);
	player->LinkEndChild(team);
	player->LinkEndChild(time);
	player->LinkEndChild(kills);
	player->LinkEndChild(deaths);
	player->LinkEndChild(dmg);
	player->LinkEndChild(caps);
	player->LinkEndChild(attempts);
	player->LinkEndChild(returns);

	parent->LinkEndChild(player);
}

/*void ReportGen::genGameResultsCTF(TiXmlElement* results)
{
}

void ReportGen::genGameResultsDM(TiXmlElement* results)
{
}

void ReportGen::genGameResultsTDM(TiXmlElement* results)
{
}

void ReportGen::genGameResultsSND(TiXmlElement* results)
{
	results->LinkEndChild(new TiXmlText("Not yet supported :("));
}*/

