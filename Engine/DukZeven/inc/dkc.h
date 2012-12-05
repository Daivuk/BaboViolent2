/// \brief Module de gestion du temps, de la cadence de rendu et de la cadence d'ex�ution
///
/// \file dkc.h
/// Ce module prend en charge essentiellement 3 choses :
/// 	- le temps qui s'�oule
/// 	- le temps que prend un cycle de rendu (le temps que prend une image ��re enti�ement dessin��l'�ran)
/// 	- le temps que prend un cycle d'ex�ution de mise �jour (le temps que dure un cycle d'update)
///
/// Le tout �ant essentiel afin de pr�erver un certain synchronisme peu importe l'environnement ext�ieur (un autre processus qui ralenti l'ex�ution par exemple).
/// Le module contient:
/// 	- une fonction qui initialise le module et qui doit �re appel�une seule fois et avant tout autres appels de ce module
/// 	- une fonction qui fait la mise �jour des rep�es internes du module et qui doit �re appel�une fois avant chaque image dessin� �l'�ran
/// 	- des fonctions qui permettent d'obtenir diff�entes informations concernant le temps ou le cycle de rendu ou d'ex�ution
/// 	- une fonction qui permet de changer le nombre de cycle d'ex�ution qui s'est �oul�
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (�des fins de documentation seulement)
#ifndef DKC_H
#define DKC_H


#ifdef WIN32
	#include <windows.h>
#endif

#include "platform_types.h"



// Les fonction du DKC



/// \brief retourne le temps allou�pour un cycle d'ex�ution
///
/// Cette fonction retourne en fait simplement : 1.0f / le param�re framePerSecond pass��dkcInit. Cette valeur est constante et repr�ente la dur� d'un cycle d'ex�ution de la mise �jour.
///
/// \return 1.0f / le nombre d'update d�ir�par seconde
float			dkcGetElapsedf();



/// \brief retourne le nombre d'image dessin�s �l'�ran pas seconde
///
/// Cette fonction retourne le nombre d'image dessin�s �l'�ran pas seconde. cette valeur peut grandement varier (lors d'un lag par exemple)
///
/// \return le nombre d'image dessin�s �l'�ran pas seconde
float			dkcGetFPS();



/// \brief retourne le nombre de cycles d'ex�utions de mise �jour (update) �oul� depuis un certain rep�e
///
/// Cette fonction retourne le nombre de cycles d'ex�utions de mise �jour (update) �oul� depuis le d�ut de l'ex�ution du programme entier dans le cas o aucun appel �dkcJumpToFrame() n'a ��fait.
/// Dans le cas o dkcJumpToFrame() a ��appel� l'�uation suivante pr�aut avec le dernier appel fait �dkcJumpToFrame():
/// nombre de cycles d'ex�utions de mise �jour (update) �oul� depuis le dernier appel de dkcJumpToFrame() = valeur de retour de dkcGetFrame() - param�re pass�au dernier appel de dkcJumpToFrame()
///
/// \return retourne le nombre de cycles d'ex�utions de mise �jour (update) �oul� depuis un certain rep�e
INT4			dkcGetFrame();



/// \brief initialise le module de gestion du temps
///
/// Cette fonction initialise le module de gestion du temps et doit �re appel�une seule fois et avant tout autres appels de ce module.
///
/// \param framePerSecond sp�ifie le nombre de cycle d'ex�ution de mise �jour (update) d�ir�par seconde
void			dkcInit(int framePerSecond);



/// \brief modifie le num�o du cycle d'ex�ution de mise �jour courant
///
/// Cette fonction modifie le num�o du cycle d'ex�ution de mise �jour courant.
///
/// \param frame nouveau num�o du cycle d'ex�ution de mise �jour courant
void			dkcJumpToFrame(int frame);



/// \brief met �jour les rep�es de temps et retourne le nombre de cycle d'ex�ution de mise �jour (update) ��re effectu� avant d'effectuer le prochain rendu
///
/// Cette fonction met �jour les rep�es de temps n�essaire au bon fonctionnement du module et retourne le nombre de cycle d'ex�ution de mise �jour (update) ��re effectu� avant d'effectuer le prochain rendu pour rester conforme au nombre de cycle d'ex�ution de mise �jour �abli par le param�re framePerSecond pass��la fonction dkcInit()
///
/// \return le nombre de cycle d'ex�ution de mise �jour (update) ��re effectu� 
INT4			dkcUpdateTimer();


void			dkcSleep(INT4 ms);



#endif
