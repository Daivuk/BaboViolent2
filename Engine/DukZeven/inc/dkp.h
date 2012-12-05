/// \brief Module de gestion de particules
///
/// \file dkp.h
/// Ce module prend en charge la gestion des particules. Ceci comprend :
/// 	- une fonction d'initialisation du module
/// 	- une fonction de terminaison d'utilisation du module
/// 	- des fonctions de création de particules
/// 	- une fonction de rendu des particules
/// 	- une fonction de mise à jour des particules
/// 	- diverses autres fonctions utilitaires
///
/// Une particule est simplement une image affichée sur un polygone qui fait toujours face à la caméra et qui se déplace, qui a une certaine durée de vie et qui peut changer d'image avec le temps ou selon d'autres circomstances.
/// Dépendant du type d'effet que l'on veut simuler avec des particules, on utilisera un mélange de couleur (blending) différent.
///
/// Le blending est la facon dont la couleur d'un certain pixel (pixel source) sera mélanger avec la couleur du pixel qui se trouve déjà dans le tampon d'image (framebuffer) à la même coordonnée (pixel destination) pour remplacer ce dernier. L'ordre dans lequel les primitives 3D sont dessinées est donc importante ici puisque que lors du rendu d'une certaine primitive, c'est la valeur présentement dans le tampon d'image (framebuffer) qui sera utilisé pour le mélange de couleur et non avec le pixel de la primitive se trouvant directement derrière (en profondeur) dans le tampon, comme on pourrait s'y attendre.
/// Chaque composante de couleur passant à une échelle de [0,1] depuis [0,255], OpenGL défini 11 configurations RGBA pour mélanger les couleurs d'un pixel source avec un pixel destination:
/// 	- GL_ZERO : (0,0,0,0)
/// 	- GL_ONE : (1,1,1,1)
/// 	- GL_SRC_COLOR : (R,G,B,A) du pixel source
/// 	- GL_ONE_MINUS_SRC_COLOR : (1-R,1-G,1-B,1-A) du pixel source
/// 	- GL_SRC_ALPHA : (A,A,A,A) du pixel source
/// 	- GL_ONE_MINUS_SRC_ALPHA : (1-A,1-A,1-A,1-A) du pixel source
/// 	- GL_DST_ALPHA : (A,A,A,A) du pixel destination
/// 	- GL_ONE_MINUS_DST_ALPHA : (1-A,1-A,1-A,1-A) du pixel destination
/// 	- GL_DST_COLOR : (R,G,B,A) du pixel destination
/// 	- GL_ONE_MINUS_DST_COLOR : (1-R,1-G,1-B,1-A) du pixel destination
/// 	- GL_SRC_ALPHA_SATURATE : (i,i,i,1) où i = la valeur la plus petite entre le A du pixel source et 1-A du pixel destination
/// 
/// Après avoir spécifier une de ces configurations pour le pixel source et pour le pixel destination, la formule suivante est utiliser pour générer la nouvelle couleur :
///
/// Rd = min ( 255 , Rs*sR + Rd*dR )
/// Gd = min ( 255 , Gs*sG + Gd*dG )
/// Bd = min ( 255 , Bs*sB + Bd*dB )
/// Ad = min ( 255 , As*sA + Ad*dA )
/// où :
/// 	- (Rd,Gd,Bd,Ad) = pixel destination (déjà présent dans le tampon d'image et qui sera mis à jour) sur une échelle de [0,255]
/// 	- (Rs,Gs,Bs,As) = pixel source sur une échelle de [0,255]
/// 	- (sR,sG,sB,sA) = coefficients de mélange du pixel source (l'une des 9 configurations possibles) sur une échelle de [0,1]
/// 	- (dR,dG,dB,dA) = coefficients de mélange du pixel destination (l'une des 8 configurations possibles)  sur une échelle de [0,1]
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)
///


#ifndef DKP_H
#define DKP_H



