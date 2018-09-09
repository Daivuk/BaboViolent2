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

#ifndef CCHECKSUMQUERY_H_INCLUDED
#define CCHECKSUMQUERY_H_INCLUDED

#include "md5_2.h"
#include "Console.h"


//
// --- The server can queue the queries he asked to a certain client
//

class CChecksumQuery
{
private:

	float	m_elapsed;

	// id of the player this checksum query is associated to
	unsigned long m_id;
	unsigned long m_bbnetId;

	short m_s1,m_s2,m_s3,m_s4;

public:

	unsigned long GetBBid() { return m_bbnetId; }
	unsigned long GetID() { return m_id; }

	CChecksumQuery( unsigned long in_id, unsigned long in_bbnetId )
	{
		m_id = in_id;
		m_bbnetId = in_bbnetId;
		m_elapsed = 0;

		// generate random shorts
		m_s1 = (short)(rand() % 60000) + 10;
		m_s2 = (short)(rand() % 60000) + 10;
		m_s3 = (short)(rand() % 60000) + 10;
		m_s4 = (short)(rand() % 60000) + 10;

		// send the checksum request!
		net_svcl_hash_seed hashSeed;
		hashSeed.s1 = m_s1;
		hashSeed.s2 = m_s2;
		hashSeed.s3 = m_s3;
		hashSeed.s4 = m_s4;

		bb_serverSend((char*)&hashSeed, sizeof(net_svcl_hash_seed), NET_SVCL_HASH_SEED, m_bbnetId);
		
	}

	~CChecksumQuery()
	{
	}

	bool isValid( net_svcl_hash_seed & in_hash )
	{
        #ifdef LINUX64
        return true;
        #else
		int output[4];
		int result = md5_file("./bv2.exe", (unsigned char*)&output);

		//console->add(CString("\x03> MD5 Hash1 : %i",output[0]));
		//console->add(CString("\x03> MD5 Hash2 : %i",output[1]));
		//console->add(CString("\x03> MD5 Hash3 : %i",output[2]));
		//console->add(CString("\x03> MD5 Hash4 : %i",output[3]));

		int hashedOutput[4];
		hashedOutput[0] = output[0] ^ (int)m_s1;
		hashedOutput[1] = output[1] ^ (int)m_s2;
		hashedOutput[2] = output[2] ^ (int)m_s3;
		hashedOutput[3] = output[3] ^ (int)m_s4;

		//console->add(CString("\x03> MD5 HashedOutput1 : %i",hashedOutput[0]));
		//console->add(CString("\x03> MD5 HashedOutput2 : %i",hashedOutput[1]));
		//console->add(CString("\x03> MD5 HashedOutput3 : %i",hashedOutput[2]));
		//console->add(CString("\x03> MD5 HashedOutput4 : %i",hashedOutput[3]));

		//console->add(CString("\x03> MD5 client : %i",in_hash.s1));
		//console->add(CString("\x03> MD5 client : %i",in_hash.s2));
		//console->add(CString("\x03> MD5 client : %i",in_hash.s3));
		//console->add(CString("\x03> MD5 client : %i",in_hash.s4));

		// check if what our client is telling us is good
		if( (short)hashedOutput[0] == in_hash.s1
				&& (short)hashedOutput[1] == in_hash.s2
					&& (short)hashedOutput[2] == in_hash.s3
						&& (short)hashedOutput[3] == in_hash.s4
		  )
		{
			// the hash is good
			return true;
		}
		return false;
        #endif
	}

	int Update( const float & in_Delta )
	{
		m_elapsed += in_Delta;
		if( m_elapsed > 10.0f )
		{
			return 0;
		}
		return 1;
	}

};


#endif