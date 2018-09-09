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


#ifndef CPARTICLE_H
#define CPARTICLE_H


#include "CVector.h"


class CParticle
{
public:
	// Sa texture utilisé
	unsigned int texture;

	// La position de la particle
	CPoint3f position;

	// Sa position au dernier frame
	CPoint3f lastPosition;

	// Sa couleur au début
	CColor4f startColor;

	// Sa couleur à la fin
	CColor4f endColor;

	// Sa velocité
	CVector3f vel;

	// Sa vie (0 à 1) (1 étant mort)
	float life;

	// Sa duré de vie, sec / duré
	float fadeSpeed;

	// Sa grosseur au début
	float startSize;

	// Sa grosseur à la fin
	float endSize;

	// Sa densité (1 = fully attracted by gravity)
	float density;

	// Son angle de rotation
	float angle;

	// La vitesse de rotation de son angle
	float rotationSpeed;

	// La fonction de blending utilisé
	unsigned int srcBlend;
	unsigned int dstBlend;

	// Pour la résistance de l'air
	float airResistanceInfluence;

	// La distance avec la camera
	float camDis;

	// Si c'est un billboard
	bool billboard;

	// Son opacity
	float billboardOpacity;

	// Sa distance avant de fader
	float billboardFadeDis;

	float billboardFadeDelay;

	bool toDelete;

	bool isMultipleTexture;
	unsigned int * textureArray;
	int nbFrame;

public:
	// Constructeur / Destructeur
	CParticle(
		float *position,
		float *vel,
		float *startColor,
		float *endColor,
		float startSize,
		float endSize,
		float duration,
		float density,
		float airResistanceInfluence,
		float rotationSpeed,
		unsigned int texture,
		unsigned int srcBlend,
		unsigned int dstBlend,
		int transitionFunc);
	virtual ~CParticle();
	
	// Pour l'updater
	int update();

	// Pour le rendre
	void render();
};


#endif