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

/// \brief Module de gestion d'une fenêtre Windows
///
/// \file dkw.h
/// Ce module prend en charge la gestion d'une fenêtre Windows. Ceci comprend :
/// 	- une fonction d'initialisation du module et d'une fenêtre
/// 	- une fonction de terminaison d'utilisation du module
/// 	- des fonctions permettant d'obtenir diverses informations sur la fenêtre et sur le module
/// 	- une fonction de destruction de la fenêtre
/// 	- une fonction de traitement des messages système dans une boucle (le mainloop du programme)
/// 	- une fonction de traitement des messages système pouvant être appelé de l'intérieur du programme
/// 	- une petite classe contenant 2 fonctions virtuel pure devant être définie qui devront être passé au module et qui seront appelé par celui-ci aux moments opportuns
///
/// Je me doit ici d'expliquer le concept général d'exécution du programme. Ce module utilise la programmation événementielle présente dans Windows pour gérer les différents message système.
/// La fonction accomplissant cette tâche est une fonction 'callback' qui est appelé par le système sur réception d'un message provenant de ce dernier. Selon le message reçu, une certaine action ou appel de fonction sera effectué.
/// Mais avant d'appeler la fonction 'callback', on doit vérifier si des événements systèmes sont générés et si oui, les rediriger vers notre fonction 'callback'. Pour ce faire, on appel dkwMainLoop() qui verifée constamment si un message système a été reçu et qui le redirige vers notre 'callback' s'il y en a un. Cette vérification s'arrête lorsque le message système WM_QUIT est reçu (ceci est un signal de fermeture de l'application).
/// Dans le cas où le programme passe beaucoup de temps dans une fonction, il devient alors possible de rater des messages systèmes. On peut alors utiliser dkwUpdate() qui effectue une vérification et renvoie d'un message système par appel.
/// Voici maintenant les différents messages systèmes gérés par le module:
/// 	- WM_CLOSE : message système désignant l'état de fermeture de la fenêtre. Suite à ce message, un message WM_QUIT sera généré par le module
/// 	- WM_PAINT : message système désignant qu'aucun autre message plus prioritaire est présent et que l'on peut effectuer un cycle d'exécution du programme et le module appel paint() (paint() inclu habituellement un certain nombre d'appel successif à update() et un seul appel à render())
/// 	- WM_SIZE : message système désignant qu'il vient d'y avoir une modification de la taille de la fenêtre, le module prend en compte les changement automatiquement
/// 	- WM_CHAR : message système désignant qu'un caractère vient d'être tappé au clavier, le module utilisera la fonction textWrite() pour traiter le caractêre tappé
/// 	- WM_KEYDOWN : message système désignant qu'une touche du clavier est appuyée, ce message est ignoré étant donné la présence de la librairie DKI qui gère toutes les entrées de clavier
/// 	- WM_KEYUP : message système désignant qu'une touche du clavier est relachée, ce message est ignoré étant donné la présence de la librairie DKI qui gère toutes les entrées de clavier
/// 	- WM_QUIT : message système désignant la fermeture du programme. Ce message n'est pas géré directement dans la fonction 'callback' mais plutot dans dkwMainLoop(). C'est lorsque ce que message est reçu que dkwMainLoop() retourne.
/// 
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKW_H
#define DKW_H


#include "platform_types.h"

#ifdef WIN32
#include <windows.h>
#else
#include "LinuxHeader.h"
#endif

#include "CVector.h"



/// \brief Interface principale entre le module et le code externe
///
/// Cette classe est l'interface principale entre le module et le code externe. Elle ne contient que 2 fonctions virtuelles pures devant être redéfinies dans une classe-dérivé.
/// Ces 2 fonctions seront automatiquement appelées par la fonction 'callback'. Une instance d'une classe dérivée de celle-ci devra être passée en paramètre à dkwInit() afin de générer une fenêtre Windows valide et active.
///
/// \note : Il est possible de ne pas dériver cette classe abstraite et de simplement passer 0 en paramètre à dkwInit(). Ceci permettra quand même à l'application de s'exécuter sans toutefois générer de rendu ni de mise à jour de la logique d'affaire.
class CMainLoopInterface
{
public:
	
	
	/// \brief fonction essentielle qui doit inclure l'aspect Vue et Logique d'affaire de l'application
	///
	/// Cette fonction doit être redéfinie de façon à gérer toutes les opérations de rendu et toute la logique du jeu. Elle sera appelé automatiquement par la fonction 'callback' à chaque fois qu'un message système WM_PAINT sera reçu. Cet action marquera le début d'un cycle d'exécution du programme.
	///
	virtual void paint() = 0;
	
	
	
