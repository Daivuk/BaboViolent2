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

/// \brief Module de gestion des sons
///
/// \file dks.h
/// Ce module prend en charge la gestion des sons.
/// Ceci comprend :
/// 	- une fonction de chargement d'un son
/// 	- une fonction de destruction d'un son
/// 	- une fonction de destruction de tout les sons présentement chargées et qui termine l'utilisation du module
/// 	- une fonction d'initialisation du module
///
/// Se référer à la documentation de FMOD pour connaitre les fonctions disponibles pour faire jouer les sons et musiques chargées.
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKS_H
#define DKS_H


#ifdef WIN32
#pragma comment( lib, "dks.lib" )


// Link
#pragma comment( lib, "fmodvc.lib" )
#pragma comment( lib, "fmodlcc.lib" )
#endif


// Les includes pour Fmod
#include <fmod.h>
#include "CVector.h"


// Les fonction du DKS


/// \brief initialise le module et FMOD
/// 
/// Cette fonction effectue l'initialisation du module et de FMOD. Ceci est fait en spécifiant un mixrate et un nombre maximal de cannaux audio. Cette onction DOIT être appelée avant tout autres appels à d'autres fonctions de ce module.
///
/// \param mixrate mixrate
/// \param maxsoftwarechannels nombre de cannaux audio
/// \return true si l'initialisation a réussi, false sinon
bool			dksInit(int mixrate, int maxsoftwarechannels);



/// \brief libère toute la mémoire allouée pour les sons et termine l'utilisation du module.
///
/// Cette fonction libère toute la mémoire allouée pour les sons et termine l'utilisation du module. Le module pourra être redémarré avec un nouvel appel à dksInit()
///
void			dksShutDown();



/// \brief chargement d'un son en mémoire
///
/// Cette fonction effectue le chargement d'un son ou d'une musique en mémoire. Les formats valides sont les même que FMOD. Avant de créer un nouveau son, le module vérifie si ce fichier a déjà été chargé. Si c'est le cas, aucun son ne sera créé et le pointeur FMOD correspondant à ce fichier sera retourné.
///
/// \param filename chemin menant au fichier son ou musique à charger depuis l'endroit où se situe le fichier EXE du programme.
/// \param loop est-ce que ce son ou cette musique doit jouer en boucle?
/// \return pointeur FMOD vers le son ou la musique chargée en mémoire
#ifdef USE_FMODEX
FMOD_SOUND *	dksCreateSoundFromFile(char* filename, bool loop=false);
#else
FSOUND_SAMPLE*	dksCreateSoundFromFile(char* filename, bool loop=false);
#endif



/// \brief destruction d'un son chargé en mémoire
///
/// Cette fonction permet de libérer la mémoire allouée pour un son présentement chargée en mémoire.
///
/// \param pointeur FMOD du son à effacer

#ifdef USE_FMODEX
void dksDeleteSound(FMOD_SOUND * fsound_sample);
#else
void			dksDeleteSound(FSOUND_SAMPLE* fsound_sample);
#endif


// Ajou de fonctions
#ifdef USE_FMODEX
int	dksPlaySound(FMOD_SOUND * fsound_sample, int channel, int volume=255);
void dksPlay3DSound(FMOD_SOUND * fsound_sample, int channel, float range, CVector3f & position, int volume=255);
#else
int			dksPlaySound(FSOUND_SAMPLE * fsound_sample, int channel, int volume=255);
void			dksPlay3DSound(FSOUND_SAMPLE * fsound_sample, int channel, float range, CVector3f & position, int volume=255);
#endif

void			dksPlayMusic(char* filename, int channel=-1, int volume=255);
void			dksStopMusic();

#ifdef USE_FMODEX
void dksSet3DListenerAttributes(const CVector3f * pos, const CVector3f * vel, const CVector3f * forward, const CVector3f * up);
void dksUpdate();
void dksSetSfxMasterVolume(float volume);
void dksStopSound(FMOD_SOUND * s);
FMOD_SYSTEM * dksGetSystem();
FMOD_CHANNEL * dksGetChannel(FMOD_SOUND * s);
#endif


#endif
