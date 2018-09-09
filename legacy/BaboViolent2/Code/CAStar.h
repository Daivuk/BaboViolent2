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

/*
 	Author: David St-Louis (Alias Daivuk)
	email: duktroa@hotmail.com
	Date: 27/01/2007
	Description: The goal of this class is to have a fast generic 2D A* 
				 quadtree pathfinding. Maps don't need to be power of two.


    -----------------------------------------------------
	Revision 1.0
	Initial file - Daivuk, 27/01/2007
*/


#ifndef CASTAR_H_INCLUDED
#define CASTAR_H_INCLUDED


#include "CPathNode.h"
#include <list>


class CAStar
{
public:
	/** Default Constructor */
	CAStar();

	/** Copy constructor 
	* @param in_aStar An another CAStar object.
	*/
	CAStar(const CAStar & in_aStar);

	/** Copy operator
	* @param in_aStar An another CAStar object.
	* @return A reference on the object
	*/
	CAStar& operator=(const CAStar & in_aStar);

	/** Destructor */
	virtual ~CAStar();

	/** To clean everything
	* @return Nothing.
	*/
	void Clean();

	/** Check, if the tiles are placed diagonally, to see if they don't have
	*	2 diagonal walls blocking them.
	*	@param in_n1 First node
	*	@param in_n2 Second node
	*	@return True if it is blocked
	*/
	bool CheckForDiagonalBlocker(const CPathNode * in_n1, const CPathNode * in_n2);

	/** To build it.
	* @param in_mapArray The one dimensionnal array containing all the cells of
						 the map. A cell should contain 0 for impassable, 255 
						 for fully passable. Values between will be heuristic.
	* @param in_sizeX Size of the map on X axis. (Number of tiles)
	* @param in_sizeY Size of the map on Y axis. (Number of tiles)
	* @return 1 if sucessful, 0 if failed.
	*/
	int Build(const unsigned char * in_mapArray, 
			  const int in_sizeX, 
			  const int in_sizeY);

	/** get/set the max path size */
	int GetMaxPathSize() const {return m_maxPathSize;}

	/** Must be power of 2 and at max 64 */
	void SetMaxPathSize(int in_maxPathSize) 
	{
		m_maxPathSize = in_maxPathSize;
		if (m_maxPathSize <= 1)
		{
			m_maxPathSize = 1;
		}
		else if (m_maxPathSize <= 2)
		{
			m_maxPathSize = 2;
		}
		else if (m_maxPathSize <= 4)
		{
			m_maxPathSize = 4;
		}
		else if (m_maxPathSize <= 8)
		{
			m_maxPathSize = 8;
		}
		else if (m_maxPathSize <= 16)
		{
			m_maxPathSize = 16;
		}
		else if (m_maxPathSize <= 32)
		{
			m_maxPathSize = 32;
		}
		else
		{
			m_maxPathSize = 64;
		}
	}

	/** Get the head list of the nodes */
	CPathNode *GetNodes() const {return m_lstNodes[0];}

	/** Get a node at a certain position on the map */
	CPathNode *GetNodeAt(int in_x, int in_y) const
	{
		if (in_x < 0) return 0;
		if (in_y < 0) return 0;
		if (in_x >= m_sizeX) return 0;
		if (in_y >= m_sizeY) return 0;
		return m_mapNodesRef[in_y * m_sizeX + in_x];
	}

	/** Find the path !!! finally */
	CPathNode *FindPath(CPathNode* in_start, CPathNode* in_end);
	

private:
	/** To init the members. Used by contructors and Clean function.
	* @return Nothing.
	*/
	void InitMembers();

	/** Create the nodes.
	* @param in_x Top left corner of the node
	* @param in_y "
	* @param in_size The square size of the node
	* @return Nothing.
	*/
	void CreateNodes(int in_x, int in_y, int in_size);

private:
	/** A copy of the map array */
	unsigned char* m_mapArray;

	/** This is the array containing all the node */
	CPathNode **m_mapNodesRef;

	/** The map dimensions */
	int m_sizeX;
	int m_sizeY;

	/** Pathnode list */ // lol voir que jfais une liste à la old school
	CPathNode * m_lstNodes[2];

	/** The maximum path size.
	* Note that the minimum is always 1.
	*/
	int m_maxPathSize;

public:
	/** The current search frame ID */
	unsigned long m_frameID;
};


#endif
