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


#ifndef DKPI_H
#define DKPI_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif

#ifdef WIN32


#include <windows.h>
#endif

#include <vector>

#include "dkgl.h"

#include "CParticle.h"



const int DKP_TRANS_LINEAR = 0;
const int DKP_TRANS_FASTIN = 1;
const int DKP_TRANS_FASTOUT = 2;
const int DKP_TRANS_SMOOTH = 3;



/* BlendingFactorDest */
#define DKP_ZERO                           0
#define DKP_ONE                            1
#define DKP_SRC_COLOR                      0x0300
#define DKP_ONE_MINUS_SRC_COLOR            0x0301
#define DKP_SRC_ALPHA                      0x0302
#define DKP_ONE_MINUS_SRC_ALPHA            0x0303
#define DKP_DST_ALPHA                      0x0304
#define DKP_ONE_MINUS_DST_ALPHA            0x0305

/* BlendingFactorSrc */
/*      DKP_ZERO */
/*      DKP_ONE */
#define DKP_DST_COLOR                      0x0306
#define DKP_ONE_MINUS_DST_COLOR            0x0307
#define DKP_SRC_ALPHA_SATURATE             0x0308
/*      DKP_SRC_ALPHA */
/*      DKP_ONE_MINUS_SRC_ALPHA */
/*      DKP_DST_ALPHA */
/*      DKP_ONE_MINUS_DST_ALPHA */



// une tite struct pour aider
struct dkp_preset {
	CVector3f positionFrom;
	CVector3f positionTo;
	CVector3f direction;
	float speedFrom;
	float speedTo;
	float pitchFrom;
	float pitchTo;
	float startSizeFrom;
	float startSizeTo;
	float endSizeFrom;
	float endSizeTo;
	float durationFrom;
	float durationTo;
	CColor4f startColorFrom;
	CColor4f startColorTo;
	CColor4f endColorFrom;
	CColor4f endColorTo;
	float angleFrom;
	float angleTo;
	float angleSpeedFrom;
	float angleSpeedTo;
	float gravityInfluence;
	float airResistanceInfluence;
	unsigned int particleCountFrom;
	unsigned int particleCountTo;
	unsigned int *texture;
	int textureFrameCount;
	unsigned int srcBlend;
	unsigned int dstBlend;
};




// Les fonction du DKP
DLL_API void			dkpCreateBillboard(	CVector3f & positionFrom,
											CVector3f & positionTo,
											float fadeSpeed,
											float fadeOutDistance,
											float size,
											CColor4f & color,
											unsigned int textureID,
											unsigned int srcBlend,
											unsigned int dstBlend);
DLL_API void			dkpCreateParticle(	float *position,
											float *vel,
											float *startColor,
											float *endColor,
											float startSize,
											float endSize,
											float duration,
											float gravityInfluence,
											float airResistanceInfluence,
											float rotationSpeed,
											unsigned int texture,
											unsigned int srcBlend,
											unsigned int dstBlend,
											int transitionFunc);
DLL_API void			dkpCreateParticleEx(CVector3f & positionFrom,
											CVector3f & positionTo,
											CVector3f & direction,
											float speedFrom,
											float speedTo,
											float pitchFrom,
											float pitchTo,
											float startSizeFrom,
											float startSizeTo,
											float endSizeFrom,
											float endSizeTo,
											float durationFrom,
											float durationTo,
											CColor4f & startColorFrom,
											CColor4f & startColorTo,
											CColor4f & endColorFrom,
											CColor4f & endColorTo,
											float angleFrom,
											float angleTo,
											float angleSpeedFrom,
											float angleSpeedTo,
											float gravityInfluence,
											float airResistanceInfluence,
											unsigned int particleCountFrom,
											unsigned int particleCountTo,
											unsigned int *texture,
											int textureFrameCount,
											unsigned int srcBlend,
											unsigned int dstBlend);
DLL_API void			dkpCreateParticleExP(dkp_preset & preset);
DLL_API	void			dkpInit();
DLL_API void			dkpRender();
DLL_API void			dkpReset();
DLL_API void			dkpSetAirDensity(float airDensity);
DLL_API void			dkpSetGravity(float *vel);
DLL_API void			dkpSetSorting(bool sort);
DLL_API void			dkpShutDown();
DLL_API int				dkpUpdate(float delay);



// La class static pour tenir les informations importante sur les particles
class CDkp
{
public:
	// La liste de toute les particles ouais poupé
	static std::vector<CParticle*> particles;

	// La gravité
	static CVector3f gravity;

	// Le delay
	static float delay;

	// La modelview
	static float modelView[16];

	// La position de la camera
	static CVector3f camPos;

	// La DP pour une particle
//	static unsigned int dpSprite;
	static float* vertexArray;
	static float* normalArray;
	static float* texCoordArray;

	static unsigned int lastTexture;

	// La densité de l'air (en kPa)
	static float airDensity;

	// Si on doit les sorter ou pas
	static bool sorting;

	// Pour sorter où on est rendu
	static std::vector<CParticle*>::size_type currentSortInteration;

public:
};



#endif
