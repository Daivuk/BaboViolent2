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


#ifdef _PRO_

#include "Player.h"
#include "Game.h"



//
//--- Constructor
//
CMiniBot::CMiniBot()
{
	nukeBot = false;
	cFProgression = 0;
	m_seekingTime = 5;
	m_fireRate = 0;
	m_checkAStarTimer = 1;
	m_state = MINIBOT_STATE_SEEKING;
}



//
//--- Destructor
//
CMiniBot::~CMiniBot()
{
	path.clear();
}



//
//--- Control it
//
void CMiniBot::MakeNukeBot()
{
	nukeBot = true;
}
void CMiniBot::Think(float delay)
{
	int i;

	//--- Distance from him to us, always keep that
	float disSqr = distanceSquared(owner->currentCF.position, currentCF.position);

	m_fireRate -= delay;

	//--- At all time, get the closest enemy player and shoot him!
	Player * closest = 0;
	float closestDis = 10000;
	CVector3f p1;
	CVector3f p2;
	CVector3f normal;
	for (i=0;i<MAX_PLAYER;++i)
	{
		if (game->players[i])
		{
			if (game->players[i]->status == PLAYER_STATUS_ALIVE)
			{
				//--- Ennemy team?
				if (game->players[i] != owner && (
					game->players[i]->teamID != owner->teamID || game->gameType == GAME_TYPE_DM || game->gameType == GAME_TYPE_SND)
					)
				{
					//--- Close enough?
					float disWithEnnemySqr = 
						distanceSquared(currentCF.position, game->players[i]->currentCF.position);
					if (disWithEnnemySqr < 6 * 6)
					{
						if (disWithEnnemySqr < closestDis)
						{
							p1 = currentCF.position;
							p2 = game->players[i]->currentCF.position;
							CVector3f shootDir = p2 - p1;
							normalize(shootDir);
							CVector3f z = CVector3f(0,0,1);
							CVector3f mountOffset = rotateAboutAxis(shootDir,-90,z);
							normalize(mountOffset);
							mountOffset *= 0.1f;
							CVector3f origin = p1 + mountOffset;
							if (!game->map->rayTest(origin, p2, normal))
							{
								closestDis = disWithEnnemySqr;
								closest = game->players[i];
							}
						}
					}
				}
			}
		}
	}

	if (m_fireRate <= 0 && closest)
	{
		m_fireRate = .25f;
		currentCF.mousePosOnMap = closest->currentCF.position;

		//--- Shoot him!!!
		CVector3f shootDir = closest->currentCF.position - currentCF.position;
		normalize(shootDir);
		CVector3f z = CVector3f(0,0,1);
		CVector3f mountOffset = rotateAboutAxis(shootDir,-90,z);
		normalize(mountOffset);
		mountOffset *= 0.1f;
		CVector3f origin = p1 + mountOffset;
		if (!game->map->rayTest(origin, p2, normal) && !nukeBot)
			game->shootMinibotSV(this, 10, currentCF.position + mountOffset, currentCF.position + shootDir * 10.0f);
	}

	//--- States switch
	/*if (m_state == MINIBOT_STATE_SEEKING)
	{
		if (disSqr > 5 * 5)
		{
			destination = owner->currentCF.position;
		}
		else
		{
			m_seekingTime -= delay;
			if (m_seekingTime <= 0)
			{
				m_seekingTime = 1;

				//--- Find a new position around us, one of the eight possible spots
				CVector3f newPos[8];
				newPos[0] = CVector3f(
					owner->currentCF.position[0], owner->currentCF.position[1] + 1.0f, .15f);
				newPos[1] = CVector3f(
					owner->currentCF.position[0] + .8f, owner->currentCF.position[1] + .8f, .15f);
				newPos[2] = CVector3f(
					owner->currentCF.position[0] + 1.0f, owner->currentCF.position[1], .15f);
				newPos[3] = CVector3f(
					owner->currentCF.position[0] + .8f, owner->currentCF.position[1] - .8f, .15f);
				newPos[4] = CVector3f(
					owner->currentCF.position[0], owner->currentCF.position[1] - 1.0f, .15f);
				newPos[5] = CVector3f(
					owner->currentCF.position[0] - .8f, owner->currentCF.position[1] - .8f, .15f);
				newPos[6] = CVector3f(
					owner->currentCF.position[0] - 1.0f, owner->currentCF.position[1], .15f);
				newPos[7] = CVector3f(
					owner->currentCF.position[0] - .8f, owner->currentCF.position[1] + .8f, .15f);
				CVector3f normal;
				std::vector<int> possibles;
				for (i=0;i<8;++i)
				{
					if (!game->map->rayTest(owner->currentCF.position, newPos[i], normal))
					{
						possibles.push_back(i);
					}
				}

				if (possibles.size() > 0)
				{
					destination = newPos[possibles[rand()%(int)possibles.size()]];
					possibles.clear();
				}
			}
		}

		m_checkAStarTimer -= delay;

		//--- Apply the vel to the destination
		float disToDest;
		if (path.size() != 0)
		{
			disToDest = distanceSquared(path.front(), currentCF.position);
			if (disToDest <= .25f)
			{
				path.pop_front();
			}
		}
		disToDest = distanceSquared(destination, currentCF.position);
		if (disToDest > .25f)
		{
			if (m_checkAStarTimer <= 0)
			{
				m_checkAStarTimer = 1;

				//--- If the bot can't see the destination, let's do a AStar search
				p1 = currentCF.position;
				p2 = destination;
				if (game->map->rayTest(p1, p2, normal) && game->map->aStar)
				{
					path.clear();
					CPathNode * pathList = game->map->aStar->FindPath(
						game->map->aStar->GetNodeAt((int)currentCF.position[0], (int)currentCF.position[1]),
						game->map->aStar->GetNodeAt((int)destination[0], (int)destination[1]));
					for (CPathNode * p = pathList;p;p = p->parentNode)
					{
						path.push_front(CVector3f(p->m_centerX, p->m_centerY, .15f));
					}
					//--- ignore the first node
					path.pop_front();
				}
				else
				{
					//--- Erase steps path
					path.clear();
				}
			}

			CVector3f dir;
			if (path.size() == 0)
				dir = destination - currentCF.position;
			else
				dir = path.front() - currentCF.position;
			normalize(dir);
			currentCF.vel += dir * delay * 12.0f;
		}
		else
		{
			path.clear();
		}*/

		// On clamp sa vel /* Upgrade, faster ! haha */
		/*float size = currentCF.vel.length();
		if (size > 7.0f)
		{
			currentCF.vel /= size;
			currentCF.vel = currentCF.vel * 7.0f;
		}
	}*/
}

#endif