#include "CVector.h"


/// \name not used
/// constantes non utilisées
//@{
const int DKP_TRANS_LINEAR = 0;
const int DKP_TRANS_FASTIN = 1;
const int DKP_TRANS_FASTOUT = 2;
const int DKP_TRANS_SMOOTH = 3;
//@}


// BlendingFactorDest
/// \name BlendingFactorDest
/// Drapeaux représentant les 8 configurations possibles pour les coefficients de mélange de couleur (blending) du pixel destination
//@{
#define DKP_ZERO                           0
#define DKP_ONE                            1
#define DKP_SRC_COLOR                      0x0300
#define DKP_ONE_MINUS_SRC_COLOR            0x0301
#define DKP_SRC_ALPHA                      0x0302
#define DKP_ONE_MINUS_SRC_ALPHA            0x0303
#define DKP_DST_ALPHA                      0x0304
#define DKP_ONE_MINUS_DST_ALPHA            0x0305
//@}

// BlendingFactorSrc
/// \name BlendingFactorSrc
/// Drapeaux représentant les 9 configurations possibles pour les coefficients de mélange de couleur (blending) du pixel source
/// \note Il y a 6 drapeaux communs entre BlendingFactorDest et BlendingFactorSrc
//@{
///     DKP_ZERO
///     DKP_ONE
#define DKP_DST_COLOR                      0x0306
#define DKP_ONE_MINUS_DST_COLOR            0x0307
#define DKP_SRC_ALPHA_SATURATE             0x0308
///     DKP_SRC_ALPHA
///     DKP_ONE_MINUS_SRC_ALPHA
///     DKP_DST_ALPHA
///     DKP_ONE_MINUS_DST_ALPHA
//@}


// Struct pratique pour se créer des presets

/// \brief conteneur de configurations de particules
///
/// Cette structure permet une utilisation plus flexible de la création de particules en isolant les nombreux paramètres de création. Cette structure peut être passée à dkpCreateParticleExP().
/// Voir la définition des paramètres de dkpCreateParticleEx() pour plus de détails sur les membres de cette structure : il s'agit exactement des mêmes champs.
///
struct dkp_preset {
	CVector3f positionFrom;
	CVector3f positionTo;
	CVector3f direction;
	float speedFrom;
	float speedTo;
	float pitchFrom;
	float pitchTo;
	float startSizeFrom;
	float startSizeTo;
	float endSizeFrom;
	float endSizeTo;
	float durationFrom;
	float durationTo;
	CColor4f startColorFrom;
	CColor4f startColorTo;
	CColor4f endColorFrom;
	CColor4f endColorTo;
	float angleFrom;
	float angleTo;
	float angleSpeedFrom;
	float angleSpeedTo;
	float gravityInfluence;
	float airResistanceInfluence;
	unsigned int particleCountFrom;
	unsigned int particleCountTo;
	unsigned int *texture;
	int textureFrameCount;
	unsigned int srcBlend;
	unsigned int dstBlend;
};



// Les fonction du DKP

/// \brief non utilisée
///
/// Non utilisée
///
void			dkpCreateBillboard(	CVector3f & positionFrom,
									CVector3f & positionTo,
									float fadeSpeed,
									float fadeOutDistance,
									float size,
									CColor4f & color,
									unsigned int textureID,
									unsigned int srcBlend,
									unsigned int dstBlend);
									
									

