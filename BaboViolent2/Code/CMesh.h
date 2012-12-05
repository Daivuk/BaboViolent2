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

#ifndef CMESH_H
#define CMESH_H

#include "CVertexBuffer.h"

//--- CMesh: A renderable set of vertex buffers
class CMesh
{
public:
	typedef std::vector<CVertexBuffer>	vb_list_t;

	//--- Constructor/Destructor
	CMesh(vb_list_t& vbs);
	virtual ~CMesh();

	//--- Render
	void render();
	void renderSubMesh(size_t index);

	//--- Size
	size_t size() { return m_vbs.size(); }

protected:
	vb_list_t	m_vbs;
};

#endif
