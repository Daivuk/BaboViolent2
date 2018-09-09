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

#ifndef CFACE_H
#define CFACE_H


#include "platform_types.h"
#include "CVector.h"


class CFace
{
public:
	// ses points
	CVector point[3];

	// Normal �chaque point
	CVector normals[9];

	// This is to prevent to alocate the memory each time.
	CVector colPoint[9];

	// La d�inition du plan de la face (normal incluse)
	float a,b,c,d;

	// Son ID
	INT4 frameID;

	// Constructeur
	CFace()
	{
		frameID = 0;
		a = 0;
		b = 0;
		c = 1;
		d = 0;
	}

	// Trouver si un segment intersect le plan (juste oui ou non l�
	bool segmentToFace(const CVector &p1, const CVector &p2)
	{
		(void)p2;
		CVector intersect;
		if (findIntersect(p1, p1, intersect))// BUG: shouldn't this be p1, p2?
		{
			if (pointInFace(intersect)) return true;
		}
		return false;
	}

	// Trouver le point d'intersection d'un segment dans ce plan
	bool findIntersect(const CVector& p1, const CVector& p2, CVector& intersectPoint)
	{
		//float epsilon = 0.1f;

		float dis1 = distancePoint(p1);
		float dis2 = distancePoint(p2);

		if (/*(dis1 < -0 && dis2 > 0) || */(dis1 > 0 && dis2 < -0))
		{
			float percent = fabsf(dis1) / (fabsf(dis1) + fabsf(dis2));
			intersectPoint = p1 + (p2-p1) * percent;

			return true;
		}

		return false;
	}

	// Pour calculer la distance d'un point �ce plan
	float distancePoint(const CVector& mpoint) const
	{
		// Trop rapide :P
		return a*mpoint[0] + b*mpoint[1] + c*mpoint[2] - d;
	}

	// Pour cr�r les normals �chaque point
	void createNormals()
	{
		int i=0;
		for (i=0;i<3;++i)
		{
			normals[i*3 + 0] = cross(point[(i+1)%3] - point[i], CVector(a,b,c));
			normalize(normals[i*3 + 0]);
			normals[i*3 + 1] = normals[i*3 + 0];
			normals[i*3 + 2] = normals[i*3 + 0];
		}

		//--- Create point normals
		for (i=0;i<3;++i)
		{
			normals[(i*3+8)%9] = normals[(i*3+0)%9] + normals[(i*3+7)%9];
			normalize(normals[(i*3+8)%9]);
		}
	}

	// savoir si le point est sur la surface
	bool pointInFace(CVector& mpoint, float radius)
	{
	/*	for (int i=0;i<3;i++)
		{
			// On cr�deux vecteur avec chaque point
			CVector d = CVector::CrossProduct(point[(i+1)%3] - point[i], CVector(a,b,c));
			d.Normalize();
			d *= radius;
			CVector u = (point[i] + d) - mpoint;
			CVector v = (point[(i+1)%3] + d) - mpoint;

			// On effectu le produit crois�de �
			CVector norm = CVector::CrossProduct(u,v);

			// On calcul la distance du produit crois�au plan
			float dis = distancePoint(mpoint+norm);

			// Si on est derri�e, ha ah!! le point n'est pas dans le plan
			if (dis < 0) return false;
		}*/

		// ses points de collision avec sphere
		colPoint[0] = point[0] + normals[0] * radius;
		colPoint[1] = point[1] + normals[1] * radius;
		colPoint[2] = point[1] + normals[2] * radius;
		colPoint[3] = point[1] + normals[3] * radius;
		colPoint[4] = point[2] + normals[4] * radius;
		colPoint[5] = point[2] + normals[5] * radius;
		colPoint[6] = point[2] + normals[6] * radius;
		colPoint[7] = point[0] + normals[7] * radius;
		colPoint[8] = point[0] + normals[8] * radius;

		for (int i=0;i<9;++i)
		{
			// On cr�deux vecteur avec chaque point
			CVector u = (colPoint[i]) - mpoint;
			CVector v = (colPoint[(i+1)%9]) - mpoint;

			// On effectu le produit crois�de �
			CVector norm = cross(u,v);

			// On calcul la distance du produit crois�au plan
			float dis = distancePoint(mpoint+norm);

			// Si on est derri�e, ha ah!! le point n'est pas dans le plan
			if (dis < 0) return false;
		}

		return true;
	}

	// savoir si le point est sur la surface
	bool pointInFace(CVector& mpoint)
	{
		for (int i=0;i<3;++i)
		{
			// On cr�deux vecteur avec chaque point
			CVector u = point[i] - mpoint;
			CVector v = point[(i+1)%3] - mpoint;

			// On effectu le produit crois�de �
			CVector norm = cross(u,v);

			// On calcul la distance du produit crois�au plan
			float dis = distancePoint(mpoint+norm);

			// Si on est derri�e, ha ah!! le point n'est pas dans le plan
			if (dis < 0) return false;
		}

		return true;
	}
};



#endif
