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

//*******************************************************************
///	\brief Definition des classes CVector2i, CVector2f, CVector3i, CVector3f
///
/// \file CVector.h
/// Pour d�inir un vecteur en 2D ou en 3D ainsi que ses operateurs
///	Ici ce n'est pas juste pour les vecteurs mais aussi des maths en g��al
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (�des fins de documentation seulement)
//*******************************************************************

#ifndef CVECTOR_H
#define CVECTOR_H



#include <math.h>

/// \name Constantes mathematiques
/// Differentes constantes mathematiques et constantes de conversions.
//@{
#define EPSILON 0.0001f
#define TO_RADIANT 0.017453f
#define TO_DEGREE 57.295780f
#define PI 3.141593f
//@}

/// \brief Racine carre inverse de Carmack
///
/// Cette fonction permet de calculer 1/sqrt(number) par l'approximation de Carmack.
/// 
/// \param number qu'on veut obtenir la racine carre inverse.
/// \return reponse du calcul 1/sqrt(number)
float dk_sqrtf(float number);

//*******************************************************************
//*******************************************************************
/// \brief Classe de vector 2D en int (ie. un point)
//*******************************************************************
///
///	Un vecteur 2d est un veteur avec la queue a l'origine et la tete a la coordonnee (x,y)
/// Classe derivee de CAttribute
class CVector2i
{
public:
	/// Ses valeurs public
	/// s[0] = position en X
	/// s[1] = position en Y
	int s[2];

public:
	// Constructeur
	CVector2i();					///< constructeur par default: coord (0,0)
	CVector2i(int x, int y);		///< constructeur avec initialisation: coord (x,y)
	CVector2i(int* array);			///< constructeur par tableau
	CVector2i(const CVector2i& vector);	///< constructeur copie
	
	/// Fonctions d'affectation
	void set(int x, int y){s[0] = x; s[1] = y;}				///< affectation par x,y
	void set(int* array){s[0] = array[0]; s[1] = array[1];}	///< affectation par tableau[2]

	/// Ses accesseurs
	int& operator[](const int i) {return s[i];}	///< obtenir x ou y selon i
	int operator[](const int i)const {return s[i];}	///< obtenir x ou y selon i
	int& x() {return s[0];}	///< obtenir x
	int& y() {return s[1];}	///< obtenir y

	/// Sa longueur
	float length() {return sqrtf((float)(s[0]*s[0] + s[1]*s[1]));}	///< retourne la longueur du vecteur
	float lengthFast() {return dk_sqrtf((float)(s[0]*s[0] + s[1]*s[1]));} ///< retourne la longueur du vecteur en utilisant Carmack

	/// Les operateurs
	void operator=(const CVector2i &vector) {s[0]=vector.s[0];s[1]=vector.s[1];}	///< affectation de la position d'un autre vecteur

	/// \name Operateurs retournants un CVector2i
	//@{
	CVector2i operator-() {return CVector2i(-s[0],-s[1]);}	///< inverser la direction du vecteur
	CVector2i operator+(const CVector2i& vector) const {return CVector2i(s[0]+vector.s[0], s[1]+vector.s[1]);}	///< somme de 2 vecteurs
	CVector2i operator-(const CVector2i& vector) const {return CVector2i(s[0]-vector.s[0], s[1]-vector.s[1]);}	///< soustraction de 2 vecteurs
	CVector2i operator*(const CVector2i& vector) const {return CVector2i(s[0]*vector.s[0], s[1]*vector.s[1]);}	///< multiplication de 2 vecteurs, utile pour le dot product
	CVector2i operator/(const CVector2i& vector) const {return CVector2i(s[0]/vector.s[0], s[1]/vector.s[1]);}  ///< division de 2 vecteurs ??? je ne connais pas l'application mathematique de la division de 2 vecteurs...
	CVector2i operator*(int scalar) const {return CVector2i(s[0]*scalar, s[1]*scalar);}	///< produit scalaire du vecteur
	CVector2i operator/(int scalar) const {return CVector2i(s[0]/scalar, s[1]/scalar);}	///< division scalaire du vecteur
	//@}

