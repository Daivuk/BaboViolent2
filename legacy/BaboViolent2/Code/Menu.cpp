#ifndef CONSOLE
#include "Menu.h"
#include "MainMenu.h"
#include "OptionMenu.h"
#include "Credits.h"
#include "CreateGame.h"
#include "Helper.h"
#include "GameVar.h"
#include "NoMapSelected.h"
#include "Quit.h"
#include "NoGameRunning.h"
#include "ConnectFailed.h"
#include "ServerClose.h"
#include "JoinGame.h"
#include "IncorrectName.h"
#include "Password.h"
#include "IncorrectPassword.h"
#include "WrongVersion.h"
#include "Extended.h"


//
// Constructeur
//
Menu::Menu()
{
	currentMenu = MENU_MAIN;

	font = dkfCreateFont("main/fonts/babo.tga");

	// We create our menus
	menus.push_back(new MainMenu(this));
	menus.push_back(new OptionMenu(this));
	menus.push_back(new Credits(this));
	menus.push_back(new CreateGame(this));
	menus.push_back(new NoMapSelected(this));
	menus.push_back(new Quit(this));
	menus.push_back(joinGameMenu = new JoinGame(this));
	menus.push_back(new NoGameRunning(this));
	menus.push_back(new ConnectFailed(this));
	menus.push_back(new ServerClose(this));
	menus.push_back(new IncorrectName(this));
	menus.push_back(new Password(this));
	menus.push_back(new IncorrectPassword(this));
	menus.push_back(new WrongVersion(this));
	menus.push_back(new Extended(this));

	isShowing = true;

	tex_sky = dktCreateTextureFromFile("main/textures/bg_sky.tga", DKT_FILTER_LINEAR);
	tex_cloud = dktCreateTextureFromFile("main/textures/Cloud.tga", DKT_FILTER_LINEAR);

	dko_background = dkoLoadFile("main/models/MenuBackground.DKO");
	dkoGetDummyPosition("smoke", dko_background, smokeDummy[0].s);
	dkoGetDummyPosition("farSmoke", dko_background, smokeDummy[1].s);

	sfx_wind = dksCreateSoundFromFile("main/sounds/Wind.wav", true);

	animCloud = 0;
	smokeTime = 0;

	isReady = false;

//	isReady = true;
//	dksPlayMusic("main/sounds/menu.ogg", -1);

//	soundPlayChannel = dksPlaySound(sfx_wind, -1, 100);
}



//
// Destructeur
//
Menu::~Menu()
{
	int i;
	ZEVEN_DELETE_VECTOR(menus, i);
	dkoDeleteModel(&dko_background);
	dktDeleteTexture(&tex_sky);
	dktDeleteTexture(&tex_cloud);
	dksDeleteSound(sfx_wind);
	if (soundPlayChannel >= 0)
	{
		FSOUND_StopSound(soundPlayChannel);
		soundPlayChannel = -1;
		dksStopMusic();
	}

	dkfDeleteFont(&font);
}



//
// Afficher les menu
//
void Menu::render()
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1,1,1);
		// On render le ciel
		dkglPushOrtho(1,1);
			glDisable(GL_DEPTH_TEST);
			renderTexturedQuad(0,0,1,1,tex_sky);
		dkglPopOrtho();

		// On positionne notre camera
		CVector2i res = dkwGetResolution();
		dkglSetProjection(55, 1, 1000, (float)res[0], (float)res[1]);
		glLoadIdentity();
		gluLookAt(0,0,0,10,0,0,0,0,1);

		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex_cloud);
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
				glTranslatef(animCloud,0,0);
				glScalef(2,2,2);
				glBegin(GL_QUADS);
					glColor4f(1,1,1,1);
					glTexCoord2f(0,1);
					glVertex3f(-1000,-1000,100);
					glTexCoord2f(0,0);
					glVertex3f(-1000,1000,100);
					glTexCoord2f(1,0);
					glVertex3f(1000,1000,100);
					glTexCoord2f(1,1);
					glVertex3f(1000,-1000,100);
				glEnd();
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
		glPopAttrib();

		// Les particules
		glEnable(GL_DEPTH_TEST);
		dkoRender(dko_background);
		glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
			if (gameVar.r_particleLighting) 
			{
				glEnable(GL_LIGHTING);
				dkglSetPointLight(1, 200, -1000, 100, 1, 1, 1);
			}
			dkpRender();
		glPopAttrib();

		glColor3f(1,0,0);
		dkglPushOrtho(800,600);
			dkfBindFont(font);
			glColor3f(1,.6f,.4f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			printCenterText(400,0,128,gameVar.lang_gameName);
		dkglPopOrtho();

	glPopAttrib();

	menus[currentMenu]->renderMenu();
}



