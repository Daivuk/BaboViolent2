#ifndef JOINGAME_H
#define JOINGAME_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"

#include <vector>
#include <string>


#define SORT_GAME_NAME 0
#define SORT_NB_PLAYER 1
#define SORT_GAME_TYPE 2
#define SORT_MAP_NAME 3
#define SORT_PING 4



//extern masterServerID;

class CGameInfo
{
public:
	static int sortBy;

	std::string gameName;
	std::string password;
	int nbPlayer;
	int maxPlayer;
	std::string mapName;
	std::string gameType;
	int ping;
	int flag;
	std::string ipAdress;
	int port;
	int pingPeerID;
	long pingFrame;

	//--- Constructeur
	CGameInfo() {}
	CGameInfo(CString in_gameName, CString in_password, int in_nbPlayer, int in_maxPlayer, CString in_mapName, int in_gameType, int in_ping, int in_flag, CString in_ipAdress, int in_port)
	{
		gameName = in_gameName.s;
		password = in_password.s;
		nbPlayer = in_nbPlayer;
		maxPlayer = in_maxPlayer;
		mapName = in_mapName.s;
		ping = in_ping;
		flag = in_flag;
		ipAdress = in_ipAdress.s;
		port = in_port;

		switch (in_gameType)
		{
		case 0:gameType = "DM";break;
		case 1:gameType = "TDM";break;
		case 2:gameType = "CTF";break;
		default:gameType = "N/A";break;
		}
	}

	inline bool operator<(const CGameInfo & gameInfo) const
	{
		switch (sortBy)
		{
		case SORT_GAME_NAME:
			return (gameName < gameInfo.gameName);
			break;
		case SORT_NB_PLAYER:
			return (nbPlayer < gameInfo.nbPlayer);
			break;
		case SORT_GAME_TYPE:
			return (gameType < gameInfo.gameType);
			break;
		case SORT_MAP_NAME:
			return (mapName < gameInfo.mapName);
			break;
		case SORT_PING:
			return (ping < gameInfo.ping);
			break;
		default: return false;
		}
	}

	inline bool operator>(const CGameInfo & gameInfo) const
	{
		switch (sortBy)
		{
		case SORT_GAME_NAME:
			return (gameName > gameInfo.gameName);
			break;
		case SORT_NB_PLAYER:
			return (nbPlayer > gameInfo.nbPlayer);
			break;
		case SORT_GAME_TYPE:
			return (gameType > gameInfo.gameType);
			break;
		case SORT_MAP_NAME:
			return (mapName > gameInfo.mapName);
			break;
		case SORT_PING:
			return (ping > gameInfo.ping);
			break;
		default: return false;
		}
	}
};


class JoinGame : public ControlListener
{
public:
	// The master server ID
//	int masterServerID;
	bool stoped;
	bool deleted;
	int nbTotalGame;

	// The main buttons
	Button * btn_back;
	Button * btn_launch;
	Write * txt_gameIP;

	// Internet - Lan Button
	Choice * chk_network;
	Button * btn_refresh;

	Button * btn_quickRefresh;

	// Ici on a la liste des game trié
	std::vector<CGameInfo> m_games;

	Button * btn_gameName;
	Button * btn_nbPlayer;
	Button * btn_gameType;
	Button * btn_mapName;
	Button * btn_ping;

	CString gamePassword;

	// Scroll info
	int scroll_pos;
	int scroll_mouseOver;
	int scroll_selected;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

	// On vient de faire refresh?
	bool bJustRefreshed;
	float refreshDelay;
	bool problemConnectingToMaster;
	float sendTimeOut;

	//--- Les icones pour les games
	unsigned int tex_icones;

	CVector2i oldMousePos;
	float clickDelay;

	bool firstTime;

	int animSearchingPing;

public:
	// Constructor
	JoinGame(Menu * pRootMenu);

	// Destructor
	virtual ~JoinGame();

	// Si on clic sur un bouton
	void onClick(Control * control);

	// Afficher (on a des special cases icite)
	void renderUnique();
	void updateUnique(float delay);

	// Recv data from the master server (list game !)
	void RecvDataFromMaster(char* dataBuffer, int typeID, long frameID);

	// On passe le frameID aussi pour pinger la game hehe
	void RecvDataFromLan(char* bv2RowBuffer, long frameID);
};


#endif

