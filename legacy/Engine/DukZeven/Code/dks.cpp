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

#ifndef WIN32
#include "linux_types.h"
#endif
#include "dksi.h"
#include <vector>
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

#ifdef USE_FMODEX
#include <fmod_errors.h>
#endif

class CSound
{
public:
	int loadedXTime;
	CString filename;
#ifdef USE_FMODEX
    FMOD_SOUND * fsound_sample;
    FMOD_CHANNEL * channel;
#else
	FSOUND_SAMPLE *fsound_sample;
#endif

public:
	CSound()
	{
		loadedXTime = 1;
		fsound_sample = 0;
#ifdef USE_FMODEX
        channel = 0;
#endif
	}

	virtual ~CSound()
	{
#ifdef USE_FMODEX
		if (fsound_sample) 
        {
            FMOD_Channel_Stop(channel);
            FMOD_Sound_Release(fsound_sample);
        }
        fsound_sample = 0;
        channel = 0;
#else
		//if (fsound_sample) 
            //FSOUND_Sample_Free(fsound_sample);
#endif
	}
};

std::vector<CSound *> sounds;

using FSOUND_STREAM = void;

#ifdef USE_FMODEX
FMOD_SYSTEM * s_system = 0;
FMOD_SOUND * stream_music = 0;
FMOD_CHANNEL * music_channel = 0;

#else
FSOUND_STREAM * stream_music = 0;
#endif


//
// Initialiser FMod
//
bool			dksInit(int mixrate, int maxsoftwarechannels)

{
#ifdef USE_FMODEX
    float ds, df, rs;
    bool r = true;
    const char * message = 0;
    unsigned int version;
    FMOD_RESULT  result;

    result = FMOD_System_Create(& s_system);
    if(result != FMOD_OK)
    {
        message = FMOD_ErrorString(result);
        goto l_abort;
    }

    result = FMOD_System_GetVersion(s_system, & version);
    if((result != FMOD_OK) || (version < FMOD_VERSION))
    {
        message = FMOD_ErrorString(result);
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        goto l_abort;
    }

    result = FMOD_System_SetOutput(s_system, 
#ifdef __MACOSX__
	FMOD_OUTPUTTYPE_AUTODETECT
#else
	FMOD_OUTPUTTYPE_ALSA
#endif
);
    if(result != FMOD_OK)
    {
        message = FMOD_ErrorString(result);
        goto l_abort;
    }
 
    result = FMOD_System_Init(s_system, maxsoftwarechannels, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
    if(result != FMOD_OK)
    {
        message = FMOD_ErrorString(result);
        goto l_abort;
    }

    result = FMOD_System_Get3DSettings(s_system, & ds, & df, & rs);
    if(result != FMOD_OK)
    {
        message = FMOD_ErrorString(result);
        goto l_abort;
    }

    df = 64.0f;
    result = FMOD_System_Set3DSettings(s_system, ds, df, rs);
    if(result != FMOD_OK)
    {
        message = FMOD_ErrorString(result);
        goto l_abort;
    }

l_abort:
    if((result != FMOD_OK) && message)
    {
        printf("FMOD error: %d (%s)\n", result, message);
        r = false;

        if(s_system)
        {
            result = FMOD_System_Close(s_system);
            if(result != FMOD_OK)
                printf("FMOD error: %d (%s)\n", result, FMOD_ErrorString(result));

            result = FMOD_System_Release(s_system);

            if(result != FMOD_OK)
                printf("FMOD error: %d (%s)\n", result, FMOD_ErrorString(result));
            s_system = 0;
        }
    }
    return r;

#else
	//if (FSOUND_Init(mixrate, maxsoftwarechannels, 0) == TRUE)
	//{
	//	FSOUND_3D_SetDistanceFactor(64.0f);
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
    return true;
#endif
}



//
// Pour deleter tout les sons puis fermer FMod
//
void			dksShutDown()
{

	dksStopMusic();
	// On efface tout nos sons
	for (int i=0;i<(int)sounds.size();i++)
	{
		CSound * sound = sounds.at(i);
		delete sound;
	}
	sounds.clear();
#ifdef USE_FMODEX
    FMOD_RESULT r = FMOD_System_Close(s_system);
    if(r != FMOD_OK)
    {
        printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));   
    }
    r = FMOD_System_Release(s_system);
    if(r != FMOD_OK)
    {
        printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));   
    }

#else
	//FSOUND_Close();
#endif
}



