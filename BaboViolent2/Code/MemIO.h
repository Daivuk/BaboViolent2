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

#ifndef MEMIO_H
#define MEMIO_H


#include "CString.h"
#include "CVector.h"
#include "FileIO.h"


class MemIO : public FileIO
{
private:
	// Buffer
	char*			m_data;
	unsigned int	m_pos,
					m_size;


public:
	// Constructor
	MemIO(unsigned int size);

	// Destructor
	virtual ~MemIO();

	// Not implemented for MemIO, do nothing
	void putLine(CString line);
	CString getLine();
	void put(bool data);
	void put(char data);
	void put(char * data, int size);
	void put(unsigned char data);
	void put(int data);
	void put(unsigned int data);
	void put(long data);
	void put(long * data, int size);
	void put(unsigned long data);
	void put(unsigned long * data, int size);
	void put(float data);
	void put(float * data, int size);
	void put(double data);
	void put(double * data, int size);
	void put(CString data);
	void putFixedString(CString data);
	void put(CVector3f data);
	void put(CVector4f data);

	// Pour savoir si il est valide
	bool isValid() {return (m_data != 0);}

	// Reset position
	void reset() { m_pos = 0; }

	// Les get pour le binairy
	bool getBool();
	char getByte();
	char * getByteArray(int size);
	unsigned char getUByte();
	unsigned char * getUByteArray(int size);
	int getInt(); // short
	unsigned int getUInt(); // unsigned short
	long getLong();
	long * getLongArray(int size);
	unsigned long getULong();
	unsigned long * getULongArray(int size);
	float getFloat();
	float * getFloatArray(int size);
	double getDouble();
	double * getDoubleArray(int size);
	CString getString();
	CString getFixedString();
	CVector3f getVector3f();
	CVector4f getVector4f();

	// Les puts, pour écrire en binaire dans le fichier
	void put(unsigned char * data, int size);

	unsigned int getPos() { return m_pos; }
};



#endif

