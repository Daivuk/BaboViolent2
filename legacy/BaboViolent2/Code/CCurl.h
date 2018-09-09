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

#ifndef CCURL_H_INCLUDED
#define CCURL_H_INCLUDED

#define LOCATION_OFFLINE	0
#define LOCATION_WEBSITE	1
#define LOCATION_ONLINE		2
#define LOCATION_INGAME		3

#include <string>
#include "CString.h"
#include "CThread.h"
#include <curl/curl.h>


class CCurl : public CThread
{
public:
	CCurl(CString url, std::string data);
	virtual ~CCurl();

	// Size of response recieved
	int		recieved() { return m_recieved; }

	// Response recieved
	std::string	response() { return m_response; }

	void*	userData() { return arg(); }

protected:
	void execute(void* pArg = 0);

private:
	CCurl();

	// Member variables
	CURL*		m_handle;
	CString		m_url;
	std::string	m_data;
	std::string	m_response;
	int			m_recieved;

	char * buffer;

	// Response structure
	struct SResponse {
		char*	data;
		size_t	size;
	};

	// Max response size (2048)
	static const int s_maxResponse;

	// Writes data to buffer, must be static
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
};


class CUrlData {
public:
	CUrlData();
	virtual ~CUrlData();

	void add(CString key, std::string value, int flags = CUrlData::NONE);
	//void add(CString key, CString value, int flags = CUrlData::NONE);
	void add(CString key, int value, int flags = CUrlData::NONE);
	void add(CString key, float value, int flags = CUrlData::NONE);

	std::string get() { return m_data; }

	enum {
		NONE = 0,
		MD5 = 1,
		BASE64 = 2
	};

private:
	std::string m_data;
};


#endif

