/// \brief Module de gestion des polices de caractères
///
/// \file dkf.h
/// Ce module prend en charge la gestion des polices de caractères créées à partir d'ume image TGA.
/// Ceci comprend :
/// 	- une fonction de chargement de police de caractères
/// 	- une fonction de destruction de polices de caractères
/// 	- des fonctions retournant diverses informations sur une chaine de caractère selon la police choisie
/// 	- une fonction de sélection de la police à utiliser
/// 	- une fonction de destruction de toutes les polices de caractères présentement chargées
/// 	- une fonction permettant de dessiner une chaine de caractère sur un QUAD (polygone a 4 coté) en 3D d'une certaine taille et avec une certaine couleur
///
/// Les couleurs peuvent être spécifiées à même la chaine de caractères. Voici la liste des couleurs disponibles et leur caractère correspondant (la couleur sera utilisé pour les caractère suivant) :
/// 	- \x1 = texte bleu
/// 	- \x2 = texte vert
/// 	- \x3 = texte cyan
/// 	- \x4 = texte rouge
/// 	- \x5 = texte magenta
/// 	- \x6 = texte brun
/// 	- \x7 = texte gris clair
/// 	- \x8 = texte gris foncé
/// 	- \x9 = texte jaune
///
/// De plus, le caractère retour de chariot (\n) peut être utilisé afin de pouvoir écrire sur plus d'une ligne avec un seul appel à dkfPrint().
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKF_H
#define DKF_H


#include "CVector.h"



// Les fonction du DKT

/// \brief spécifie la police de caractère à devant être utilisé
///
/// Cette fonction permet de spécifier la police de caractères qui sera active. La police de caractères active est celle qui sera utilisé pour l'affichage d'une chaine de caractères et pour divers calculs de longueurs de chaines.
/// Si on désire changer de police, un nouvel appel spécifiant la nouvelle police de caractère devant être active devra être fait.
///
/// \param ID identifiant unique de la police de caractères
void			dkfBindFont(unsigned int ID);



/// \brief crée une police de caractères
///
/// Cette fonction crée une police de caractères à partir d'une image TGA valide. Pour être considérer valide, une image TGA doit respecter certaine règle:
/// 	- doit être de dimension 512x512 pixels
/// 	- doit représenter un caractère dans une zone de 32x64 pixels maximum (afin de disposer d'assez d'aire pour 128 caractères)
/// 	- un masque alpha doit délimiter verticalement chaque caractère avec au moins une ligne de 64 pixels de couleur (0,0,0) (ceci veut aussi dire que le dessin d'un caractère ne doit pas être divisé en deux par une ligne verticale de 64 pixels de noir)
/// 	- les caractères représentées doivent être ceux entre le code ASCII 32 et 160 exclusivement et dans l'ordre
/// 	- le premier caractère (33) doit être représenté à partir du coin supérieur gauche de l'image, les caractères suivants étant successivement représentés vers la droite et ensuite de nouveau à partir de gauche lorsqu'on atteint la limite de l'image
///
/// \param filename chemin menant au fichier image TGA depuis l'endroit où se situe le fichier EXE du programme à partir de laquelle une police de caractère sera créé
/// \return identifiant unique référant la nouvelle police de caractère
unsigned int	dkfCreateFont(char *filename);



/// \brief destruction d'une police de caractères
///
/// Cette fonction libère la mémoire allouée dans la création d'une police de caractères
///
/// \param ID pointeur vers l'identifiant unique référant la police de caractères devant être effacée
void			dkfDeleteFont(unsigned int *ID);



/// \brief obtient la position d'un caractère dans une chaine de caractère
///
/// Cette fonction obtient la position de la première occurence d'un caractère qui se trouve possiblement dans une chaine de caractère.
/// La police de caractères active est utilisé pour évaluer la largeur de chaque caractère. Les caractères \n sont aussi considérés dans le calcul. Ce qui veut dire que la hauteur du caractère peut ne pas être nulle.
///
/// \param size hauteur en pixel de la chaine de caractères
/// \param text chaine de caractères à considérer
/// \param caracter caractère dont la position sera retourner
/// \return position de la première occurence du caractère dans la chaine de caractères à considérer
CPoint2f		dkfGetCaracterPos(float size, char *text, int caracter);



/// \brief obtient l'index du caractère se trouvant à une certaine position dans une chaine de caractères
///
/// Cette fonction retourne le nombre désignant le n ième caractère d'une chaine de caractères qui se trouve à la position onStringPos.
///
/// \param size hauteur en pixel de la chaine de caractères
/// \param text chaine de caractères à considérer
/// \param onStringPos position cible en pixel dont l'origine se trouve dans le coin supérieur gauche du premier caractère
/// \return le n ième caractère d'une chaine de caractères qui se trouve à la position onStringPos
int				dkfGetOverStringCaracter(float size, char *text, CPoint2f & onStringPos);



/// \brief retourne la hauteur total d'une chaine de caractères en considérant les caractères \n
///
/// Cette fonction retourne la hauteur total en pixel d'une chaine de caractères en considérant les caractères \n.
///
/// \param size hauteur en pixel d'un caractère
/// \param text chaine de caractères à considérer
/// \return hauteur total en pixels
float			dkfGetStringHeight(float size, char *text);



/// \brief retourne la largeur de la plus grande sous-chaine d'une chaine de caractères délimitées par les caractères \n
///
/// Cette fonction retourne la largeur de la plus grande sous-chaine d'une chaine de caractères délimitées par les caractères \n
///
/// \param size hauteur en pixel d'un caractère
/// \param text chaine de caractères à considérer
/// \return largeur de la plus grande sous-chaine
float			dkfGetStringWidth(float size, char *text);



/// \brief dessine une chaine de caractères
///
/// Cette fonction dessine une chaine de caractères sur un QUAD (polygone à 4 coté) en 3D en utilisant la police de caractères active, les caractères de couleurs et le caractères retour de chariot (\n).
///
/// \param size grandeur du texte à dessiner en pourcentage par rapport à la hauteur originale (l'aggrandissement est valide pour la largeur et la hauteur du texte)
/// \param x position du texte en 3D
/// \param y position du texte en 3D
/// \param z position du texte en 3D
/// \param text chaine de caractères à dessiner à l'écran
void			dkfPrint(float size, float x, float y, float z, char *text);



/// \brief destruction de toutes les polices de caractères présentement chargées en mémoire
///
/// Cette fonction libère toute la mémoire allouée pour toutes les polices de caractères présentement chargées.
///
void			dkfShutDown();




#endif