#ifndef CONSOLE
#include "OptionMenu.h"
#include "GameVar.h"
#include "Console.h"



// 
// Constructor
//
OptionMenu::OptionMenu(Menu * pRootMenu)
{
	languageJustChanged = false;

	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(btn_apply = new Button(CVector2i(50,600-75),gameVar.lang_apply,font,50,JUSTIFY_LEFT,this));
	add(btn_cancel = new Button(CVector2i(275,600-75),gameVar.lang_cancel,font,50,JUSTIFY_LEFT,this));
	add(btn_extended = new Button(CVector2i(225,20),"Extended Options",font,30,JUSTIFY_LEFT,this));
	add(new Label(CVector2i(400,600-75+12-50),CString("\x7(*) ") + gameVar.lang_someOptionRequireRestart, font, 25, JUSTIFY_LEFT));

	add(new Label(CVector2i(25,25),CString("\x9( ") + gameVar.lang_clientOptions + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,50),gameVar.lang_playerName, font, 25, JUSTIFY_LEFT));
	add(txt_playerName = new Write(CVector2i(225,50),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,75),gameVar.lang_inGameMusic, font, 25, JUSTIFY_LEFT));
	add(chk_cubicMotion = new Choice(CVector2i(225,75),"", font, 25, JUSTIFY_LEFT));
	chk_cubicMotion->addChoice(gameVar.lang_no);
	chk_cubicMotion->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,100),gameVar.lang_language, font, 25, JUSTIFY_LEFT));
	add(chk_language = new Choice(CVector2i(225,100),"", font, 25, JUSTIFY_LEFT));
	chk_language->addChoice("English");
	chk_language->addChoice(CString("Fran\x87") + "ais");
	chk_language->addChoice("Suomi");
	chk_language->addChoice("Nederlands");
	chk_language->addChoice("Svenska");
	chk_language->addChoice("Magyar");


	add(new Label(CVector2i(25,150),CString("\x9( ") + gameVar.lang_renderingOptions + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,175),gameVar.lang_showStats, font, 25, JUSTIFY_LEFT));
	add(chk_showStats = new Choice(CVector2i(225,175),"", font, 25, JUSTIFY_LEFT));
	chk_showStats->addChoice(gameVar.lang_no);
	chk_showStats->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,200),CString("\x7(*)") + gameVar.lang_fullscreen, font, 25, JUSTIFY_LEFT));
	add(chk_fullScreen = new Choice(CVector2i(225,200),"", font, 25, JUSTIFY_LEFT));
	chk_fullScreen->addChoice(gameVar.lang_no);
	chk_fullScreen->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,225),CString("\x7(*)") + gameVar.lang_screenResolution, font, 25, JUSTIFY_LEFT));
	add(chk_resolution = new Choice(CVector2i(225,225),"", font, 25, JUSTIFY_LEFT));
	chk_resolution->addChoice("640 x 480");
	chk_resolution->addChoice("800 x 600");
	chk_resolution->addChoice("1024 x 768");
	chk_resolution->addChoice("1280 x 1024");
	add(new Label(CVector2i(25,250),CString("\x7(*)") + gameVar.lang_colorDepth, font, 25, JUSTIFY_LEFT));
	add(chk_bitDepth = new Choice(CVector2i(225,250),"", font, 25, JUSTIFY_LEFT));
	chk_bitDepth->addChoice(CString("16 ") + gameVar.lang_bits);
	chk_bitDepth->addChoice(CString("32 ") + gameVar.lang_bits);
	add(new Label(CVector2i(25,275),gameVar.lang_wallShadowQuality, font, 25, JUSTIFY_LEFT));
	add(chk_shadowQuality = new Choice(CVector2i(225,275),"", font, 25, JUSTIFY_LEFT));
	chk_shadowQuality->addChoice(CString("\x4") + gameVar.lang_noShadows);
	chk_shadowQuality->addChoice(gameVar.lang_hardEdges);
	chk_shadowQuality->addChoice(gameVar.lang_softEdges);
	add(new Label(CVector2i(25,300),gameVar.lang_baboShadow, font, 25, JUSTIFY_LEFT));
	add(chk_playerShadow = new Choice(CVector2i(225,300),"", font, 25, JUSTIFY_LEFT));
	chk_playerShadow->addChoice(gameVar.lang_no);
	chk_playerShadow->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,325),gameVar.lang_projectileShadow, font, 25, JUSTIFY_LEFT));
	add(chk_projectileShadow = new Choice(CVector2i(225,325),"", font, 25, JUSTIFY_LEFT));
	chk_projectileShadow->addChoice(gameVar.lang_no);
	chk_projectileShadow->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,350),gameVar.lang_particleLighting, font, 25, JUSTIFY_LEFT));
	add(chk_particleLighting = new Choice(CVector2i(225,350),"", font, 25, JUSTIFY_LEFT));
	chk_particleLighting->addChoice(gameVar.lang_no);
	chk_particleLighting->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,375),gameVar.lang_particleSorting, font, 25, JUSTIFY_LEFT));
	add(chk_particleSorting = new Choice(CVector2i(225,375),"", font, 25, JUSTIFY_LEFT));
	chk_particleSorting->addChoice(gameVar.lang_no);
	chk_particleSorting->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,400),gameVar.lang_terrainSplatter, font, 25, JUSTIFY_LEFT));
	add(chk_terrainSplater = new Choice(CVector2i(225,400),"", font, 25, JUSTIFY_LEFT));
	chk_terrainSplater->addChoice(gameVar.lang_no);
	chk_terrainSplater->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,425),gameVar.lang_showCasing, font, 25, JUSTIFY_LEFT));
	add(chk_showCasing = new Choice(CVector2i(225,425),"", font, 25, JUSTIFY_LEFT));
	chk_showCasing->addChoice(gameVar.lang_no);
	chk_showCasing->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,450),gameVar.lang_groundMarkNBlood, font, 25, JUSTIFY_LEFT));
	add(chk_groundMark = new Choice(CVector2i(225,450),"", font, 25, JUSTIFY_LEFT));
	chk_groundMark->addChoice(gameVar.lang_no);
	chk_groundMark->addChoice(gameVar.lang_yes);


	add(new Label(CVector2i(425,25),CString("\x9( ") + gameVar.lang_soundOptions + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,50),CString("\x7(*)") + gameVar.lang_mixRate, font, 25, JUSTIFY_LEFT));
	add(chk_mixRate = new Choice(CVector2i(625,50),"", font, 25, JUSTIFY_LEFT));
	chk_mixRate->addChoice(CString("22050 ") + gameVar.lang_hertz);
	chk_mixRate->addChoice(CString("44100 ") + gameVar.lang_hertz);
	add(new Label(CVector2i(425,75),CString("\x7(*)") + gameVar.lang_maxSoftwareChannels, font, 25, JUSTIFY_LEFT));
	add(chk_maxSoftwareChannels = new Choice(CVector2i(625,75),"", font, 25, JUSTIFY_LEFT));
	chk_maxSoftwareChannels->addChoice("8");
	chk_maxSoftwareChannels->addChoice("16");
	chk_maxSoftwareChannels->addChoice("24");
	chk_maxSoftwareChannels->addChoice("32");
	chk_maxSoftwareChannels->addChoice("48");
	chk_maxSoftwareChannels->addChoice("64");
	add(new Label(CVector2i(425,100),gameVar.lang_masterVolume, font, 25, JUSTIFY_LEFT));
	add(lbl_volume = new Label(CVector2i(675,100),"0", font, 25, JUSTIFY_CENTER));
	add(btn_volumeDown = new Button(CVector2i(625,100),"<",font,25,JUSTIFY_RIGHT,this));
	add(btn_volumeUp = new Button(CVector2i(725,100),">",font,25,JUSTIFY_LEFT,this));


	add(new Label(CVector2i(425,150),CString("\x9( ") + gameVar.lang_keyOptions + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,175),gameVar.lang_moveUp, font, 25, JUSTIFY_LEFT));
	add(key_moveUp = new Key(CVector2i(625,175),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,200),gameVar.lang_moveDown, font, 25, JUSTIFY_LEFT));
	add(key_moveDown = new Key(CVector2i(625,200),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,225),gameVar.lang_moveRight, font, 25, JUSTIFY_LEFT));
	add(key_moveRight = new Key(CVector2i(625,225),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,250),gameVar.lang_moveLeft, font, 25, JUSTIFY_LEFT));
	add(key_moveLeft = new Key(CVector2i(625,250),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,275),gameVar.lang_shoot, font, 25, JUSTIFY_LEFT));
	add(key_shoot = new Key(CVector2i(625,275),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,300),gameVar.lang_throwGrenade, font, 25, JUSTIFY_LEFT));
	add(key_throwGrenade = new Key(CVector2i(625,300),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,325),gameVar.lang_throwCocktailMolotov, font, 25, JUSTIFY_LEFT));
	add(key_throwMolotov = new Key(CVector2i(625,325),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,350),gameVar.lang_use, font, 25, JUSTIFY_LEFT));
	add(key_pickUp = new Key(CVector2i(625,350),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,375),gameVar.lang_chatToAll, font, 25, JUSTIFY_LEFT));
	add(key_chatAll = new Key(CVector2i(625,375),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,400),gameVar.lang_chatToTeam, font, 25, JUSTIFY_LEFT));
	add(key_chatTeam = new Key(CVector2i(625,400),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,425),gameVar.lang_showScoreBoard, font, 25, JUSTIFY_LEFT));
	add(key_showScore = new Key(CVector2i(625,425),"", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(425,450),gameVar.lang_toggleMenu, font, 25, JUSTIFY_LEFT));
	add(key_menuAccess = new Key(CVector2i(625,450),"", font, 25, JUSTIFY_LEFT));


	resetToConfigValue();
}



