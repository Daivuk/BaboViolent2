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

/* TCE (c) All rights reserved */


#ifndef CMatrix3x3f_H
#define CMatrix3x3f_H


#include "CVector.h"
#include "dkgl.h"


class CMatrix3x3f
{
public:
	// Sa matrice
	float s[16];

	// Constructeur
	CMatrix3x3f(){
		s[0]=1; s[1]=0; s[2]=0;
		s[3]=0; s[4]=1; s[5]=0;
		s[6]=0; s[7]=0; s[8]=1;
	}
	CMatrix3x3f(float a11, float a12, float a13,
			float a21, float a22, float a23,
			float a31, float a32, float a33){
		s[0]=a11; s[1]=a12; s[2]=a13;
		s[3]=a21; s[4]=a22; s[5]=a23;
		s[6]=a31; s[7]=a32; s[8]=a33;
	}
	CMatrix3x3f(const float *Array9Float){
		s[0]=Array9Float[0];
		s[1]=Array9Float[1];
		s[2]=Array9Float[2];
		s[3]=Array9Float[3];
		s[4]=Array9Float[4];
		s[5]=Array9Float[5];
		s[6]=Array9Float[6];
		s[7]=Array9Float[7];
		s[8]=Array9Float[8];
	}

	// Constructeur de copie
	CMatrix3x3f(const CMatrix3x3f &matrix){
		s[0]=matrix.s[0];
		s[1]=matrix.s[1];
		s[2]=matrix.s[2];
		s[3]=matrix.s[3];
		s[4]=matrix.s[4];
		s[5]=matrix.s[5];
		s[6]=matrix.s[6];
		s[7]=matrix.s[7];
		s[8]=matrix.s[8];
	}

	// Pour la setter
	void set(float a11, float a12, float a13,
			float a21, float a22, float a23,
			float a31, float a32, float a33){
		s[0]=a11; s[1]=a12; s[2]=a13;
		s[3]=a21; s[4]=a22; s[5]=a23;
		s[6]=a31; s[7]=a32; s[8]=a33;
	}
	void set(const float *Array9Float){
		s[0]=Array9Float[0];
		s[1]=Array9Float[1];
		s[2]=Array9Float[2];
		s[3]=Array9Float[3];
		s[4]=Array9Float[4];
		s[5]=Array9Float[5];
		s[6]=Array9Float[6];
		s[7]=Array9Float[7];
		s[8]=Array9Float[8];
	}

	// revenir à la matrice identity
	void LoadIdentity(){
		s[0]=1; s[1]=0; s[2]=0;
		s[3]=0; s[4]=1; s[5]=0;
		s[6]=0; s[7]=0; s[8]=1;
	}

