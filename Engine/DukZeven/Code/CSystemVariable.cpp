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


#include "CSystemVariable.h"
#include "dksvari.h"



// Notre objet
CSystemVariable systemVariable;


//
// Constructeur / Destructeurs
//
CSystemVariable::CSystemVariable()
{
	stringInterface = 0;
}

CSystemVariable::~CSystemVariable()
{
	// On effaces les variables
	for (int i=0;i<(int)variables.size();i++)
	{
		CSVType *svType = variables.at(i);
		if (svType) delete svType;
	}
	variables.clear();
}




bool CSVString::setValue(const CString & paramsT){
	CString params = paramsT;
	params.trim(' ');
	params.trim('\"');

	// On call la string interface, sinon on ne peut PAS modifier le string
	if (systemVariable.stringInterface)
        systemVariable.stringInterface->updateString(value, params.s);
	else return false;

	return true;
}



//
// On load un fichier config
//
void CSystemVariable::loadConfig(char * filename)
{
	ifstream ficIn(filename, ios::in);

	if (ficIn.fail())
	{
		// On imprime message à la console? (pkoi pas, on est dedans :P)
		return;
	}

	while (!ficIn.eof())
	{
		char variable[256];
		ficIn >> variable;
		if ((variable[0] == '/') && (variable[1] == '/'))
		{
			ficIn.ignore(512, '\n');
			continue;
		}

		// On check à quel variable ça correspond
		for (int i=0;i<(int)variables.size();i++)
		{
			CSVType *svType = variables.at(i);
			CString varNameTmp = svType->variableName;
			if (varNameTmp.getFirstToken(' ') == variable)
			{
				svType->loadConfig(ficIn);
				break;
			}
		}

		// On flush le reste de la ligne
		ficIn.ignore(512, '\n');
	}
	ficIn.close();
}

void CSystemVariable::loadConfigSVOnly(char * filename)
{
	ifstream ficIn(filename, ios::in);

	if (ficIn.fail())
	{
		// On imprime message à la console? (pkoi pas, on est dedans :P)
		return;
	}

	while (!ficIn.eof())
	{
		char variable[256];
		ficIn >> variable;
		if ((variable[0] == '/') && (variable[1] == '/'))
		{
			ficIn.ignore(512, '\n');
			continue;
		}

		// On check à quel variable ça correspond
		for (int i=0;i<(int)variables.size();i++)
		{
			CSVType *svType = variables.at(i);
			CString varNameTmp = svType->variableName;
			if (varNameTmp.getFirstToken(' ') == variable)
			{
				if (strnicmp(variable, "sv_", 3) == 0)
				{
					svType->loadConfig(ficIn);
					break;
				}
				else
				{
					ficIn.ignore(512, '\n');
					continue;
				}
			}
		}

		// On flush le reste de la ligne
		ficIn.ignore(512, '\n');
	}
	ficIn.close();
}



//
// Pour saver un fichier config contenant la prédéfinition des variables
//
void CSystemVariable::saveConfig(char * filename)
{
	FILE * fic = fopen(filename, "wb");
	if (!fic) return;
	fclose(fic);

	ofstream ficOut(filename, ios::in);

	if (ficOut.fail())
	{
		// On imprime message à la console? (pkoi pas, on est dedans :P)
		return;
	}

	ficOut << "// Modifying this file can cause game crashes\n";
	ficOut << "// If you have corrupted something, just delete this file and run the game again\n";

	for (int i=0;i<(int)variables.size();i++)
	{
		ficOut << endl;
		CSVType *svType = variables.at(i);

		CString varNameTmp = svType->variableName;
		ficOut <<varNameTmp.getFirstToken(' ').s << " ";
		svType->saveConfig(ficOut);
		ficOut << endl;
	}

/*		char variable[256];
		ficIn >> variable;
		if (variable[0] == '/' && variable[0] == '/')
		{
			ficIn.ignore(512, '\n');
			continue;
		}

		// On check à quel variable ça correspond
		for (int i=0;i<variables.size();i++)
		{
			CSVType *svType = variables.at(i);
			if (svType->variableName == variable)
			{
				svType->loadConfig(ficIn);
				continue;
			}
		}

		// On flush le reste de la ligne
		ficIn.ignore(512, '\n');
		*/
	ficOut.close();
}



//
// Pour effacer une variable du stack
//
void CSystemVariable::unregisterSystemVariable(const CString &screenName)
{
	for (int i=0;i<(int)variables.size();i++)
	{
		CSVType *svType = variables.at(i);
		CString varCopy = svType->variableName;
		if (varCopy.getFirstToken(' ') == screenName)
		{
			variables.erase(variables.begin() + i);
			delete svType;
			return;
		}
	}
}



//
// Pour gèrer les commandes pour modifier les variables
//
CMD_RET CSystemVariable::command(CString & commandName, CString & params)
{
	if (commandName == "set")
	{
		// On prend le nom de la variable
		CString varName = params.getFirstToken(' ');
		varName.trim('\"'); // Au cas où, y en a des épais tse

		// On loop les variables pour savoir si elle existe et on lui transfer ses params
		for (int i=0;i<(int)variables.size();i++)
		{
			CSVType *svType = variables.at(i);
			CString varCopy = svType->variableName;
			if (varCopy.getFirstToken(' ') == varName)
			{
				if (svType->setValue(params)) return CR_OK;
				else
				{
					// Oups, un ti message d'erreur. Mauvais nombre de param probablement
					return CR_INVALIDARGS;
				}
			}
		}

		// Oups un ti message d'erreur pour dire que la variable n'existe pas
		return CR_NOSUCHVAR;
	}

	// Aucune commande n'a été trouvé ici
	return CR_NOTSUPPORTED;
}



//
// Pour enregistrer les variable
//
void CSystemVariable::registerSystemVariable(const CString &screenName, bool *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVBool(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, int *defaultValue, int minValue,
											 int maxValue, int flags, bool mConfigBypass)
{
	CSVType *newType = new CSVInt(screenName, defaultValue, minValue, maxValue, flags, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, float *defaultValue, float minValue,
											 float maxValue, int flags, bool mConfigBypass)
{
	CSVType *newType = new CSVFloat(screenName, defaultValue, minValue, maxValue, flags, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVVector2i(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVVector2f(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVVector3i(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVVector3f(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVVector4f(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}

void CSystemVariable::registerSystemVariable(const CString &screenName, CString *defaultValue, bool mConfigBypass)
{
	CSVType *newType = new CSVString(screenName, defaultValue, mConfigBypass);
	variables.push_back(newType);
}