//
// Pour cr�er un son
//
#ifdef USE_FMODEX
FMOD_SOUND
#else
FSOUND_SAMPLE
#endif
	* dksCreateSoundFromFile(char* filename, bool loop)
{
	CSound * newSound = 0;
	std::vector<CSound *>::size_type i;


#ifdef USE_FMODEX
    if(!s_system)
        return 0;
#endif

	// On check si il n'existe pas d�j� en comparant les filename
	for (i=0;i<sounds.size();i++)
	{
		CSound * sound = sounds.at(i);
		if (sound->filename == filename) 
		{
			newSound = sound;
			newSound->loadedXTime++;
			return newSound->fsound_sample;
		}
	}

	// Si on ne l'a pas trouv� un le cr�
	if (i == sounds.size())
	{
		newSound = new CSound();
		newSound->filename = filename;

#ifdef USE_FMODEX
        FMOD_RESULT r = FMOD_System_CreateSound(s_system, filename, FMOD_SOFTWARE | (loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF), 0, & newSound->fsound_sample);
        if(r != FMOD_OK)
        {
            printf("FMOD error: %d (%s), filename: %s\n", r, FMOD_ErrorString(r), filename);
            delete newSound;
            return 0;
        }
#else
		//newSound->fsound_sample = FSOUND_Sample_Load(FSOUND_FREE, filename, (loop)?FSOUND_LOOP_NORMAL:FSOUND_LOOP_OFF, 0,0);
#endif
		sounds.push_back(newSound);
	}

	if (newSound)
		return newSound->fsound_sample;
	else
		return 0;
}



//
// Pour effacer un son
//
void dksDeleteSound(
#ifdef USE_FMODEX
    FMOD_SOUND
#else
    FSOUND_SAMPLE 
#endif 
    * fsound_sample)
{
	// On le cherche
	for (int i=0;i<(int)sounds.size();i++)
	{
		CSound * sound = sounds.at(i);

		if (sound->fsound_sample == fsound_sample)
		{
			sound->loadedXTime--;

			// Si on les a toute pogn�, on l'efface
			if (sound->loadedXTime <= 0)
			{
				delete sound;
				sounds.erase(sounds.begin() + i);
				return;
			}

			return; // Fini
		}
	}
}



//
// Pour faire jouer un son
//
int dksPlaySound(
#ifdef USE_FMODEX
    FMOD_SOUND
#else
    FSOUND_SAMPLE
#endif
    * fsound_sample, int mchannel, int volume)
{
#ifdef USE_FMODEX
    FMOD_RESULT r;
    int channel = -1;
    FMOD_MODE mode;
    const char * message = 0;
    CSound * s = 0;
    float vol = (float)volume / 255.0f;


    std::vector<CSound *>::iterator i;

    if(!s_system)
        goto l_abort;
    for(i = sounds.begin(); i != sounds.end(); i ++)
    {
        if(fsound_sample == (*i)->fsound_sample)
        {
            s = *i;
            break;
        }
    }

    r = FMOD_Sound_GetMode(fsound_sample, & mode);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }
    mode &= ~FMOD_3D;
    mode |= FMOD_2D;
    r = FMOD_Sound_SetMode(fsound_sample, mode);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }   
 
    r = FMOD_System_PlaySound(s_system, (FMOD_CHANNELINDEX) mchannel, fsound_sample, 1, & s->channel); 
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_SetVolume(s->channel, vol);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }
    r = FMOD_Channel_SetPaused(s->channel, 0);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_GetIndex(s->channel, & channel);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        channel = -1;
        goto l_abort;
    }
    
#else
	//FSOUND_Sample_SetMode(fsound_sample, FSOUND_2D);
	//int channel = FSOUND_PlaySoundEx(mchannel, fsound_sample, 0, TRUE);
	//FSOUND_SetVolume(channel, volume);
	//FSOUND_SetPaused(channel, FALSE);
#endif

#ifdef USE_FMODEX
l_abort:
    if((r != FMOD_OK) && message)
        printf("FMOD error: %d (%s)\n", r, message);
#endif
	//return channel;
    return 0;
}


//
// Pour jouer un son 3D
//
void dksPlay3DSound(
#ifdef USE_FMODEX
    FMOD_SOUND
#else
    FSOUND_SAMPLE
#endif
    * fsound_sample, int mchannel, float range, CVector3f & position, int volume)
{
#ifdef USE_FMODEX
    FMOD_RESULT r;
    FMOD_CHANNEL * ch;
    FMOD_MODE mode;
    const char * message = 0;
    std::vector<CSound *>::iterator i;
    CSound * s = 0;
    FMOD_VECTOR pos = {position.s[0], position.s[1], position.s[2]};

    float vol = (float)volume / 255.0f;



    if(!s_system)
        goto l_abort;

    for(i = sounds.begin(); i != sounds.end(); i ++)
    {
        if((*i)->fsound_sample == fsound_sample)
        {
            s = *i;
            break;
        }
    }

    r = FMOD_Sound_GetMode(fsound_sample, & mode);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    mode &= ~FMOD_2D;
    mode |= FMOD_3D;
    r = FMOD_Sound_SetMode(fsound_sample, mode);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }   
 
    r = FMOD_System_PlaySound(s_system, (FMOD_CHANNELINDEX) mchannel, fsound_sample, 1, & s->channel); 
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_Set3DMinMaxDistance(s->channel, range, 10000.0f);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_Set3DAttributes(s->channel, & pos, 0);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_SetVolume(s->channel, vol);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_SetPaused(s->channel, 0);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