	// Les operateurs ya!!!!
	CMatrix3x3f operator-() const
	{
		return CMatrix3x3f(
			-s[0],
			-s[1],
			-s[2],
			-s[3],
			-s[4],
			-s[5],
			-s[6],
			-s[7],
			-s[8]);
	}
	CMatrix3x3f operator+(const CMatrix3x3f & matrix) const
	{
		return CMatrix3x3f(
			s[0]+matrix.s[0],
			s[1]+matrix.s[1],
			s[2]+matrix.s[2],
			s[3]+matrix.s[3],
			s[4]+matrix.s[4],
			s[5]+matrix.s[5],
			s[6]+matrix.s[6],
			s[7]+matrix.s[7],
			s[8]+matrix.s[8]);
	}
	CMatrix3x3f operator-(const CMatrix3x3f & matrix) const
	{
		return CMatrix3x3f(
			s[0]-matrix.s[0],
			s[1]-matrix.s[1],
			s[2]-matrix.s[2],
			s[3]-matrix.s[3],
			s[4]-matrix.s[4],
			s[5]-matrix.s[5],
			s[6]-matrix.s[6],
			s[7]-matrix.s[7],
			s[8]-matrix.s[8]);
	}
	CMatrix3x3f operator*(float Scalar) const
	{
		return CMatrix3x3f(
			s[0]*Scalar,
			s[1]*Scalar,
			s[2]*Scalar,
			s[3]*Scalar,
			s[4]*Scalar,
			s[5]*Scalar,
			s[6]*Scalar,
			s[7]*Scalar,
			s[8]*Scalar);
	}
	CMatrix3x3f operator/(float Scalar) const
	{
		return CMatrix3x3f(
			s[0]/Scalar,
			s[1]/Scalar,
			s[2]/Scalar,
			s[3]/Scalar,
			s[4]/Scalar,
			s[5]/Scalar,
			s[6]/Scalar,
			s[7]/Scalar,
			s[8]/Scalar);
	}
	void operator+=(const CMatrix3x3f & matrix)
	{
		s[0]+=matrix.s[0];
		s[1]+=matrix.s[1];
		s[2]+=matrix.s[2];
		s[3]+=matrix.s[3];
		s[4]+=matrix.s[4];
		s[5]+=matrix.s[5];
		s[6]+=matrix.s[6];
		s[7]+=matrix.s[7];
		s[8]+=matrix.s[8];
	}
	void operator-=(const CMatrix3x3f & matrix)
	{
		s[0]-=matrix.s[0];
		s[1]-=matrix.s[1];
		s[2]-=matrix.s[2];
		s[3]-=matrix.s[3];
		s[4]-=matrix.s[4];
		s[5]-=matrix.s[5];
		s[6]-=matrix.s[6];
		s[7]-=matrix.s[7];
		s[8]-=matrix.s[8];
	}
	void operator*=(float Scalar)
	{
		s[0]*=Scalar;
		s[1]*=Scalar;
		s[2]*=Scalar;
		s[3]*=Scalar;
		s[4]*=Scalar;
		s[5]*=Scalar;
		s[6]*=Scalar;
		s[7]*=Scalar;
		s[8]*=Scalar;
	}
	void operator/=(float Scalar)
	{
		s[0]/=Scalar;
		s[1]/=Scalar;
		s[2]/=Scalar;
		s[3]/=Scalar;
		s[4]/=Scalar;
		s[5]/=Scalar;
		s[6]/=Scalar;
		s[7]/=Scalar;
		s[8]/=Scalar;
	}
	void operator=(const CMatrix3x3f & matrix)
	{
		s[0]=matrix.s[0];
		s[1]=matrix.s[1];
		s[2]=matrix.s[2];
		s[3]=matrix.s[3];
		s[4]=matrix.s[4];
		s[5]=matrix.s[5];
		s[6]=matrix.s[6];
		s[7]=matrix.s[7];
		s[8]=matrix.s[8];
	}
	bool operator==(const CMatrix3x3f & matrix) const
	{
		return (
			s[0]==matrix.s[0] &&
			s[1]==matrix.s[1] &&
			s[2]==matrix.s[2] &&
			s[3]==matrix.s[3] &&
			s[4]==matrix.s[4] &&
			s[5]==matrix.s[5] &&
			s[6]==matrix.s[6] &&
			s[7]==matrix.s[7] &&
			s[8]==matrix.s[8]);
	}
	bool operator!=(const CMatrix3x3f & matrix) const
	{
		return !(
			s[0]==matrix.s[0] &&
			s[1]==matrix.s[1] &&
			s[2]==matrix.s[2] &&
			s[3]==matrix.s[3] &&
			s[4]==matrix.s[4] &&
			s[5]==matrix.s[5] &&
			s[6]==matrix.s[6] &&
			s[7]==matrix.s[7] &&
			s[8]==matrix.s[8]);
	}

	// La matrice transposé
	void Transpose();

	// Multiplication de matrice
	CMatrix3x3f operator*(const CMatrix3x3f & matrix) const;
	void operator*=(const CMatrix3x3f & matrix);

	// Multiplication par un vecteur
	CVector3f operator*(const CVector3f &u) const;

	// Pour calculer un mineur
	float Minor(int Row, int Col) const;

	// Pour trouver le déterminant de la matrice
	float Determinant() const;

	// Pour trouver la matrice inverse
	CMatrix3x3f Inverse() const;

	// Pour acèder aux éléments
	float& operator[](const int i) {return s[i];}
	float operator[](const int i) const {return s[i];}


