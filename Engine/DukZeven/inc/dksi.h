/* TCE (c) All rights reserved */


#ifndef DKSI_H
#define DKSI_H



#ifdef WIN32
#ifndef DLL_EXPORTS
#define DKS_DLL_API __declspec(dllexport)
#else
#define DKS_DLL_API __declspec(dllimport)
#endif
#else
#define DKS_DLL_API
#endif


#ifdef WIN32
#include <windows.h>


// Link
#pragma comment( lib, "fmodvc.lib" )
#pragma comment( lib, "fmodlcc.lib" )

#endif

// Les includes pour FMod
#include <fmod.h>
#include "CString.h"
#include "CVector.h"



// Les fonction du DKS
DKS_DLL_API bool			dksInit(int mixrate, int maxsoftwarechannels);
DKS_DLL_API void			dksShutDown();


#ifdef USE_FMODEX
DKS_DLL_API FMOD_SOUND*	dksCreateSoundFromFile(char* filename, bool loop=false);
DKS_DLL_API void			dksDeleteSound(FMOD_SOUND* fsound_sample);
DKS_DLL_API int	dksPlaySound(FMOD_SOUND * fsound_sample, int channel=-1, int volume=255);
DKS_DLL_API void			dksPlay3DSound(FMOD_SOUND * fsound_sample, int channel, float range, CVector3f & position, int volume=255);
#else
DKS_DLL_API FSOUND_SAMPLE*	dksCreateSoundFromFile(char* filename, bool loop=false);
DKS_DLL_API void			dksDeleteSound(FSOUND_SAMPLE* fsound_sample);
DKS_DLL_API int			dksPlaySound(FSOUND_SAMPLE * fsound_sample, int channel=-1, int volume=255);
DKS_DLL_API void			dksPlay3DSound(FSOUND_SAMPLE * fsound_sample, int channel, float range, CVector3f & position, int volume=255);
#endif

DKS_DLL_API void			dksPlayMusic(char* filename, int channel=-1, int volume=255);
DKS_DLL_API void			dksStopMusic();

#ifdef USE_FMODEX
void dksSet3DListenerAttributes(const CVector3f * pos, const CVector3f * vel, const CVector3f * forward, const CVector3f * up);
void dksUpdate();
void dksSetSfxMasterVolume(float volume);
void dksStopSound(FMOD_SOUND * s);
FMOD_SYSTEM * dksGetSystem();
FMOD_CHANNEL * dksGetChannel(FMOD_SOUND * s);
#endif




#endif
