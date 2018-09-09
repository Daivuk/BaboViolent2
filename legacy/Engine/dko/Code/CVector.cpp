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


#include "CVector.h"
#include <math.h>
#include <stdlib.h>
#include "platform_types.h"



//
// CVector2i
//
CVector2i::CVector2i()
{
	s[0] = 0;
	s[1] = 0;
}

CVector2i::CVector2i(int x, int y)
{
	s[0] = x;
	s[1] = y;
}

CVector2i::CVector2i(const int* array)
{
	s[0] = array[0];
	s[1] = array[1];
}

CVector2i::CVector2i(const CVector2i& vector)
{
	s[0] = vector.s[0];
	s[1] = vector.s[1];
}



//
// CVector2f
//
CVector2f::CVector2f()
{
	s[0] = 0;
	s[1] = 0;
}

CVector2f::CVector2f(float x, float y)
{
	s[0] = x;
	s[1] = y;
}

CVector2f::CVector2f(const float* array)
{
	s[0] = array[0];
	s[1] = array[1];
}

CVector2f::CVector2f(const CVector2f& vector)
{
	s[0] = vector.s[0];
	s[1] = vector.s[1];
}



//
// CVector3i
//
CVector3i::CVector3i()
{
	s[0] = 0;
	s[1] = 0;
	s[2] = 0;
}

CVector3i::CVector3i(int x, int y, int z)
{
	s[0] = x;
	s[1] = y;
	s[2] = z;
}

CVector3i::CVector3i(const int* array)
{
	s[0] = array[0];
	s[1] = array[1];
	s[2] = array[2];
}

CVector3i::CVector3i(const CVector3i& vector)
{
	s[0] = vector.s[0];
	s[1] = vector.s[1];
	s[2] = vector.s[2];
}



//
// CVector3f
//
CVector3f::CVector3f()
{
	s[0] = 0;
	s[1] = 0;
	s[2] = 0;
}

CVector3f::CVector3f(float x, float y, float z)
{
	s[0] = x;
	s[1] = y;
	s[2] = z;
}

CVector3f::CVector3f(const float* array)
{
	s[0] = array[0];
	s[1] = array[1];
	s[2] = array[2];
}

CVector3f::CVector3f(const CVector3f& vector)
{
	s[0] = vector.s[0];
	s[1] = vector.s[1];
	s[2] = vector.s[2];
}



//
// CVector4f
//
CVector4f::CVector4f()
{
	s[0] = 0;
	s[1] = 0;
	s[2] = 0;
	s[3] = 1;
}

CVector4f::CVector4f(float x, float y, float z, float r)
{
	s[0] = x;
	s[1] = y;
	s[2] = z;
	s[3] = r;
}

CVector4f::CVector4f(const float* array)
{
	s[0] = array[0];
	s[1] = array[1];
	s[2] = array[2];
	s[3] = array[3];
}

CVector4f::CVector4f(const CVector4f& vector)
{
	s[0] = vector.s[0];
	s[1] = vector.s[1];
	s[2] = vector.s[2];
	s[3] = vector.s[3];
}


// Le sqrtf mongole de Carmack!
// ---------------------------------------------------------
// Function: float dk_sqrtf(float number)
//
// Parameters:	[in]:	float number	: the number to be 1/sqrt(number)
//				[out]:	float y			: result of 1/sqrt(number)
// 
// Description: This is Carmack's fast inverse square root function. 
//				It will gives the inverse of the sqrt of a number by 1st taking
//				a guess and calculate the right number with Newton's iteration.
//
//
// ---------------------------------------------------------
float dk_sqrtf(float number)
{
	INT4 i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( INT4 * ) &y;						// get bits for floating value
	i  = 0x5f3759df - ( i >> 1 );               // gives initial guess y0. 0x5f3759df => gives the smalless error approximation of the final value.
	y  = * ( float * ) &i;						// converts bits back to float
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed. Put back if more precision is needed.

	return y;
}



//
// Distance entre deux vecteur
//
float distance(const CVector2f &u, const CVector2f &v)
{
	return sqrtf(
		(u[0]-v[0]) * (u[0]-v[0]) + 
		(u[1]-v[1]) * (u[1]-v[1]));
}

float distanceFast(const CVector2f &u, const CVector2f &v)
{
	return dk_sqrtf(
		(u[0]-v[0]) * (u[0]-v[0]) + 
		(u[1]-v[1]) * (u[1]-v[1]));
}

float distance(const CVector3f &u, const CVector3f &v)
{
	return sqrtf(
		(u[0]-v[0]) * (u[0]-v[0]) + 
		(u[1]-v[1]) * (u[1]-v[1]) + 
		(u[2]-v[2]) * (u[2]-v[2]));
}

float distanceFast(const CVector3f &u, const CVector3f &v)
{
	return dk_sqrtf(
		(u[0]-v[0]) * (u[0]-v[0]) + 
		(u[1]-v[1]) * (u[1]-v[1]) + 
		(u[2]-v[2]) * (u[2]-v[2]));
}

float distanceSquared(const CVector3f &u, const CVector3f &v)
{
	return (
		(u[0]-v[0]) * (u[0]-v[0]) + 
		(u[1]-v[1]) * (u[1]-v[1]) + 
		(u[2]-v[2]) * (u[2]-v[2]));
}



