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

#include "CUserLogin.h"
#include "Helper.h"
#include "GameVar.h"
#include "Console.h"
#include "CStatus.h"
#include <ShellAPI.h>
#include <process.h>
#include <tchar.h>


CUserLogin::CUserLogin(CControl * in_parent, CControl * in_alignTo)
{
	m_sfxClic = dksCreateSoundFromFile("main/sounds/Button.wav", false);
	m_sfxOver = dksCreateSoundFromFile("main/sounds/ControlOver.wav", false);

#ifndef _DX_
	//qObj = gluNewQuadric();
#endif
	tex_baboShadow = dktCreateTextureFromFile("main/textures/BaboShadow.tga", DKT_FILTER_BILINEAR);

	parent = in_parent;

	//--- Da big frame
	instance = new CControl(parent, CVector2i(0,0), CVector2i(736, 506/*in_parent->size.s[0]-20, 450*/), "", this, "FRAME", in_alignTo, CONTROL_SNAP_BOTTOM);
	instance->texture = dktCreateTextureFromFile("main/textures/Menu1Back.tga", DKT_FILTER_LINEAR);
//	instance->texture = dktCreateTextureFromFile("main/textures/Smoke2.tga", DKT_FILTER_LINEAR);
	instance->borderColor.set(1,.5f,.25f);

	//--- Labels and controls
/*	CControl * label1 = new CControl(pnl_login, CVector2i(10,10), CVector2i(200,25),"User name :", this, "LABEL");
	label1->toolTips = "Enter your user name.";
	txt_userName = new CControl(pnl_login, CVector2i(10,10), CVector2i(200,25),"", this, "EDIT", label1, CONTROL_SNAP_BOTTOM);
	CControl * label2 = new CControl(pnl_login, CVector2i(10,10), CVector2i(200,25),"Password :", this, "LABEL", txt_userName, CONTROL_SNAP_BOTTOM);
	label2->toolTips = "Type down your password.";
	txt_password = new CControl(pnl_login, CVector2i(10,10), CVector2i(200,25),"", this, "EDIT", label2, CONTROL_SNAP_BOTTOM);
	txt_password->password = true;
	CControl * label3 = new CControl(pnl_login, CVector2i(10,10), CVector2i(375,400),"Note : There is two ways to create an acount:\n  - You can create it directly here.\n  - You can register on the official forum : \x3www.rndlabs.ca\x8\n\nYou will then be able to identify yourself both on\nthe forum and in the game by using the same\naccount.\nIf you already have a forum's account\nyou can use it here to log on.\n\nHave fun!", this, "LABEL", label1, CONTROL_SNAP_RIGHT, 100);
	label3->textAlign = CONTROL_TEXTALIGN_TOPLEFT;

	btn_login = new CControl(pnl_login, CVector2i(32,32),CVector2i(128,25),"Login",this,"BUTTON", txt_password, CONTROL_SNAP_BOTTOM, 15);
	btn_login->toolTips = "Validate your user/pass and log into the world of babo!";
*/
	//--- CLIENT OPTIONS
	/*CControl * separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Profile", this, "SEPARATOR");

	//--- Player name
	CControl * label1 = new CControl(instance, CVector2i(20,20), CVector2i(90,25),"Username:", this, "LABEL", separator, CONTROL_SNAP_BOTTOM, 5);
	label1->textAlign = CONTROL_TEXTALIGN_TOPLEFT;
	label1->toolTips = "(Optional) Your account username.";
	txt_userName = new CControl(instance, CVector2i(20,20), CVector2i(200,25),gameVar.cl_accountUsername, this, "EDIT", label1, CONTROL_SNAP_RIGHT, 5);
//	txt_userName->password = true;

	label1 = new CControl(instance, CVector2i(20,20), CVector2i(90,25),"Password:", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
	label1->textAlign = CONTROL_TEXTALIGN_TOPLEFT;
	label1->toolTips = "(Optional) Your account password.";
	txt_password = new CControl(instance, CVector2i(20,20), CVector2i(200,25),gameVar.cl_accountPassword, this, "EDIT", label1, CONTROL_SNAP_RIGHT, 5);
	txt_password->password = true;

	CControl * label2 = new CControl(instance, CVector2i(20,20), CVector2i(90,25),"Player name:", this, "LABEL", txt_userName, CONTROL_SNAP_RIGHT, 15);
	label2->textAlign = CONTROL_TEXTALIGN_TOPLEFT;
	label2->toolTips = "Your player name visible to other players in game.";
	txt_playerName = new CControl(instance, CVector2i(20,20), CVector2i(250,25),gameVar.cl_playerName, this, "EDIT", label2, CONTROL_SNAP_RIGHT, 5);
	txt_playerName->SetMaxCarac(24);

	btn_login = new CControl(instance, CVector2i(32,32),CVector2i(64,36),CString("") + "Login",this,"BUTTON", txt_password, CONTROL_SNAP_RIGHT, 5);

	label2 = new CControl(instance, CVector2i(20,20), CVector2i(90, 25), "\x9(OPTIONAL) : ", this, "LABEL", label1, CONTROL_SNAP_BOTTOM, 5);
	btn_createAccount = new CControl(instance, CVector2i(32,32),CVector2i(128,36),CString("") + "Create Account",this,"BUTTON", label2, CONTROL_SNAP_RIGHT, 5);
	btn_createAccount->toolTips = "If this doesn't launch Internet Explorer, go to http://ladder.rndlabs.ca manually";

	separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Appearance", this, "SEPARATOR", btn_createAccount, CONTROL_SNAP_BOTTOM, 15);*/
	CControl * separator = new CControl(instance, CVector2i(10,20), CVector2i(200,25),"Appearance", this, "SEPARATOR");

	//--- Stats view : Perso config
	pic_babo = new CControl(instance, CVector2i(64,32),CVector2i(256,256),"",this,"FRAME", separator, CONTROL_SNAP_BOTTOM, 10);
	pic_babo->frame3D = false;
	pic_babo->frame2D = true;

	//--- His skin
	CControl * label1 = new CControl(instance, CVector2i(32,100), CVector2i(40,20),"Skin:", this, "LABEL", pic_babo, CONTROL_SNAP_RIGHT, 32);
	sld_skin = new CControl(instance, CVector2i(32,100),CVector2i(256,15),"",this,"SLIDER", label1, CONTROL_SNAP_RIGHT); sld_skin->valueMax = 23; sld_skin->valueMin = 1;

	//--- To choose babo color
	label1 = new CControl(instance, CVector2i(32,225), CVector2i(200,20),"Layer 1 (red/green/blue):", this, "LABEL", sld_skin, CONTROL_SNAP_BOTTOM, 10);
	sld_layer1_r = new CControl(instance, CVector2i(32,150),CVector2i(256,15),"",this,"SLIDER", label1, CONTROL_SNAP_BOTTOM); sld_layer1_r->valueMax = 255;
	sld_layer1_g = new CControl(instance, CVector2i(32,180),CVector2i(256,15),"",this,"SLIDER", sld_layer1_r, CONTROL_SNAP_BOTTOM); sld_layer1_g->valueMax = 255;
	sld_layer1_b = new CControl(instance, CVector2i(32,210),CVector2i(256,15),"",this,"SLIDER", sld_layer1_g, CONTROL_SNAP_BOTTOM); sld_layer1_b->valueMax = 255;

	label1 = new CControl(instance, CVector2i(32,225), CVector2i(200,20),"Layer 2 (red/green/blue):", this, "LABEL", sld_layer1_b, CONTROL_SNAP_BOTTOM, 10);
	sld_layer2_r = new CControl(instance, CVector2i(32,230),CVector2i(256,15),"",this,"SLIDER", label1, CONTROL_SNAP_BOTTOM); sld_layer2_r->valueMax = 255;
	sld_layer2_g = new CControl(instance, CVector2i(32,250),CVector2i(256,15),"",this,"SLIDER", sld_layer2_r, CONTROL_SNAP_BOTTOM); sld_layer2_g->valueMax = 255;
	sld_layer2_b = new CControl(instance, CVector2i(32,270),CVector2i(256,15),"",this,"SLIDER", sld_layer2_g, CONTROL_SNAP_BOTTOM); sld_layer2_b->valueMax = 255;

	label1 = new CControl(instance, CVector2i(32,225), CVector2i(200,20),"Layer 3 (red/green/blue):", this, "LABEL", sld_layer2_b, CONTROL_SNAP_BOTTOM, 10);
	sld_layer3_r = new CControl(instance, CVector2i(32,300),CVector2i(256,15),"",this,"SLIDER", label1, CONTROL_SNAP_BOTTOM); sld_layer3_r->valueMax = 255;
	sld_layer3_g = new CControl(instance, CVector2i(32,320),CVector2i(256,15),"",this,"SLIDER", sld_layer3_r, CONTROL_SNAP_BOTTOM); sld_layer3_g->valueMax = 255;
	sld_layer3_b = new CControl(instance, CVector2i(32,340),CVector2i(256,15),"",this,"SLIDER", sld_layer3_g, CONTROL_SNAP_BOTTOM); sld_layer3_b->valueMax = 255;

	lbl_killWeapon[8] = 0;
	lbl_killWeapon[9] = 0;
	lbl_killWeapon[10] = 0;
	lbl_killWeapon[11] = 0;
	lbl_killWeapon[12] = 0;
	lbl_killWeapon[13] = 0;
	lbl_killWeapon[14] = 0;
	lbl_killWeapon[15] = 0;
	lbl_killWeapon[16] = 0;
	lbl_killWeapon[17] = 0;
	lbl_killWeapon[18] = 0;
	lbl_killWeapon[19] = 0;

	instance->backColor.set(0,.3f,.7f);
	instance->imgColor = instance->backColor;

	animY = 0;
	velY = 0;
	originalY = instance->localPos[1];

	rollingAngle = 0;

	//--- For skin purpose
	tex_skinOriginal = dktCreateTextureFromFile(CString("main/skins/%s.tga", gameVar.cl_skin.s).s, DKT_FILTER_BILINEAR);
	tex_skin = dktCreateEmptyTexture(64, 32, 3, DKT_FILTER_BILINEAR);
	m_medals = 1430;
	updateSkinInt = 0;

	updateSkin();
}


