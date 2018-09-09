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

#ifndef FILEIO_H
#define FILEIO_H


#include "CString.h"
#include "CVector.h"
#include "stdio.h"
#include <stdint.h>

class FileIO
{
private:
	// Pointeur sur notre structure du fichier
	FILE * m_file;

protected:
	FileIO();

public:
	// Constructeur
	FileIO(CString filename, CString how);

	// Destructeur
	virtual ~FileIO();

	// Open/Close files without creating/destroying  FileIO object
	virtual void Open(CString filename, CString how);
	virtual void Close();

	// Pour �crire normalement dans le fichier
	virtual void putLine(CString line);
	virtual CString getLine();

	// Pour savoir si il est valide
	virtual bool isValid() {return (m_file != 0);}

	// Les get pour le binairy
	virtual bool getBool();
	virtual char getByte();
	virtual char * getByteArray(int size);
	virtual unsigned char getUByte();
	virtual unsigned char * getUByteArray(int size);
	virtual int getInt(); // short
	virtual unsigned int getUInt(); // unsigned short
	virtual int32_t getLong();
	virtual int32_t * getLongArray(int size);
	virtual uint32_t getULong();
	virtual uint32_t * getULongArray(int size);
	virtual float getFloat();
	virtual float * getFloatArray(int size);
	virtual double getDouble();
	virtual double * getDoubleArray(int size);
	virtual CString getString();
	virtual CString getFixedString();
	virtual CVector3f getVector3f();
	virtual CVector4f getVector4f();

	// Les puts, pour �crire en binaire dans le fichier
	virtual void put(bool data);
	virtual void put(char data);
	virtual void put(char * data, int size);
	virtual void put(unsigned char data);
	virtual void put(unsigned char * data, int size);
	virtual void put(int data);
	virtual void put(unsigned int data);
	virtual void put(long data);
	virtual void put(long * data, int size);
	virtual void put(unsigned long data);
	virtual void put(unsigned long * data, int size);
	virtual void put(float data);
	virtual void put(float * data, int size);
	virtual void put(double data);
	virtual void put(double * data, int size);
	virtual void put(CString data);
	virtual void putFixedString(CString data);
	virtual void put(CVector3f data);
	virtual void put(CVector4f data);
};



#endif

