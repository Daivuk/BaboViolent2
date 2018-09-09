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
#include "Console.h"



void Server::updateCTF(float delay)
{
	int i;

	if (game->map->flagState[0] == -2)
	{
		for (i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->teamID == PLAYER_TEAM_RED &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPodPos[0], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[0] = (char)i;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 0;
						flagState.newFlagState = game->map->flagState[0];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s took the blue flag ID:%i", game->players[i]->name.s, i));
						game->players[i]->flagAttempts++;
						break;
					}
				}
				if (game->players[i]->teamID == PLAYER_TEAM_BLUE &&
					game->players[i]->status == PLAYER_STATUS_ALIVE &&
					game->map->flagState[1] == (char)i)
				{
					float dis = distanceSquared(game->map->flagPodPos[0], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[1] = -2; // On a scoooréééé !!!!!!

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 1;
						flagState.newFlagState = game->map->flagState[1];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						CString message("\x03> \x01%s \x08scores for the Blue team! ID:%i", game->players[i]->name.s,i);
						//console->add(CString("\x3> Blue team scores!"));
						console->add(message);
						game->players[i]->score++;
						game->blueWin++;
						game->blueScore = game->blueWin;
						break;
					}
				}
			}
		}
	}
	else if (game->map->flagState[0] == -1)
	{
		for (i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->teamID == PLAYER_TEAM_RED &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPos[0], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .5f*.5f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[0] = (char)i;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 0;
						flagState.newFlagState = game->map->flagState[0];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s took the blue flag ID:%i", game->players[i]->name.s, i));
						game->players[i]->flagAttempts++;
						break;
					}
				}
				if (game->players[i]->teamID == PLAYER_TEAM_BLUE &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPos[0], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .5f*.5f)
					{
						// Ce joueur retourne le flag !
						game->map->flagState[0] = -2;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 0;
						flagState.newFlagState = game->map->flagState[0];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s returned the blue flag", game->players[i]->name.s));
						game->players[i]->returns++; // Il gagne deux points pour avoir sauvé le flag !
						//game->blueScore += 2;
						break;
					}
				}
			}
		}
	}


	if (game->map->flagState[1] == -2)
	{
		for (i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->teamID == PLAYER_TEAM_BLUE &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPodPos[1], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[1] = (char)i;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 1;
						flagState.newFlagState = game->map->flagState[1];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s took the red flag ID:%i", game->players[i]->name.s,i));
						game->players[i]->flagAttempts++;
						break;
					}
				}
				if (game->players[i]->teamID == PLAYER_TEAM_RED &&
					game->players[i]->status == PLAYER_STATUS_ALIVE &&
					game->map->flagState[0] == (char)i)
				{
					float dis = distanceSquared(game->map->flagPodPos[1], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[0] = -2; // On a scoooréééé !!!!!!

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 0;
						flagState.newFlagState = game->map->flagState[0];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						CString message("\x03> \x01%s \x08scores for the Red team! ID:%i", game->players[i]->name.s,i);
						//console->add(CString("\x3> Red team scores!"));
						console->add(message);
						game->players[i]->score++;
						game->redWin++;
						game->redScore = game->redWin;
						break;
					}
				}
			}
		}
	}
	else if (game->map->flagState[1] == -1)
	{
		for (i=0;i<MAX_PLAYER;++i)
		{
			if (game->players[i])
			{
				if (game->players[i]->teamID == PLAYER_TEAM_BLUE &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPos[1], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur pogne le flag !!!!!!
						game->map->flagState[1] = (char)i;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 1;
						flagState.newFlagState = game->map->flagState[1];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s took the red flag ID:%i", game->players[i]->name.s,i));
						game->players[i]->flagAttempts++;
						break;
					}
				}
				if (game->players[i]->teamID == PLAYER_TEAM_RED &&
					game->players[i]->status == PLAYER_STATUS_ALIVE)
				{
					float dis = distanceSquared(game->map->flagPos[1], CVector3f(
						game->players[i]->currentCF.position[0], game->players[i]->currentCF.position[1], 0));
					if (dis <= .25f*.25f)
					{
						// Ce joueur retourne le flag !
						game->map->flagState[1] = -2;

						// On le dis au autres
						net_svcl_change_flag_state flagState;
						flagState.flagID = 1;
						flagState.newFlagState = game->map->flagState[1];
						flagState.playerID = (char)i;
						bb_serverSend((char*)&flagState, sizeof(net_svcl_change_flag_state), NET_SVCL_CHANGE_FLAG_STATE, 0);

						// On fait emet un sont dépendament du team :P
						console->add(CString("\x3> %s returned the blue flag", game->players[i]->name.s));
						game->players[i]->returns++; // Il gagne deux points pour avoir sauvé le flag !
						//game->redScore += 2;
						break;
					}
				}
			}
		}
	}
}