//
// Destructor
//
OptionMenu::~OptionMenu()
{
	dkfDeleteFont(&font);
}



//
// Pour mettre les valeur config
//
void OptionMenu::resetToConfigValue()
{
	txt_playerName->changeCaption(gameVar.cl_playerName);
	txt_playerName->set(gameVar.cl_playerName.s);
	if (gameVar.s_inGameMusic) chk_cubicMotion->setChoice(1); else chk_cubicMotion->setChoice(0);
	if (gameVar.languageFile == "main/languages/en.lang") chk_language->setChoice(0);
	else if (gameVar.languageFile == "main/languages/fr.lang") chk_language->setChoice(1);
	else if (gameVar.languageFile == "main/languages/fn.lang") chk_language->setChoice(2);
	else if (gameVar.languageFile == "main/languages/nl.lang") chk_language->setChoice(3);
	else if (gameVar.languageFile == "main/languages/swe.lang") chk_language->setChoice(4);
	else if (gameVar.languageFile == "main/languages/hu.lang") chk_language->setChoice(5);

	if (gameVar.r_showStats) chk_showStats->setChoice(1); else chk_showStats->setChoice(0);
	if (gameVar.r_fullScreen) chk_fullScreen->setChoice(1); else chk_fullScreen->setChoice(0);
	if (gameVar.r_resolution == CVector2i(640,480)) chk_resolution->setChoice(0);
	else if (gameVar.r_resolution == CVector2i(800,600)) chk_resolution->setChoice(1);
	else if (gameVar.r_resolution == CVector2i(1024,768)) chk_resolution->setChoice(2);
	else if (gameVar.r_resolution == CVector2i(1280,1024)) chk_resolution->setChoice(3);
	else chk_resolution->setChoice(2);
	if (gameVar.r_bitdepth == 16) chk_bitDepth->setChoice(0);
	else if (gameVar.r_bitdepth == 32) chk_bitDepth->setChoice(1);
	else chk_resolution->setChoice(1);
	if (gameVar.r_shadowQuality == 0) chk_shadowQuality->setChoice(0);
	else if (gameVar.r_shadowQuality == 1) chk_shadowQuality->setChoice(1);
	else if (gameVar.r_shadowQuality == 2) chk_shadowQuality->setChoice(2);
	else chk_shadowQuality->setChoice(2);
	if (gameVar.r_playerShadow) chk_playerShadow->setChoice(1); else chk_playerShadow->setChoice(0);
	if (gameVar.r_projectileShadow) chk_projectileShadow->setChoice(1); else chk_projectileShadow->setChoice(0);
	if (gameVar.r_particleLighting) chk_particleLighting->setChoice(1); else chk_particleLighting->setChoice(0);
	if (gameVar.r_particleSort) chk_particleSorting->setChoice(1); else chk_particleSorting->setChoice(0);
	if (gameVar.r_terrainSplater) chk_terrainSplater->setChoice(1); else chk_terrainSplater->setChoice(0);
	if (gameVar.r_showCasing) chk_showCasing->setChoice(1); else chk_showCasing->setChoice(0);
	if (gameVar.r_showGroundMark) chk_groundMark->setChoice(1); else chk_groundMark->setChoice(0);

	if (gameVar.s_mixRate == 22050) chk_mixRate->setChoice(0);
	else if (gameVar.s_mixRate == 44100) chk_mixRate->setChoice(1);
	else chk_mixRate->setChoice(0);
	if (gameVar.s_maxSoftwareChannels == 8) chk_maxSoftwareChannels->setChoice(0);
	else if (gameVar.s_maxSoftwareChannels == 16) chk_maxSoftwareChannels->setChoice(1);
	else if (gameVar.s_maxSoftwareChannels == 24) chk_maxSoftwareChannels->setChoice(2);
	else if (gameVar.s_maxSoftwareChannels == 32) chk_maxSoftwareChannels->setChoice(3);
	else if (gameVar.s_maxSoftwareChannels == 48) chk_maxSoftwareChannels->setChoice(4);
	else if (gameVar.s_maxSoftwareChannels == 64) chk_maxSoftwareChannels->setChoice(5);
	else chk_maxSoftwareChannels->setChoice(1);
	masterVolume = (int)(gameVar.s_masterVolume * 100.0f);
	lbl_volume->changeCaption(CString("%i", masterVolume));

	key_moveUp->setKeyValue(gameVar.k_moveUp);
	key_moveDown->setKeyValue(gameVar.k_moveDown);
	key_moveRight->setKeyValue(gameVar.k_moveRight);
	key_moveLeft->setKeyValue(gameVar.k_moveLeft);
	key_shoot->setKeyValue(gameVar.k_shoot);
	key_throwGrenade->setKeyValue(gameVar.k_throwGrenade);
	key_throwMolotov->setKeyValue(gameVar.k_throwMolotov);
	key_pickUp->setKeyValue(gameVar.k_pickUp);
	key_chatAll->setKeyValue(gameVar.k_chatAll);
	key_chatTeam->setKeyValue(gameVar.k_chatTeam);
	key_showScore->setKeyValue(gameVar.k_showScore);
	key_menuAccess->setKeyValue(gameVar.k_menuAccess);

	setConfigValue(false);
}