CUserLogin::~CUserLogin()
{
#ifndef _DX_
	//gluDeleteQuadric(qObj);
#endif
	dktDeleteTexture(&tex_baboShadow);
	dksDeleteSound(m_sfxClic);
	dksDeleteSound(m_sfxOver);
	dktDeleteTexture(&tex_skinOriginal);
	dktDeleteTexture(&tex_skin);
}



void CUserLogin::updateSkin()
{
	CColor3f redDecalT;
	CColor3f greenDecalT;
	CColor3f blueDecalT;

	int i,j,k;
	float r,g,b;
	CColor3f finalColor;

/*	for (i=0;i<11;++i)
	{
		if (!(1 << i & m_medals))
		{
			pic_medals[i]->enable = false;
		}
		pic_medals[i]->visible = true;
	}*/

	//--- Les stats
/*	lbl_honor->text = CString("") + (int)gameVar.scl_honor;
	lbl_xp->text = CString("") + (int)gameVar.scl_xp;
	lbl_leftToNextLevel->text = CString("") + (int)gameVar.scl_leftToNextLevel;
	lbl_totalKill->text = CString("") + (int)gameVar.scl_totalKill;
	lbl_totalDeath->text = CString("") + (int)gameVar.scl_totalDeath;
	lbl_ratio->text = CString("%.02f", (float)gameVar.scl_totalKill / (float)gameVar.scl_totalDeath);
	lbl_weaponOfChoice->text = "None";
	for (i=0;i<20;++i)
	{
		if (lbl_killWeapon[i]) lbl_killWeapon[i]->text = CString("") + (int)gameVar.scl_killWeapon[i];
	}
	if (gameVar.weapons[gameVar.scl_weaponOfChoice]) lbl_weaponOfChoice->text = gameVar.weapons[gameVar.scl_weaponOfChoice]->weaponName;
*/
	//--- Ici c'est nowhere on update les couleurs lol
	//--- Si ça changé on update ça au autres joueur!
	redDecalT = gameVar.cl_redDecal;
	greenDecalT = gameVar.cl_greenDecal;
	blueDecalT = gameVar.cl_blueDecal;

	sld_layer1_r->value = (int)(gameVar.cl_redDecal[0] * 255);
	sld_layer1_g->value = (int)(gameVar.cl_redDecal[1] * 255);
	sld_layer1_b->value = (int)(gameVar.cl_redDecal[2] * 255);

	sld_layer2_r->value = (int)(gameVar.cl_greenDecal[0] * 255);
	sld_layer2_g->value = (int)(gameVar.cl_greenDecal[1] * 255);
	sld_layer2_b->value = (int)(gameVar.cl_greenDecal[2] * 255);

	sld_layer3_r->value = (int)(gameVar.cl_blueDecal[0] * 255);
	sld_layer3_g->value = (int)(gameVar.cl_blueDecal[1] * 255);
	sld_layer3_b->value = (int)(gameVar.cl_blueDecal[2] * 255);

	CString skinStr = gameVar.cl_skin;
	skinStr.resizeInverse(skinStr.len() - 4);
	sld_skin->value = skinStr.toInt();

	//--- On reload le skin si ça changé
	dktDeleteTexture(&tex_skinOriginal);
	tex_skinOriginal = dktCreateTextureFromFile(CString("main/skins/%s.tga", gameVar.cl_skin.s).s, DKT_FILTER_BILINEAR);
//	dktBlurTexture(tex_skinOriginal, 1);

	//--- Hey oui, un recré une texture ogl à chaque fois pour chaque babo qui spawn!!!!
	//--- On est en ogl, faq ça kick ass MOUHOUHOUHAHAHA
	unsigned char imgData[64*32*3];
	dktGetTextureData(tex_skinOriginal, imgData);

	//--- Celon son team, on set la couleur du babo en conséquence
	for (j=0;j<32;++j)
	{
		for (i=0;i<64;++i)
		{
			k = ((j*64) + i) * 3;
			r = (float)imgData[k + 0] / 255.0f;
			g = (float)imgData[k + 1] / 255.0f;
			b = (float)imgData[k + 2] / 255.0f;
			finalColor = (redDecalT * r + greenDecalT * g + blueDecalT * b) / (r+g+b);
			imgData[k + 0] = (unsigned char)(finalColor[0] * 255.0f);
			imgData[k + 1] = (unsigned char)(finalColor[1] * 255.0f);
			imgData[k + 2] = (unsigned char)(finalColor[2] * 255.0f);
		}
	}

	// update
	dktCreateTextureFromBuffer(&tex_skin, imgData, 64, 32, 3, DKT_FILTER_BILINEAR);
}



