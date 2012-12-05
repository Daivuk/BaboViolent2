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

#include "COctree.h"
#include "dkoInner.h"



//
// Constructeur / Destructeur
//
COctreeNode::COctreeNode(CFace **mFaceArray, int mNbFace, CVector mCenter, float mSize, int mRecurs)
{
//	color = 0;

	recurs = mRecurs;

	// On init deux ou trois ti truc avant
	nbFace = 0;
	faceArray = 0;
	haveChild = false;
	int i;
	for(i=0;i<8;child[i++]=0);

	if (recurs == MAX_RECURS) return;

	size = mSize;
	center = mCenter;

	// On passe les faces pis on check si elles font parti de notre cube, si oui on la rajoute
	for (i=0;i<mNbFace;i++)
	{
		if (faceIntersection(mFaceArray[i])) nbFace++;
	}

	if (nbFace)
	{
		// On ajoute les faces trouv�
		int j=0;
		faceArray = new CFace*[nbFace];
		for (i=0;i<mNbFace;i++)
		{
			if (faceIntersection(mFaceArray[i])) 
			{
				faceArray[j] = mFaceArray[i]; // On l'ajoute yea!
				j++;
			}
		}

		// On check si on est en dessous du nb limite
		if (nbFace > MAX_PER_NODE)
		{
			// On check pour ses child asteur
			for (i=0;i<8;i++)
			{
				CVector newPos;
				switch (i)
				{
				case 0: newPos = center + CVector(-size/2, -size/2, -size/2); break;
				case 1: newPos = center + CVector(-size/2, size/2, -size/2); break;
				case 2: newPos = center + CVector(size/2, size/2, -size/2); break;
				case 3: newPos = center + CVector(size/2, -size/2, -size/2); break;
				case 4: newPos = center + CVector(-size/2, -size/2, size/2); break;
				case 5: newPos = center + CVector(-size/2, size/2, size/2); break;
				case 6: newPos = center + CVector(size/2, size/2, size/2); break;
				case 7: newPos = center + CVector(size/2, -size/2, size/2); break;
				}

				child[i] = new COctreeNode(faceArray, nbFace, newPos, size/2, recurs+1);
				if (child[i]->nbFace == 0) 
				{
					delete child[i]; 
					child[i] = 0;
				}
				else haveChild = true;
			}
		}
	}
}

COctreeNode::~COctreeNode()
{
	if (faceArray) delete faceArray;

	for(int i=0;i<8;i++)
	{
		// On delete r�ursivement
		if (child[i]) delete child[i];
	}
}