	/// \name Operator overload CVector2i
	//@{
	void operator+=(CVector2i vector) {s[0]+=vector.s[0]; s[1]+=vector.s[1];}	///< surcharge la somme de 2 vecteurs
	void operator-=(CVector2i vector) {s[0]-=vector.s[0]; s[1]-=vector.s[1];}	///< surcharge la soustraction de 2 vecteurs
	void operator*=(CVector2i vector) {s[0]*=vector.s[0]; s[1]*=vector.s[1];}	///< surcharge la multiplication de 2 vecteurs, utile pour le dot product
	void operator/=(CVector2i vector) {s[0]/=vector.s[0]; s[1]/=vector.s[1];}	///< surcharge la division de 2 vecteurs???
	void operator*=(int scalar) {s[0]*=scalar; s[1]*=scalar;}	///< surcharge du produit scalaire du vecteur
	void operator/=(int scalar) {s[0]/=scalar; s[1]/=scalar;}	///< surcharge de la division scalaire du vecteur
	
	bool operator==(const CVector2i &vector) {return (s[0]==vector.s[0] && s[1]==vector.s[1]);}		///< surcharge de l'operateur de comparaison	positive
	bool operator!=(const CVector2i &vector) {return !(s[0]==vector.s[0] && s[1]==vector.s[1]);}	///< surcharge de l'operateur de comparaison negative
	//@}
};


//*******************************************************************
//*******************************************************************
/// \brief Classe de vector 2D en float (ie. un point)
//*******************************************************************
///
///	Un vecteur 2d est un veteur avec la queue a l'origine et la tete a la coordonnee (x,y)
/// Classe derivee de CAttribute
class CVector2f
{
public:
	/// Ses valeurs public
	/// s[0] = position en X
	/// s[1] = position en Y
	float s[2];

public:
	/// Constructeur
	CVector2f();						///< constructeur par default: coord (0,0)
	CVector2f(float x, float y);		///< constructeur avec initialisation: coord (x,y)
	CVector2f(float* array);			///< constructeur par tableau
	CVector2f(const CVector2f& vector);	///< constructeur copie

	/// Fonctions d'affectation
	void set(float x, float y){s[0] = x; s[1] = y;}				///< affectation par x,y
	void set(float* array){s[0] = array[0]; s[1] = array[1];}	///< affectation par tableau[2]

	/// Ses accesseurs
	float& operator[](const int i) {return s[i];}	/// obtenir x ou y selon i
	float operator[](const int i) const {return s[i];}	/// obtenir x ou y selon i
	float& x() {return s[0];}	/// obtenir x
	float& y() {return s[1];}	/// obtenir y

	/// Sa longueur
	float length() {return sqrtf(s[0]*s[0] + s[1]*s[1]);}			///< retourne la longueur du vecteur
	float lengthFast() {return dk_sqrtf(s[0]*s[0] + s[1]*s[1]);}	///< retourne la longueur du vecteur en utilisant Carmack

	/// Les operateurs
	void operator=(const CVector2f &vector) {s[0]=vector.s[0];s[1]=vector.s[1];}	///< affectation de la position d'un autre vecteur

	/// \name Operateurs retournants un CVector2f
	//@{
	CVector2f operator-() {return CVector2f(-s[0],-s[1]);}		///< inverser la direction du vecteur
	CVector2f operator+(const CVector2f& vector) const {return CVector2f(s[0]+vector.s[0], s[1]+vector.s[1]);}	///< somme de 2 vecteurs
	CVector2f operator-(const CVector2f& vector) const {return CVector2f(s[0]-vector.s[0], s[1]-vector.s[1]);}	///< soustraction de 2 vecteurs
	CVector2f operator*(const CVector2f& vector) const {return CVector2f(s[0]*vector.s[0], s[1]*vector.s[1]);}	///< multiplication de 2 vecteurs
	CVector2f operator/(const CVector2f& vector) const {return CVector2f(s[0]/vector.s[0], s[1]/vector.s[1]);}	///< division de 2 vecteurs???
	CVector2f operator*(float scalar) {return CVector2f(s[0]*scalar, s[1]*scalar);}	///< mulitiplication scalaire du vecteur
	CVector2f operator/(float scalar) {return CVector2f(s[0]/scalar, s[1]/scalar);}	///< division scalaire du vecteur
	//@}

