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

#ifndef CVERTEXBUFFER_H
#define CVERTEXBUFFER_H

#include "CMaterial.h"
#include <vector>

//--- CVertexBuffer: A set of tris rendered using the same texture
class CVertexBuffer
{
public:
	typedef SVertex					vertex_t;
	typedef std::vector<vertex_t>	vertex_buf_t;
	
	//--- Constructor/Destructor
	CVertexBuffer(const CMaterial& mat);
	virtual ~CVertexBuffer();

	//--- Adds a tri
	void add(const SVertex& a, const SVertex& b, const SVertex& c);

	//--- Gets the material
	const CMaterial& material()	{ return m_mat; }

	//--- Gets a pointer to the first vertex
	SVertex*		first()	{ return m_vb.size() > 0 ? &m_vb[0] : 0; }

	//--- Size
	size_t size() { return m_vb.size(); }

protected:
	vertex_buf_t	m_vb;
	CMaterial		m_mat;
};

#endif