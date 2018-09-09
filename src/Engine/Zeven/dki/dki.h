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

/// \brief Module de gestion des controles (clavier, souris, manette de jeu, joystick)
///
/// \file dki.h
/// Ce module prend en charge la gestion des événements système relevant de l'état de chaque touche du clavier, de la position de la souris, de chaque bouton de la souris et possiblement, la position de chaque axe d'un joystick ou d'une manette de jeu et de chaque boutons d'un joystick ou d'une manette de jeu.
/// Ceci comprend:
/// 	- une fonction d'initialisation du module
/// 	- une fonction de terminaison du module
/// 	- une fonction de mise à jour du module
/// 	- des fonctions permettant d'obtenir l'état de chaque touche, boutons, axe ou position d'un clavier, d'un joystick ou d'une souris.
///
/// \note Ce module nécessite Direct Input pour fonctionner
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKI_H
#define DKI_H


#ifdef WIN32


#include <windows.h>
#include <dinput.h>
#else
#ifdef CALLBACK
#undef CALLBACK
#endif
#define CALLBACK
#endif

#include "CVector.h"



// Les états des touches
/// \name États des touches et boutons possibles
/// Dans le cas normal, la séquence d'états pour une touche qui est appuyé pendant un certain temps sera :
/// ..., DKI_NOTHING, DKI_NOTHING,(l'événement d'enfoncement de la touche se produit ici) DKI_DOWN, DKI_HOLD, DKI_HOLD, ..., DKI_HOLD, DKI_HOLD, (l'événement de relachement de la touche se produit ici)DKI_UP, DKI_NOTHING, DKI_NOTHING,...
//@{
/// utilisée pour désigner qu'aucune touche n'a encore été pesé (voir dkiGetFirstDown())
const int DKI_NOKEY = -1;
/// utilisée pour désigner qu'une touche n'est pas appuyée
const int DKI_NOTHING = 0;
/// utilisée pour désigner qu'une touche vient d'être enfoncée. Cet état est présent seulement une fois pour toute la durée de l'enfoncement d'une touche.
const int DKI_DOWN = 1;
/// utilisée pour désigner qu'une touche est maintenue enfoncée. Cet état est présent tant que la touche reste enfoncé.
const int DKI_HOLD = 2;
/// utilisée pour désigner qu'une touche vient d'être relachée. Cet état est présent seulement une fois dès que la touche a été relachée.
const int DKI_UP = 3;
/// non utilisé
const int DKI_CLIC = 3;
/// non utilisé
const int DKI_DBL_CLIC = 4;
//@}


// Les boutons de la mouse
/// \name Constantes désignants les différents boutons d'une souris
//@{
const int DKI_MOUSE_BUTTON1 = 256;
const int DKI_MOUSE_BUTTON2 = 257;
const int DKI_MOUSE_BUTTON3 = 258;
const int DKI_MOUSE_BUTTON4 = 259;
const int DKI_MOUSE_BUTTON5 = 260;
const int DKI_MOUSE_BUTTON6 = 261;
const int DKI_MOUSE_BUTTON7 = 262;
const int DKI_MOUSE_BUTTON8 = 263;
//@}


// Les boutons du joystick
/// \name Constante désignant les différents boutons d'un joystick
/// Il y a une limite maximum de 128 buttons détectables. Pour atteindre le n ième bouton, on n'a qu'à faire : DKI_JOY_BUTTON1 + n
//@{
const int DKI_JOY_BUTTON1 = 264; // 128 Buttons. DKI_JOY_BUTTON1 + n
//@}


// Les fonction du DKI

/// \brief retourne la constante désignant la première touche ayant été appuyée
///
/// Cette fonction permet de connaitre la touche ou le bouton qui a été appuyé en premier. La fonction effectue la vérification pour un instant seulement (lors de l'appel). Un appel pour chaque cycle d'exécution est donc nécessaire afin de savoir si une touche a été pesé dans un certain intervalle de temps.
///
/// \return l'index représentant la touche ou le bouton qui a été appuyé en premier
int				dkiGetFirstDown();