	/// \name Operator overload CVector2f
	//@{
	void operator+=(CVector2f vector) {s[0]+=vector.s[0]; s[1]+=vector.s[1];}	///< surcharge de l'operateur d'addition entre 2 vecteurs
	void operator-=(CVector2f vector) {s[0]-=vector.s[0]; s[1]-=vector.s[1];}	///< surcharge de l'operateur de soustraction entre 2 vecteurs
	void operator*=(CVector2f vector) {s[0]*=vector.s[0]; s[1]*=vector.s[1];}	///< surcharge de l'operateur de multiplication entre 2 vecteurs
	void operator/=(CVector2f vector) {s[0]/=vector.s[0]; s[1]/=vector.s[1];}	///< surcharge de l'operateur de division entre 2 vecteurs
	void operator*=(float scalar) {s[0]*=scalar; s[1]*=scalar;}		///< surcharge de l'operateur de mulitiplication pour le produit scalaire
	void operator/=(float scalar) {s[0]/=scalar; s[1]/=scalar;}		///< surcharge de l'operateur de division pour le produit scalaire

	/// surcharge de l'operateur de comparaison positive entre 2 verteurs avec ajustement EPSILON
	bool operator==(const CVector2f &vector) {return (
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON));}
	/// surcharge de l'operateur de comparaison negative entre 2 verteurs avec ajustement EPSILON
	bool operator!=(const CVector2f &vector) {return !(
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON));}
	//@}
};

//*******************************************************************
//*******************************************************************
/// \brief Classe de vector 3D en int
//*******************************************************************
///
/// Classe derivee de CAttribute
class CVector3i
{
public:
	/// Ses valeurs public
	/// s[0] = position en X
	/// s[1] = position en Y
	/// s[2] = position en Z
	int s[3];

public:
	/// Constructeur
	CVector3i();					///< constructeur par default (0,0,0)
	CVector3i(int x, int y, int z);	///< constructeur avec initialisation (x,y,z)
	CVector3i(int* array);			///< constructeur avec initialisation par tableau
	CVector3i(const CVector3i& vector);	///< constructeur copie

	/// Fonctions d'affectation
	void set(int x, int y, int z){s[0] = x; s[1] = y; s[2] = z;}	///< affectation de x,y,z
	void set(int* array){s[0] = array[0]; s[1] = array[1]; s[2] = array[2];}	///< affectation de x,y,zn par un tableau

	/// Ses accesseurs
	int& operator[](const int i) {return s[i];}		///< obtenir la valeur de x,y ou z selon i
	int operator[](const int i) const {return s[i];}		///< obtenir la valeur de x,y ou z selon i
	int& x() {return s[0];}		///< obtenir la position x
	int& y() {return s[1];}		///< obtenir la position y
	int& z() {return s[2];}		///< obtenir la position z

	/// Sa grandeur
	float length() {return sqrtf((float)(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]));}			///< obtenir la longueur du vecteur
	float lengthFast() {return dk_sqrtf((float)(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]));}	///< obtenir la longueur du vecteur en utilisant Carmack

	/// Les operateurs
	void operator=(const CVector3i &vector) {s[0]=vector.s[0];s[1]=vector.s[1];s[2]=vector.s[2];}	///< affectation de la position d'un vecteur x,y,z au vecteur courant

	/// \name Surcharge d'operators avec retour d'un CVector3i
	//@{
	CVector3i operator-() {return CVector3i(-s[0],-s[1],-s[2]);}		///< inverser la direction du vecteur
	CVector3i operator+(const CVector3i& vector) const {return CVector3i(s[0]+vector.s[0], s[1]+vector.s[1], s[2]+vector.s[2]);}	///< addition de 2 vecteurs
	CVector3i operator-(const CVector3i& vector) const {return CVector3i(s[0]-vector.s[0], s[1]-vector.s[1], s[2]-vector.s[2]);}	///< soustraction de 2 vecteurs
	CVector3i operator*(const CVector3i& vector) const {return CVector3i(s[0]*vector.s[0], s[1]*vector.s[1], s[2]*vector.s[2]);}	///< multiplication de 2 vecteurs, utile pour le dot product
	CVector3i operator/(const CVector3i& vector) const {return CVector3i(s[0]/vector.s[0], s[1]/vector.s[1], s[2]/vector.s[2]);}	///< division de 2 vecteurs???
	CVector3i operator*(int scalar) {return CVector3i(s[0]*scalar, s[1]*scalar, s[2]*scalar);}	///< produit scalaire
	CVector3i operator/(int scalar) {return CVector3i(s[0]/scalar, s[1]/scalar, s[2]/scalar);}	///< division scalaire
	//@}

	/// \name Surcharge d'operateurs CVector3i
	//@{
	void operator+=(CVector3i vector) {s[0]+=vector.s[0]; s[1]+=vector.s[1]; s[2]+=vector.s[2];}	///< surcharge de l'operateur d'addition
	void operator-=(CVector3i vector) {s[0]-=vector.s[0]; s[1]-=vector.s[1]; s[2]-=vector.s[2];}	///< surcharge de l'operateur de soustraction
	void operator*=(CVector3i vector) {s[0]*=vector.s[0]; s[1]*=vector.s[1]; s[2]*=vector.s[2];}	///< surcharge de l'operateur de multiplication
	void operator/=(CVector3i vector) {s[0]/=vector.s[0]; s[1]/=vector.s[1]; s[2]/=vector.s[2];}	///< surcharge de l'operateur de division ???
	void operator*=(int scalar) {s[0]*=scalar; s[1]*=scalar; s[2]*=scalar;}		///< surcharge de l'operateur de multiplication scalaire
	void operator/=(int scalar) {s[0]/=scalar; s[1]/=scalar; s[2]/=scalar;}		///< surcharge de l'operateur ddivision scalaire

	bool operator==(const CVector3i &vector) {return (s[0]==vector.s[0] && s[1]==vector.s[1] && s[2]==vector.s[2]);}	///< surcharge de l'operateur de comparaison positive
	bool operator!=(const CVector3i &vector) {return !(s[0]==vector.s[0] && s[1]==vector.s[1] && s[2]==vector.s[2]);}	///< surcharge de l'operateur de comparaison negative
	//@}
};


