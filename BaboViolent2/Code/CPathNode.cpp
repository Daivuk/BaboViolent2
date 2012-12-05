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
#include "CPathNode.h"
#include <math.h>



//
//--- Default Constructor
//
CPathNode::CPathNode()
{
	next = 0;
	previous = 0;
	InitMembers();
}



//
//--- Copy constructor 
//
CPathNode::CPathNode(const CPathNode & in_pathNode)
{
	next = 0;
	previous = 0;
	InitMembers();
	*this = in_pathNode;
}



//
//--- Copy operator
//
CPathNode& CPathNode::operator=(const CPathNode & in_pathNode)
{
	if (this == &in_pathNode) return *this;

	//--- Clean it first
	Clean();

	//--- Copy everything from the other
	m_x = in_pathNode.m_x;
	m_y = in_pathNode.m_y;
	m_sizeX = in_pathNode.m_sizeX;
	m_sizeY = in_pathNode.m_sizeY;
	m_centerX = in_pathNode.m_centerX;
	m_centerY = in_pathNode.m_centerX;

	unsigned int i;
	for (i=0;i<in_pathNode.m_neighbors.size();++i)
	{
		m_neighbors.push_back(in_pathNode.m_neighbors[i]);
		m_neighborsHeuristic.push_back(in_pathNode.m_neighborsHeuristic[i]);
	}

	//--- Return a reference on us.
	return *this;
}



//
//--- Destructor
//
CPathNode::~CPathNode()
{
	Clean();
}



//
//--- To clean everything
//
void CPathNode::Clean()
{
	m_neighbors.clear();
	m_neighborsHeuristic.clear();
	InitMembers();
}



//
//--- To add a pathNode neighbor
//
void CPathNode::AddNeighbor(CPathNode* in_neighbor)
{
	//--- Check if not already registered
	unsigned int i;
	for (i=0;i<m_neighbors.size();++i)
	{
		if (m_neighbors[i] == in_neighbor) return; //--- Ok good
	}

	//--- Register it and compute his heuristic from center to center
	m_neighbors.push_back(in_neighbor);
	m_neighborsHeuristic.push_back(sqrtf(
		(in_neighbor->m_centerX - m_centerX) * (in_neighbor->m_centerX - m_centerX) +
		(in_neighbor->m_centerY - m_centerY) * (in_neighbor->m_centerY - m_centerY)));
}



//
//--- To init the members. Used by contructors and Clean function.
//
void CPathNode::InitMembers()
{
	m_frameID = 0;
	m_x = 0;
	m_y = 0;
	m_sizeX = 1;
	m_sizeY = 1;
	m_centerX = .5f;
	m_centerY = .5f;
	m_value = 0;
	nextPath = 0;
	parentNode = 0;
	fScore = 0;
	gScore = 0;
	hScore = 0;
	sortedNext = 0;
	sortedPrevious = 0;
}
#endif
