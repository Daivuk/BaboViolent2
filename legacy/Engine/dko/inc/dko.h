//
// Fichier : dko.h
// Auteur : David St-Louis (alias Daivuk)
// Description : Importeur de fichier .dko (duk object)
//


#ifndef DKO_H
#define DKO_H

#ifndef WIN32
	#include "LinuxHeader.h"
#endif


// Les constantes
#define DKO_BUMP_MAP			0x0001 // Not yet
#define DKO_COLOR_ARRAY			0x0002 // Not yet
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
int				dkoAddAnimationFromFile(unsigned int modelID, char* filename, char* animationName);
void			dkoAddLight(unsigned int modelID, float *position, float *diffuse, float *specular, float range);
void			dkoBuildOctree(unsigned int modelID);
void			dkoComputeStaticLighting(unsigned int modelID);
void			dkoEnable(unsigned int renderFlags);
void			dkoDeleteModel(unsigned int *modelID);
void			dkoDisable(unsigned int renderFlags);
unsigned int	dkoGetDummy(char * dummyName, unsigned int modelID);
void			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat);
void			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat);
void			dkoGetDummyMatrix(char * dummyName, unsigned int modelID, float * mat, short frameID);
void			dkoGetDummyMatrix(unsigned int dummyID, unsigned int modelID, float * mat, short frameID);
void			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos);
void			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos);
void			dkoGetDummyPosition(char * dummyName, unsigned int modelID, float * pos, short frameID);
void			dkoGetDummyPosition(unsigned int dummyID, unsigned int modelID, float * pos, short frameID);
char*			dkoGetDummyName(unsigned int dummyID, unsigned int modelID);
char*			dkoGetLastError();
int				dkoGetNbVertex(unsigned int modelID);
void			dkoGetOABB(unsigned int modelID, float *OABB);
float			dkoGetRadius(unsigned int modelID);
short			dkoGetTotalFrame(unsigned int modelID);
float*			dkoGetVertexArray(unsigned int modelID, float frameID, int & nbVertex, float *vertexArray);
void			dkoInit();
void			dkoInitLightList(unsigned int modelID);
unsigned int	dkoLoadFile(char* filename);
unsigned int	dkoLoadFile(unsigned int modelID);
void			dkoOutputDebugInfo(unsigned int modelID, char *filename);
void			dkoPopRenderState();
void			dkoPushRenderState();
bool			dkoRayIntersection(unsigned int modelID, float *p1, float *p2, float *intersect, float *normal, int &n);
void			dkoRender(unsigned int modelID); // Va renderer automatiquement le premier frame
void			dkoRender(unsigned int modelID, unsigned short frameID); // Sp�ifions un frame
void			dkoRender(unsigned int modelID, float frameID); // Avec interpolation (MaLaDe)
void			dkoShutDown();
bool			dkoSphereIntersection(unsigned int modelID, float *p1, float *p2, float rayon, float *intersect, float *normal, int &n);


#endif