#ifndef CONSOLE
#include "CreateGame.h"
#include "GameVar.h"
#include "Console.h"
#include <direct.h>



// 
// Constructor
//
CreateGame::CreateGame(Menu * pRootMenu)
{
	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	add(btn_back = new Button(CVector2i(50,600-75),gameVar.lang_back,font,50,JUSTIFY_LEFT,this));
	add(btn_launch = new Button(CVector2i(250,600-75),gameVar.lang_launch,font,50,JUSTIFY_LEFT,this));
	add(btn_dedicate = new Button(CVector2i(450,600-75),gameVar.lang_dedicate,font,50,JUSTIFY_LEFT,this));

	add(new Label(CVector2i(25,25), CString("\x9( ") + gameVar.lang_generalGameOption + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,50), gameVar.lang_gameNameS, font, 25, JUSTIFY_LEFT));
	add(txt_gameName = new Write(CVector2i(150,50),"",font,25,JUSTIFY_LEFT));
	add(new Label(CVector2i(25,75), "Password", font, 25, JUSTIFY_LEFT));
	add(txt_password = new Write(CVector2i(150,75),gameVar.sv_password,font,25,JUSTIFY_LEFT));
	add(new Label(CVector2i(25,100), gameVar.lang_gameType, font, 25, JUSTIFY_LEFT));
	add(chk_gameType = new Choice(CVector2i(325,100),"",font,25,JUSTIFY_LEFT));
	chk_gameType->addChoice(gameVar.lang_freeForAll);
	chk_gameType->addChoice(gameVar.lang_teamDeathmatch);
	chk_gameType->addChoice(gameVar.lang_captureTheFlag);
//	chk_gameType->addChoice("Counter Baborist");
	add(new Label(CVector2i(25,125), gameVar.lang_TCPnUDPport, font, 25, JUSTIFY_LEFT));
	add(txt_port = new Write(CVector2i(325,125),"",font,25,JUSTIFY_LEFT, VALIDATE_INT));
	add(new Label(CVector2i(25,150), "BaboStats", font, 25, JUSTIFY_LEFT));
	add(chk_baboStats = new Choice(CVector2i(325,150),"",font,25,JUSTIFY_LEFT));
	chk_baboStats->addChoice(gameVar.lang_no);
	chk_baboStats->addChoice(gameVar.lang_yes);

//	

	add(new Label(CVector2i(25,175), CString("\x9( ") + gameVar.lang_gameLimits + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,200), gameVar.lang_scoreLimit, font, 25, JUSTIFY_LEFT));
	add(txt_scoreLimit = new Write(CVector2i(325,200),"",font,25,JUSTIFY_LEFT, VALIDATE_INT));
	add(new Label(CVector2i(25,225), gameVar.lang_teamWinLimit, font, 25, JUSTIFY_LEFT));
	add(txt_winLimit = new Write(CVector2i(325,225),"",font,25,JUSTIFY_LEFT, VALIDATE_INT));
	add(new Label(CVector2i(25,250), gameVar.lang_timeLimit, font, 25, JUSTIFY_LEFT));
	add(txt_gameTimeLimit = new Write(CVector2i(325,250),"",font,25,JUSTIFY_LEFT, VALIDATE_FLOAT));
