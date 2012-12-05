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

#ifndef ZEVEN_H
#define ZEVEN_H


#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800


#ifdef CONSOLE
	#pragma comment (lib, "dkc.lib")
	#pragma comment (lib, "dksvar.lib")
	#pragma comment (lib, "baboNet.lib")
	#pragma comment (lib, "dkolight.lib")

	#include "dko.h"
	#include "dkc.h"
	#include "dksvar.h"
	#include "baboNet.h"
	#include "cMSstruct.h"

	#include "CVector.h"
	#include "CMatrix.h"
	#include "CString.h"
#else
#ifdef _DX_
	#pragma comment (lib, "dkw.lib")
	#pragma comment (lib, "dki.lib")
	#pragma comment (lib, "dkgl_dx.lib")
	#pragma comment (lib, "dkt_dx.lib")
	#pragma comment (lib, "dkf_dx.lib")
	#pragma comment (lib, "dko_dx.lib")
	#pragma comment (lib, "dkp_dx.lib")
	#pragma comment (lib, "dks.lib")
	#pragma comment (lib, "dkc.lib")
	#pragma comment (lib, "dksvar.lib")
	#pragma comment (lib, "baboNet.lib")

	#include "dkc.h"
	#include "dkw.h"
	#include "dki.h"
	#include "dkgl.h"
	#include "dkt.h"
	#include "dkf.h"
	#include "dko.h"
	#include "dkp.h"
	#include "dks.h"
	#include "dksvar.h"
	#include "baboNet.h"
	#include "cMSstruct.h"

	#include "CVector.h"
	#include "CMatrix.h"
	#include "CString.h"
#else
	#pragma comment (lib, "dkw.lib")
	#pragma comment (lib, "dki.lib")
	#pragma comment (lib, "dkgl.lib")
	#pragma comment (lib, "dkt.lib")
	#pragma comment (lib, "dkf.lib")
	#pragma comment (lib, "dko.lib")
	#pragma comment (lib, "dkp.lib")
	#pragma comment (lib, "dks.lib")
	#pragma comment (lib, "dkc.lib")
	#pragma comment (lib, "dksvar.lib")
	#pragma comment (lib, "baboNet.lib")

	#include "dkc.h"
	#include "dkw.h"
	#include "dki.h"
	#include "dkgl.h"
	#include "dkt.h"
	#include "dkf.h"
	#include "dko.h"
	#include "dkp.h"
	#include "dks.h"
	#include "dksvar.h"
	#include "baboNet.h"
	#include "cMSstruct.h"

	#include "CVector.h"
	#include "CMatrix.h"
	#include "CString.h"
#endif
#endif


#define ZEVEN_SAFE_DELETE(a) if (a) {delete a; a = 0;}
#define ZEVEN_SAFE_DELETE_ARRAY(a) if (a) {delete [] a; a = 0;}

#define ZEVEN_SAFE_ALLOCATE(a, type, size) ZEVEN_SAFE_DELETE_ARRAY(a) a = new type [size];

#define ZEVEN_SUCCEEDED(a) (a > 0) ? true : false
#define ZEVEN_FAILED(a) (a <= 0) ? true : false

#define ZEVEN_SUCCESS 1
#define ZEVEN_FAIL 0

#ifdef WIN32
	#define ZEVEN_DELETE_VECTOR(a, cpt) for (cpt=0;cpt<(int)a.size();delete a[cpt++]); a.clear();
	#define ZEVEN_VECTOR_CALL(a, cpt, func) for (cpt=0;cpt<(int)a.size();++cpt) a[cpt]->func;
#else
	#define ZEVEN_DELETE_VECTOR(a, cpt) for (int cpt=0;cpt<(int)a.size();delete a[cpt++]); a.clear();
	#define ZEVEN_VECTOR_CALL(a, cpt, func) for (int cpt=0;cpt<(int)a.size();++cpt) a[cpt]->func;
#endif


///---- Prototype
CVector2i dkwGetCursorPos_main();


#endif