//
// Produit scalaire
//
float dot(const CVector3f &u, const CVector3f &v)
{
	return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

float dot(const CVector2f &u, const CVector2f &v)
{
	return u[0]*v[0] + u[1]*v[1];
}



//
// Projection orthogonale de u sur v
//
CVector3f projection(const CVector3f &u, const CVector3f &Onv)
{
	return Onv * dot(u, Onv) / Onv.length();
}



//
// r�lection sur un plan
//
CVector3f reflect(const CVector3f &u, const CVector3f &normal)
{
	CVector3f temp = (projection(u, normal) * 2);
	return u - temp;
}



//
// Normalizer un vecteur
//
void normalize(CVector3f &u)
{
	float x = 0;
	float y = sqrtf(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
	if (y != 0) x = (1 / y);
	u[0] *= x;
	u[1] *= x;
	u[2] *= x;
}

void normalizeFast(CVector3f &u)
{
	float x = 0;
	float y = dk_sqrtf(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
	if (y != 0) x = (1 / y);
	u[0] *= x;
	u[1] *= x;
	u[2] *= x;
}

void normalize(CVector2f &u)
{
	float x = 0;
	float y = sqrtf(u[0]*u[0] + u[1]*u[1]);
	if (y != 0) x = (1 / y);
	u[0] *= x;
	u[1] *= x;
}

void normalizeFast(CVector2f &u)
{
	float x = 0;
	float y = dk_sqrtf(u[0]*u[0] + u[1]*u[1]);
	if (y != 0) x = (1 / y);
	u[0] *= x;
	u[1] *= x;
}



//
// Effectuer le cross product
//
CVector3f cross(const CVector3f &u, const CVector3f &v)
{
	return CVector3f(
		  (u[1] * v[2]) - (u[2] * v[1]),
		-((u[0] * v[2]) - (u[2] * v[0])),
		  (u[0] * v[1]) - (u[1] * v[0]));
}

float crossZ(const CVector2f &u, const CVector2f &v)
{
	return (u[0] * v[1]) - (u[1] * v[0]);
}



//
// Tourner autour d'un axe
//
CVector3f rotateAboutAxis(const CVector3f & point, float angle, const CVector3f & axis)
{
	angle *= TO_RADIANT;
	float	costheta = cosf(angle), 
			sintheta = sinf(angle);

	return CVector3f(
		(costheta + (1 - costheta) * axis[0] * axis[0]) * point[0] +
		((1 - costheta) * axis[0] * axis[1] - axis[2] * sintheta) * point[1] +
		((1 - costheta) * axis[0] * axis[2] + axis[1] * sintheta) * point[2],

		((1 - costheta) * axis[0] * axis[1] + axis[2] * sintheta) * point[0] +
		(costheta + (1 - costheta) * axis[1] * axis[1]) * point[1] +
		((1 - costheta) * axis[1] * axis[2] - axis[0] * sintheta) * point[2],

		((1 - costheta) * axis[0] * axis[2] - axis[1] * sintheta) * point[0] +
		((1 - costheta) * axis[1] * axis[2] + axis[0] * sintheta) * point[1] +
		(costheta + (1 - costheta) * axis[2] * axis[2]) * point[2]);
}



//
// Tourner autour d'un pivot en 2D
//
CVector2f rotateAboutPivot(const CVector2f & point, float angle, const CVector2f & pivot)
{
	CVector2f p = point - pivot;

	angle *= TO_RADIANT;
	float	costheta = cosf(angle),
			sintheta = sinf(angle);

	return CVector2f(
		costheta * p[0] + -sintheta * p[1] + pivot[0],
		sintheta * p[0] + costheta * p[1] + pivot[1]);
}



//
// Pour cr�r un vecteur right et up �partir d'un vecteur front donn�
//
void createRightUpVectors(CVector3f & right, const CVector3f & _front, CVector3f & up)
{
	CVector3f front = _front;
	normalize(front);

	// On essaye un cross avec le up
	if (front[1] == 1) // pointe en y > 0
	{
		right = CVector3f(1,0,0);
		up = CVector3f(0,0,1);
	}
	else if (front[1] == -1)  // pointe en y < 0
	{
		right = CVector3f(-1,0,0);
		up = CVector3f(0,0,1);
	}
	else
	{
		CVector3f temp = CVector3f(0,1,0);
		up = cross(front, temp);
		right = cross(front, up);
	}
}



//
// Pour les random
//
CVector2i rand(const CVector2i & from, const CVector2i & to)
{
	return CVector2i(
		rand(from[0], to[0]),
		rand(from[1], to[1]));
}

CVector2f rand(const CVector2f & from, const CVector2f & to)
{
	return CVector2f(
		rand(from[0], to[0]),
		rand(from[1], to[1]));
}

CVector3i rand(const CVector3i & from, const CVector3i & to)
{
	return CVector3i(
		rand(from[0], to[0]),
		rand(from[1], to[1]),
		rand(from[2], to[2]));
}

CVector3f rand(const CVector3f & from, const CVector3f & to)
{
	return CVector3f(
		rand(from[0], to[0]),
		rand(from[1], to[1]),
		rand(from[2], to[2]));
}

CVector4f rand(const CVector4f & from, const CVector4f & to)
{
	return CVector4f(
		rand(from[0], to[0]),
		rand(from[1], to[1]),
		rand(from[2], to[2]),
		rand(from[3], to[3]));
}

int rand(int from, int to)
{
	if (from > to)
	{
		int tmp = to;
		to = from;
		from = tmp;
	}
	return (from == to) ? from : from + rand()%(to-from);
}

float rand(float from, float to)
{
	if (from > to)
	{
		float tmp = to;
		to = from;
		from = tmp;
	}
	float eccart = (to-from);
	float precision = 30000.0f / eccart;
	return (eccart == 0) ? from : from + (((float)(rand()%((int)((to-from)*precision)))) / precision);
}
