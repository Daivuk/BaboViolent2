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

/// \brief Module de gestion des variables de la console
///
/// \file dksvar.h
/// Ce module prend en charge la gestion des variables de la console.
/// Ceci comprend :
/// 	- des fonction d'enregistrement de variables
/// 	- une fonction de désenregistrement de variables
/// 	- une fonction de d'exécution d'un fichier de configuration
/// 	- une fonction d'exécution de commandes
///
/// Ce module est très utile pour débugger : on peut modifier le contenu d'une variables préalablement enregistrée en temps-réel lors de l'exécution du programme (habituellement via une fenêtre console). 
///
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (à des fins de documentation seulement)



#ifndef DKSVAR_H
#define DKSVAR_H


#include "CVector.h"
#include "CString.h"
#include "dksvardef.h"

class CStringInterface
{
public:
	virtual void updateString(CString* string, char * newValue) = 0;
        virtual ~CStringInterface()
        {
        }
};

// Les fonction du DKSVAR


/// \brief exécute une commande
///
/// Cette fonction permet d'exécuter une commande passée sous forme d'une chaine de caractères. Présentement, la seul commande valide est 'set' et sa syntaxe est la suivante:
/// set nomDeLaVariable param1 [param2 param3 ...]
/// L'exécution de cette commande tente de remplir les champs de la variable enregistrée nomDeLaVariable avec les paramètres param1 param2 ... fournis.
///
/// \param command chaine de caractères représentant une commande valide
CMD_RET			dksvarCommand(char * command);



/// \brief exécution d'un fichier de configuration
///
/// Cette fonction permet d'exécuter toutes les commandes contenues dans un fichier de configuration.
///
/// \param filename chemin menant au fichier de configuration à exécuter depuis l'endroit où se situe le fichier EXE du programme.
void			dksvarLoadConfig(char * filename);
void			dksvarLoadConfigSVOnly(char * filename);
void			dksvarSaveConfig(char * filename);



/// \name VariableRegistering
///
/// \brief enregistre une variable d'un certain type
/// Ce groupe de fonctions permet d'enregistrer une variable d'un certain type (bool,int,float,CVector2i,CVector2f,CVector3i,CVector3f,CVector4f,CString) en spécifiant si cette variable permet d'être modifiée par l'exécution d'un fichier de configuration.
///
/// \param screenName nom de la variable qui sera associé à la variable elle-même
/// \param defaultValue valeur par défaut de la variable
/// \param true si cette variable permet d'être modifiée par l'exécution d'un fichier de configuration, false sinon
//@{
void			dksvarRegister(const CString &screenName, bool		 *defaultValue,	bool mConfigBypass);
void			dksvarRegister(const CString &screenName, int		 *defaultValue, int minValue,
											 int maxValue, int flags, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, float		*defaultValue, float minValue,
											 float maxValue, int flags, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass);
void			dksvarRegister(const CString &screenName, CString	 *defaultValue,	bool mConfigBypass);
//@}


/// \brief désenregistre une variable enregistrée
///
/// Cette fonction permet de désenregistrer une variable enregistrée. La variable correspondant au nom fournis ne sera plus assujettie à des modifications provenant de l'exécution de commandes.
///
/// \param screenName nom de la variable associé à la variable elle-même
void			dksvarUnregister(const CString &screenName);



void			dksvarInit(CStringInterface * stringInterface);
void			dksvarGetFilteredVar(char * varName, char ** array, int size);
void			dksvarGetFormatedVar(char * varName, CString * formatedString);



#endif