void OptionMenu::updateUnique(float delay)
{
	if (chk_language->getChoice() == "English" && gameVar.languageFile != "main/languages/en.lang") 
	{
		gameVar.languageFile = "main/languages/en.lang";
		languageJustChanged = true;
	}
	else if (chk_language->getChoice() == CString("Fran\x87") + "ais" && gameVar.languageFile != "main/languages/fr.lang") 
	{
		gameVar.languageFile = "main/languages/fr.lang";
		languageJustChanged = true;
	}
	else if (chk_language->getChoice() == "Suomi" && gameVar.languageFile != "main/languages/fn.lang") 
	{
		gameVar.languageFile = "main/languages/fn.lang";
		languageJustChanged = true;
	}
	else if (chk_language->getChoice() == "Nederlands" && gameVar.languageFile != "main/languages/nl.lang") 
	{
		gameVar.languageFile = "main/languages/nl.lang";
		languageJustChanged = true;
	}
	else if (chk_language->getChoice() == "Svenska" && gameVar.languageFile != "main/languages/swe.lang") 
	{
		gameVar.languageFile = "main/languages/swe.lang";
		languageJustChanged = true;
	}
	else if (chk_language->getChoice() == "Magyar" && gameVar.languageFile != "main/languages/hu.lang") 
	{
		gameVar.languageFile = "main/languages/hu.lang";
		languageJustChanged = true;
	}
	if (languageJustChanged)
	{

		// On load tout suite le language utilisé par le joueur
		if (!gameVar.loadLanguage(gameVar.languageFile.s))
		{
			console->add(CString("\x4 Error > Can not load %s", gameVar.languageFile.s));
			return;
		}

		// On modifi les valeurs du config
		setConfigValue(true);
	}
}