//*******************************************************************
//*******************************************************************
/// \brief Classe de vector 3D en float
//*******************************************************************
///
/// Classe derivee de CAttribute
class CVector3f
{
public:
	/// Ses valeurs public
	/// s[0] = position en X
	/// s[1] = position en Y
	/// s[2] = position en Z
	float s[3];

public:
	/// Constructeur
	CVector3f();							///< constructeur par default (0,0,0)
	CVector3f(float x, float y, float z);	///< constructeur avec initialisation (x,y,z)
	CVector3f(float* array);				///< constructeur avec initialisation par tableau
	CVector3f(const CVector3f& vector);		///< constructeur copie

	/// Fonctions d'affectation
	void set(float x, float y, float z){s[0] = x; s[1] = y; s[2] = z;}			///< affectation de x,y,z
	void set(float* array){s[0] = array[0]; s[1] = array[1]; s[2] = array[2];}	///< affectation de x,y,zn par un tableau

	/// Ses accesseurs
	float& operator[](const int i) {return s[i];}	///< obtenir la valeur de x,y ou z selon i
	float operator[](const int i) const {return s[i];}	///< obtenir la valeur de x,y ou z selon i
	float& x() {return s[0];}	///< obtenir la position x
	float& y() {return s[1];}	///< obtenir la position y
	float& z() {return s[2];}	///< obtenir la position z

