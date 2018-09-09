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

#ifndef DEDICATED_SERVER
#include "CMeshBuilder.h"

CMeshBuilder::CMeshBuilder(const CMaterial& material): m_i(0), m_colour(1,1,1,1)
{
	//--- Start the first buffer
	m_vbs.push_back( CVertexBuffer(material) );
}

CMeshBuilder::~CMeshBuilder() {}

void CMeshBuilder::bind(const CMaterial& material, bool forceNew)
{
	//--- Empty the temporary buffer
	m_tempBuf.resize(0);

	//--- Check if this texture/mode combo is used already
	if(!forceNew)
	{
		for(size_t i = 0; i < m_vbs.size(); ++i)
		{
			if(m_vbs[i].material() == material)
			{
				m_i = i;
				return;
			}
		}
	}

	//--- Start a new buffer
	m_vbs.push_back( CVertexBuffer(material) );
	m_i = m_vbs.size() - 1;
}

void CMeshBuilder::vertex(float x, float y, float z, float u, float v)
{
	//--- Create the vertex
	SVertex vtx = { x, y, z,
		m_normal[0], m_normal[1], m_normal[2],
		u, v, m_colour[0], m_colour[1], m_colour[2], m_colour[3] 
	};

	//--- Add to the temporary buffer
	m_tempBuf.push_back( vtx );

	//--- Do we have enough for a tri?
	if(m_tempBuf.size() > 2)
	{
		m_vbs[m_i].add( m_tempBuf[0], m_tempBuf[1], m_tempBuf[2] );

		//--- Empty the temporary buffer
		m_tempBuf.resize(0);
	}
}

void CMeshBuilder::normal(float x, float y, float z)
{
	m_normal.set(x,y,z);
}

void CMeshBuilder::colour(float r, float g, float b, float a)
{
	m_colour.set(r,g,b,a);
}

CMesh* CMeshBuilder::compile()
{
	//--- Empty the temporary buffer
	m_tempBuf.resize(0);

	//--- Return a CMesh (this will clear m_vbs)
	return new CMesh( m_vbs );
}

#endif
