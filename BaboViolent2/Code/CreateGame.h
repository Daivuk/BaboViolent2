#ifndef CREATEGAME_H
#define CREATEGAME_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class CreateGame : public ControlListener
{
public:
	// The main buttons
	Button * btn_back;
	Button * btn_launch;
	Button * btn_dedicate;

	Write * txt_gameName;
	Choice * chk_gameType;
	Write * txt_port;
	Write * txt_password;

	Write * txt_scoreLimit;
	Write * txt_winLimit;
	Write * txt_gameTimeLimit;
//	Write * txt_roundTimeLimit;
//	Write * txt_bombTime;
	Write * txt_timeToSpawn;
	Write * txt_maxPlayer;

	Choice * chk_ff;
	Choice * chk_ffReflect;
	Choice * chk_forceRespawn;
	Choice * chk_baboStats;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

	// Nos map possibles
	std::vector<CString> maps;
	std::vector<Button *> btn_maps;

	// Nos map à inclure
	std::vector<CString> includedMaps;

public:
	// Constructor
	CreateGame(Menu * pRootMenu);

	// Destructor
	virtual ~CreateGame();

	// Si on clic sur un bouton
	void onClick(Control * control);

	// Quand on ouvre le menu, on fou les valeurs par default des config
	void initConfigValue();
	void setConfigValue();

	// Pour afficher les trucs en plus

	// Pour ajouter une map
	void addMap(CString mapName, Button * btn);
};


#endif