	/// Sa grandeur
	float length() const {return sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);}			///< obtenir la longueur du vecteur
	float lengthFast() const {return dk_sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);}	///< obtenir la longueur du vecteur en utilisant Carmack

	/// Pour uniformiser
	/// Q: Pourquoi est qu'on veut obtenir un vecteur dans la direction moyenne du vecteur courant?
	void grayScale() {float m = (s[0]+s[1]+s[2]) / 3; set(m,m,m);}

	/// Les operateurs
	void operator=(const CVector3f &vector) {s[0]=vector.s[0];s[1]=vector.s[1];s[2]=vector.s[2];}	///< affectation de la position d'un vecteur x,y,z au vecteur courant

	/// \name Surcharge d'operators vectoriels avec retour d'un CVector3f
	/// \param CVector3f
	/// \return CVector3f
	//@{ 
	CVector3f operator-() {return CVector3f(-s[0],-s[1],-s[2]);}		///< inverser la direction du vecteur
	CVector3f operator+(const CVector3f& vector) const {return CVector3f(s[0]+vector.s[0], s[1]+vector.s[1], s[2]+vector.s[2]);}	///< addition de 2 vecteurs
	CVector3f operator-(const CVector3f& vector) const {return CVector3f(s[0]-vector.s[0], s[1]-vector.s[1], s[2]-vector.s[2]);}	///< soustraction de 2 vecteurs
	CVector3f operator*(const CVector3f& vector) const {return CVector3f(s[0]*vector.s[0], s[1]*vector.s[1], s[2]*vector.s[2]);}	///< multiplication de 2 vecteurs
	CVector3f operator/(const CVector3f& vector) const {return CVector3f(s[0]/vector.s[0], s[1]/vector.s[1], s[2]/vector.s[2]);}	///< division de 2 vecteurs ???
	//@}

	/// \name Surcharge d'operators scalaire avec retour d'un CVector3f
	///	\param float scalar
	///	\return CVector3f
	//@{ 
	CVector3f operator*(float scalar) const {return CVector3f(s[0]*scalar, s[1]*scalar, s[2]*scalar);}	///< multiplication scalaire
	CVector3f operator/(float scalar) const {return CVector3f(s[0]/scalar, s[1]/scalar, s[2]/scalar);}	///< division scalaire
	CVector3f operator+(float scalar) const {return CVector3f(s[0]+scalar, s[1]+scalar, s[2]+scalar);}	///< addition scalaire
	CVector3f operator-(float scalar) const {return CVector3f(s[0]-scalar, s[1]-scalar, s[2]-scalar);}	///< soustraction scalaire
	//@}

	/// \name Surcharge d'operateurs CVector3f
	//@{
	void operator+=(CVector3f vector) {s[0]+=vector.s[0]; s[1]+=vector.s[1]; s[2]+=vector.s[2];}	///< surcharge de l'operateur d'addition
	void operator-=(CVector3f vector) {s[0]-=vector.s[0]; s[1]-=vector.s[1]; s[2]-=vector.s[2];}	///< surcharge de l'operateur de soustraction
	void operator*=(CVector3f vector) {s[0]*=vector.s[0]; s[1]*=vector.s[1]; s[2]*=vector.s[2];}	///< surcharge de l'operateur de multiplication
	void operator/=(CVector3f vector) {s[0]/=vector.s[0]; s[1]/=vector.s[1]; s[2]/=vector.s[2];}	///< surcharge de l'operateur de division ???
	void operator*=(float scalar) {s[0]*=scalar; s[1]*=scalar; s[2]*=scalar;}	///< produit scalaire
	void operator/=(float scalar) {s[0]/=scalar; s[1]/=scalar; s[2]/=scalar;}	///< division scalaire

	/// surcharge de l'operateur de comparaison positive entre 2 verteurs avec ajustement EPSILON
	bool operator==(const CVector3f &vector) const {return (
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON) &&
			(s[2]>=vector.s[2]-EPSILON && s[2]<=vector.s[2]+EPSILON));}
	/// surcharge de l'operateur de comparaison negative entre 2 verteurs avec ajustement EPSILON
	bool operator!=(const CVector3f &vector) const {return !(
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON) &&
			(s[2]>=vector.s[2]-EPSILON && s[2]<=vector.s[2]+EPSILON));}
	/// surcharge de l'operateur de comparaison negative entre 2 verteurs avec ajustement EPSILON
	bool IsEqual(const CVector3f &vector) const {return !(
			(s[0]==vector.s[0]) &&
			(s[1]==vector.s[1]) &&
			(s[2]==vector.s[2]));}
	//@}
};

//*******************************************************************
//*******************************************************************
/// \brief Classe de vector 4D en float
//*******************************************************************
///
/// Un vecteur en 4D est utile pour les couleurs
/// Classe derivee de CAttribute
class CVector4f
{
public:
	/// Ses valeurs public
	/// s[0] = x
	/// s[1] = y
	/// s[2] = z
	/// s[3] = w
	float s[4];

public:
	/// Constructeur
	CVector4f();			///< constructeur par default (0,0,0,0)
	CVector4f(float x, float y, float z, float w);	///< constructeur avec initialisation (x,y,z,w)
	CVector4f(float* array);	///< constructeur avec inialisation par tableau
	CVector4f(const CVector4f& vector);	///< constructeur copie

	/// Fonctions d'affectation
	void set(float x, float y, float z, float w){s[0] = x; s[1] = y; s[2] = z; s[3] = w;}		///< affectation des valeurs x,y,z,w
	void set(float* array){s[0] = array[0]; s[1] = array[1]; s[2] = array[2]; s[3] = array[3];}	///< affectation des valeurs x,y,z,w par un tableau