void CUserLogin::MouseEnter(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxOver, -1, 200);
	}
}
void CUserLogin::MouseLeave(CControl * control)
{
}
void CUserLogin::MouseDown(CControl * control)
{
}
void CUserLogin::MouseUp(CControl * control)
{
}
void CUserLogin::MouseMove(CControl * control)
{
}
void CUserLogin::Click(CControl * control)
{
	if (control->style == "BUTTON")
	{
		dksPlaySound(m_sfxClic, -1, 200);
	}
	if (control == btn_login)
	{
		// Username can be no longer than 20 characters
		CString username = txt_userName->text;
		username.resize(20);

		// Check if settings changed
		if(gameVar.cl_accountUsername != username || gameVar.cl_accountPassword != txt_password->text)
		{
			//status->set(CStatus::OFFLINE);
			// Save Settings
			gameVar.cl_accountUsername = username;
			gameVar.cl_accountPassword = txt_password->text;
			status->set(CStatus::ONLINE);
		}
	}
	if (control == btn_createAccount)
	{
#ifdef WIN32
	//	ShellExecute(dkwGetHandle(), "OPEN", "Register.lnk", NULL, NULL, SW_SHOW);
	//	ShellExecute(0, "OPEN", "http://p3.envision3d.org/~league/", "", NULL/*lcTPath*/, 1);
	//	ShellExecute(0, "", "http://p3.envision3d.org/~league/", "", "", SW_SHOWNORMAL);
	//	execl("http://p3.envision3d.org/~league/", 0);
	//	CreateProcess();
	//LPCTSTR url = _T("www.microsoft.com");
	//CString args;
	//args.Format(_T("url.dll,FileProtocolHandler www.microsoft.com"), url);
	ShellExecute(NULL, _T("open"), _T("rundll32.exe"), _T("url.dll,FileProtocolHandler http://ladder.rndlabs.ca"), 0, SW_SHOW);

#endif
		dkwForceQuit();
	}
}
void CUserLogin::Validate(CControl * control)
{
	if(control->style == "EDIT")
	{
		dksPlaySound(m_sfxClic, -1, 200);
		if (control == txt_playerName)
		{
			gameVar.cl_playerName = txt_playerName->text;
		}
	}
	else
	{
		if (control == sld_layer1_r)
		{
			gameVar.cl_redDecal[0] = (float)sld_layer1_r->value / 255.0f;
		}
		if (control == sld_layer1_g)
		{
			gameVar.cl_redDecal[1] = (float)sld_layer1_g->value / 255.0f;
		}
		if (control == sld_layer1_b)
		{
			gameVar.cl_redDecal[2] = (float)sld_layer1_b->value / 255.0f;
		}
		if (control == sld_layer2_r)
		{
			gameVar.cl_greenDecal[0] = (float)sld_layer2_r->value / 255.0f;
		}
		if (control == sld_layer2_g)
		{
			gameVar.cl_greenDecal[1] = (float)sld_layer2_g->value / 255.0f;
		}
		if (control == sld_layer2_b)
		{
			gameVar.cl_greenDecal[2] = (float)sld_layer2_b->value / 255.0f;
		}
		if (control == sld_layer3_r)
		{
			gameVar.cl_blueDecal[0] = (float)sld_layer3_r->value / 255.0f;
		}
		if (control == sld_layer3_g)
		{
			gameVar.cl_blueDecal[1] = (float)sld_layer3_g->value / 255.0f;
		}
		if (control == sld_layer3_b)
		{
			gameVar.cl_blueDecal[2] = (float)sld_layer3_b->value / 255.0f;
		}
		if (control == sld_skin)
		{
			gameVar.cl_skin.set("skin%02i", sld_skin->value);
		}
	}
}
void CUserLogin::Paint(CControl * control)
{
	if (control == pic_babo)
	{
		//--- We render the babo in 3D
#ifndef _DX_
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			//	glPushAttrib(GL_ALL_ATTRIB_BITS);
				glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT | GL_VIEWPORT_BIT);

					glDepthMask(GL_TRUE);
					CVector2i	res = dkwGetResolution();
					float		offset = 0;

					if(gameVar.r_widescreen > 1)
					{
						offset = (float)res[0];
						res[0] = (int)(res[1]*1.333f);
						offset = (offset - res[0])/2;	
					}

					glViewport(
						(GLint)((((float)control->Rect[0]/800.0f) * (float)res[0]) + offset), 
						res[1] - (int)(((float)(control->Rect[1])/600.0f) * (float)res[1]) - (int)(((float)control->Rect[3]/600.0f) * (float)res[1] + 1),
						(int)(((float)control->Rect[2]/800.0f) * (float)res[0]), 
						(int)(((float)control->Rect[3]/600.0f) * (float)res[1] + 1));

					dkglSetProjection(70, 1, 1000, (float)control->size[0], (float)control->size[1]);

					// Truc par default à enabeler
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_CULL_FACE);
					glDisable(GL_TEXTURE_2D);
					glColor3f(1,1,1);

					//--- Camera
					gluLookAt(-4, -12, 7, 0, 0, 5, 0, 0, 1);

					//--- Shadow
					glColor4f(1,1,1,.75f);
					renderTexturedQuad(-8, 8, 16, -16, tex_baboShadow);

					glColor3f(1,1,1);

					//--- La boule
					glDisable(GL_BLEND);
					glEnable(GL_CULL_FACE);
					glEnable(GL_LIGHTING);
					glDepthFunc(GL_LEQUAL);

					dkglSetPointLight(1, -1000, -500, 2000, 1, 1, 1);
					glPushMatrix();
						glTranslatef(0, 0, 5);
						glRotatef(90, 0, 1, 0);
						glRotatef(rollingAngle, 0, 0, 1);


						glEnable(GL_COLOR_MATERIAL);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, tex_skin);

						glColor3f(1,1,1);
						glPolygonMode(GL_FRONT, GL_FILL);
						//gluQuadricTexture(qObj, true);
						//gluSphere(qObj, 5, 24, 24);
                        drawSphere(5, 24, 24, GL_TRIANGLES);
					glPopMatrix();
					
				glPopAttrib();
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
#endif
	}
}



#endif