//
// Updater
//
void Menu::update(float delay)
{
	if (soundPlayChannel == -1) 
	{
		soundPlayChannel = dksPlaySound(sfx_wind, -1, 100);
	}

	animCloud += delay *.05f;
	while (animCloud >= 1) animCloud -= 1;
	smokeTime++;

	if (isReady) menus[currentMenu]->updateMenu(delay);

	if (((OptionMenu*)(menus[MENU_OPTION]))->languageJustChanged)
	{
		int i;
		ZEVEN_DELETE_VECTOR(menus, i);
		// We recreate our menus
		menus.push_back(new MainMenu(this));
		menus.push_back(new OptionMenu(this));
		menus.push_back(new Credits(this));
		menus.push_back(new CreateGame(this));
		menus.push_back(new NoMapSelected(this));
		menus.push_back(new Quit(this));
		menus.push_back(joinGameMenu = new JoinGame(this));
		menus.push_back(new NoGameRunning(this));
		menus.push_back(new ConnectFailed(this));
		menus.push_back(new ServerClose(this));
		menus.push_back(new IncorrectName(this));
	}

//	dkpUpdate(delay);

	// Smoke en background
/*	if (smokeTime%5 == 0)
	{
	}*/
	if (smokeTime >= 10)
	{
		dkpCreateParticle(	smokeDummy[1].s,//float *position,
							CVector3f(0,rand(3.0f,1.0f),10).s,//float *vel,
							CVector4f(.3f,.3f,.3f,.75f).s,//float *startColor,
							CVector4f(.7f,.7f,.7f,.75f).s,//float *endColor,
							10,//float startSize,
							rand(20.0f, 30.0f),//float endSize,
							15.0f,//float duration,
							0,//float gravityInfluence,
							0,//float airResistanceInfluence,
							rand(0.0f, 30.0f),//float rotationSpeed,
							gameVar.tex_smoke1,//unsigned int texture,
							DKP_SRC_ALPHA,//unsigned int srcBlend,
							DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
							0);//int transitionFunc);
		dkpCreateParticle(	smokeDummy[0].s,//float *position,
							CVector3f(rand(-2.0f,2.0f),rand(15.0f,6.0f),1).s,//float *vel,
							CVector4f(1,1,1,0).s,//float *startColor,
							CVector4f(1,1,1,.75f).s,//float *endColor,
							10,//float startSize,
							rand(10.0f, 10.0f),//float endSize,
							5.0f,//float duration,
							0,//float gravityInfluence,
							0,//float airResistanceInfluence,
							rand(0.0f, 30.0f),//float rotationSpeed,
							gameVar.tex_smoke1,//unsigned int texture,
							DKP_SRC_ALPHA,//unsigned int srcBlend,
							DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
							0);//int transitionFunc);
		dkpCreateParticle(	CVector3f(1,-5,0).s,//float *position,
							CVector3f(rand(-2.0f,2.0f),rand(7.5f,3.0f),1).s,//float *vel,
							CVector4f(1,1,1,0).s,//float *startColor,
							CVector4f(1,1,1,.75f).s,//float *endColor,
							10,//float startSize,
							rand(10.0f, 10.0f),//float endSize,
							5.0f,//float duration,
							0,//float gravityInfluence,
							0,//float airResistanceInfluence,
							rand(0.0f, 30.0f),//float rotationSpeed,
							gameVar.tex_smoke1,//unsigned int texture,
							DKP_SRC_ALPHA,//unsigned int srcBlend,
							DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
							0);//int transitionFunc);
		smokeTime = 0;
	}
}
#endif

