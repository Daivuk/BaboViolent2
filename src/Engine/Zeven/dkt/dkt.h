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

/// \brief Module de gestion de textures
///
/// \file dkt.h
/// Ce module prend en charge la gestion de textures (une texture étant simplement une image chargée en mémoire).
/// Ceci comprend :
/// 	- le chargement en mémoire d'un fichier targa (fichier avec extension TGA)
/// 	- la création en mémoire d'une texure
/// 	- la désallocation de la mémoire allouée pour une texture
/// 	- certaines fonctions de manipulations d'une texture
/// 	- une fonction de mise à jour des textures
/// 	- certaines fonctions permettant d'obtenir de l'information sur une texture
/// 	- une fonction permettant de libérer toute la mémoire allouée par des chargements de textures
///
/// Deux groupe de constantes sont disponibles pour spécifier les différents filtre commun de texturage et pour représenter les valeurs possibles du nombre de byte par pixel.
/// Un fichier TGA non compressé contient les informations suivantes:
/// 	- un en-tête de 12 octets (plutot inutile pour nous)
/// 	- les dimensions de l'images (2 octets pour la longueur et 2 autres pour la hauteur)
/// 	- le nombre de bit par pixel (24 ou 32, RGB ou RGBA)
/// 	- le data de l'image ((hauteur*largeur*nombre de bit par pixel/8) octets)
///
/// Les fichiers TGA valides pour ce module doivent être non compressé et ayant comme dimensions des puissances de 2 variant de 16 à 512.
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///

#ifndef DKT_H
#define DKT_H


#include "CVector.h"


/// \name Filtering
/// Les drapeaux représentants les différents filtres communs de texturage qui détermineront la facon dont une texture affichée en 3D sera transposée en pixel d'une certaine couleur.
/// Avant d'aller plus loin, je me doit d'abord expliquer ce qu'est un Mipmap. Un Mipmap est un assortiment d'images toutes générées à partir d'une image source. L'assortiment est simplement l'ensemble des différentes résolutions de l'image de base trouvées en divisant toujours la résolution de l'image source par 2.
/// Par exemple, le Mipmap d'une image source de 64X128 sera l'ensemble : 64X128, 32X64, 16X32, 8X16, 4X8, 2X4, 1X2 et 1X1 (l'image source sera 'stretcher' pour s'ajuster à chaque résolution). En utilisant cet ensemble d'image plutot que toujours l'image source, on se retrouve à avoir un gain important de performance plus l'image à être afficher se trouve loin en 3D. Le seul cout étant la mémoire nécessaire pour conserver le Mipmap entier moins l'image source.
/// 
/// Il existe 2 cas dans lequel un certain calcul est nécessaire pour déterminer la couleur qu'un certain pixel aura avant d'être afficher:
/// 	-# le pixel couvre une région égale ou plus petite qu'un texel.
/// 	-# le pixel couvre une plus grande région qu'un texel (un texel est un élément de texture)
///
/// OpenGL a défini 6 facons de calculer(ou approximer) la couleur que le pixel aura avant d'être afficher:
/// 	- GL_NEAREST : utilise le texel de l'image source le plus près du centre du pixel à être texturé (valide pour les 2 cas)
/// 	- GL_LINEAR : utilise la moyenne de la couleur des 4 texels les plus près du centre du pixel à être texturé (valide pour les 2 cas)
/// 	- GL_NEAREST_MIPMAP_NEAREST : utilise l'image du Mipmap ayant la taille qui se rapproche le plus de la taille du pixel et applique le critère GL_NEAREST avec cette image (valide seulement dans le cas 2)
/// 	- GL_LINEAR_MIPMAP_NEAREST : utilise l'image du Mipmap ayant la taille qui se rapproche le plus de la taille du pixel et applique le critère GL_LINEAR avec cette image (valide seulement dans le cas 2)
/// 	- GL_NEAREST_MIPMAP_LINEAR : utilise les 2 images du Mipmap ayant les tailles qui se rapprochent le plus de la taille du pixel et applique le critère GL_NEAREST avec l'image résultante de la moyenne des couleur des 2 images du Mipmap choisies (valide seulement dans le cas 2)
/// 	- GL_LINEAR_MIPMAP_LINEAR : utilise les 2 images du Mipmap ayant les tailles qui se rapprochent le plus de la taille du pixel et applique le critère GL_LINEAR avec l'image résultante de la moyenne des couleur des 2 images du Mipmap choisies (valide seulement dans le cas 2)
///
/// On peut noter que les Mipmap ne sont utiles que dans le cas 2.
/// De ces principes, il résulte 4 configurations les plus fréquemments utilisés:
/// 	- DKT_FILTER_NEAREST
/// 		- cas 1 : GL_NEAREST
/// 		- cas 2 : GL_NEAREST
/// 	- DKT_FILTER_LINEAR
/// 		- cas 1 : GL_LINEAR
/// 		- cas 2 : GL_LINEAR
/// 	- DKT_FILTER_BILINEAR
/// 		- cas 1 : GL_LINEAR
/// 		- cas 2 : GL_LINEAR_MIPMAP_NEAREST
/// 	- DKT_FILTER_TRILINEAR
/// 		- cas 1 : GL_LINEAR
/// 		- cas 2 : GL_LINEAR_MIPMAP_LINEAR
/// 	- par défaut (si aucun spécifier)
/// 		- cas 1 : GL_NEAREST
/// 		- cas 2 : GL_NEAREST_MIPMAP_NEAREST
///
/// Ce procédé est déterminé lors de l'application d'une texture sur une face d'un modèle ou lors de la création d'une texture à partir d'une autre. On peut donc changer de filtre tant qu'on veut avant le rendu sur l'écran ou sur un tampon mémoire.
/// Ces drapeaux peuvent être utilisé pour chaque fonctions du module qui contient un paramètre nommé filter. La combinaison de plusieurs drapeaux n'est pas permise.
//@{
const int DKT_FILTER_NEAREST = 0;
const int DKT_FILTER_LINEAR = 1;
const int DKT_FILTER_BILINEAR = 2;
const int DKT_FILTER_TRILINEAR = 3;
//@}

