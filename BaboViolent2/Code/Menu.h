#ifndef MENU_H
#define MENU_H


#include "ControlListener.h"
class JoinGame;


#define MENU_MAIN 0
#define MENU_OPTION 1
#define MENU_CREDITS 2
#define MENU_CREATE_SERVER 3
#define MENU_NO_MAP_SELECTED 4
#define MENU_QUIT 5
#define MENU_JOIN_GAME 6
#define MENU_NO_GAME_RUNNING 7
#define MENU_CONNECTION_FAILED 8
#define MENU_SERVER_SHUTDOWN 9
#define MENU_INCORRECT_NAME 10
#define MENU_PASSWORD 11
#define MENU_INCORRECT_PASSWORD 12
#define MENU_WRONG_VERSION 13
#define MENU_EXTENDED 14


class Menu
{
public:
	// Son status
	bool isShowing;

	// Our menus
	std::vector<ControlListener*> menus;
	JoinGame * joinGameMenu;

	// Our current state
	int currentMenu;

	// Notre background
	unsigned int dko_background;
	unsigned int tex_sky;
	unsigned int tex_cloud;

	// Pour animer notre background
	float animCloud;

	CVector3f smokeDummy[2];

	int smokeTime;

	bool isReady;

	// Le son pour le vent
	FSOUND_SAMPLE * sfx_wind;
	int soundPlayChannel;

	// La font
	unsigned int font;

public:
	// Constructeur
	Menu();

	// Destructeur
	virtual ~Menu();

	// Pour afficher le menu
	void show() {isShowing=true;}

	// Pour le closer
	void hide() {isShowing=false;}

	// Afficher les menu
	void render();

	// Updater
	void update(float delay);
};


#endif