	// Pour obtenir les vecteur de la base
	CVector3f getRight() const {return CVector3f(s[0],s[1],s[2]);}
	CVector3f getFront() const {return CVector3f(s[3],s[4],s[5]);}
	CVector3f getUp() const {return CVector3f(s[6],s[7],s[8]);}

	void setRight(const CVector3f & vector) {s[0] = vector[0];s[1] = vector[1];s[2] = vector[2];}
	void setFront(const CVector3f & vector) {s[3] = vector[0];s[4] = vector[1];s[5] = vector[2];}
	void setUp(const CVector3f & vector) {s[6] = vector[0];s[7] = vector[1];s[8] = vector[2];}

	// Effectuer une rotation autour d'un axe déterminé
	void RotateArbitrary(float Angle, const CVector3f& u);

	// Effectuer une rotation autour d'un axe déterminé
	void RotateArbitrary(const CVector3f& u);

	// Transformation d'un vecteur
	CVector3f TransformVectorToLocal(const CVector3f& u) const
	{
		return	CVector3f(
				dot(getRight(),u),
				dot(getFront(),u),
				dot(getUp(),u));
	}
	CVector3f TransformVectorToParent(const CVector3f& u) const
	{
		return	getRight()*u.s[0] +
				getFront()*u.s[1] +
				getUp()*u.s[2];
	}

	// Pour openGL
	void MultOglMatrix() const
	{
		float Matrix[16] = {
			s[0], s[1], s[2], 0,
			s[3], s[4], s[5], 0,
			s[6], s[7], s[8], 0,
			0,    0,    0,    1};

#ifndef _DX_
		glMultMatrixf(Matrix);
#endif
	}

	// Les rotations de base autour des axes
	void RotateAboutRight(float Angle)
	{
		// On check si l'angle n'est pas 0, dans ce cas c inutile
		if(Angle)
		{
			// On fou l'angle en radian
			Angle = Angle*TO_RADIANT;

			// On calcul le cos et le sin
			float cosAngle = cosf(Angle);
			float sinAngle = sinf(Angle);

			CVector3f b1 = getFront()*cosAngle + getUp()*sinAngle;
			CVector3f b2 = -getFront()*sinAngle + getUp()*cosAngle;

			// on modifi le Y et le Z
			s[3] = b1[0];
			s[4] = b1[1];
			s[5] = b1[2];
			s[6] = b2[0];
			s[7] = b2[1];
			s[8] = b2[2];
		}
	}


	void RotateAboutFront(float Angle)
	{
		// On check si l'angle n'est pas 0, dans ce cas c inutile
		if(Angle)
		{
			// On fou l'angle en radian
			Angle = Angle*TO_RADIANT;

			// On calcul le cos et le sin
			float cosAngle = cosf(Angle);
			float sinAngle = sinf(Angle);

			CVector3f b2 = getUp()*cosAngle + getRight()*sinAngle;
			CVector3f b0 = -getUp()*sinAngle + getRight()*cosAngle;

			// on modifi le X et le Z
			s[6] = b2[0];
			s[7] = b2[1];
			s[8] = b2[2];
			s[0] = b0[0];
			s[1] = b0[1];
			s[2] = b0[2];
		}
	}

	void RotateAboutUp(float Angle)
	{
		// On check si l'angle n'est pas 0, dans ce cas c inutile
		if(Angle)
		{
			// On fou l'angle en radian
			Angle = Angle*TO_RADIANT;

			// On calcul le cos et le sin
			float cosAngle = cosf(Angle);
			float sinAngle = sinf(Angle);

			CVector3f b0 = getRight()*cosAngle + getFront()*sinAngle;
			CVector3f b1 = -getRight()*sinAngle + getFront()*cosAngle;

			// on modifi le X et le Y
			s[0] = b0[0];
			s[1] = b0[1];
			s[2] = b0[2];
			s[3] = b1[0];
			s[4] = b1[1];
			s[5] = b1[2];
		}
	}

	// Pour remettre la base de grandeur 1
	void normalize();
};


#endif
