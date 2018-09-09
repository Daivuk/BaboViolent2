#ifndef CONSOLE
#include "Extended.h"
#include "GameVar.h"
#include "Console.h"
#include "helper.h"



// 
// Constructor
//
Extended::Extended(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");
	qObj = gluNewQuadric();

	add(btn_apply = new Button(CVector2i(50,600-75),gameVar.lang_apply,font,50,JUSTIFY_LEFT,this));
	add(btn_cancel = new Button(CVector2i(275,600-75),gameVar.lang_cancel,font,50,JUSTIFY_LEFT,this));

	add(btn_nextSkin = new Button(CVector2i(380,350),"<- Previous skin",font,30,JUSTIFY_RIGHT,this));
	add(btn_previousSkin = new Button(CVector2i(420,350),"Next skin ->",font,30,JUSTIFY_LEFT,this));

	add(new Label(CVector2i(300,100),"First Decal", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(300,130),"Second Decal", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(300,160),"Third Decal", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(500,70),"\x4Red", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(600,70),"\x2Green", font, 30, JUSTIFY_LEFT));
	add(new Label(CVector2i(700,70),"\x1Blue", font, 30, JUSTIFY_LEFT));
	add(btn_redDecal[0] = new Button(CVector2i(500,100),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_redDecal[1] = new Button(CVector2i(530,100),">",font,30,JUSTIFY_LEFT,this));
	add(btn_redDecal[2] = new Button(CVector2i(600,100),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_redDecal[3] = new Button(CVector2i(630,100),">",font,30,JUSTIFY_LEFT,this));
	add(btn_redDecal[4] = new Button(CVector2i(700,100),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_redDecal[5] = new Button(CVector2i(730,100),">",font,30,JUSTIFY_LEFT,this));
	add(btn_greenDecal[0] = new Button(CVector2i(500,130),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_greenDecal[1] = new Button(CVector2i(530,130),">",font,30,JUSTIFY_LEFT,this));
	add(btn_greenDecal[2] = new Button(CVector2i(600,130),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_greenDecal[3] = new Button(CVector2i(630,130),">",font,30,JUSTIFY_LEFT,this));
	add(btn_greenDecal[4] = new Button(CVector2i(700,130),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_greenDecal[5] = new Button(CVector2i(730,130),">",font,30,JUSTIFY_LEFT,this));
	add(btn_blueDecal[0] = new Button(CVector2i(500,160),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_blueDecal[1] = new Button(CVector2i(530,160),">",font,30,JUSTIFY_LEFT,this));
	add(btn_blueDecal[2] = new Button(CVector2i(600,160),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_blueDecal[3] = new Button(CVector2i(630,160),">",font,30,JUSTIFY_LEFT,this));
	add(btn_blueDecal[4] = new Button(CVector2i(700,160),"<",font,30,JUSTIFY_RIGHT,this));
	add(btn_blueDecal[5] = new Button(CVector2i(730,160),">",font,30,JUSTIFY_LEFT,this));

	skins.push_back("skin01");
	skins.push_back("skin02");
	skins.push_back("skin03");
	skins.push_back("skin04");
	skins.push_back("skin05");
	skins.push_back("skin06");
	skins.push_back("skin07");
	skins.push_back("skin08");
	skins.push_back("skin09");
	skins.push_back("skin10");

	animSphere = 0;
	tex_skinOriginal = 0;
	tex_skin = 0;

	resetToConfigValue();
}



//
// Destructor
//
Extended::~Extended()
{
	gluDeleteQuadric(qObj);
	dkfDeleteFont(&font);
	dktDeleteTexture(&tex_skin);
	dktDeleteTexture(&tex_skinOriginal);
}



//
// Pour mettre les valeur config
//
void Extended::resetToConfigValue()
{
	currentSkin = 0;
	for (int i=0;i<(int)skins.size(); ++i)
	{
		if (skins[i] == gameVar.cl_skin)
		{
			currentSkin = i;
			break;
		}
	}
	redDecal = gameVar.cl_redDecal;
	greenDecal = gameVar.cl_greenDecal;
	blueDecal = gameVar.cl_blueDecal;
	if (tex_skinOriginal) dktDeleteTexture(&tex_skinOriginal);
	if (tex_skin) dktDeleteTexture(&tex_skin);
	tex_skinOriginal = dktCreateTextureFromFile((CString("main/skins/") + skins[currentSkin] + ".tga").s, DKT_FILTER_BILINEAR);
	tex_skin = dktCreateEmptyTexture(64, 32, 3, DKT_FILTER_BILINEAR);
	updateSkinColor();

	setConfigValue(false);
}


void Extended::updateUnique(float delay)
{

	animSphere += delay * 45;
	while (animSphere >= 360) animSphere -= 360;
}

void Extended::updateSkinColor()
{
	CColor3f redDecalT;
	CColor3f greenDecalT;
	CColor3f blueDecalT;

	//--- Hey oui, un recré une texture ogl à chaque fois pour chaque babo qui spawn!!!!
	//--- On est en ogl, faq ça kick ass MOUHOUHOUHAHAHA
	unsigned char imgData[64*32*3];
	dktGetTextureData(tex_skinOriginal, imgData);

	//--- Celon son team, on set la couleur du babo en conséquence
/*	if (game->gameType != GAME_TYPE_DM)
	{
		switch (teamID)
		{
		case PLAYER_TEAM_RED:
			{
				redDecalT.set(1,.5f,.5f);
				greenDecalT.set(1,.0f,.0f);
				blueDecalT.set(.5f,0,0);
				break;
			}
		case PLAYER_TEAM_BLUE:
			{
				redDecalT.set(.5f,.5f,1);
				greenDecalT.set(0,0,1);
				blueDecalT.set(0,0,.5f);
				break;
			}
		}
	}
	else
	{*/
		//--- Sinon on prend les couleurs que le gars a mis
		redDecalT = redDecal;
		greenDecalT = greenDecal;
		blueDecalT = blueDecal;
//	}

	int i,j,k;
	float r,g,b;
	CColor3f finalColor;
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


void Extended::setConfigValue(bool resetAfter)
{
	//--- Notre skin
	gameVar.cl_skin = skins[currentSkin];
	gameVar.cl_redDecal = redDecal;
	gameVar.cl_greenDecal = greenDecal;
	gameVar.cl_blueDecal = blueDecal;

	// Au cas où y a des if qui n'auraient pas fonctionné (validation) ;)
	if (resetAfter) resetToConfigValue();
}



//
// Pour dessiner notre babo skin!
//
void Extended::renderUnique()
{
	glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
		//	dkglSetPointLight(1, -1000, -1000, 2000, 1, 1, 1);
			glTranslatef(400, 300, 0);
			glDepthMask(GL_TRUE);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		//	glEnable(GL_LIGHT1);
		//	glEnable(GL_LIGHTING);
			glBindTexture(GL_TEXTURE_2D, tex_skin);
			glColor3f(1,1,1);
			glPolygonMode(GL_FRONT, GL_FILL);
			gluQuadricTexture(qObj, true);
			glRotatef(90, 0, 1, 0);
			glRotatef(animSphere,0,0,1);
			gluSphere(qObj, 32, 16, 16);
		glPopAttrib();
	glPopMatrix();

	glColor3fv(redDecal.s);
	renderTexturedQuad(200,105,80,20,0);
	glColor3fv(greenDecal.s);
	renderTexturedQuad(200,135,80,20,0);
	glColor3fv(blueDecal.s);
	renderTexturedQuad(200,165,80,20,0);
}



//
// Si on clic sur un bouton
//
void Extended::onClick(Control * control)
{



	if (control == btn_redDecal[0])
	{
		redDecal[0] -= .1f;
		if (redDecal[0] < 0) redDecal[0] = 0;
		updateSkinColor();
	}
	if (control == btn_redDecal[1])
	{
		redDecal[0] += .1f;
		if (redDecal[0] > 1) redDecal[0] = 1;
		updateSkinColor();
	}
	if (control == btn_redDecal[2])
	{
		redDecal[1] -= .1f;
		if (redDecal[1] < 0) redDecal[1] = 0;
		updateSkinColor();
	}
	if (control == btn_redDecal[3])
	{
		redDecal[1] += .1f;
		if (redDecal[1] > 1) redDecal[1] = 1;
		updateSkinColor();
	}
	if (control == btn_redDecal[4])
	{
		redDecal[2] -= .1f;
		if (redDecal[2] < 0) redDecal[2] = 0;
		updateSkinColor();
	}
	if (control == btn_redDecal[5])
	{
		redDecal[2] += .1f;
		if (redDecal[2] > 1) redDecal[2] = 1;
		updateSkinColor();
	}



	if (control == btn_greenDecal[0])
	{
		greenDecal[0] -= .1f;
		if (greenDecal[0] < 0) greenDecal[0] = 0;
		updateSkinColor();
	}
	if (control == btn_greenDecal[1])
	{
		greenDecal[0] += .1f;
		if (greenDecal[0] > 1) greenDecal[0] = 1;
		updateSkinColor();
	}
	if (control == btn_greenDecal[2])
	{
		greenDecal[1] -= .1f;
		if (greenDecal[1] < 0) greenDecal[1] = 0;
		updateSkinColor();
	}
	if (control == btn_greenDecal[3])
	{
		greenDecal[1] += .1f;
		if (greenDecal[1] > 1) greenDecal[1] = 1;
		updateSkinColor();
	}
	if (control == btn_greenDecal[4])
	{
		greenDecal[2] -= .1f;
		if (greenDecal[2] < 0) greenDecal[2] = 0;
		updateSkinColor();
	}
	if (control == btn_greenDecal[5])
	{
		greenDecal[2] += .1f;
		if (greenDecal[2] > 1) greenDecal[2] = 1;
		updateSkinColor();
	}



	if (control == btn_blueDecal[0])
	{
		blueDecal[0] -= .1f;
		if (blueDecal[0] < 0) blueDecal[0] = 0;
		updateSkinColor();
	}
	if (control == btn_blueDecal[1])
	{
		blueDecal[0] += .1f;
		if (blueDecal[0] > 1) blueDecal[0] = 1;
		updateSkinColor();
	}
	if (control == btn_blueDecal[2])
	{
		blueDecal[1] -= .1f;
		if (blueDecal[1] < 0) blueDecal[1] = 0;
		updateSkinColor();
	}
	if (control == btn_blueDecal[3])
	{
		blueDecal[1] += .1f;
		if (blueDecal[1] > 1) blueDecal[1] = 1;
		updateSkinColor();
	}
	if (control == btn_blueDecal[4])
	{
		blueDecal[2] -= .1f;
		if (blueDecal[2] < 0) blueDecal[2] = 0;
		updateSkinColor();
	}
	if (control == btn_blueDecal[5])
	{
		blueDecal[2] += .1f;
		if (blueDecal[2] > 1) blueDecal[2] = 1;
		updateSkinColor();
	}


	if (control == btn_nextSkin)
	{
		currentSkin++;
		if (currentSkin == (int)skins.size()) currentSkin = 0;
		if (tex_skinOriginal) dktDeleteTexture(&tex_skinOriginal);
		tex_skinOriginal = dktCreateTextureFromFile((CString("main/skins/") + skins[currentSkin] + ".tga").s, DKT_FILTER_BILINEAR);
		updateSkinColor();
	}
	if (control == btn_previousSkin)
	{
		currentSkin--;
		if (currentSkin == -1) currentSkin = (int)skins.size() - 1;
		if (tex_skinOriginal) dktDeleteTexture(&tex_skinOriginal);
		tex_skinOriginal = dktCreateTextureFromFile((CString("main/skins/") + skins[currentSkin] + ".tga").s, DKT_FILTER_BILINEAR);
		updateSkinColor();
	}
	if (control == btn_apply)
	{
		rootMenu->currentMenu = MENU_OPTION;

		// On modifi les valeurs du config
		setConfigValue(true);
	}
	if (control == btn_cancel)
	{
		rootMenu->currentMenu = MENU_OPTION;

		// On revient au valeur du config
		resetToConfigValue();
	}
}
#endif

