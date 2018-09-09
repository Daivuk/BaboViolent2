#ifndef OPTIONMENU_H
#define OPTIONMENU_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class OptionMenu : public ControlListener
{
public:
	// The main buttons
	Button * btn_apply;
	Button * btn_cancel;
	Button * btn_extended;

	// Client options
	Write * txt_playerName;
	Choice * chk_cubicMotion;
	Choice * chk_language;

	// Rendering options
	Choice * chk_showStats;
	Choice * chk_fullScreen;
	Choice * chk_resolution;
	Choice * chk_bitDepth;
	Choice * chk_shadowQuality;
	Choice * chk_playerShadow;
	Choice * chk_projectileShadow;
	Choice * chk_particleLighting;
	Choice * chk_particleSorting;
	Choice * chk_terrainSplater;
	Choice * chk_showCasing;
	Choice * chk_groundMark;

	// Sound options
	Choice * chk_mixRate;
	Choice * chk_maxSoftwareChannels;
	Label * lbl_volume;
	int masterVolume;
	Button * btn_volumeDown;
	Button * btn_volumeUp;

	// Key options
	Key * key_moveUp;
	Key * key_moveDown;
	Key * key_moveRight;
	Key * key_moveLeft;
	Key * key_shoot;
	Key * key_throwGrenade;
	Key * key_throwMolotov;
	Key * key_pickUp;
	Key * key_chatAll;
	Key * key_chatTeam;
	Key * key_showScore;
	Key * key_menuAccess;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

	bool languageJustChanged;

public:
	// Constructor
	OptionMenu(Menu * pRootMenu);

	// Destructor
	virtual ~OptionMenu();

	// Si on clic sur un bouton
	void onClick(Control * control);
	void updateUnique(float delay);

	// Pour mettre les valeur config
	void resetToConfigValue();
	void setConfigValue(bool resetAfter);
};


#endif

