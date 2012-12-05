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

#include "MemIO.h"



//
// Constructeur
//
MemIO::MemIO(unsigned int size): m_pos(0), m_size(size)
{
	m_data = new char[m_size];
}



//
// Destructeur
//
MemIO::~MemIO()
{
	delete[] m_data;
}



//
// Not implemented for MemIO
//
void MemIO::putLine(CString line) {}
CString MemIO::getLine() { return ""; }



//
// Les gets binary
//
bool MemIO::getBool()
{
	return (getByte() == 1) ? true : false;
}

char MemIO::getByte()
{
	char tmp = m_data[m_pos]; 
	m_pos++;
	return tmp;
}

char * MemIO::getByteArray(int size)
{
	char * tmp = new char [size];
	memcpy(tmp,&m_data[m_pos], size);
	m_pos += size;
	return tmp;
}

unsigned char MemIO::getUByte()
{
	unsigned char tmp = (unsigned char)m_data[m_pos];
	m_pos++;
	return tmp;
}

unsigned char * MemIO::getUByteArray(int size)
{
	unsigned char * tmp = new unsigned char [size];
	memcpy(tmp,&m_data[m_pos], size);
	m_pos += size;
	return tmp;
}

int MemIO::getInt()
{
	short tmp;
	memcpy(&tmp,&m_data[m_pos], sizeof(short));
	m_pos += sizeof(short);
	return tmp;
}

unsigned int MemIO::getUInt()
{
	unsigned short tmp;
	memcpy(&tmp,&m_data[m_pos], sizeof(unsigned short));
	m_pos += sizeof(unsigned short);
	return tmp;
}

long MemIO::getLong()
{
	long tmp;
	memcpy(&tmp,&m_data[m_pos], sizeof(long));
	m_pos += sizeof(long);
	return tmp;
}

long * MemIO::getLongArray(int size)
{
	long * tmp = new long [size];
	memcpy(tmp,&m_data[m_pos], size*sizeof(long));
	m_pos += size*sizeof(long);
	return tmp;
}

unsigned long MemIO::getULong()
{
	unsigned long tmp;
	memcpy(&tmp, &m_data[m_pos], sizeof(unsigned long));
	m_pos += sizeof(unsigned long);
	return tmp;
}

unsigned long * MemIO::getULongArray(int size)
{
	unsigned long * tmp = new unsigned long [size];
	memcpy(tmp,&m_data[m_pos], size*sizeof(unsigned long));
	m_pos += size*sizeof(unsigned long);
	return tmp;
}

float MemIO::getFloat()
{
	float tmp;
	memcpy(&tmp,&m_data[m_pos], sizeof(float));
	m_pos += sizeof(float);
	return tmp;
}

float * MemIO::getFloatArray(int size)
{
	float * tmp = new float [size];
	memcpy(tmp,&m_data[m_pos], size*sizeof(float));
	m_pos += size*sizeof(float);
	return tmp;
}

double MemIO::getDouble()
{
	double tmp;
	memcpy(&tmp,&m_data[m_pos], sizeof(double));
	m_pos += sizeof(double);
	return tmp;
}

double * MemIO::getDoubleArray(int size)
{
	double * tmp = new double [size];
	memcpy(tmp,&m_data[m_pos], size*sizeof(double));
	m_pos += size*sizeof(double);
	return tmp;
}

CString MemIO::getFixedString()
{
	struct typ_prefixByte
	{
		unsigned int len : 7;
		unsigned int encoded : 1;
	};

	unsigned long finalLen = 0;

	unsigned char result = getUByte();
	typ_prefixByte prefix = *((typ_prefixByte*)(&result));

	finalLen = finalLen | (unsigned long)prefix.len;

	int bitshift = 0;

	while (prefix.encoded)
	{
		bitshift += 7;
		result = getUByte();
		prefix = *((typ_prefixByte*)(&result));
		finalLen = finalLen | ((unsigned long)prefix.len) << bitshift;
	}

	char * string = getByteArray(finalLen);
	char * tmpString = new char [finalLen + 1];
	memcpy(tmpString, string, finalLen);
	tmpString[finalLen] = '\0';
	CString finalString(tmpString);
	delete [] tmpString;
	delete [] string;

	return finalString;
}

CString MemIO::getString()
{
	char tmp[256];
	char carac = 1;
	for (int i=0;carac!='\0';carac = tmp[i++] = getByte());
	return tmp;
}

CVector3f MemIO::getVector3f()
{
	float x,y,z;
	x = getFloat();
	y = getFloat();
	z = getFloat();
	return CVector3f(x,y,z);
}

CVector4f MemIO::getVector4f()
{
	float r,g,b,a;
	r = getFloat();
	g = getFloat();
	b = getFloat();
	a = getFloat();
	return CVector4f(r,g,b,a);
}



//
// Les puts, pour écrire en binaire dans le fichier
//
void MemIO::put(bool data) {}

void MemIO::put(char data) {}

void MemIO::put(char * data, int size)
{
	// Expand by 50% if limit reached
	if(m_pos + size > m_size) {
		unsigned int newsize = m_size+m_size/2;
		char* tmp = new char[newsize];
		memcpy(tmp, m_data, m_size);
		delete[] m_data;
		m_data = tmp;
		m_size = newsize;

	}
	memcpy(m_data+m_pos,data,size);
	m_pos += size;
}

void MemIO::put(unsigned char data) {}

void MemIO::put(unsigned char * data, int size) {}

void MemIO::put(int data) {}

void MemIO::put(unsigned int data) {}

void MemIO::put(long data) {}

void MemIO::put(long * data, int size) {}

void MemIO::put(unsigned long data) {}

void MemIO::put(unsigned long * data, int size) {}

void MemIO::put(float data) {}

void MemIO::put(float * data, int size) {}

void MemIO::put(double data) {}

void MemIO::put(double * data, int size) {}

void MemIO::put(CString data) {}

void MemIO::putFixedString(CString data) {}

void MemIO::put(CVector3f data) {}

void MemIO::put(CVector4f data) {}