void OptionMenu::setConfigValue(bool resetAfter)
{
	// Client options
	gameVar.cl_playerName = *txt_playerName;
	gameVar.s_inGameMusic = (chk_cubicMotion->getChoiceIndex()==1)?true:false;	

	// Rendering options
	gameVar.r_showStats = (chk_showStats->getChoiceIndex()==1)?true:false;
	gameVar.r_fullScreen = (chk_fullScreen->getChoiceIndex()==1)?true:false;
	if (chk_resolution->getChoice() == "640 x 480") gameVar.r_resolution.set(640,480);
	if (chk_resolution->getChoice() == "800 x 600") gameVar.r_resolution.set(800,600);
	if (chk_resolution->getChoice() == "1024 x 768") gameVar.r_resolution.set(1024,768);
	if (chk_resolution->getChoice() == "1280 x 1024") gameVar.r_resolution.set(1280,1024);
	if (chk_bitDepth->getChoice() == CString("16 ") + gameVar.lang_bits) gameVar.r_bitdepth = 16;
	if (chk_bitDepth->getChoice() == CString("32 ") + gameVar.lang_bits) gameVar.r_bitdepth = 32;
	if (chk_shadowQuality->getChoice() == CString("\x4") + gameVar.lang_noShadows) gameVar.r_shadowQuality = 0;
	if (chk_shadowQuality->getChoice() == gameVar.lang_hardEdges) gameVar.r_shadowQuality = 1;
	if (chk_shadowQuality->getChoice() == gameVar.lang_softEdges) gameVar.r_shadowQuality = 2;
	gameVar.r_playerShadow = (chk_playerShadow->getChoiceIndex()==1)?true:false;
	gameVar.r_projectileShadow = (chk_projectileShadow->getChoiceIndex()==1)?true:false;
	gameVar.r_particleLighting = (chk_particleLighting->getChoiceIndex()==1)?true:false;
	gameVar.r_particleSort = (chk_particleSorting->getChoiceIndex()==1)?true:false;
	gameVar.r_terrainSplater = (chk_terrainSplater->getChoiceIndex()==1)?true:false;
	gameVar.r_showCasing = (chk_showCasing->getChoiceIndex()==1)?true:false;
	gameVar.r_showGroundMark = (chk_groundMark->getChoiceIndex()==1)?true:false;

	// Sound options
	if (chk_mixRate->getChoice() == CString("22050 ") + gameVar.lang_hertz) gameVar.s_mixRate = 22050;
	if (chk_mixRate->getChoice() == CString("44100 ") + gameVar.lang_hertz) gameVar.s_mixRate = 44100;
	if (chk_maxSoftwareChannels->getChoice() == "8") gameVar.s_maxSoftwareChannels = 8;
	if (chk_maxSoftwareChannels->getChoice() == "16") gameVar.s_maxSoftwareChannels = 16;
	if (chk_maxSoftwareChannels->getChoice() == "24") gameVar.s_maxSoftwareChannels = 24;
	if (chk_maxSoftwareChannels->getChoice() == "32") gameVar.s_maxSoftwareChannels = 32;
	if (chk_maxSoftwareChannels->getChoice() == "48") gameVar.s_maxSoftwareChannels = 48;
	if (chk_maxSoftwareChannels->getChoice() == "64") gameVar.s_maxSoftwareChannels = 64;
	gameVar.s_masterVolume = (float)(masterVolume) / 100.0f;

	// Key options
	gameVar.k_moveUp = key_moveUp->getKeyValue();
	gameVar.k_moveDown = key_moveDown->getKeyValue();
	gameVar.k_moveRight = key_moveRight->getKeyValue();
	gameVar.k_moveLeft = key_moveLeft->getKeyValue();
	gameVar.k_shoot = key_shoot->getKeyValue();
	gameVar.k_throwGrenade = key_throwGrenade->getKeyValue();
	gameVar.k_throwMolotov = key_throwMolotov->getKeyValue();
	gameVar.k_pickUp = key_pickUp->getKeyValue();
	gameVar.k_chatAll = key_chatAll->getKeyValue();
	gameVar.k_chatTeam = key_chatTeam->getKeyValue();
	gameVar.k_showScore = key_showScore->getKeyValue();
	gameVar.k_menuAccess = key_menuAccess->getKeyValue();

	// Au cas où y a des if qui n'auraient pas fonctionné (validation) ;)
	if (resetAfter) resetToConfigValue();
}



//
// Si on clic sur un bouton
//
void OptionMenu::onClick(Control * control)
{
	if (control == btn_volumeDown)
	{
		masterVolume-=10;
		if (masterVolume < 0) masterVolume = 0;
		lbl_volume->changeCaption(CString("%i", masterVolume));
		gameVar.s_masterVolume = (float)(masterVolume) / 100.0f;
	}
	if (control == btn_volumeUp)
	{
		masterVolume+=10;
		if (masterVolume > 100) masterVolume = 100;
		lbl_volume->changeCaption(CString("%i", masterVolume));
		gameVar.s_masterVolume = (float)(masterVolume) / 100.0f;
	}
	if (control == btn_apply)
	{
		rootMenu->currentMenu = MENU_MAIN;

		// On modifi les valeurs du config
		setConfigValue(true);
	}
	if (control == btn_cancel)
	{
		rootMenu->currentMenu = MENU_MAIN;

		// On revient au valeur du config
		resetToConfigValue();
	}
	if (control == btn_extended)
	{
		rootMenu->currentMenu = MENU_EXTENDED;

		//--- We dont touch the config values here
	}
}
#endif

