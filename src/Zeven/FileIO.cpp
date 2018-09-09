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

#include "FileIO.h"
#include <stdint.h>


//
// Constructeur
//
FileIO::FileIO(CString filename, CString how)
{
	m_file = fopen(filename.s, how.s);
}

FileIO::FileIO()
{
	m_file = 0;
}



//
// Destructeur
//
FileIO::~FileIO()
{
	if (m_file) fclose(m_file);
}


void FileIO::Open(CString filename, CString how)
{
	Close(); // Just in case
	m_file = fopen(filename.s, how.s);
}

void FileIO::Close()
{
	if (m_file)
		fclose(m_file);
	m_file = 0;
}


//
// Pour �crire normalement dans le fichier
//
void FileIO::putLine(CString line)
{
	fprintf(m_file, "%s\n", line.s);
}

CString FileIO::getLine()
{
	char buffer[160];
	fgets(buffer, 160, m_file);
	return buffer;
}



//
// Les gets binary
//
bool FileIO::getBool()
{
	return (getByte() == 1) ? true : false;
}

char FileIO::getByte()
{
	char tmp;
	fread(&tmp, 1, sizeof(char), m_file);
	return tmp;
}

char * FileIO::getByteArray(int size)
{
	char * tmp = new char [size];
	fread(tmp, 1, sizeof(char)*size, m_file);
	return tmp;
}

unsigned char FileIO::getUByte()
{
	unsigned char tmp;
	fread(&tmp, 1, sizeof(unsigned char), m_file);
	return tmp;
}

unsigned char * FileIO::getUByteArray(int size)
{
	unsigned char * tmp = new unsigned char [size];
	fread(tmp, 1, sizeof(unsigned char)*size, m_file);
	return tmp;
}

int FileIO::getInt()
{
	short tmp;
	fread(&tmp, 1, sizeof(short), m_file);
	return tmp;
}

unsigned int FileIO::getUInt()
{
	unsigned short tmp;
	fread(&tmp, 1, sizeof(unsigned short), m_file);
	return tmp;
}

int32_t FileIO::getLong()
{
	int32_t tmp;
	fread(&tmp, sizeof(tmp), 1, m_file);
	return tmp;
}

int32_t * FileIO::getLongArray(int size)
{
	int32_t * tmp = new int32_t [size];
	fread(tmp, sizeof(int32_t), size, m_file);
	return tmp;
}

uint32_t FileIO::getULong()
{
	uint32_t tmp;
	fread(&tmp, sizeof(tmp), 1, m_file);
	return tmp;
}

uint32_t * FileIO::getULongArray(int size)
{
	uint32_t * tmp = new uint32_t [size];
	fread(tmp, sizeof(uint32_t), size, m_file);
	return tmp;
}

float FileIO::getFloat()
{
	float tmp;
	fread(&tmp, 1, sizeof(float), m_file);
	return tmp;
}

float * FileIO::getFloatArray(int size)
{
	float * tmp = new float [size];
	fread(tmp, 1, sizeof(float)*size, m_file);
	return tmp;
}

double FileIO::getDouble()
{
	double tmp;
	fread(&tmp, 1, sizeof(double), m_file);
	return tmp;
}

double * FileIO::getDoubleArray(int size)
{
	double * tmp = new double [size];
	fread(tmp, 1, sizeof(double)*size, m_file);
	return tmp;
}

CString FileIO::getFixedString()
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

CString FileIO::getString()
{
	char tmp[256];
	char carac = 1;
	for (int i=0;carac!='\0';carac = tmp[i++] = getByte());
	return tmp;
}

CVector3f FileIO::getVector3f()
{
	float x,y,z;
	x = getFloat();
	y = getFloat();
	z = getFloat();
	return CVector3f(x,y,z);
}

CVector4f FileIO::getVector4f()
{
	float r,g,b,a;
	r = getFloat();
	g = getFloat();
	b = getFloat();
	a = getFloat();
	return CVector4f(r,g,b,a);
}



//
// Les puts, pour �crire en binaire dans le fichier
//
void FileIO::put(bool data)
{
	char tmp = (data) ? 1 : 0;
	fwrite(&tmp, 1, sizeof(char), m_file);
}

void FileIO::put(char data)
{
	fwrite(&data, 1, sizeof(char), m_file);
}

void FileIO::put(char * data, int size)
{
	fwrite(data, 1, sizeof(char) * size, m_file);
}

void FileIO::put(unsigned char data)
{
	fwrite(&data, 1, sizeof(unsigned char), m_file);
}

void FileIO::put(unsigned char * data, int size)
{
	fwrite(data, 1, sizeof(unsigned char) * size, m_file);
}

void FileIO::put(int data)
{
	short tmp = (short)data;
	fwrite(&tmp, 1, sizeof(short), m_file);
}

void FileIO::put(unsigned int data)
{
	unsigned short tmp = (unsigned short)data;
	fwrite(&tmp, 1, sizeof(unsigned short), m_file);
}

void FileIO::put(long data)
{
	fwrite(&data, sizeof(data), 1, m_file);
}

void FileIO::put(long * data, int size)
{
	fwrite(data, 1, sizeof(long) * size, m_file);
}

void FileIO::put(unsigned long data)
{
	fwrite(&data, 1, sizeof(unsigned long), m_file);
}

void FileIO::put(unsigned long * data, int size)
{
	fwrite(data, 1, sizeof(unsigned long) * size, m_file);
}

void FileIO::put(float data)
{
	fwrite(&data, 1, sizeof(float), m_file);
}

void FileIO::put(float * data, int size)
{
	fwrite(data, 1, sizeof(float) * size, m_file);
}

void FileIO::put(double data)
{
	fwrite(&data, 1, sizeof(double), m_file);
}

void FileIO::put(double * data, int size)
{
	fwrite(data, 1, sizeof(double) * size, m_file);
}

void FileIO::put(CString data)
{
	put(data.s, data.len() + 1);
}

void FileIO::putFixedString(CString data)
{
	// Ici c'est compliqu�, mais c'est de la faute � CSharp, qui compresse la grosseur du string wtf lol
	// Faut faire l'inverse de sque jai trouv� plus haut
	struct typ_prefixByte
	{
		unsigned int len : 7;
		unsigned int encoded : 1;

		// Constructor
		typ_prefixByte()
		{
			len = 0;
			encoded = 0;
		}
	} prefix;

	// On pogne son len d'abords
	int len = data.len();

	// On pogne seuleument les 7 premier bit (127 en decimal ;))
	prefix.len = len & 0x7F;
	len = len >> 7;
	if (len > 0) prefix.encoded = 1;
	put(*((unsigned char*)(&prefix)));

	// Tant qu'on est plus grand que 0 on l'encode
	while (len > 0)
	{
		prefix.len = len & 0x7F;
		len = len >> 7;
		if (len > 0) prefix.encoded = 1;
		else prefix.encoded = 0;
		put(*((unsigned char*)(&prefix)));
	}

	// Finalement, on put le string
	put(data.s, data.len());
}

void FileIO::put(CVector3f data)
{
	put(data[0]);
	put(data[1]);
	put(data[2]);
}

void FileIO::put(CVector4f data)
{
	put(data[0]);
	put(data[1]);
	put(data[2]);
	put(data[3]);
}

