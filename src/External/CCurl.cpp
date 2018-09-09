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

#include "CCurl.h"
#include "Console.h"
#include "md5.h"
#include <sstream>
#include <fstream>
#include <algorithm>
using std::min;
using std::max;

// Could be changed to dynamically allocate
// but for now this is enough for 40 friends
// with full info
// changed to 65536 since it's used for other things that require bigger buffer (cnik)
const int CCurl::s_maxResponse = 65536;

// For sending xml via postdata
std::string base64_encode(std::string in);



CCurl::CCurl(CString url, std::string data): m_handle(0), m_data(data), m_url(url), m_recieved(0), buffer(0)
{
}


CCurl::~CCurl()
{
	if (m_handle != 0)
	{
		curl_easy_cleanup(m_handle);
	}
	if (buffer) delete[] buffer;
}

void CCurl::execute(void* pArg)
{
	// First, create a handle
	m_handle = curl_easy_init();

	// Ensure handle was created and response string provided
	if(m_handle == 0)
		return;

	//--- Timeout in seconds for connection
	curl_easy_setopt(m_handle, CURLOPT_CONNECTTIMEOUT, 3);

	//--- Timeout for whole connection+transfer, thus must be > connect timeout
	curl_easy_setopt(m_handle, CURLOPT_TIMEOUT, 5);

	// Create response buffer
	CCurl::SResponse r = {"", 0};
//	char* buffer = new char[s_maxResponse+1];
	if (buffer) delete [] buffer;
	buffer = new char[s_maxResponse+1];
	r.data = buffer;

	// Set curl settings
	curl_easy_setopt(m_handle, CURLOPT_URL, m_url.s);
	curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL, true);
	curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, m_data.c_str());
	curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, CCurl::write_data);
	curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &r); 

	// Send to server
	curl_easy_perform(m_handle);

	// Check response size
	m_recieved = (int)(r.size);
	if(m_recieved > 0)
	{
		buffer[m_recieved] = '\0';
		m_response = buffer;
	}

	// Clean up
	if (buffer) delete[] buffer;
	buffer = 0;

	return;
}

size_t CCurl::write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	if(userp != 0)
	{
		SResponse& r = *((SResponse*)userp);
		size_t can_handle = max(min(static_cast<int>(CCurl::s_maxResponse-r.size), static_cast<int>(size*nmemb)),0);
		memcpy(r.data + r.size, buffer, can_handle);
		r.size += can_handle;
		return can_handle;
	}
	else
		return 0;
}


CUrlData::CUrlData() { m_data = ""; }
CUrlData::~CUrlData() {}

void CUrlData::add(CString key, std::string value, int flags /* = CUrlData::NONE */)
{
	if(m_data.size() > 0)
		m_data += "&";

	std::stringstream ss;
	ss << key.s << "=";

	if(flags == MD5)
	{
		RSA::MD5 md5((unsigned char*)value.c_str());
		char * hex_digest = md5.hex_digest();
		ss << hex_digest;
		delete[] hex_digest;
	}
	else if(flags == BASE64)
	{
		console->add("\x2>Base64 Encoding Data", true);
		std::string b64 = base64_encode(value);
		console->add("\x2>Escaping Data", true);
#ifdef WIN32
		char* enc = curl_easy_escape(0, b64.c_str(), (int)b64.size());
#else
		char* enc = curl_escape(b64.c_str(), (int)b64.size());
#endif
		ss << enc;
		console->add("\x2>Releasing Temporary Memory", true);
		curl_free(enc);
	}
	else
	{
		ss << value;
	}
	
	m_data += ss.str();
}

/*void CUrlData::add(CString key, CString value, int flags /* = CUrlData::NONE *///)
/*{
	if(m_data.size() > 0)
		m_data += "&";

	if(flags == CUrlData::MD5)
	{
		RSA::MD5 md5((unsigned char*)value.s);
		m_data += CString("%s=%s", key.s, md5.hex_digest()).s;
	}
	else
	{
		m_data += CString("%s=%s", key.s, value.s).s;
	}
	
}*/

void CUrlData::add(CString key, int value, int flags /* = CUrlData::NONE */)
{
	add(key, CString("%i", value).s, flags);
}

void CUrlData::add(CString key, float value, int flags /* = CUrlData::NONE */)
{
	add(key, CString("%f", value).s, flags);
}


// For base64 encoding
static const char base64_table[] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

std::string base64_encode(std::string in)
{
	std::string r;
	std::size_t pos;
	typedef unsigned char uchar;

	// keep going until we have less than 24 bits
	for(pos = 0; in.size() - pos > 2; pos+=3)
	{
		r += base64_table[ uchar(in[pos+0]) >> 2];
		r += base64_table[(( uchar(in[pos+0]) & 0x03) << 4) + ( uchar(in[pos+1]) >> 4)];
		r += base64_table[(( uchar(in[pos+1]) & 0x0f) << 2) + ( uchar(in[pos+2]) >> 6)];
		r += base64_table[ uchar(in[pos+2]) & 0x3f];
	}

	if(in.size() - pos != 0)
	{
		r += base64_table[uchar(in[pos+0]) >> 2];
		
		if (in.size() - pos > 1)
		{
			r += base64_table[(( uchar(in[pos+0]) & 0x03) << 4) + (uchar(in[pos+1]) >> 4)];
			r += base64_table[(uchar(in[pos+1]) & 0x0f) << 2];
			r += base64_pad;
		}
		else
		{
			r += base64_table[(uchar(in[pos+0]) & 0x03) << 4];
			r += base64_pad;
			r += base64_pad;
		}

	}

	return r;
}

