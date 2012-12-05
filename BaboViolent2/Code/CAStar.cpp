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

#include "CAStar.h"
#include <memory.h>



#define ASTAR_DELETE_LIST(head, type) {type *toKill; for (type *p = head[0]; p; delete toKill){toKill = p;p = p->next;}head[0]=0;head[1]=0;}
#define ASTAR_PUSH_BACK(head, element) {if (!head[0]){head[0] = head[1] = element;}else{head[1]->next = element;element->previous = head[1];head[1] = element;}}
#define ASTAR_PUSH_FRONT(head, element) {if (!head[0]){head[0] = head[1] = element;}else{head[0]->previous = element;element->next = head[0];head[0] = element;}}
#define ASTAR_INSERT(head, element, frontOf){if (!frontOf){head[0] = element;head[1] = element;}else{if (frontOf->previous){frontOf->previous->next = element;element->previous = frontOf->previous;frontOf->previous = element;element->next = frontOf;}else{frontOf->previous = element;element->next = frontOf;head[0] = element;}}}



//
//-- To init the variables. Used by contructors and Clean.
//
void CAStar::InitMembers()
{
	m_mapArray = 0;
	m_sizeX = 0;
	m_sizeY = 0;
	m_lstNodes[0] = 0;
	m_lstNodes[1] = 0;
	m_mapNodesRef = 0;
	m_frameID = 1;
}



//
//--- Default Constructor
//
CAStar::CAStar()
{
	m_maxPathSize = 4;
	InitMembers();
}



//
//--- Copy constructor 
//
CAStar::CAStar(const CAStar & in_aStar)
{
	m_maxPathSize = 4;
	InitMembers();

	//--- Call the copy operator
	*this = in_aStar;
}



//
//--- Copy operator
//
CAStar& CAStar::operator=(const CAStar & in_aStar)
{
	//--- Be sure it's not the same object.
	if (this == &in_aStar) return *this;

	//--- Start by cleaning ourself.
	Clean();

	//--- Copy our map data
	if (m_mapArray)
	{
		m_sizeX = in_aStar.m_sizeX;
		m_sizeY = in_aStar.m_sizeY;
		m_maxPathSize = in_aStar.m_maxPathSize;
		m_mapArray = new unsigned char [m_sizeX * m_sizeY];
		memcpy(m_mapArray, in_aStar.m_mapArray, m_sizeX * m_sizeY);

		//--- Copy the m_mapNodesRef
		m_mapNodesRef = new CPathNode * [m_sizeX * m_sizeY];
		memcpy(m_mapNodesRef, in_aStar.m_mapNodesRef, m_sizeX * m_sizeY);

		//--- Copy the CPathNodes.
		for (CPathNode *p = in_aStar.m_lstNodes[0]; p; p = p->next)
		{
			CPathNode *newNode = new CPathNode(*p);
			ASTAR_PUSH_BACK(m_lstNodes, newNode);
		}
	}

	//--- return a reference on us
	return *this;
}



//
//--- Destructor
//
CAStar::~CAStar()
{
	Clean();
}



//
//--- To clean everything
//
void CAStar::Clean()
{
	if (m_mapArray) delete [] m_mapArray;
	if (m_mapNodesRef) delete [] m_mapNodesRef;

	//--- Delete the nodes
	ASTAR_DELETE_LIST(m_lstNodes, CPathNode);

	//--- We reinit the members now
	InitMembers();
}



//
//--- Check, if the tiles are placed diagonally, to see if they don't have
//
bool CAStar::CheckForDiagonalBlocker(const CPathNode * in_n1, const CPathNode * in_n2)
{
	//--- Top left
	if (in_n2->m_x + in_n2->m_sizeX == in_n1->m_x &&
		in_n2->m_y - in_n2->m_sizeY == in_n1->m_y)
	{
		if (GetNodeAt(in_n1->m_x - 1, in_n2->m_y - 1)->m_value == 0 &&
			GetNodeAt(in_n1->m_x, in_n2->m_y)->m_value == 0) return true;
	}

	//--- Bottom left
	if (in_n2->m_x + in_n2->m_sizeX == in_n1->m_x &&
		in_n2->m_y + in_n2->m_sizeY == in_n1->m_y)
	{
		if (GetNodeAt(in_n1->m_x - 1, in_n1->m_y)->m_value == 0 &&
			GetNodeAt(in_n1->m_x, in_n1->m_y - 1)->m_value == 0) return true;
	}

	//--- Top right
	if (in_n2->m_x - in_n2->m_sizeX == in_n1->m_x &&
		in_n2->m_y - in_n2->m_sizeY == in_n1->m_y)
	{
		if (GetNodeAt(in_n2->m_x - 1, in_n2->m_y)->m_value == 0 &&
			GetNodeAt(in_n2->m_x, in_n2->m_y - 1)->m_value == 0) return true;
	}

	//--- Bottom right
	if (in_n2->m_x - in_n2->m_sizeX == in_n1->m_x &&
		in_n2->m_y + in_n2->m_sizeY == in_n1->m_y)
	{
		if (GetNodeAt(in_n2->m_x, in_n1->m_y)->m_value == 0 &&
			GetNodeAt(in_n2->m_x - 1, in_n1->m_y - 1)->m_value == 0) return true;
	}

	return false;
}



