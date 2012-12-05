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

