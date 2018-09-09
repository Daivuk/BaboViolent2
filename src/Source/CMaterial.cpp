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
#include "CMaterial.h"

CMaterial::CMaterial(texture_t tex, blend_t blend, bool diffuse, bool lit): m_tex(tex), m_blend(blend), m_diffuse(diffuse), m_lit(lit)
{

}

CMaterial::~CMaterial()
{

}

void CMaterial::enable(SVertex* first) const
{
	//--- Push enable bit, this causes OpenGL to track and revert glEnable states
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT);

	//--- Always need verticies
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex), &(first->x) );

	//--- Texturing
	if(m_tex != no_texture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, m_tex );
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex), &(first->u) );
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	//--- Lighting
	if(m_lit)
	{
		glEnable(GL_LIGHTING);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(SVertex), &(first->nx) );
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	//--- Blending
	if(m_blend > BLEND_NONE)
	{
		glEnable(GL_BLEND);

		if(m_blend == BLEND_ALPHA)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	else
	{
		glDisable(GL_BLEND);
	}

	//--- Diffuse
	if(m_diffuse)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, sizeof(SVertex), &(first->r) );
	}
#endif
}

void CMaterial::disable() const
{
#ifndef _DX_
	glDisableClientState(GL_VERTEX_ARRAY);

	if(m_tex != no_texture)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(m_lit)
		glDisableClientState(GL_NORMAL_ARRAY);

	if(m_diffuse)
		glDisableClientState(GL_COLOR_ARRAY);

	//--- Return OpenGL to normal
	glPopAttrib();
#endif
}

bool CMaterial::operator==(const CMaterial &rhs) const
{
	return (m_tex == rhs.m_tex && m_blend == rhs.m_blend && m_diffuse == rhs.m_diffuse && m_lit == rhs.m_lit);
}

#endif