l_abort:
    if((r != FMOD_OK) && message)
        printf("FMOD error: %d (%s) for %s\n", r, message, s->filename.s);
#else
 //  int channel = FSOUND_PlaySoundEx(mchannel, fsound_sample, 0, TRUE);
	//FSOUND_3D_SetMinMaxDistance(channel, range, 10000000.0f);
	//FSOUND_3D_SetAttributes(channel, position.s, 0);
	//FSOUND_SetVolume(channel, volume);
	//FSOUND_SetPaused(channel, FALSE);
#endif
}



//
// Jouer de la music
//
void			dksPlayMusic(char* filename, int mchannel, int volume)
{
	dksStopMusic();

#ifdef USE_FMODEX

    FMOD_RESULT r;
    const char * message = 0;
    float vol = (float) volume / 255.0f;

    r = FMOD_System_CreateStream(s_system, filename, FMOD_LOOP_NORMAL, 0, & stream_music);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_System_PlaySound(s_system, (FMOD_CHANNELINDEX) mchannel, stream_music, 1, & music_channel); 
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

    r = FMOD_Channel_SetVolume(music_channel, vol);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }
    
    r = FMOD_Channel_SetPaused(music_channel, 0);
    if(r != FMOD_OK)
    {
        message = FMOD_ErrorString(r);
        goto l_abort;
    }

l_abort:
    if((r != FMOD_OK) && message)
        printf("FMOD error: %d (%s)\n", r, message);    

#else
	//stream_music = FSOUND_Stream_Open(filename,FSOUND_LOOP_NORMAL,0,0);
	//if (stream_music) 
	//{
	//	int channel = FSOUND_Stream_PlayEx(mchannel, stream_music, 0, TRUE);
	//	FSOUND_SetVolume(channel, volume);
	//	FSOUND_SetPaused(channel, FALSE);
	//}
#endif
}



//
// Arr�ter la music
// 
void			dksStopMusic()
{
	if (stream_music)
	{
#ifdef USE_FMODEX
        if(!s_system) return;
        FMOD_Channel_Stop(music_channel);
        FMOD_Sound_Release(stream_music);
#else
		//FSOUND_Stream_Stop(stream_music);
		//FSOUND_Stream_Close(stream_music);
#endif
		stream_music = 0;
	}
}

#ifdef USE_FMODEX
void dksSet3DListenerAttributes(const CVector3f * pos, const CVector3f * vel, const CVector3f * forward, const CVector3f * up)
{
    FMOD_RESULT r;

    if(!s_system) return;
    
    FMOD_VECTOR v[4] = {0};
    FMOD_VECTOR * pv = v;

    if(pos)
        memcpy(pv, pos->s, sizeof(*pv));
    pv ++;

    if(vel)
        memcpy(pv, vel->s, sizeof(*pv));
    pv ++;
    
    if(forward)
        memcpy(pv, forward->s, sizeof(*pv));
    pv ++;

    if(up)
        memcpy(pv, up->s, sizeof(*pv));

    r = FMOD_System_Set3DListenerAttributes(s_system, 0, pos ? & v[0] : 0, vel ? & v[1] : 0, forward ? & v[2] : 0, up ? & v[3] : 0);
    if(r != FMOD_OK)
        printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));
}

void dksUpdate()
{
    FMOD_RESULT r;
    if(s_system)
    {
        r = FMOD_System_Update(s_system);
        if(r != FMOD_OK)
            printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));
    }
}

void dksSetSfxMasterVolume(float volume)
{
    FMOD_RESULT r;
    FMOD_CHANNELGROUP * g;

    if(!s_system) return;

    r = FMOD_System_GetMasterChannelGroup(s_system, & g);

    if(r == FMOD_OK)
        r = FMOD_ChannelGroup_SetVolume(g, volume);

    if(r != FMOD_OK)
        printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));
  
}

void dksStopSound(FMOD_SOUND * s)
{
    FMOD_RESULT r;
    std::vector<CSound *>::iterator i;
    if(!s_system) return;
    for(i = sounds.begin(); i != sounds.end(); i ++)
    {
        if(s == (*i)->fsound_sample)
        {
            r = FMOD_Channel_Stop((*i)->channel);
            if(r != FMOD_OK)
                printf("FMOD error: %d (%s)\n", r, FMOD_ErrorString(r));
            break;
        }
    }
}

//hacking
FMOD_SYSTEM * dksGetSystem()
{
    return s_system;
}

FMOD_CHANNEL * dksGetChannel(FMOD_SOUND * s)
{
    FMOD_CHANNEL * c = 0;
    std::vector<CSound *>::iterator i;

    if(!s_system) goto l_abort;

    for(i = sounds.begin(); i != sounds.end(); i ++)
    {
        if(s == (*i)->fsound_sample)
        {
            c = (*i)->channel;
            break;
        }
    }

l_abort:
    return c;
}

#endif