	/// Ses accesseurs
	float& operator[](const int i) {return s[i];}	///< obtention de x,y,z,w selon i
	float operator[](const int i) const {return s[i];}	///< obtention de x,y,z,w selon i
	float& x() {return s[0];}	///< obtenir x
	float& y() {return s[1];}	///< obtenir y
	float& z() {return s[2];}	///< obtenir z
	float& r() {return s[4];}	///< obtenir w

	/// Sa grandeur
	float length() {return sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2] + s[3]*s[3]);}	///< longueur du vecteur
	float lengthFast() {return dk_sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2] + s[3]*s[3]);}	///< longueur du vecteur avec Carmack

	/// Pour uniformiser
	void grayScale() {float m = (s[0]+s[1]+s[2]) / 3; set(m,m,m,s[3]);}	///< obtention d'un nouveau vecteur avec la moyenne des positions x,y,z

	/// Les operateurs
	void operator=(const CVector4f &vector) {s[0]=vector.s[0];s[1]=vector.s[1];s[2]=vector.s[2];s[3]=vector.s[3];}	///< affectation de la position d'un vecteur x,y,z,w au vecteur courant

	/// \name Surcharge d'operators vectoriels avec retour d'un CVector4f
	/// \param CVector4f
	/// \return CVector4f
	//@{
	CVector4f operator-() {return CVector4f(-s[0],-s[1],-s[2],-s[3]);}	///< inverser le vecteur courant
	CVector4f operator+(const CVector4f& vector) const {return CVector4f(s[0]+vector.s[0], s[1]+vector.s[1], s[2]+vector.s[2], s[3]+vector.s[3]);}	///< addition de 2 vecteurs
	CVector4f operator-(const CVector4f& vector) const {return CVector4f(s[0]-vector.s[0], s[1]-vector.s[1], s[2]-vector.s[2], s[3]-vector.s[3]);}	///< soustraction de 2 vecteurs
	CVector4f operator*(const CVector4f& vector) const {return CVector4f(s[0]*vector.s[0], s[1]*vector.s[1], s[2]*vector.s[2], s[3]*vector.s[3]);}	///< multiplication de 2 vecteurs
	CVector4f operator/(const CVector4f& vector) const {return CVector4f(s[0]/vector.s[0], s[1]/vector.s[1], s[2]/vector.s[2], s[3]/vector.s[3]);}	///< division de 2 vecteurs???
	//@}

	/// Produits scalaire avec retour d'un vecteur CVector4f
	CVector4f operator*(float scalar) {return CVector4f(s[0]*scalar, s[1]*scalar, s[2]*scalar, s[3]*scalar);}	///< produit scalaire
	CVector4f operator/(float scalar) {return CVector4f(s[0]/scalar, s[1]/scalar, s[2]/scalar, s[3]/scalar);}	///< division scalaire

	/// \name Surcharge d'operators vectoriels
	/// \param CVector4f
	//@{
	void operator+=(CVector4f vector) {s[0]+=vector.s[0]; s[1]+=vector.s[1]; s[2]+=vector.s[2]; s[3]+=vector.s[3];}		///< surcharge de l'operateur d'addition
	void operator-=(CVector4f vector) {s[0]-=vector.s[0]; s[1]-=vector.s[1]; s[2]-=vector.s[2]; s[3]-=vector.s[3];}		///< surcharge de l'operateur de soustraction
	void operator*=(CVector4f vector) {s[0]*=vector.s[0]; s[1]*=vector.s[1]; s[2]*=vector.s[2]; s[3]*=vector.s[3];}		///< surcharge de l'operateur de multiplication
	void operator/=(CVector4f vector) {s[0]/=vector.s[0]; s[1]/=vector.s[1]; s[2]/=vector.s[2]; s[3]/=vector.s[3];}		///< surcharge de l'operateur de division ???
	//@}

	/// Produits scalaire
	void operator*=(float scalar) {s[0]*=scalar; s[1]*=scalar; s[2]*=scalar; s[3]*=scalar;}	///< produit scalaire
	void operator/=(float scalar) {s[0]/=scalar; s[1]/=scalar; s[2]/=scalar; s[3]/=scalar;} ///< division scalaire

	/// surcharge de l'operateur de comparaison positive entre 2 verteurs avec ajustement EPSILON
	bool operator==(const CVector4f &vector) {return (
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON) &&
			(s[2]>=vector.s[2]-EPSILON && s[2]<=vector.s[2]+EPSILON) &&
			(s[3]>=vector.s[3]-EPSILON && s[3]<=vector.s[3]+EPSILON));}
	/// surcharge de l'operateur de comparaison negative entre 2 verteurs avec ajustement EPSILON
	bool operator!=(const CVector4f &vector) {return !(
			(s[0]>=vector.s[0]-EPSILON && s[0]<=vector.s[0]+EPSILON) &&
			(s[1]>=vector.s[1]-EPSILON && s[1]<=vector.s[1]+EPSILON) &&
			(s[2]>=vector.s[2]-EPSILON && s[2]<=vector.s[2]+EPSILON) &&
			(s[3]>=vector.s[3]-EPSILON && s[3]<=vector.s[3]+EPSILON));}
};