//
// Pour l'intersection ak une face
//
bool COctreeNode::faceIntersection(CFace * face)
{
	// On check pour chaque segment du triangle
	for (int i=0;i<3;i++)
	{
	/*	CVector p1 = face->point[i];
		CVector p2 = face->point[(i+1)%3];
		float hl = CVector::Distance(p1, p2) / 2;
		if (SegmentToBox(p1, p2, center, CVector(size, size, size), hl)) return true;*/
		if (SegmentToBox(face->point[i], face->point[(i+1)%3], center-CVector(size, size, size), center+CVector(size, size, size))) return true;
	}

	// Ensuite on check pour chaque segment (arr�es) du cube
	CVector p1 = center + CVector(-size, -size, -size);
	CVector p2 = center + CVector(-size, -size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(size, -size, -size);
	p2 = center + CVector(size, -size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(-size, size, -size);
	p2 = center + CVector(-size, size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(size, size, -size);
	p2 = center + CVector(size, size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}

	p1 = center + CVector(-size, -size, size);
	p2 = center + CVector(size, -size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(-size, size, -size);
	p2 = center + CVector(size, size, -size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(-size, -size, -size);
	p2 = center + CVector(size, -size, -size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}
	p1 = center + CVector(-size, size, size);
	p2 = center + CVector(size, size, size);
	if (face->segmentToFace(p1, p2))
	{
		return true;
	}

	return false;
}



//
// On va trouver le point d'intersection avec un ray tracing
//
bool COctreeNode::findRayIntersection(CVector &p1, CVector &p2, float &dis, CVector &intersection, CVector &normal, int &n)
{
	n++;

	// On test avec notre cube
	if (SegmentToBox(p1, p2, center-CVector(size,size,size), center+CVector(size,size,size)))
	{
	//	color = 1;

		// Est-ce quil a des child?
		if (haveChild)
		{
			bool found = false;
			for (int i=0;i<8;i++)
			{
				if (child[i])
				{
					if (child[i]->findRayIntersection(p1, p2, dis, intersection, normal, n)) found = true;
				}
			}
			return found;
		}
		else
		{
			// On test ses faces
			bool found = false;
			for (int i=0;i<nbFace;i++)
			{
				if (faceArray[i]->frameID != CDko::globalFrameID)
				{
					faceArray[i]->frameID = CDko::globalFrameID;
					n++;
					CVector intersect;
					if (faceArray[i]->findIntersect(p1, p2, intersect))
					{
						float disc = distanceFast(intersect, p1);
						if (disc < dis)
						{
							if (faceArray[i]->pointInFace(intersect))
							{
								p2 = intersect; // � optimise pour les tests �venir
								intersection = intersect;
								normal = CVector(faceArray[i]->a, faceArray[i]->b, faceArray[i]->c);
								dis = disc;
								found = true;
							}
						}
					}
				}
			}
			return found;
		}
	}

	return false;
}



//
// Pour trouver un point d'intersection avec un sphere casting
//
bool COctreeNode::findSphereIntersection(CVector &p1, CVector &p2, float rayon, float &dis, CVector &intersection, CVector &normal, int &n)
{
	n++;

	// On test avec notre cube
	if (SphereToBox(p1, p2, center-CVector(size,size,size), center+CVector(size,size,size), rayon))
	{
	//	color = 1;

		// Est-ce quil a des child?
		if (haveChild)
		{
			bool found = false;
			for (int i=0;i<8;i++)
			{
				if (child[i])
				{
					if (child[i]->findSphereIntersection(p1, p2, rayon, dis, intersection, normal, n)) found = true;
				}
			}
			return found;
		}
		else
		{
			// On test ses faces
			bool found = false;
			for (int i=0;i<nbFace;i++)
			{
				if (faceArray[i]->frameID != CDko::globalFrameID)
				{
					faceArray[i]->frameID = CDko::globalFrameID;
					n++;
					CVector intersect;
					CVector faceNormal(faceArray[i]->a,faceArray[i]->b,faceArray[i]->c);
					if (faceArray[i]->findIntersect(
						p1 - faceNormal*rayon,
						p2 - faceNormal*rayon,
						intersect))
					{
						float disc = distanceFast(intersect + faceNormal*rayon, p1);
						if (disc < dis)
						{
							if (faceArray[i]->pointInFace(intersect, rayon))
							{
								p2 = intersect + faceNormal*rayon; // � optimise pour les tests �venir
								intersection = p2;
								normal = faceNormal;
								dis = disc;
								found = true;
							}
						/*	else
							{
								// Il n'y a pas d'intersection avec l'interieur du triangle, alors on check ak ses arr�es
								for (int a=0;a<3;a++)
								{
									// On trouve la distance entre les deux droite
									CVector d1 = faceArray[i]->point[a];
									CVector d2 = p1;
									CVector u1 = faceArray[i]->point[(a+1)%3]-d1;
									CVector u2 = p2-p1;
									CVector d1d2 = d2-d1;
									CVector cross = CVector::CrossProduct(u1, u2);
									float scalar = (d1d2) * cross;
									float d = fabsf(scalar / CVector::Distance(CVector(0,0,0), cross));

									// On check si cette distance est plus petite que le rayon, sinon ce n'est pas utile de faire le test
									if (d <= rayon)
									{
										u1.Normalize();
										u2.Normalize();
										float scalar = u1*u2;
										CVector newPos = -u2 * rayon * scalar

										float r = rayon / scalar;
										CVector D = (cross * (cross * d1d2));
										float a = sqrtf(r*r - d*d);
										CVector A = -u2;
										A.Normalize();
										A *= a;
										CVector R = A-D;

										// On d�lace notre segment avec R et on trouve le point d'intersection des deux segment
										CVector d3 = d1 + R;
										CVector d3d2 = (d2 - d3);
										CVector u2u1 = (u1 - u2);
										d3d2 /= u2u1;
										CVector newPos = d3d2;
										CVector onSeg = newPos - R;

										// Bon, maintenant on check s'il fait parti du segment
										float dis1 = CVector::Distance(onSeg, d1);
										float dis2 = CVector::Distance(onSeg, d1+u1);
										float dist = CVector::Distance(d1, d1+u1);
										if (dis1+dis2 <= dist + d + .1f)
										{
											// COLLISIONS ENFIN CRISS!!!!!!
											intersection = newPos;
											normal = R / r;
											p2 = intersection;
											dis = disc;
											found = true;
											break;
										}
									}
								}
							}*/
						}
					}
				}
			}
			return found;
		}
	}

	return false;
}



//
// Pour dessiner le octree (sera jamais utilis�sauf pour le premier test)
//
void COctreeNode::render()
{
#ifndef CONSOLE
#ifndef _DX_
	glPushAttrib(GL_POLYGON_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glLineWidth(1);
		glColor3f(1,1,0);
		glPushMatrix();
			glTranslatef(center[0], center[1], center[2]);
			glScalef(size, size, size);
			glBegin(GL_QUADS);
				glNormal3f(0, -1, 0);
				glTexCoord2i(0, 1);
				glVertex3i(-1, -1, 1);
				glTexCoord2i(0, 0);
				glVertex3i(-1, -1, -1);
				glTexCoord2i(1, 0);
				glVertex3i(1, -1, -1);
				glTexCoord2i(1, 1);
				glVertex3i(1, -1, 1);

				glNormal3f(1, 0, 0);
				glTexCoord2i(0, 1);
				glVertex3i(1, -1, 1);
				glTexCoord2i(0, 0);
				glVertex3i(1, -1, -1);
				glTexCoord2i(1, 0);
				glVertex3i(1, 1, -1);
				glTexCoord2i(1, 1);
				glVertex3i(1, 1, 1);

				glNormal3f(0, 1, 0);
				glTexCoord2i(0, 1);
				glVertex3i(1, 1, 1);
				glTexCoord2i(0, 0);
				glVertex3i(1, 1, -1);
				glTexCoord2i(1, 0);
				glVertex3i(-1, 1, -1);
				glTexCoord2i(1, 1);
				glVertex3i(-1, 1, 1);

				glNormal3f(-1, 0, 0);
				glTexCoord2i(0, 1);
				glVertex3i(-1, 1, 1);
				glTexCoord2i(0, 0);
				glVertex3i(-1, 1, -1);
				glTexCoord2i(1, 0);
				glVertex3i(-1, -1, -1);
				glTexCoord2i(1, 1);
				glVertex3i(-1, -1, 1);

				glNormal3f(0, 0, 1);
				glTexCoord2i(0, 1);
				glVertex3i(-1, 1, 1);
				glTexCoord2i(0, 0);
				glVertex3i(-1, -1, 1);
				glTexCoord2i(1, 0);
				glVertex3i(1, -1, 1);
				glTexCoord2i(1, 1);
				glVertex3i(1, 1, 1);

				glNormal3f(0, 0, -1);
				glTexCoord2i(0, 1);
				glVertex3i(-1, -1, -1);
				glTexCoord2i(0, 0);
				glVertex3i(-1, 1, -1);
				glTexCoord2i(1, 0);
				glVertex3i(1, 1, -1);
				glTexCoord2i(1, 1);
				glVertex3i(1, -1, -1);
			glEnd();
		glPopMatrix();
	glPopAttrib();
#endif
#endif
	
	// On dessine le cube genreeee
	if (haveChild)
	{
		for (int i=0;i<8;i++)
		{
			if (child[i]) child[i]->render();
		}
	}
}



//
// On check segment-to-box
//
bool SegmentToBox(const CVector& p1, const CVector& p2, const CVector& min, const CVector& max)
{
    CVector d = (p2 - p1) * 0.5f;
    CVector e = (max - min) * 0.5f;
    CVector c = p1 + d - (min + max) * 0.5f;
    CVector ad = CVector(fabsf(d[0]),fabsf(d[1]),fabsf(d[2]));; // Returns same vector with all components positive

    if (fabsf(c[0]) > e[0] + ad[0])
        return false;
    if (fabsf(c[1]) > e[1] + ad[1])
        return false;
    if (fabsf(c[2]) > e[2] + ad[2])
        return false;
  
    if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1]/* + EPSILON*/)
        return false;
    if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2]/* + EPSILON*/)
        return false;
    if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0]/* + EPSILON*/)
        return false;
            
    return true;
}



//
// On check sphere-to-box
//
bool SphereToBox(const CVector& p1, const CVector& p2, const CVector& omin, const CVector& omax, float rayon)
{
 	CVector min = omin - CVector(rayon, rayon, rayon);
	CVector max = omax + CVector(rayon, rayon, rayon);
    CVector d = (p2 - p1) * 0.5f;
    CVector e = (max - min) * 0.5f;
    CVector c = p1 + d - (min + max) * 0.5f;
    CVector ad = CVector(fabsf(d[0]),fabsf(d[1]),fabsf(d[2]));; // Returns same vector with all components positive

    if (fabsf(c[0]) > e[0] + ad[0])
        return false;
    if (fabsf(c[1]) > e[1] + ad[1])
        return false;
    if (fabsf(c[2]) > e[2] + ad[2])
        return false;
  
    if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1]/* + EPSILON*/)
        return false;
    if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2]/* + EPSILON*/)
        return false;
    if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0]/* + EPSILON*/)
        return false;
            
    return true;
}
