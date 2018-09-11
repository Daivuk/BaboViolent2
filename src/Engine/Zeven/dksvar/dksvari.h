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


#ifndef DKSVARI_H
#define DKSVARI_H



// #ifndef DLL_EXPORTS
// #define DLL_API __declspec(dllexport)
// #else
// #define DLL_API __declspec(dllimport)
// #endif

#include "CString.h"
#include "CVector.h"
#include "dksvardef.h"

#ifdef WIN32
	#ifndef DLL_EXPORTS
		#define DLL_API(p) __declspec(dllexport) p
	#else
		#define DLL_API(p) __declspec(dllimport) p
	#endif
#else
	#include "LinuxHeader.h"
	#define DLL_API(p) p
#endif


class CStringInterface
{
public:
	virtual void updateString(CString* string, char * newValue) = 0;
};





// Les fonction du DKSVAR
DLL_API(CMD_RET)		dksvarCommand(char * command);
DLL_API(void)			dksvarLoadConfig(char * filename);
DLL_API(void)			dksvarLoadConfigSVOnly(char * filename);
DLL_API(void)			dksvarSaveConfig(char * filename);
DLL_API(void)			dksvarRegister(const CString &screenName, bool		 *defaultValue,	bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, int		 *defaultValue, int minValue,
											 int maxValue, int flags, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, float		*defaultValue, float minValue,
											 float maxValue, int flags, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CVector2i *defaultValue, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CVector2f *defaultValue, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CVector3i *defaultValue, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CVector3f *defaultValue, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CVector4f *defaultValue, bool mConfigBypass);
DLL_API(void)			dksvarRegister(const CString &screenName, CString	 *defaultValue,	bool mConfigBypass);
DLL_API(void)			dksvarUnregister(const CString &screenName);
DLL_API(void)			dksvarInit(CStringInterface * stringInterface);
DLL_API(void)			dksvarGetFilteredVar(char * varName, char ** array, int size);
DLL_API(void)			dksvarGetFormatedVar(char * varName, CString * formatedString);



#endif