/// \name BytePerPixel
/// Ces drapeaux font référence à certains formats internes de pixel qu'OpenGL utilise. Ils doivent être utilisé pour chaque fonctions du module qui contient un paramètre nommé bpp ou internalFormat.
//@{
/// 1 byte par pixel qui peut représenter 256 tons de gris pour chaque pixel
const int DKT_LUMINANCE = 1;
/// 3 byte par pixel qui peuvent représenter 256 tons de rouge, de vert et de bleu pour chaque pixel
const int DKT_RGB = 3;
/// 4 byte par pixel qui peuvent représenter 256 tons de rouge, de vert, de bleu et de transparence pour chaque pixel
const int DKT_RGBA = 4;
//@}


// Les fonction du DKT


/// \brief effectue une effet de bluring sur une texture déjà chargée en mémoire
///
/// Cette fonction permet de modifier une texture chargée en mémoire en prenant la moyenne des 8 pixels adjacents pour chaque pixel de l'image. Le nombre d'itération est déterminé par nbPass.
///
/// \param textureID identifiant unique de la texture
/// \param nbPass nombre d'itération de l'effet de bluring
void			 dktBlurTexture(unsigned int textureID, int nbPass);



/// \brief permet de changer le filtre de texturage utilisée pour toute les textures présentement chargées en mémoire
///
/// Cette fonction permet de changer le filtre de texturage utilisée pour toute les textures présentement chargées en mémoire.
/// Le filtre final qui sera utilisé lors du rendu sera celui spécifié par le dernier appel fait à cette fonction (sauf si d'autre fonctions modifie aussi le filtre).
///
/// \param filter filtre de texturage à être utiliser pour toute les textures
void			 dktChangeFilter(int filter);



/// \brief crée une texture blanche
///
/// Cette texture permet de créé une texture vierge ((255,255,255,255) ou (255,255,255) ou (255) pour chaque pixel). La texture créé aura les dimension wXh, aura bpp byte par pixel et utilisera le filtre de texturage filter.
///
/// \param w largeur en pixel de la texture à créer
/// \param h hauteur en pixel de la texture à créer
/// \param bpp drapeau du format de pixel à être utilisé
/// \param filter drapeau de filtre de texturage à être utiliser
/// \return identifiant unique de la texture créée
unsigned int	 dktCreateEmptyTexture(int w, int h, int bpp, int filter);



