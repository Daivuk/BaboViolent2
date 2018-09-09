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

#ifndef CPATHNODE_H_INCLUDED
#define CPATHNODE_H_INCLUDED



#include <vector>



class CPathNode
{
public:
	/** Default Constructor */
	CPathNode();

	/** Copy constructor 
	* @param in_aStar An another CPathNode object.
	*/
	CPathNode(const CPathNode & in_pathNode);

	/** Copy operator
	* @param in_aStar An another CPathNode object.
	* @return A reference on the object
	*/
	CPathNode& operator=(const CPathNode & in_pathNode);

	/** Destructor */
	virtual ~CPathNode();

	/** To clean everything
	* @return Nothing.
	*/
	void Clean();

	/** To add a pathNode neighbor
	* @param in_neighbor The neighbor to pus in his neighbor's array.
	* @return Nothing.
	*/
	void AddNeighbor(CPathNode* in_neighbor);

private:
	/** To init the members. Used by contructors and Clean function.
	* @return Nothing.
	*/
	void InitMembers();

public:
	/** Next in the list */
	CPathNode *next;

	/** Previous in the list */
	CPathNode *previous;

	/** The next node in the path */
	CPathNode *nextPath;

	/** The node that is currently working it (his parent) */
	CPathNode *parentNode;

	/** The sorted list pointers */
	CPathNode *sortedNext;
	CPathNode *sortedPrevious;

	/** The current Heuristic of this node */
	float fScore;
	float gScore;
	float hScore;

	/** The array of neighbors */
	std::vector<CPathNode*> m_neighbors;

	/** The heuristic array for the neighbors */
	std::vector<float> m_neighborsHeuristic;

	/** His position on the map - top left */
	int m_x;
	int m_y;

	/** His size */
	int m_sizeX;
	int m_sizeY;

	/** His REAL center position, in float */
	float m_centerX;
	float m_centerY;

	/** His value */
	unsigned char m_value;

	/** His search frame ID */
	unsigned long m_frameID;
};


#endif
