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
#ifndef CUSERLOGIN_H_INCLUDED
#define CUSERLOGIN_H_INCLUDED


#include "CPanel.h"


class CUserLogin : public CPanel
{
public:
	CControl * parent;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	// Client options
	CControl * txt_playerName;

	// Pour dessiner notre sphere
#ifndef _DX_
	GLUquadricObj* qObj;
#endif

	// Son shadow
	unsigned int tex_baboShadow;
	unsigned int tex_skin;
	unsigned int tex_skinOriginal;

	float rollingAngle;

	//--- we have 2 possible view
	CControl * pnl_login;

		//--- Text field
		CControl * txt_userName;
		CControl * txt_password;

		//--- Some controls (Login and create acount)
		CControl * btn_login;
		CControl * btn_createAccount;

	//--- The stats panel
	CControl * pnl_stats;
	
		//--- Le render zone où on va dessiner notre babo yea poupé
		CControl * pic_babo;

		//--- To choose babo color
		CControl * sld_layer1_r;
		CControl * sld_layer1_g;
		CControl * sld_layer1_b;

		CControl * sld_layer2_r;
		CControl * sld_layer2_g;
		CControl * sld_layer2_b;

		CControl * sld_layer3_r;
		CControl * sld_layer3_g;
		CControl * sld_layer3_b;

		//--- His skin
		CControl * sld_skin;

		//--- His medals
		unsigned long m_medals;
		CControl * pic_medals[32];

		//--- Other stats
		CControl * lbl_honor;
		CControl * lbl_xp;
		CControl * lbl_leftToNextLevel;
		CControl * lbl_totalKill;
		CControl * lbl_totalDeath;
		CControl * lbl_ratio;
		CControl * lbl_killWeapon[20];
		CControl * lbl_weaponOfChoice;

public:
	CUserLogin(CControl * in_parent, CControl * in_alignTo);
	virtual ~CUserLogin();

	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
	void Validate(CControl * control);
	void Paint(CControl * control);

	int updateSkinInt;

	void updatePerso(float delay)
	{
		rollingAngle += delay * 90;
		while (rollingAngle >= 360) rollingAngle -= 360;

		if (instance->visible)
		{
			updateSkinInt++;
			if (updateSkinInt == 15)
			{
				updateSkin();
				updateSkinInt = 0;
			}
		}
	}

	void updateSkin();
};


#endif
#endif

