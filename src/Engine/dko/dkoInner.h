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

#ifndef DKOINNER_H
#define DKOINNER_H




// #ifndef DLL_EXPORTS
// #define DLL_API __declspec(dllexport)
// #else
// #define DLL_API __declspec(dllimport)
// #endif

#include "platform_types.h"

#ifdef WIN32
	#ifndef DLL_EXPORTS
		#define DLL_API(p) __declspec(dllexport) p
	#else
		#define DLL_API(p) __declspec(dllimport) p
	#endif
#else
	#define DLL_API(p) p
#endif


// Les constantes
#define DKO_BUMP_MAP			0x0001
#define DKO_COLOR_ARRAY			0x0002
#define DKO_DETAIL_MAP			0x0004
#define DKO_DYNAMIC_LIGHTING	0x0008
#define DKO_FORCE_WIREFRAME		0x0010
#define DKO_MULTIPASS			0x0020
#define DKO_SELFILL_MAP			0x0040
#define DKO_SPECULAR			0x0080
#define DKO_SPECULAR_MAP		0x0100
#define DKO_TEXTURE_MAP			0x0200
#define DKO_RENDER_BB			0x0400
#define DKO_RENDER_NODE			0x0800
#define DKO_RENDER_FACE			0x1000
#define DKO_CLAMP_TEXTURE		0x2000


// Les fonction du DKO
DLL_API(int)				dkoAddAnimationFromFile(unsigned int modelID, char* filename, char* animationName);
DLL_API(void)			dkoAddLight(unsigned int modelID, float *position, float *diffuse, float *specular, float range);
DLL_API(void)			dkoBuildOctree(unsigned int modelID);
DLL_API(void)			dkoComputeStaticLighting(unsigned int modelID);
DLL_API(void)			dkoEnable(unsigned int renderFlags);
DLL_API(void)			dkoDeleteModel(unsigned int *modelID);
DLL_API(void)			dkoDisable(unsigned int renderFlags);
DLL_API(unsigned int)	dkoGetDummy(char * dummyName, unsigned int modelID);
DLL_API(void)			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat);
DLL_API(void)			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat);
DLL_API(void)			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat, short frameID);
DLL_API(void)			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat, short frameID);
DLL_API(void)			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos);
DLL_API(void)			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos);
DLL_API(void)			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos, short frameID);
DLL_API(void)			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos, short frameID);
DLL_API(char*)			dkoGetDummyName(unsigned int dummyID, unsigned int modelID);
DLL_API(char*)		dkoGetLastError();
DLL_API(int)				dkoGetNbVertex(unsigned int modelID);
DLL_API(void)			dkoGetOABB(unsigned int modelID, float *OABB);
DLL_API(float)			dkoGetRadius(unsigned int modelID);
DLL_API(short)			dkoGetTotalFrame(unsigned int modelID);
DLL_API(float*)			dkoGetVertexArray(unsigned int modelID, float frameID, int & nbVertex, float *vertexArray);
DLL_API(void)			dkoInit();
DLL_API(void)			dkoInitLightList(unsigned int modelID);
DLL_API(unsigned int)	dkoLoadFile(char* filename);
DLL_API(unsigned int)	dkoLoadFile(unsigned int modelID);
DLL_API(void)			dkoOutputDebugInfo(unsigned int modelID, char *filename);
DLL_API(void)			dkoPopRenderState();
DLL_API(void)			dkoPushRenderState();
DLL_API(bool)			dkoRayIntersection(unsigned int modelID, float *p1, float *p2, float *intersect, float *normal, int &n);
DLL_API(void)			dkoRender(unsigned int modelID);
DLL_API(void)			dkoRender(unsigned int modelID, unsigned short frameID); // Sp�ifions un frame
DLL_API(void)			dkoRender(unsigned int modelID, float frameID); // Avec interpolation (MaLaDe)
DLL_API(void)			dkoShutDown();
DLL_API(bool)			dkoSphereIntersection(unsigned int modelID, float *p1, float *p2, float rayon, float *intersect, float *normal, int &n);

#ifdef WIN32
#ifndef DEDICATED_SERVER

#include <windows.h>
#ifndef _DX_
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"
#endif
#endif

#else
#include "LinuxHeader.h"

#ifdef __MACOSX__
#include <SDL_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif
#endif


// Directives de pr�ompilation
#include "CdkoModel.h"
#include "CdkoMaterial.h"
#ifndef DEDICATED_SERVER
#include <dkt.h>
#endif


const int MAX_MODEL	= 1024;


// Pour la pile des push/pop
struct _typBitFieldPile
{
public:
	unsigned int bitField;
	// Un pointeur vers le pr��ent dans la pile
	_typBitFieldPile *previous;

	// Constructeur
	_typBitFieldPile(_typBitFieldPile*pile, unsigned int mBitField)
	{
		bitField = mBitField;
		previous = pile;
	}
};


// Pour tenir les chunk du 3ds
struct typChunk
{
	unsigned short int ID; // Le chunk ID
	unsigned int mlength;  // La grandeur du chunk
	unsigned int bytesRead;// Le nb de byte lu jusqu'�maintenant
};


// La class static pour contenir le tout
class CDko
{
public:
	// Pour tenir la derni�e erreur
	static char *lastErrorString;

	// Le array des models
	static CDkoModel *modelArray[MAX_MODEL];

	// Si il a ��initialis�
	static bool inited;

	// Le bitMaskCourant
	static unsigned int renderStateBitField;

	// La pile pour le bitMaskField
	static _typBitFieldPile *bitFieldPile;

	// Le ID global utilis�pour incr�enter exemple pour les octrees
	static INT4 globalFrameID;

public:
	// Pour updater l'erreur
	static void updateLastError(char *error);
};



#endif
