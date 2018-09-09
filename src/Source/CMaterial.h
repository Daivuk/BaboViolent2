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

#ifndef CMATERIAL_H
#define CMATERIAL_H

#include "Zeven.h"


//--- SVertex: A single vertex with a normal, single texture, and colour
struct SVertex
{
	float x,y,z;
	float nx,ny,nz;
	float u,v;
	float r,g,b,a;
};


//--- CMaterial: A set of settings used to render a vertex buffer
class CMaterial
{
public:
	typedef unsigned int texture_t;

	//--- No texture constant
	static const texture_t no_texture = static_cast<texture_t>(-1);

	//--- Blend modes
	enum blend_t
	{
		BLEND_NONE,
		BLEND_ALPHA,
	};

	//--- Constructor/Destructor
	CMaterial(texture_t tex = no_texture, blend_t blend = BLEND_NONE, bool diffuse = true, bool lit = false);
	virtual ~CMaterial();

	//--- Enable/Disable (sets OpenGL states)
	void enable(SVertex* first) const;
	void disable() const;

	//--- Equality operator
	bool operator==(const CMaterial &rhs) const;

protected:
	texture_t	m_tex;
	blend_t		m_blend;
	bool		m_lit;
	bool		m_diffuse;
};

#endif