//*******************************************************************
//*******************************************************************
/// Les fonctions plus global
//*******************************************************************
/// \name Pour vecteur 2D
//@{
///
/// \brief dot product
///
/// dot product entre 2 vecteurs 2D
///
/// \param CVector &u: 1er vecteur
/// \param CVector &v: 2e vecteur
/// \return float resultat scalaire du dot product entre u et v
float dot(const CVector2f &u, const CVector2f &v);

/// \brief normalize
///
/// normalisation du vecteur courant == transformer la longueur du vecteur a 1
/// \param CVector2f &u: vecteur a normaliser
void normalize(CVector2f &u);

/// \brief normalize fast
///
/// normalisation du vecteur courant == transformer la longueur du vecteur a 1 en utilisant Carmack
/// \param CVector2f &u: vecteur a normaliser
void normalizeFast(CVector2f &u);

/// \brief Obtenir la distance entre 2 vecteurs
///
/// \param CVector &u: 1er vecteur
/// \param CVector &v: 2e vecteur
/// \return distance entre les 2 vecteurs en float
float distance(const CVector2f &u, const CVector2f &v);

/// \brief Obtenir la distance entre 2 vecteurs avec Carmack
///
/// \param CVector &u: 1er vecteur
/// \param CVector &v: 2e vecteur
/// \return distance entre les 2 vecteurs en float
float distanceFast(const CVector2f &u, const CVector2f &v);

/// \brief Rotation par rapport a un pivot
///	
///	permet de faire la rotation autour d'un point(vecteur 2d) donne.
///
/// \param CVector2f & p: point a faire tourner autour du pivot
/// \param float Angle: angle de rotation
/// \param CVector2f & pivot: le point de pivot
/// \return CVector2f: nouvelle position du point
CVector2f rotateAboutPivot(const CVector2f & p, float Angle, const CVector2f & pivot);
//@}
//*******************************************************************
/// \name Pour vecteur 3D
//@{
///
/// \brief dot product
///
/// dot product entre 2 vecteurs 3D
///
/// \param CVector &u: 1er vecteur
/// \param CVector &v: 2e vecteur
/// \return float resultat scalaire du dot product entre u et v
float dot(const CVector3f &u, const CVector3f &v);

/// \brief normalize
///
/// normalisation du vecteur courant == transformer la longueur du vecteur a 1
/// \param CVector3f &u: vecteur a normaliser
void normalize(CVector3f &u);

/// \brief normalize fast
///
/// normalisation du vecteur courant == transformer la longueur du vecteur a 1 en utilisant Carmack
/// \param CVector3f &u: vecteur a normaliser
void normalizeFast(CVector3f &u);

/// \brief cross product en 2d
///
/// Donne le determinant d'une matrice 2x2. 
///	Disons une matrice A = [u v] =	[u1 v1]
///									[u2 v2]
/// det(A) = |u v| = u1v2 - u2v1
///
/// \param CVector2f &u
/// \param CVector2f &v
/// \return float
float crossZ(const CVector2f &u, const CVector2f &v);

/// \brief cross product en 3d
///
/// Donne le vecteur perpendiculaire au plan forme par les 2 vecteurs. 
/// Le nouveau vecteur pointe dans la direction selon la regle de la main droite.
///
/// \param CVector3f &u
/// \param CVector3f &v
/// \return CVector3f
CVector3f cross(const CVector3f &u, const CVector3f &v);

