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

#ifndef __PLATFORM_TYPES_H__
#define __PLATFORM_TYPES_H__

#ifdef WIN32
typedef long int INT4;
typedef unsigned long int UINT4;
#else
#ifdef LINUX64
typedef int INT4;
typedef unsigned int UINT4;
#else
typedef long int INT4;
typedef unsigned long int UINT4;
#endif 
#endif
#endif