/// \brief remplace une texture existante par une autre
///
/// Cette fonction permet de remplacer une texture existante par le contenu d'un certain tableau qui contient une liste de pixel défini proprement (de dimension wXh et de bpp byte par pixel). Un filtre de texturage peut aussi être spécifié.
///
/// \param textureID identifiant unique de la texture existante dont le contenu sera remplacé
/// \param buffer pointeur vers la liste des pixels qui défini une images de dimension wXh et de bpp byte par pixel
/// \param w largeur en pixel de la texture à créer
/// \param h  hauteur en pixel de la texture à créer
/// \param bpp drapeau du format de pixel à être utilisé
/// \param filter drapeau de filtre de texturage à être utiliser
void			 dktCreateTextureFromBuffer(unsigned int *textureID, unsigned char *buffer, int w, int h, int bpp, int filter);



/// \brief crée une texture à partir d'un fichier targa (TGA)
///
/// Cette fonction permet de créer une texture à partir d'un fichier targa (TGA) en utilisant un filtre de texturage.
///
/// \param filename chemin menant au fichier TGA à charger depuis l'endroit où se situe le fichier EXE du programme.
/// \param filter drapeau de filtre de texturage à être utiliser
/// \return identifiant unique de la texture créée
unsigned int	 dktCreateTextureFromFile(char *filename, int filter);



/// \brief libère la mémoire allouée pour une texture
///
/// Cette fonction libère la mémoire allouée pour une texture chargé précédemment.
///
/// \param textureID identifiant unique de la texture à être effacé
void			 dktDeleteTexture(unsigned int *textureID);



/// \brief obtient le nombre de byte par pixel d'une texture
///
/// Cette fonction retourne le nombre de byte par pixel d'une texture déjà chargée en mémoire.
///
/// \param textureID identifiant unique de la texture
/// \return le nombre de byte par pixel de la texture
int				 dktGetTextureBytePerPixel(unsigned int textureID);



/// \brief obtient la description de la dernière erreur encourue par ce module
///
/// Cette fonction retourne la description de la dernière erreur encourue par ce module
///
/// \return description de la dernière erreur encourue par ce module
char*			 dktGetLastError();



/// \brief obtient le tableau des pixels qui définissent une texture déjà chargée
///
/// Cette fonction permet d'obtenir le tableau des pixels qui définissent une texture déjà chargée.
///
/// \param textureID identifiant unique de la texture cible
/// \param data pointeur qui contiendra l'adresse du tableau des pixels de la texture cible
void			 dktGetTextureData(unsigned int textureID, unsigned char * data);



/// \brief obtient les dimensions d'une texture déjà chargée
///
/// Cette fonction permet d'obtenir les dimensions d'une texture déjà chargée.
///
/// \param textureID identifiant unique de la texture cible
/// \return dimension de la texture cible
CVector2i		 dktGetTextureSize(unsigned int textureID);



/// n'est plus utilisé
void			 dktInit();



/// \brief remplace une texture existante par le contenu d'une portion du framebuffer (la dernière image affichée à l'écran)
///
/// Cette fonction permet de capturer une partie de l'écran en la mettant dans une texture existante. Le rectangle de la capture de l'écran est défini en pixel par la position (x,y) et la dimension wXh (la position désignant le coin inférieur gauche du rectangle).
///
/// \param textureID identifiant unique de la texture
/// \param x coordonnée de la position du coin inférieur gauche du rectangle de capture
/// \param y coordonnée de la position du coin inférieur gauche du rectangle de capture
/// \param w dimension de la nouvelle texture (doit être une puissance de 2)
/// \param h dimension de la nouvelle texture (doit être une puissance de 2)
/// \param internalFormat drapeau du format de pixel à être utilisé pour la nouvelle texture
void			 dktRenderToTexture(unsigned int textureID, int x, int y, int w, int h, unsigned int internalFormat);



/// \brief désalloue toute la mémoire allouée par la création de textures
///
/// Cette fonction désalloué toute la mémoire allouée par la création de textures générées par l'appel de fonctions de ce module.
///
void			 dktShutDown();



/// \brief met à jour toutes les textures présentement chargées en mémoire
///
/// Cette fonction vérifie si des changements ont été apportés aux textures présentement chargée en mémoire et fait la mise à jour de leur contenu si nécessaire.
/// La vérification est faite pour une seule texture à la fois par appel. Chaque appel successif fera la vérification pour la texture suivante. Lorsqu'on atteint la dernière texture, on recommence à vérifier la première texture.
/// Ceci est particulièrement utile lorsque le module est utilisé par plus d'une application : on peut modifier la texture avec un éditeur et voir immédiatement le résultat dans le jeu par exemple.
///
void			 dktUpdate();



#endif