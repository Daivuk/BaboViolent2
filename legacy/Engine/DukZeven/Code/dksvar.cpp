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


#include "dksvari.h"
#include "CString.h"
#include "CSystemVariable.h"
#include <vector>
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

extern CSystemVariable systemVariable;


//
// Pour envoyer une commande console
//
CMD_RET			dksvarCommand(char * str_command)
{
	// On set une commande
	CString strCommand(str_command);

	// On prend le premier token pour savoir c'est quoi notre commande
	CString commandName = strCommand.getFirstToken(' ');

	// On check maintenant si elle contient une commande en liens avec les system variable
	return systemVariable.command(commandName, strCommand); // Elle a ��trait�
}



void			dksvarGetFormatedVar(char * varName, CString * formatedString)
{
	for (int i=0;i<(int)systemVariable.variables.size();++i)
	{
		CSVType * var = systemVariable.variables[i];
		if (strnicmp(var->variableName.s, varName, strlen(varName)) == 0)
		{
			// On pogne son nom format�avec la valeur apres
			CString formatedName = CString(varName) + " " + var->getValue();
			systemVariable.stringInterface->updateString(formatedString, formatedName.s);
			return;
		}
	}
}



void			dksvarGetFilteredVar(char * varName, char ** array, int size)
{
	int len = int(strlen(varName));
	std::vector<CString> matchs;
	int i = 0;
	for (i=0;i<(int)systemVariable.variables.size();++i)
	{
		CSVType * var = systemVariable.variables[i];
		if (strnicmp(var->variableName.s, varName, len) == 0)
		{
			// On le place en ordre alphab�ique dans la liste trouv�
			int j=0;
			for (j=0;j<(int)matchs.size();++j)
			{
				if (var->variableName < matchs[j])
				{
					break;
				}
			}
			matchs.insert(matchs.begin() + j, var->variableName);
		}
	}

	// Finalement, on fou les "size" premier dans notre array
	for (i=0;i<size;++i)
	{
		if (i < (int)matchs.size())
		{
			if (i == 0 && matchs.size() == 1)
			{
				strcpy(array[i], matchs[i].s);
			}
			else
			{
				CString varCopy = matchs[i];
				strcpy(array[i], varCopy.getFirstToken(' ').s);
			}
		}
		else
		{
			array[i][0] = '\0';
		}
	}

	matchs.clear();
}



void			dksvarInit(CStringInterface * stringInterface)
{
	systemVariable.stringInterface = stringInterface;
}



//
// Pour loader un fichier config
//
void			dksvarLoadConfig(char * filename)
{
	systemVariable.loadConfig(filename);
}

void			dksvarLoadConfigSVOnly(char * filename)
{
	systemVariable.loadConfigSVOnly(filename);
}



//
// Pour sauvegarder un fichier config
//
void			dksvarSaveConfig(char * filename)
{
	systemVariable.saveConfig(filename);
}



//
// Pour enregistrer des variables dans le system variable
//
void			dksvarRegister(const CString &screenName, bool		 *defaultValue,	bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, int		 *defaultValue, int minValue,
											 int maxValue, int flags, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, minValue, maxValue, flags, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, float	 *defaultValue, float minValue,
											 float maxValue, int flags, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, minValue, maxValue, flags, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}

void			dksvarRegister(const CString &screenName, CString	 *defaultValue,	bool mConfigBypass)
{
	systemVariable.registerSystemVariable(screenName, defaultValue, mConfigBypass);
}



//
// Pour d�enregistrer
//
void			dksvarUnregister(const CString &screenName)
{
	systemVariable.unregisterSystemVariable(screenName);
}
