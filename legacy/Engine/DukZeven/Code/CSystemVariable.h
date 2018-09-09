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


/*

  Legende pour les variables

  v = video
  r = rendering
  t = timing
  sv = server
  cl = client
  s = sound
  i = input
  ui = user interface
  g = game

  */



#ifndef CSYSTEMVARIABLE_H
#define CSYSTEMVARIABLE_H



#include <vector>
#include "CVector.h"
#include "CString.h"
#include <fstream>
#include "dksvardef.h"

using namespace std;


class CStringInterface;

class CSVType
{
public:
	// Son nom à être affiché
	CString variableName;

	// Est-ce que cette variable peut être bypassé par le fichier config
	bool configBypass;

public:
	// Pour loader d'un fichier config
	virtual void loadConfig(ifstream & ficIn) = 0;

	// Pour saver d'un fichier config
	virtual void saveConfig(ofstream & ficOut) = 0;

	// Pour setter sa valeur avec les params d'un string
	virtual bool setValue(const CString & paramsT) = 0;

	// Pour obtenir la valeur en string
	virtual CString getValue() = 0;
};

// les types de variables
class CSVBool : public CSVType
{
public:
	bool *value;
	CSVBool(const CString &screenName, bool *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		char tmp[16];
		ficIn >> tmp;
		if (configBypass)
		{
			if (stricmp(tmp, "false") == 0)
			{
				*value = false;
			}
			else if (stricmp(tmp, "true") == 0)
			{
				*value = true;
			}
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		if (*value) ficOut << "true";
		else ficOut << "false";
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		param1.trim('\"');
		if (param1 == "false") *value = false;
		else if (param1 == "true") *value = true;
		else return false; // Petit m'essage d'erreur
		return true;
	}
	CString getValue()
	{
		if (*value == true) return "true";
		else return "false";
	}
};
class CSVInt : public CSVType
{
public:
	int *value;
	CSVInt(const CString &screenName, int *defaultValue, int minValue, int maxValue, int flags,
		bool mConfigBypass): minV(minValue), maxV(maxValue), defaultV(*defaultValue), limitsFlags(flags)
	{
		variableName=screenName;
		value=defaultValue;
		configBypass=mConfigBypass;
	}
	void loadConfig(ifstream & ficIn){
		int tmp;
		ficIn >> tmp;
		if (configBypass)
		{
			if ( ( (limitsFlags & LIMIT_MIN) && (tmp < minV) ) ||
				( (limitsFlags & LIMIT_MAX) && (tmp > maxV) ) )
				*value = defaultV;
			else
				*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << *value;
	}
	bool setValue(const CString & paramsT)
	{
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		param1.trim('\"');
		int tmp = param1.toInt();
		if ( ( (limitsFlags & LIMIT_MIN) && (tmp < minV) ) ||
			( (limitsFlags & LIMIT_MAX) && (tmp > maxV) ) )
			return false;
		*value = tmp;
		return true;
	}
	CString getValue()
	{
		return CString("%i", *value);
	}
private:
	const int minV, maxV, defaultV, limitsFlags;
private:
	// To prevent generation of assignement operator
	CSVInt & operator= (const CSVInt &);
};
class CSVFloat : public CSVType
{
public:
	float *value;
	CSVFloat(const CString &screenName, float *defaultValue, float minValue, float maxValue, int flags,
		bool mConfigBypass): minV(minValue), maxV(maxValue), defaultV(*defaultValue), limitsFlags(flags)
	{
		variableName=screenName;
		value=defaultValue;
		configBypass=mConfigBypass;
	}
	void loadConfig(ifstream & ficIn)
	{
		float tmp;
		ficIn >> tmp;
		if (configBypass)
		{
			if ( ( (limitsFlags & LIMIT_MIN) && (tmp < minV) ) ||
				( (limitsFlags & LIMIT_MAX) && (tmp > maxV) ) )
				*value = defaultV;
			else
				*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << *value;
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		param1.trim('\"');
		float tmp = param1.toFloat();
		if ( ( (limitsFlags & LIMIT_MIN) && (tmp < minV) ) ||
			( (limitsFlags & LIMIT_MAX) && (tmp > maxV) ) )
			return false;
		*value = tmp;
		return true;
	}
	CString getValue()
	{
		return CString("%f", *value);
	}
private:
	const float minV, maxV, defaultV;
	const int limitsFlags;
private:
	// To prevent generation of assignement operator
	CSVFloat & operator= (const CSVFloat &);
};
class CSVVector2i : public CSVType
{
public:
	CVector2i *value;
	CSVVector2i(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		CVector2i tmp;
		ficIn >> tmp[0];
		ficIn >> tmp[1];
		if (configBypass)
		{
			*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << (*value)[0] << " ";
		ficOut << (*value)[1];
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		CString param2 = params.getFirstToken(' ');
		param1.trim('\"');
		param2.trim('\"');
		*value = CVector2i(param1.toInt(), param2.toInt());
		return true;
	}
	CString getValue()
	{
		return CString("%i %i", (*value)[0], (*value)[1]);
	}
};
class CSVVector2f : public CSVType
{
public:
	CVector2f *value;
	CSVVector2f(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		CVector2f tmp;
		ficIn >> tmp[0];
		ficIn >> tmp[1];
		if (configBypass)
		{
			*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << (*value)[0] << " ";
		ficOut << (*value)[1];
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		CString param2 = params.getFirstToken(' ');
		param1.trim('\"');
		param2.trim('\"');
		*value = CVector2f(param1.toFloat(), param2.toFloat());
		return true;
	}
	CString getValue()
	{
		return CString("%f %f", (*value)[0], (*value)[1]);
	}
};
class CSVVector3i : public CSVType
{
public:
	CVector3i *value;
	CSVVector3i(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		CVector3i tmp;
		ficIn >> tmp[0];
		ficIn >> tmp[1];
		ficIn >> tmp[2];
		if (configBypass)
		{
			*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << (*value)[0] << " ";
		ficOut << (*value)[1] << " ";
		ficOut << (*value)[2];
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		CString param2 = params.getFirstToken(' ');
		CString param3 = params.getFirstToken(' ');
		param1.trim('\"');
		param2.trim('\"');
		param3.trim('\"');
		*value = CVector3i(param1.toInt(), param2.toInt(), param3.toInt());
		return true;
	}
	CString getValue()
	{
		return CString("%i %i %i", (*value)[0], (*value)[1], (*value)[2]);
	}
};
class CSVVector3f : public CSVType
{
public:
	CVector3f *value;
	CSVVector3f(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		CVector3f tmp;
		ficIn >> tmp[0];
		ficIn >> tmp[1];
		ficIn >> tmp[2];
		if (configBypass)
		{
			*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << (*value)[0] << " ";
		ficOut << (*value)[1] << " ";
		ficOut << (*value)[2];
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		CString param2 = params.getFirstToken(' ');
		CString param3 = params.getFirstToken(' ');
		param1.trim('\"');
		param2.trim('\"');
		param3.trim('\"');
		*value = CVector3f(param1.toFloat(), param2.toFloat(), param3.toFloat());
		return true;
	}
	CString getValue()
	{
		return CString("%f %f %f", (*value)[0], (*value)[1], (*value)[2]);
	}
};
class CSVVector4f : public CSVType
{
public:
	CVector4f *value;
	CSVVector4f(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn){
		CVector4f tmp;
		ficIn >> tmp[0];
		ficIn >> tmp[1];
		ficIn >> tmp[2];
		ficIn >> tmp[3];
		if (configBypass)
		{
			*value = tmp;
		}
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << (*value)[0] << " ";
		ficOut << (*value)[1] << " ";
		ficOut << (*value)[2] << " ";
		ficOut << (*value)[3];
	}
	bool setValue(const CString & paramsT){
		CString params = paramsT;
		CString param1 = params.getFirstToken(' ');
		CString param2 = params.getFirstToken(' ');
		CString param3 = params.getFirstToken(' ');
		CString param4 = params.getFirstToken(' ');
		param1.trim('\"');
		param2.trim('\"');
		param3.trim('\"');
		param4.trim('\"');
		*value = CVector4f(param1.toFloat(), param2.toFloat(), param3.toFloat(), param4.toFloat());
		return true;
	}
	CString getValue()
	{
		return CString("%f %f %f %f", (*value)[0], (*value)[1], (*value)[2], (*value)[3]);
	}
};
class CSVString : public CSVType
{
public:
	CSVString(const CSVString& svString)
	{
		value = svString.value;
		variableName = svString.variableName;
		configBypass = svString.configBypass;
	}
	CString *value;
	CSVString(const CString &screenName, CString *defaultValue, bool mConfigBypass) {
		variableName=screenName;value=defaultValue;configBypass=mConfigBypass;}
	void loadConfig(ifstream & ficIn)
    {
		char tmp[256];
		ficIn.getline(tmp, 256);
        CString s(tmp);
		setValue(s);
	}
	void saveConfig(ofstream & ficOut)
	{
		ficOut << "\"" << value->s << "\"";
	}
	bool setValue(const CString & params);
	CString getValue()
	{
		return CString("\"%s\"", value->s);
	}
};




class CSystemVariable
{
public:
	// La listes des variables enregistré
	std::vector<CSVType*> variables;

public:
	// Constructeur / Destructeur
	CSystemVariable(); virtual ~CSystemVariable();

	CStringInterface * stringInterface;

public:
	// Nous pouvons enregistrer les nouvelles variables comme bon nous semble avec cette fonction
	void registerSystemVariable(const CString &screenName, bool		 *defaultValue,	bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, int		 *defaultValue, int minValue,
											 int maxValue, int flags, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, float	 *defaultValue, float minValue,
											 float maxValue, int flags, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass);
	void registerSystemVariable(const CString &screenName, CString	 *defaultValue,	bool mConfigBypass);

	// Pour effacer une variable du stack
	void unregisterSystemVariable(const CString &screenName);

	// pour loader un fichier config contenant la prédéfinition des variables
	void loadConfig(char * filename);
	void loadConfigSVOnly(char * filename);

	// Pour saver un fichier config contenant la prédéfinition des variables
	void saveConfig(char * filename);

	// Pour gèrer les commandes pour modifier les variables
	CMD_RET command(CString & commandName, CString & params);
};


#endif
