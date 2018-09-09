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

#ifndef CMESHBUILDER_H
#define CMESHBUILDER_H

#include "CMesh.h"

//--- CMeshBuilder: Builds a set of vertex buffers a vertex at a time
class CMeshBuilder
{
public:
	typedef CVertexBuffer::vertex_buf_t	vb_t;
	typedef CMesh::vb_list_t			vb_list_t;

	//--- Constructor/Destructor
	CMeshBuilder(const CMaterial& material);
	virtual ~CMeshBuilder();

	//--- Set current material
	void bind(const CMaterial& material, bool forceNew = false);

	//--- Adds a vertex
	void vertex(float x, float y, float z, float u = 0, float v = 0);

	//--- Set current normal
	void normal(float x, float y, float z);

	//--- Set current colour
	void colour(float r, float g, float b, float a = 1);

	//--- Compile the mesh (delete when finished)
	CMesh* compile();

protected:
	//--- List of vertex buffers
	vb_list_t	m_vbs;

	//--- Temporary buffer for incomplete tris
	vb_t		m_tempBuf;

	//--- Current normal & colour
	CVector3f	m_normal;
	CVector4f	m_colour;

	//--- Index to current buffer
	size_t		m_i;
};

#endif