/// \brief création d'une particule
///
/// Cette fonction permet de créer une particule par appel. Il s'agit ici d'une fonction impliquant un minimum de controle sur le comportement de la particule créé.
///
/// \param position position de départ de la particule par rapport à l'origine de la scène
/// \param vel vecteur vitesse de départ de la particule
/// \param startColor couleur de départ de la particule
/// \param endColor couleur de fin de la particule
/// \param startSize grosseur de départ de la particule
/// \param endSize grosseur de fin de la particule
/// \param duration durée de vie de la particule
/// \param gravityInfluence pourcentage d'influence de la gravité sur la particule
/// \param airResistanceInfluence coefficient de frottement de l'air sur la particule
/// \param rotationSpeed vitesse de rotation de la particule (l'axe de rotation est parallèle à la droite que forme la caméra et la particule et le sens de rotation est déterminé par le signe du nombre)
/// \param texture identifiant unique d'une texture OpenGL chargée en mémoire qui sera la partie visible de la particule
/// \param srcBlend drapeau représentant l'une des 9 configurations possibles du pixel source pour le mélange de couleur(blending) 
/// \param dstBlend drapeau représentant l'une des 8 configurations possibles du pixel destination pour le mélange de couleur(blending)
/// \param transitionFunc non utilisé (peut être toujours mis à 0)
void			dkpCreateParticle(	float *position,
									float *vel,
									float *startColor,
									float *endColor,
									float startSize,
									float endSize,
									float duration,
									float gravityInfluence,
									float airResistanceInfluence,
									float rotationSpeed,
									unsigned int texture,
									unsigned int srcBlend,
									unsigned int dstBlend,
									int transitionFunc);
									
									

/// \brief création d'une particule avec plus de controle
///
/// Cette fonction permet de créer une ou un groupe de particules avec ou sans animations par appel. Il s'agit ici d'une fonction impliquant plus de controle sur le comportement de la particule créé que la fonction dkpCreateParticle().
/// Chaque paire de paramètre dont les noms se terminent par 'From' et 'To' définissent une portée de valeurs à l'intérieur de laquelle une certaine valeur sera choisie aléatoirement.
///
/// \param positionFrom position de départ de la particule (extémité d'une boite alignée avec chaque axe du repère de la scène, la position généré aléatoirement se trouvera dans cette boite)
/// \param positionTo position de départ de la particule ( seconde extémité d'une boite alignée avec chaque axe du repère de la scène, la position généré aléatoirement se trouvera dans cette boite)
/// \param direction vecteur direction de départ de la particule (sera multiplié par 'speed' pour donner le vecteur vitesse de départ de la particule)
/// \param speedFrom vitesse de départ de la particule
/// \param speedTo vitesse de départ de la particule
/// \param pitchFrom angle de faisceau de départ (entre 0 et 360)
/// \param pitchTo angle de faisceau de départ(entre 0 et 360)
/// \param startSizeFrom grandeur de départ
/// \param startSizeTo grandeur de départ
/// \param endSizeFrom grandeur de fin (grandeur qu'aura la particule à la fin de sa durée de vie, l'interpolation est linéaire)
/// \param endSizeTo grandeur de fin (grandeur qu'aura la particule à la fin de sa durée de vie, l'interpolation est linéaire)
/// \param durationFrom durée de vie
/// \param durationTo durée de vie
/// \param startColorFrom couleur de départ
/// \param startColorTo couleur de départ
/// \param endColorFrom couleur de fin (couleur qu'aura la particule à la fin de sa durée de vie, l'interpolation est linéaire)
/// \param endColorTo couleur de fin (couleur qu'aura la particule à la fin de sa durée de vie, l'interpolation est linéaire)
/// \param angleFrom angle de départ
/// \param angleTo angle de départ
/// \param angleSpeedFrom vitesse de rotation
/// \param angleSpeedTo vitesse de rotation
/// \param gravityInfluence pourcentage d'influence de la gravité sur la particule
/// \param airResistanceInfluence coefficient de frottement de l'air sur la particule
/// \param particleCountFrom nombre de particule devant être créées
/// \param particleCountTo nombre de particule devant être créées
/// \param texture tableau d'identifiants uniques de textures OpenGL chargées en mémoire. L'ordre du tableau déterminera l'animation de la particule
/// \param textureFrameCount nombre de textures contenues dans le paramètre 'texture'. Ce nombre détermine aussi le nombre d'images constituants l'animation de la ou des particules
/// \param srcBlend drapeau représentant l'une des 9 configurations possibles du pixel source pour le mélange de couleur(blending)
/// \param dstBlend drapeau représentant l'une des 8 configurations possibles du pixel destination pour le mélange de couleur(blending)
void			dkpCreateParticleEx(CVector3f & positionFrom,
									CVector3f & positionTo,
									CVector3f & direction,
									float speedFrom,
									float speedTo,
									float pitchFrom,
									float pitchTo,
									float startSizeFrom,
									float startSizeTo,
									float endSizeFrom,
									float endSizeTo,
									float durationFrom,
									float durationTo,
									CColor4f & startColorFrom,
									CColor4f & startColorTo,
									CColor4f & endColorFrom,
									CColor4f & endColorTo,
									float angleFrom,
									float angleTo,
									float angleSpeedFrom,
									float angleSpeedTo,
									float gravityInfluence,
									float airResistanceInfluence,
									unsigned int particleCountFrom,
									unsigned int particleCountTo,
									unsigned int *texture,
									int textureFrameCount,
									unsigned int srcBlend,
									unsigned int dstBlend);



