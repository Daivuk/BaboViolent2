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
#include "CMesh.h"

CMesh::CMesh(vb_list_t& vbs)
{
	//--- Steal the vbs
	m_vbs.swap(vbs);
}

CMesh::~CMesh() {}

void CMesh::render()
{
	for(size_t i = 0; i < m_vbs.size(); ++i)
		renderSubMesh(i);
}

void CMesh::renderSubMesh(size_t index)
{
	//--- Check for invalid index or empty buffer
	if(index < 0 || index >= m_vbs.size() || m_vbs[index].size() < 3 )
		return;

	//--- Get a reference to the vb & material
	CVertexBuffer&		vb = m_vbs[index];
	const CMaterial&	mat = vb.material();

	//--- Enable material
	mat.enable( vb.first() );

	//--- Draw!
#ifndef _DX_
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vb.size()) );
#endif

	//--- Disable material
	mat.disable();
}

#endif