/// \brief distance entre 2 vecteurs
///
/// Donne la distance entre 2 vecteurs en 3D
///
/// \param CVector3f &u
/// \param CVector3f &v
/// \return distance entre les 2 vecteurs en float
float distance(const CVector3f &u, const CVector3f &v);


/// \brief distance entre 2 vecteurs avec Carmack
///
/// Donne la distance entre 2 vecteurs en 3D en utilisant l'approximation de Carmack
///
/// \param CVector3f &u
/// \param CVector3f &v
/// \return distance entre les 2 vecteurs en float
float distanceFast(const CVector3f &u, const CVector3f &v);

/// \brief distance au carre entre 2 vecteurs
///
/// Donne la distance au carre entre 2 vecteurs en 3D
///
/// \param CVector3f &u
/// \param CVector3f &v
/// \return distance entre les 2 vecteurs en float
float distanceSquared(const CVector3f &u, const CVector3f &v);

/// \brief Rotation par rapport a un axe
///	
///	permet de faire la rotation autour d'un axe(vecteur 3d) donne.
///
/// \param CVector3f & p: point a faire tourner autour du pivot
/// \param float Angle: angle de rotation
/// \param CVector3f & Axis: l'axe de pivot
/// \return CVector3f: nouvelle position du vecteur
CVector3f rotateAboutAxis(const CVector3f & p, float Angle, const CVector3f & Axis);

/// \brief Obtention d'un systeme d'axes base sur un seul vecteur.
///
/// On obtient un systeme d'axe en se basant sur un seul vecteur (v-front).
/// Le vecteur de reference peut pointer dans n'importe quelle direction.
/// On obtient le 1er vecteur perpendiculaire (v-right) en faisant un cross product 
///	de v-front avec l'axe des y de reference. On refait un cross product entre
/// le nouveau vecteur (v-right) et v-front pour obtenir le dernier vecteur (v-up).
/// 
/// \param CVector3f & right: vecteur de droite dans le nouveau systeme de coordonnees
/// \param const CVector3f & front: vecteur de direction dans le nouveau systeme de coordonnees
/// \param CVector & up: vecteur du haut dans le nouveau systeme de coordonnees
void createRightUpVectors(CVector3f & right, const CVector3f & front, CVector3f & up);


CVector3f projection(const CVector3f &u, const CVector3f &Onv);
CVector3f reflect(const CVector3f &u, const CVector3f &normal);
//@}
//*******************************************************************
/// \name Pour les random
///
///	Obtention de vecteurs aleatoires en 2d,3d et 4d pour des int et float
//@{
/// 
/// \brief rand pour v2d-int
///
///	\param CVector2i & from
///	\param CVector2i & to
///	\return CVector2i vecteur aleatoire
CVector2i rand(const CVector2i & from, const CVector2i & to);

/// \brief rand pour v2d-float
///
///	\param CVector2f & from
///	\param CVector2f & to
///	\return CVector2f vecteur aleatoire
CVector2f rand(const CVector2f & from, const CVector2f & to);

/// \brief rand pour v3d-int
///
///	\param CVector3i & from
///	\param CVector3i & to
///	\return CVector3i vecteur aleatoire
CVector3i rand(const CVector3i & from, const CVector3i & to);

/// \brief rand pour v3d-float
///
///	\param CVector3f & from
///	\param CVector3f & to
///	\return CVector3f vecteur aleatoire
CVector3f rand(const CVector3f & from, const CVector3f & to);

/// \brief rand pour v4d-float
///
///	\param CVector4f & from
///	\param CVector4f & to
///	\return CVector4f vecteur aleatoire
CVector4f rand(const CVector4f & from, const CVector4f & to);

/// \brief Fonction random en int
///
/// \param int from
/// \param int to
/// \return int nombre aleatoire
int rand(int from, int to);

/// \brief Fonction random en float
///
/// \param float from
/// \param float to
/// \return float nombre aleatoire
float rand(float from, float to);
//@}
//*******************************************************************
/// \name Les sub class, color, etc
//@{
#define CPoint2i CVector2i
#define CPoint2f CVector2f
#define CPoint3i CVector3i
#define CPoint3f CVector3f
#define CColor3f CVector3f
#define CColor4f CVector4f
#define cVector CVector3f // Pour les anciennes compatibilit�
//@}

#endif