//	add(new Label(CVector2i(25,250), "Round Time Limit in Second (0 = Unlimited)", font, 25, JUSTIFY_LEFT));
//	add(txt_roundTimeLimit = new Write(CVector2i(325,250),"",font,25,JUSTIFY_LEFT, VALIDATE_FLOAT));
//	add(new Label(CVector2i(25,250), "Bomb Explode Time in Second", font, 25, JUSTIFY_LEFT));
//	add(txt_bombTime = new Write(CVector2i(325,250),"",font,25,JUSTIFY_LEFT, VALIDATE_FLOAT));
	add(new Label(CVector2i(25,275), gameVar.lang_maxPlayer, font, 25, JUSTIFY_LEFT));
	add(txt_maxPlayer = new Write(CVector2i(325,275),"",font,25,JUSTIFY_LEFT, VALIDATE_INT));

	add(new Label(CVector2i(25,300), CString("\x9( ") + gameVar.lang_spawning + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,325), gameVar.lang_timeToSpawn, font, 25, JUSTIFY_LEFT));
	add(txt_timeToSpawn = new Write(CVector2i(325,325),"",font,25,JUSTIFY_LEFT, VALIDATE_FLOAT));
	add(new Label(CVector2i(25,350), gameVar.lang_forceRespawn, font, 25, JUSTIFY_LEFT));
	add(chk_forceRespawn = new Choice(CVector2i(325,350),"",font,25,JUSTIFY_LEFT));
	chk_forceRespawn->addChoice(gameVar.lang_no);
	chk_forceRespawn->addChoice(gameVar.lang_yes);

	add(new Label(CVector2i(25,400), CString("\x9( ") + gameVar.lang_friendlyFire + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(25,425), gameVar.lang_friendlyFire, font, 25, JUSTIFY_LEFT));
	add(chk_ff = new Choice(CVector2i(325,425),"",font,25,JUSTIFY_LEFT));
	chk_ff->addChoice(gameVar.lang_no);
	chk_ff->addChoice(gameVar.lang_yes);
	add(new Label(CVector2i(25,450), "Reflect Damage", font, 25, JUSTIFY_LEFT));
	add(chk_ffReflect = new Choice(CVector2i(325,450),"",font,25,JUSTIFY_LEFT));
	chk_ffReflect->addChoice(gameVar.lang_no);
	chk_ffReflect->addChoice(gameVar.lang_yes);

	initConfigValue();

	// On va chercher toute nos map

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH];  // directory specification
	DWORD dwError;
	char appPath[_MAX_PATH];

	// Chercher le path du "current working directory".
	_getcwd(appPath, _MAX_PATH);

	strncpy(DirSpec, appPath, strlen(appPath)+1);
	strncat(DirSpec, "\\main\\maps\\*.bvm", strlen("\\main\\maps\\*.bvm")+1);

	hFind = FindFirstFile(DirSpec, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Si on ne trouve pas le répertoire désiré.
	}
	else
	{
		CString filename = CString(FindFileData.cFileName);
		filename.resize(filename.len() - 4);
		maps.push_back(filename);

		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString filename = CString(FindFileData.cFileName);
				filename.resize(filename.len() - 4);
				maps.push_back(filename);
			}
		}
		
		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES)
		{
			// Si il y a une error en détectant qu'il n'y a plus de fichiers.
		}
	}

	add(new Label(CVector2i(500,25), CString("\x9( ") + gameVar.lang_maps + " )", font, 25, JUSTIFY_LEFT));
	add(new Label(CVector2i(600,25), CString("\x9( ") + gameVar.lang_includedMaps + " )", font, 25, JUSTIFY_LEFT));
	for (int i=0;i<(int)maps.size();++i)
	{
		Button * newBtn = new Button(CVector2i(500,50+i*25), maps[i],font,25,JUSTIFY_LEFT,this);
		btn_maps.push_back(newBtn);
		add(newBtn);
	}
}



//
// Destructor
//
CreateGame::~CreateGame()
{
	dkfDeleteFont(&font);
	maps.clear();
	includedMaps.clear();
}



//
// Quand on ouvre le menu, on fou les valeurs par default des config
//
void CreateGame::initConfigValue()
{
	txt_gameName->changeCaption(gameVar.sv_gameName);
	txt_gameName->set(gameVar.sv_gameName.s);
	txt_password->changeCaption(gameVar.sv_password.s);
	txt_password->set(gameVar.sv_password.s);
	chk_gameType->setChoice(gameVar.sv_gameType);
	txt_port->changeCaption(CString("%i", gameVar.sv_port));
	txt_port->set(CString("%i", gameVar.sv_port).s);
	if (gameVar.sv_baboStats) chk_baboStats->setChoice(1); else chk_baboStats->setChoice(0);

	txt_scoreLimit->changeCaption(CString("%i",gameVar.sv_scoreLimit));
	txt_scoreLimit->set(CString("%i",gameVar.sv_scoreLimit).s);
	txt_winLimit->changeCaption(CString("%i",gameVar.sv_winLimit));
	txt_winLimit->set(CString("%i",gameVar.sv_winLimit).s);
	txt_gameTimeLimit->changeCaption(CString("%.02f",gameVar.sv_gameTimeLimit));
	txt_gameTimeLimit->set(CString("%.02f",gameVar.sv_gameTimeLimit).s);
//	txt_roundTimeLimit->changeCaption(CString("%.02f",gameVar.sv_roundTimeLimit));
//	txt_roundTimeLimit->set(CString("%.02f",gameVar.sv_roundTimeLimit).s);
//	txt_bombTime->changeCaption(CString("%.02f",gameVar.sv_bombTime));
//	txt_bombTime->set(CString("%.02f",gameVar.sv_bombTime).s);
	txt_timeToSpawn->changeCaption(CString("%.02f",gameVar.sv_timeToSpawn));
	txt_timeToSpawn->set(CString("%.02f",gameVar.sv_timeToSpawn).s);
	if (gameVar.sv_forceRespawn) chk_forceRespawn->setChoice(1); else chk_forceRespawn->setChoice(0);
	txt_maxPlayer->changeCaption(CString("%i", gameVar.sv_maxPlayer));
	txt_maxPlayer->set(CString("%i", gameVar.sv_maxPlayer).s);

	if (gameVar.sv_friendlyFire) chk_ff->setChoice(1); else chk_ff->setChoice(0);
	if (gameVar.sv_reflectedDamage) chk_ffReflect->setChoice(1); else chk_ffReflect->setChoice(0);
}