	/// \brief fonction qui donne un moyen de gèrer les caractères entrée au clavier
	///
	/// Cette fonction doit être redéfinie mais cette redéfinition peut simplement être vide si un autre moyen a été utilisé pour gérer les entrées de caractères (avec le module DKI par exemple)
	///
	/// \param caracter caractère entré au clavier passé depuis la fonction 'callback' à l'appel
	virtual void textWrite(unsigned int caracter) = 0;
};


// Les fonction du DKW



/// \brief initialise le module et génère une fenêtre Windows
///
/// Cette fonction initialise le module afin que son utilisation puisse débuter. Cette fonction DOIT être appelé UNE SEULE FOIS pour toute la durée du programme et avant tout autres appels à des fonctions de ce module.
/// 
///
/// \param width dimension en pixel de la fenêtre souhaitée
/// \param height dimension en pixel de la fenêtre souhaitée
/// \param colorDepth nombre de bit utiliser pour chaque composant de couleur d'un pixel (16 ou 32.....donc 32)
/// \param title chaine de caractères définissant le nom que portera la fenêtre créée
/// \param mMainLoopObject pointeur vers une classe dérivée de la classe CMainLoopInterface qui contient la définition de la fonction principale du programme (Paint())
/// \param fullScreen booléen qui détermine si la fenêtre doit être en mode plein écran(true) ou en mode fenêtre(false)
/// \return int retourne 1 si la création de la fenêtre s'est bien déroulé, retourne 0 si une erreur s'est produite
int				dkwInit(int width, int height, int colorDepth, char *title, CMainLoopInterface *mMainLoopObject, bool fullScreen, int refreshRate = -1);



/// \brief envoie un signal de fermeture du programme et de la fenêtre
///
/// Cette fonction envoie un signal de fermeture du programme et de la fenêtre en générant un message système WM_QUIT
///
void			dkwForceQuit();



/// \brief retourne le Device Context de la fenêtre
///
/// Cette fonction retourne le Device Context de la fenêtre créée par le module. Le Device Context est une structure de donnée Windows qui représente l'aspect visuel d'une fenêtre.
///
/// \return Device Context de la fenêtre
HDC				dkwGetDC();



/// \brief retourne le Handle de la fenêtre
///
/// Cette fonction retourne le Handle de la fenêtre créée par le module. Le Handle est une structure de donnée Windows qui représente la fenêtre elle-même.
///
/// \return Handle de la fenêtre
HWND			dkwGetHandle();



/// \brief retourne l'instance de l'application
///
/// Cette fonction retourne l'instance de l'application. Celle-ci est la structure de donnée utiliser par Windows pour distinguer plusieurs applications.
///
/// \return instance de l'application
HINSTANCE		dkwGetInstance();



/// \brief obtient la description de la dernière erreur encourue par ce module
///
/// Cette fonction retourne la description de la dernière erreur encourue par ce module
///
/// \return description de la dernière erreur encourue par ce module
char*			dkwGetLastError();



/// \brief retourne la position de la souris
///
/// Cette fonction retourne la position de la souris en pixel par rapport au coin supérieur gauche de la fenêtre.
///
/// \return position de la souris
CVector2i		dkwGetCursorPos();



/// \brief retourne la résolution actuelle de la fenêtre
///
/// Cette fonction retourne la résolution actuelle de la fenêtre en pixel.
///
/// \return résolution actuelle de la fenêtre
CVector2i		dkwGetResolution();



/// \brief boucle principale du programme
///
/// Cette fonction est la boucle principale du programme. Elle ne doit être appelé qu'une fois pour toute la durée de l'exécution du programme. L'exécution de cette fonction ne se terminera que lorsque le message système WM_QUIT aura été reçu.
///
/// \return 0 si l'exécution s'est déroulée normalement, retourne 1 sinon
#ifdef WIN32
int				dkwMainLoop();
#else
int dkwMainLoop(bool *);
#endif



/// \brief terminaison d'utilisation du module
///
/// Cette fonction termine l'utilisation du module et libère la mémoire allouée pour la gestion des messages d'erreurs.
///
void			dkwShutDown();



/// \brief vérifie si un message système a été reçu
///
/// Cette fonction vérifie si un message système a été reçu et le redirige vers la fonction 'callback' si c'est le cas. Cette fonction est utile si l'on veut garder un contact avec l'environnement d'exécution pendant un long cycle d'exécution de la boucle principale.
///
void			dkwUpdate();


/// \brief Enable/Disable mouse clipping
///
/// Cette fonction active ou desactive le clipping de la souris
///

void			dkwClipMouse( bool abEnabled );

#ifndef WIN32
void dkwGetMouseState(DIMOUSESTATE2 * aMouseState);
void dkwGetKeysState(unsigned char * aState, int aSize);
#endif


#endif