/// \brief retourne la vitesse à laquelle la roulette de la souris est déplacée
///
/// Cette fonction retourne la vitesse à laquelle la roulette de la souris est déplacée en nombre de clic
/// -2 signifirait que la wheel a été décendu 2 fois. (normalement on s'en tien à -1,0 ou 1)
/// Exemple d'utilisation : if (dkiGetMouseWheelVel() < 0) zoomOut();
///
/// \return retourne la vitesse à laquelle la roulette de la souris est déplacée
int				dkiGetMouseWheelVel();



/// \brief retourne la position actuelle de la souris
///
/// Cette fonction retourne la position actuelle de la souris. Cette position est en pixel et l'origine est le coin supérieur gauche de l'écran.
///
/// \return retourne la position actuelle de la souris en pixel
CVector2i		dkiGetMouse();



/// \brief retourne la vitesse à laquelle se déplace la souris
///
/// Cette fonction retourne le déplacement effectué par la souris en pixels depuis le dernier appel a dkiUpdate().
/// Le déplacement retourné n'est pas dépendant de la grandeur de la fenêtre (il y aura un certain déplacement retourné même si on déplace la souris vers la gauche et que la dernière position était (0,0).
/// Parfait pour les jeux de style FPS
///
/// \return retourne la vitesse à laquelle se déplace la souris
CVector2i		dkiGetMouseVel();



/// \brief retourne l'état d'une touche ou d'un bouton
///
/// Cette fonction retourne l'état d'une touche ou d'un bouton.
///
/// \param inputID identifiant unique de la touche ou du bouton
/// \return état de la touche ou du bouton
int				dkiGetState(int inputID);



/// \brief retourne la position de chaque axe d'un joystick ou d'une manette de jeu
///
/// Cette fonction retourne la position de chaque axe d'un joystick ou d'une manette de jeu. La position au repos étant 0 et les extrêmes étant -1 et 1.
/// Aucune "dead zone" et courbe de progression est défini ici. C'est au client de le faire.
///
/// \return position de chaque axe
CVector3f		dkiGetJoy();
CVector3f		dkiGetJoyR();



/// \brief retourne la vitesse à laquelle se déplace chacun des axes d'un joystick ou d'une manette de jeu
///
/// Cette fonction retourne la vitesse à laquelle se déplace chacun des axes d'un joystick ou d'une manette de jeu par rapport au dernier appel à dkiUpdate().
/// Si la dernière position d'un axe était de -1 et que sa position est à 1 lors de l'appel, la valeur 2 sera retourné pour cet axe.
///
/// \return vitesse de chaque axe
CVector3f		dkiGetJoyVel();



/// \brief initialise le module pour usage
///
/// Cette fonction initialise le module afin que son utilisation puisse débuter. Cette fonction DOIT être appelé avant tout autres appels à des fonctions de ce module.
///
/// \param appInstance pointeur vers l'instance de l'application
/// \param appHandle pointeur vers le lien logique de l'application
/// \return 0 s'il y a un échec, 1 si tout s'est bien passé
int				dkiInit(HINSTANCE appInstance, HWND appHandle);



/// \brief termine l'utilisation du module
///
/// Cette fonction libère le système des ressources allouées qui étaient nécessaires au fonctionnement du module et termine sont utilisation. Cette fonction de doit pas être appelé plusieurs fois de suite : pour chaque appel à cette fonction, un appel à dkiInit() correspondant doit précédemment avoir été fait.
///
void			dkiShutDown();



/// \brief mise à jour des états des périphériques d'entrées
///
/// Cette fonction effectue la mise à jour des états des périphériques d'entrées (clavier, souris, joystick). Elle doit être appelée une fois par cycle d'exécution.
/// On doit spécifier la dimension de la fenêtre dans laquelle le pointeur de la souris ne pourra pas excéder.
///
/// \param elapsef non utilisé
/// \param width dimension en pixel de la fenêtre
/// \param height dimension en pixel de la fenêtre
void			dkiUpdate(float elapsef, int width, int height);


// Setter la position du cursor
void			dkiSetMouse(CVector2i & mousePos);




#endif
