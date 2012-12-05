/// \brief Module de gestion d'OpenGL
///
/// \file dkgl.h
/// Ce module prend en charge la gestion d'un contexte OpenGL et offre plusieurs fonctions utilitaires lui étant rattaché. Ceci comprend :
/// 	- une fonction de création d'un contexte OpenGL
/// 	- une fonction de terminaison d'utilisation du module
/// 	- une fonction de permettant de vérifier le support d'extensions d'OpenGL
/// 	- des fonctions permettant de changer la façon dont l'environnement 3D est transféré sur une surface 2D (vue de perspective ou orthogonale)
/// 	- diverses autres fonctions utilitaires
///
/// Le contexte OpenGL est une structure de donnée utilisé par OpenGL qui contient une instance de cette librairie graphique.
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKGL_H
#define DKGL_H

#ifdef WIN32
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )


#include <windows.h>

// Les includes pour opengl
#include <gl/gl.h> 
#include <gl/glu.h> 
#include "glext.h"
#else
#include "linux_types.h"
#include "LinuxHeader.h"
#ifdef __MACOSX__
#include <SDL_opengl.h> 
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif
#endif

#include "CVector.h"


/// \name BlendingPreset
/// Drapeaux représentant chacun une configuration de coefficients de mélange de couleurs d'un pixel source et d'un pixel destination fréquemment utilisées. Voir DKP pour plus de détails sur le mélange de couleur (blending)
//@{
/// représente la paire de coefficients (source, destination) : (GL_ONE, GL_ONE)
const int DKGL_BLENDING_ADD_SATURATE = 0;
/// représente la paire de coefficients (source, destination) : (GL_SRC_ALPHA, GL_ONE)
const int DKGL_BLENDING_ADD = 3;
/// représente la paire de coefficients (source, destination) : (GL_DST_COLOR, GL_ZERO)
const int DKGL_BLENDING_MULTIPLY = 1;
/// représente la paire de coefficients (source, destination) : (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
const int DKGL_BLENDING_ALPHA = 2;
//@}


// Les fonction du DKG


/// \brief vérifie la présence d'une extension d'OpenGL supportée par la carte vidéo
///
/// Cette fonction vérifie la présence d'une extension d'OpenGL supportée par la carte vidéo.
///
/// \param extension nom de l'extension
/// \return true si elle est supportée, false sinon
bool			dkglCheckExtension(char * extension);



/// \brief crée un contexte OpenGL
///
/// Cette fonction crée un contexte OpenGL essentiel au rendu. Elle doit être appelé avant tout autres appels à des fonctions de ce module ou d'OpenGL.
///
/// \param mDC Device Context de la fenêtre Windows
/// \param colorDepth nombre de bit utiliser pour chaque composant de couleur d'un pixel (16 ou 32.....donc 32)
/// \return true si la création du contexte a réussi, false sinon
int				dkglCreateContext(HDC mDC, int colorDepth);



/// \brief dessine le repère vectoriel de la scène à l'origine
///
/// Cette fonction dessine le repère vectoriel de la scène à l'origine. Le repère vectoriel est constitué de 3 vecteurs tous perpendiculaire l'un par rapport à l'autre.
///
void			dkglDrawCoordSystem();



/// \brief dessine un cube en fil de fer
///
/// Cette fonction dessine un cube en fil de fer.
///
void			dkglDrawWireCube();



/// \brief permet de revenir en mode de rendu en perspective
///
/// Cette fonction de passer du mode de rendu orthographique (surtout utilisé pour dessiner en 2D sur l'écran ou pour conserver le rapport des mesures comme dans les applications CAD) au mode de rendu en perspective 3D.
///
void			dkglPopOrtho();



/// \brief permet de passer en mode de rendu orthographique
///
/// Cette fonction passer du mode de rendu en perspective 3D au mode de rendu orthographique possédant une certaine dimension de rendu.
///
/// \param mWidth dimension en pixel du mode de rendu orthographique
/// \param mHeight dimension en pixel du mode de rendu orthographique
void			dkglPushOrtho(float mWidth, float mHeight);



/// \brief permet de spécifier la fonction de mélange de couleur qui est active
///
/// Cette fonction permet de spécifier la fonction de mélange de couleur(blending) qui est active en passant un des 4 drapeaux BlendingPreset en paramètre
///
/// \param blending drapeau BlendingPreset qui défini une fonction de mélange de couleur(blending)
void			dkglSetBlendingFunc(int blending);


/// enable vsync ( true or false )
void			dkglEnableVsync(bool enabled = true);


/// \brief active une lumière OpenGL
///
/// Cette fonction active une des 8 lumières qu'OpenGL offre avec les spécificités suivantes :
/// 	- position = {x,y,z,1}
/// 	- couleur ambiente = {r/4,g/4,b/4,1}
/// 	- couleur diffuse = {r,g,b,1}
/// 	- couleur spéculaire = {r,g,b,1}
///
/// \param ID Identifiant unique de la lumière (de 0 à 7)
/// \param x position de la lumière
/// \param y position de la lumière
/// \param z position de la lumière
/// \param r couleur de la lumière
/// \param g couleur de la lumière
/// \param b couleur de la lumière
void			dkglSetPointLight(int ID, float x, float y, float z, float r, float g, float b);



/// \brief spécifie et active un mode de rendu en perspective
///
/// Cette fonction spécifie et active un mode de rendu en perspective.
///
/// \param mFieldOfView angle de vue vertical en degrés
/// \param mNear distance la plus proche de la caméra pouvant posséder un rendu
/// \param mFar distance la plus éloignée de la caméra pouvant posséder un rendu
/// \param mWidth largeur de la fenètre (unité arbitraire, seul le ratio mWidth/mHeight importe vraiment)
/// \param mHeight hauteur de la fenètre (unité arbitraire)
void			dkglSetProjection(float mFieldOfView, float mNear, float mFar, float mWidth, float mHeight);



/// \brief détruit le contexte OpenGL précédemment créé
///
/// Cette fonction détruit le contexte OpenGL précédemment créé. Après l'appel, un nouvel appel à dkglCreateContext() devra être fait avant tout autres appels de fonctions de ce module ainsi que de fonctions provenant d'OpenGL.
///
void			dkglShutDown();



/// \brief transforme la position de la souris 2D en un vecteur 3D
///
/// Cette fonction permet de faire correspondre la position de la souris et une certaine valeur entre [0,1] à un vecteur 3D. Le vecteur est simplement construit en prenant la position de la souris et en y ajoutant zRange (z,y,zRange) et en y soustrayant la position de la caméra.
/// On obtient alors un vecteur que l'on multiplie par la valeur de profondeur la plus éloigné de la caméra pouvant posséder un rendu. C'est ce nouveau vecteur qui est retourné.
///
/// \param pos2D position de la souris à l'écran en pixel
/// \param zRange profondeur désirée (entre 0 et 1)
/// \return le nouveau vecteur représentant correspondant à la position de la souris en 3D à une certaine profondeur.
CVector3f		dkglUnProject(CVector2i & pos2D, float zRange);

CVector3f		dkglProject(CVector3f & pos3D);


#endif
