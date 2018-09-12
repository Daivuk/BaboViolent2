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


#if defined(_PRO_)
#include "CAStar.h"
#include <math.h>



//
//--- Find the path.
//    Here we avoid recusivity for speed.
//
CPathNode *CAStar::FindPath(CPathNode* in_start, CPathNode* in_end)
{	
	if (!in_start || !in_end) return 0;
	in_start->parentNode = 0;
	in_end->parentNode = 0;
	in_start->fScore = 0;
	in_start->gScore = 0;
	in_start->hScore = 0;
	if (in_start == in_end) return 0;

	//--- If the start node is impassable, the result is nothing.
	if (in_start->m_value == 0)
	{
		in_start->nextPath = 0;
		return 0;
	}

	//--- Loop until we have next node to check.
	unsigned int i, nbNeighbor;
	CPathNode* current = in_start;
	current->gScore = 0;
	CPathNode* sorted = 0;
	CPathNode* n;
	CPathNode* p;
	CPathNode* bestSoFar = in_start;
	m_frameID += 2; // Increment frameID to initiate all the nodePath
	float d1, d2;

	//--- Compute the initiate score
	current->gScore = 0;
	d1 = fabsf(in_end->m_centerX - current->m_centerX);
	d2 = fabsf(in_end->m_centerY - current->m_centerY);
	if (d1 > d2) current->hScore = d1 - d2 + d2 * 1.414214f;
	else if (d2 > d1) current->hScore = d2 - d1 + d1 * 1.414214f;
	else current->hScore = d1 * 1.414214f;
	current->fScore = current->hScore + current->gScore;

	while (current)
	{
		//--- Invalidate that node for further search
		current->m_frameID = m_frameID;

		//--- Check if we don't have a more optimized parent
		nbNeighbor = (unsigned int)current->m_neighbors.size();
		if (current->parentNode)
		{
			for (i=0;i<nbNeighbor;++i)
			{
				n = current->m_neighbors[i];
				if (n->m_frameID >= m_frameID && current->parentNode != n)
				{
					if (n->gScore + current->m_neighborsHeuristic[i] < current->gScore)
					{
						current->parentNode = n;
						current->gScore = n->gScore + current->m_neighborsHeuristic[i];
						current->fScore = current->gScore + current->hScore;
					}
				}
			}
		}

		//--- Update our best so far
		if (current->hScore < bestSoFar->hScore) bestSoFar = current;

		//--- We found it?
		if (current == in_end) return in_end; // !!!! yea!

		//--- Register his neighbors to the sorted queue list
		for (i=0;i<nbNeighbor;++i)
		{
			n = current->m_neighbors[i];
			if (n->m_frameID < m_frameID)
			{
				//--- Compute G,H and F
				//	  H = direct distance to arrival
				//	  G = walked distance from start
				//	  F = G + H, total score
				n->gScore = current->gScore + current->m_neighborsHeuristic[i];
				d1 = fabsf(in_end->m_centerX - n->m_centerX);
				d2 = fabsf(in_end->m_centerY - n->m_centerY);
				if (d1 > d2) n->hScore = d1 - d2 + d2 * 1.414214f;
				else if (d2 > d1) n->hScore = d2 - d1 + d1 * 1.414214f;
				else n->hScore = d1 * 1.414214f;
				n->fScore = n->hScore + n->gScore;
				n->parentNode = current;
				n->m_frameID = m_frameID;

				//--- Add to list here (The slow way first, just to make it work)
				for (p=sorted;p;p=p->sortedNext)
				{
					if (n->fScore <= p->fScore)
					{
						if (p->sortedPrevious) p->sortedPrevious->sortedNext = n;
						n->sortedPrevious = p->sortedPrevious;
						n->sortedNext = p;
						p->sortedPrevious = n;
						if (!n->sortedPrevious) sorted = n;
						break;
					}
					else if (!p->sortedNext)
					{
						p->sortedNext = n;
						n->sortedPrevious = p;
						n->sortedNext = 0;
						break;
					}
				}
				if (!p)
				{
					sorted = n;
					n->sortedNext = 0;
					n->sortedPrevious = 0;
				}
			}
		}

		//--- Remove now the first from the list, and use it to check the next one
		if (sorted)
		{
			current = sorted;
			sorted = sorted->sortedNext;
			if (sorted) sorted->sortedPrevious = 0;
		}
		else
		{
			//--- Finish, nothing found
			current = 0;
		}
	}

	return bestSoFar;
}
#endif
