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

#include "CMatrix.h"



//
// La matrice transposé
//
void CMatrix3x3f::Transpose(){
	for (int i=0;i<3;i++)
	{
		for (int j=i;j<3;j++)
		{
			float tmp;
			tmp = s[i*3+j];
			s[i*3+j] = s[j*3+i];
			s[j*3+i] = tmp;
		}
	}
}



//
// Multiplication de matrice
//
CMatrix3x3f CMatrix3x3f::operator*(const CMatrix3x3f & matrix) const
{
	CMatrix3x3f NewMat;

	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			float Result=0;
			for (int k=0;k<3;k++)
			{
				Result += s[i*3+k]*matrix.s[k*3+j];
			}
			NewMat.s[i*3+j] = Result;
		}
	}

	return NewMat;
}
void CMatrix3x3f::operator*=(const CMatrix3x3f & matrix)
{
	CMatrix3x3f NewMat;

	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			float Result=0;
			for (int k=0;k<3;k++)
			{
				Result += s[i*3+k]*matrix.s[k*3+j];
			}
			NewMat.s[i*3+j] = Result;
		}
	}

	*this = NewMat;
}



//
// Multiplication par un vecteur
//
CVector3f CMatrix3x3f::operator*(const CVector3f &u) const
{
	return CVector3f(
		s[0]*u.s[0] + s[1]*u.s[1] + s[2]*u.s[2], 
		s[3]*u.s[0] + s[4]*u.s[1] + s[5]*u.s[2], 
		s[6]*u.s[0] + s[7]*u.s[1] + s[8]*u.s[2]
		);
}



//
// Pour calculer un mineur
//
float CMatrix3x3f::Minor(int Row, int Col) const
{
	// On pogne les autres lignes
	int Row1 = 0, Row2 = 0, Col1 = 0, Col2 = 0;
	float Signe = 1;
	if (Row == 0) {Row1 = 1;Row2 = 2;}
	if (Row == 1) {Row1 = 0;Row2 = 2;Signe*=-1;}
	if (Row == 2) {Row1 = 0;Row2 = 1;}
	if (Col == 0) {Col1 = 1;Col2 = 2;}
	if (Col == 1) {Col1 = 0;Col2 = 2;Signe*=-1;}
	if (Col == 2) {Col1 = 0;Col2 = 1;}

	// Maintenant on retourne le mineur
	return Signe * ((s[Row1*3+Col1] * s[Row2*3+Col2]) - (s[Row1*3+Col2] * s[Row2*3+Col1]));
}



//
// Pour trouver le déterminant de la matrice
//
float CMatrix3x3f::Determinant() const
{
	return dot(CVector3f(s[0],s[3],s[6]), cross(CVector3f(s[1],s[4],s[7]), CVector3f(s[2],s[5],s[8])));
}



//
// Pour trouver la matrice inverse
//
CMatrix3x3f CMatrix3x3f::Inverse() const
{
	CMatrix3x3f Com = *this;

	// On trouve d'abords les mineurs de chaque éléments
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			Com.s[i*3+j] = Minor(i,j);
		}
	}

	// On effectu la transposé de cette matrice
	Com.Transpose();

	// On la divise par le determinant et voilà
	return Com / Determinant();
}



//
// Effectuer une rotation autour d'un axe déterminé
//
void CMatrix3x3f::RotateArbitrary(float Angle, const CVector3f& u)
{
	CVector3f RightRot = rotateAboutAxis(CVector3f(&(s[0])), Angle, u);
	CVector3f FrontRot = rotateAboutAxis(CVector3f(&(s[3])), Angle, u);
	CVector3f UpRot = rotateAboutAxis(CVector3f(&(s[6])), Angle, u);

	// on modifi le X et le Y et le Z
	s[0] = RightRot[0];
	s[1] = RightRot[1];
	s[2] = RightRot[2];
	s[3] = FrontRot[0];
	s[4] = FrontRot[1];
	s[5] = FrontRot[2];
	s[6] = UpRot[0];
	s[7] = UpRot[1];
	s[8] = UpRot[2];
}



//
// Effectuer une rotation autour d'un axe déterminé
//
void CMatrix3x3f::RotateArbitrary(const CVector3f& u)
{
	RotateAboutRight(u.s[0]);
	RotateAboutFront(u.s[1]);
	RotateAboutUp(u.s[2]);
}



//
// On reset la base de grandeur 1
//
void CMatrix3x3f::normalize()
{
	for (int i=0;i<3;i++)
	{
		float size = sqrtf(
			s[i*3+0]*s[i*3+0] + 
			s[i*3+1]*s[i*3+1] + 
			s[i*3+2]*s[i*3+2]);
		s[i*3+0] /= size;
		s[i*3+1] /= size;
		s[i*3+2] /= size;
	}
}
