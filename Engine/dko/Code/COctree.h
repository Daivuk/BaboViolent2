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

#ifndef COCTREE_H
#define COCTREE_H


#include "CVector.h"
#include "CFace.h"


const int MAX_PER_NODE = 25;
const int MAX_RECURS = 5;



class COctreeNode
{
public:
	// Le centre du cube
	CVector center;

	// Sa grosseur
	float size;

	// La liste des faces quils poc�e
	int nbFace;
	CFace **faceArray;

	// Ses 8 child
	COctreeNode *child[8];

	// Si il a des enfants
	bool haveChild;

	// sa recursive
	int recurs;

	// Sa couleur
//	float color;

public:
	// Constructeur / Destructeur
	COctreeNode(CFace **mFaceArray, int mNbFace, CVector mCenter, float mSize, int mRecurs);
	virtual ~COctreeNode();

	// Pour l'intersection ak une face
	bool faceIntersection(CFace * face);

	// Pour dessiner le octree (sera jamais utilis�sauf pour le premier test)
	void render();

	// Pour trouver un point d'intersection avec un ray casting
	bool findRayIntersection(CVector &p1, CVector &p2, float &dis, CVector &intersection, CVector &normal, int &n);

	// Pour trouver un point d'intersection avec un sphere casting
	bool findSphereIntersection(CVector &p1, CVector &p2, float rayon, float &dis, CVector &intersection, CVector &normal, int &n);
};


// La fonction la plus importante de tous
//bool SegmentToBox(CVector & p1, CVector & p2, CVector & center, CVector & size, float hl);
bool SegmentToBox(const CVector& p1, const CVector& p2, const CVector& min, const CVector& max);
bool SphereToBox(const CVector& p1, const CVector& p2, const CVector& min, const CVector& max, float rayon);

#endif