/// \brief création d'une particule avec plus de controle et de flexibilité
///
/// Cette fonction accomplie exactement la même chose que dkpCreateParticleEx() mais en utilisant la structure dkp_preset comme paramètre.
///
/// \param preset préconfiguration de la génération de particules
void			dkpCreateParticleExP(dkp_preset & preset);



/// \brief initialisation du module
///
/// Cette fonction effectue l'initialisation du module et doit être appelé AVANT tout autres appels à d'autres fonctions de ce module.
///
void			dkpInit();



/// \brief affiche toutes les particules à l'écran
///
/// Cette fonction effectue le rendu à l'ecran de toutes les particules qui ont été créées jusqu'à présent et qui sont actives.
///
void			dkpRender();




/// \brief libère la mémoire allouée pour la création de particules
///
/// Cette fonction libère toute la mémoire allouée pour la création des particules présentement actives. Toutes les particules créées seront effacées.
///
void			dkpReset();



/// \brief spécifie une densité de l'air
///
/// Cette fonction permet de changer la densité de l'air qui sera utiliser pour la simulation des particules (leurs vitesses sera décélérées proportionnellement à cette valeur)
///
/// \param airDensity nouvelle densité de l'air
void			dkpSetAirDensity(float airDensity);



/// \brief spécifie une attraction gravitationnelle
///
/// Cette fonction permet de changer l'attraction gravitationnelle qui sera utiliser pour la simulation des particules.
///
/// \param vel vecteur accélération gravitationnelle
void			dkpSetGravity(float *vel);



/// \brief active le triage des particules
///
/// Cette fonction permet d'activer ou de désactiver le triage des particules qui seront créées après l'appel. Ce triage fait en sorte que la particule la plus éloignée de la caméra sera rendue en premier, puis la suivante la plus éloignée et ainsi de suite. Ceci permet certain type de mélange de couleur (blending) de donner un effet attendu.
///
/// \param sort true pour activer le triage, false si on veut le désactiver
void			dkpSetSorting(bool sort);



/// \brief libère la mémoire allouée pour la création de particules et termine l'utilisation de ce module
///
/// Cette fonction libère la mémoire allouée pour la création de particules et termine l'utilisation de ce module. dkpInit() pourra être appelé de nouveau par la suite pour redémarrer le module.
///
void			dkpShutDown();



/// \brief effectue la mise à jour des particules pour le rendu
///
/// Cette fonction effectue la mise à jour de la position, la vitesse, la durée de vie, la couleur, l'angle, et l'image de chaque particule pour le rendu.
///
/// \param delay intervalle de temps sur lequel la mise à jour est effectuée.
/// \return le nombre de particule encore actives après l'exécution de la mise à jour
int				dkpUpdate(float delay);



#endif