//
//--- To build it.
//
int CAStar::Build(const unsigned char * in_mapArray, 
				  const int in_sizeX, 
				  const int in_sizeY)
{
	//--- We clean to be sure
	Clean();

	//--- Copy the array
	if (!in_mapArray) return 0;
	m_sizeX = in_sizeX;
	m_sizeY = in_sizeY;
	m_mapArray = new unsigned char [m_sizeX * m_sizeY];
	memcpy(m_mapArray, in_mapArray, m_sizeX * m_sizeY);
	m_mapNodesRef = new CPathNode *[m_sizeX * m_sizeY];

	//--- Alright, create the nodes recursively
	int x,y;
	for (y=0;y<m_sizeY;y+=m_maxPathSize)
	{
		for (x=0;x<m_sizeX;x+=m_maxPathSize)
		{
			CreateNodes(x, y, m_maxPathSize);
		}
	}

	//--- Ok, now create the neighborhood list.
	CPathNode* p;
	CPathNode* n;
	int lastSize;
	for (p = m_lstNodes[0]; p; p = p->next)
	{
		//--- Top
		y = p->m_y - 1;
		lastSize = 1;
		for (x = p->m_x - 1; x <= p->m_x + p->m_sizeX; x += 1)
		{
			n = GetNodeAt(x, y);
			if (n)
			{
				lastSize = n->m_sizeX;
				if (p->m_value == n->m_value && !CheckForDiagonalBlocker(p, n)) p->AddNeighbor(n);
			}
			else
			{
				lastSize = 1; // useless
			}
		}

		//--- Down
		y = p->m_y + p->m_sizeY;
		lastSize = 1;
		for (x = p->m_x - 1; x <= p->m_x + p->m_sizeX; x += 1)
		{
			n = GetNodeAt(x, y);
			if (n)
			{
				lastSize = n->m_sizeX;
				if (p->m_value == n->m_value && !CheckForDiagonalBlocker(p, n)) p->AddNeighbor(n);
			}
			else
			{
				lastSize = 1; // useless
			}
		}

		//--- Left
		x = p->m_x - 1;
		lastSize = 1;
		for (y = p->m_y - 1; y <= p->m_y + p->m_sizeY; y += 1)
		{
			n = GetNodeAt(x, y);
			if (n)
			{
				lastSize = n->m_sizeY;
				if (p->m_value == n->m_value && !CheckForDiagonalBlocker(p, n)) p->AddNeighbor(n);
			}
			else
			{
				lastSize = 1; // useless
			}
		}

		//--- Right
		x = p->m_x + p->m_sizeX;
		lastSize = 1;
		for (y = p->m_y - 1; y <= p->m_y + p->m_sizeY; y += 1)
		{
			n = GetNodeAt(x, y);
			if (n)
			{
				lastSize = n->m_sizeY;
				if (p->m_value == n->m_value && !CheckForDiagonalBlocker(p, n)) p->AddNeighbor(n);
			}
			else
			{
				lastSize = 1; // useless
			}
		}
	}
	
	//--- Successful
	return 1;
}



//
//--- Create the nodes.
//
void CAStar::CreateNodes(int in_x, int in_y, int in_size)
{
	//--- Check is there is something in the way
	int x,y;
	unsigned char value;
	for (y=in_y;y<in_y+in_size;++y)
	{
		for (x=in_x;x<in_x+in_size;++x)
		{
			//--- We have to be sure it's all the same value
			if (y == in_y && x == in_x) 
			{
				value = m_mapArray[(y * m_sizeX) + x];
			}
			else if (m_mapArray[(y * m_sizeX) + x] != value)
			{
				//--- Split it
				CreateNodes(in_x, in_y, in_size / 2);
				CreateNodes(in_x, in_y + in_size / 2, in_size / 2);
				CreateNodes(in_x + in_size / 2, in_y + in_size / 2, in_size / 2);
				CreateNodes(in_x + in_size / 2, in_y, in_size / 2);
				return;
			}
		}
	}

	//--- If we didn't stop, we have our node!
	CPathNode *newNode = new CPathNode();
	newNode->m_x = in_x;
	newNode->m_y = in_y;
	newNode->m_sizeX = in_size;
	newNode->m_sizeY = in_size;
	newNode->m_value = value;
	newNode->m_centerX = (float)in_x + (float)in_size * .5f;
	newNode->m_centerY = (float)in_y + (float)in_size * .5f;
	ASTAR_PUSH_BACK(m_lstNodes, newNode);

	//--- Fill the m_mapNodesRef
	for (y=in_y;y<in_y+in_size;++y)
	{
		for (x=in_x;x<in_x+in_size;++x)
		{
			m_mapNodesRef[(y * m_sizeX) + x] = newNode;
		}
	}
}
#endif