//
// On va modifier le contenu des variables
//
void CreateGame::setConfigValue()
{
	gameVar.sv_gameName = *txt_gameName;
	gameVar.sv_password = *txt_password;
	gameVar.sv_gameType = chk_gameType->getChoiceIndex();
	gameVar.sv_port = txt_port->toInt();
	gameVar.sv_baboStats = (chk_baboStats->getChoiceIndex() == 1)?true:false;

	gameVar.sv_scoreLimit = txt_scoreLimit->toInt();
	gameVar.sv_winLimit = txt_winLimit->toInt();
	gameVar.sv_gameTimeLimit = txt_gameTimeLimit->toFloat();
//	gameVar.sv_roundTimeLimit = txt_roundTimeLimit->toFloat();
//	gameVar.sv_bombTime = txt_bombTime->toFloat();
	gameVar.sv_timeToSpawn = txt_timeToSpawn->toFloat();
	gameVar.sv_forceRespawn = (chk_forceRespawn->getChoiceIndex() == 1)?true:false;
	gameVar.sv_maxPlayer = txt_maxPlayer->toInt();

	gameVar.sv_friendlyFire = (chk_ff->getChoiceIndex() == 1)?true:false;
	gameVar.sv_reflectedDamage = (chk_ffReflect->getChoiceIndex() == 1)?true:false;

	initConfigValue();
}



//
// Pour ajouter une map
//
void CreateGame::addMap(CString mapName, Button * btn)
{
	// On check si elle est déjà là, dans ce cas on l'enlève
	for (int i=0;i<(int)includedMaps.size();++i)
	{
		if (mapName == includedMaps[i])
		{
			includedMaps.erase(includedMaps.begin()+i);
			btn->m_screenPosition[0] = 500;
			btn->changeCaption(btn->getCaption());
			return;
		}
	}

	includedMaps.push_back(mapName);
	btn->m_screenPosition[0] = 600;
	btn->changeCaption(btn->getCaption());
}



//
// Si on clic sur un bouton
//
void CreateGame::onClick(Control * control)
{
	if (control == btn_back)
	{
		initConfigValue();
		rootMenu->currentMenu = MENU_MAIN;
		return;
	}
	if (control == btn_launch)
	{
		if (includedMaps.size() == 0)
		{
			// On switch pour le menu qui dit : heille, tu dois inclure au moins une map.
			rootMenu->currentMenu = MENU_NO_MAP_SELECTED;
			return;
		}
		setConfigValue();

		// On colle la première map
		CString command = "host ";
		command += includedMaps[0];
		console->sendCommand(command);

		// On ajoute les maps
		for (int i=1;i<(int)includedMaps.size();++i)
		{
			command = CString("addmap ") + includedMaps[i];
			console->sendCommand(command);
		}
		return;
	}
	if (control == btn_dedicate)
	{
		if (includedMaps.size() == 0)
		{
			// On switch pour le menu qui dit : heille, tu dois inclure au moins une map.
			rootMenu->currentMenu = MENU_NO_MAP_SELECTED;
			return;
		}
		setConfigValue();

		// On colle la première map
		CString command = "dedicate ";
		command += includedMaps[0];
		console->sendCommand(command);

		// On ajoute les maps
		for (int i=1;i<(int)includedMaps.size();++i)
		{
			command = CString("addmap ") + includedMaps[i];
			console->sendCommand(command);
		}
		return;
	}
	for (int i=0;i<(int)btn_maps.size();++i)
	{
		if (control == btn_maps[i])
		{
			addMap(btn_maps[i]->getCaption(), btn_maps[i]);
			return;
		}
	}
}
#endif

