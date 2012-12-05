#ifndef CONSOLE
#include "JoinGame.h"
#include "GameVar.h"
#include "Console.h"
#include <algorithm>
#include "Helper.h"
#include "Scene.h"

int CGameInfo::sortBy = SORT_GAME_NAME;

extern Scene * scene;
extern masterServerID;


// 
// Constructor
//
JoinGame::JoinGame(Menu * pRootMenu)
{
	clickDelay = 0;
	deleted = false;
	bJustRefreshed = false;
	stoped = false;
//	masterServerID = -1;
	refreshDelay = 0;
	problemConnectingToMaster = false;
	sendTimeOut = 0;
	firstTime = true;

	nbTotalGame = 0;
	scroll_pos = 0;
	scroll_mouseOver = -1;
	animSearchingPing = 0;
	scroll_selected = -1;

	rootMenu = pRootMenu;
	font = dkfCreateFont("main/fonts/babo.tga");

	tex_icones = dktCreateTextureFromFile("main/textures/Icones.tga", DKT_FILTER_LINEAR);

	add(chk_network = new Choice(CVector2i(25, 25), "", font, 25, JUSTIFY_LEFT));
	chk_network->addChoice(gameVar.lang_internet);
	chk_network->addChoice(gameVar.lang_localNetwork);
	add(btn_refresh = new Button(CVector2i(200,25), gameVar.lang_refreshList, font, 25, JUSTIFY_LEFT,this));
//	add(btn_quickRefresh = new Button(CVector2i(400,25), "Quick Refresh", font, 25, JUSTIFY_LEFT,this));
	

	add(btn_gameName = new Button(CVector2i(25+15,70), gameVar.lang_gameName, font, 25, JUSTIFY_LEFT,this));
	add(btn_nbPlayer = new Button(CVector2i(300+15,70), gameVar.lang_players, font, 25, JUSTIFY_LEFT,this));
	add(btn_gameType = new Button(CVector2i(400+15,70), gameVar.lang_gameType, font, 25, JUSTIFY_LEFT,this));
	add(btn_mapName = new Button(CVector2i(500+15,70), gameVar.lang_mapName, font, 25, JUSTIFY_LEFT,this));
	add(btn_ping = new Button(CVector2i(600+15,70), gameVar.lang_pingC, font, 25, JUSTIFY_LEFT,this));

	add(btn_back = new Button(CVector2i(50,600-75),gameVar.lang_back,font,50,JUSTIFY_LEFT,this));
	add(btn_launch = new Button(CVector2i(250,600-75),gameVar.lang_join,font,50,JUSTIFY_LEFT,this));

	add(new Label(CVector2i(25,600-125), gameVar.lang_IPAdress, font, 25, JUSTIFY_LEFT));
	add(txt_gameIP = new Write(CVector2i(250,600-125),"127.0.0.1", font, 25,JUSTIFY_LEFT));
	txt_gameIP->changeCaption(gameVar.cl_lastUsedIP);
	txt_gameIP->set(gameVar.cl_lastUsedIP.s);


	//--- TEST
/*	for (int i=0;i<10;++i)
	{
		m_games.push_back(CGameInfo("Official BV2 #1", "", 3, 32, "daivuk", 1, 30, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("Official BV2 #2", "", 5, 32, "crazy", 1, 25, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("Official BV2 #3", "", 16, 32, "field", 1, 40, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("\x4QC\x8 power Clan", "", 3, 16, "arena", 1, 100, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("~~YOOOO~~", "", 1, 32, "crazy", 2, 115, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("Test server", "", 2, 32, "daivuk", 2, 240, 0, "127.0.0.1"));
		m_games.push_back(CGameInfo("slp", "slp", 4, 8, "arena", 0, 75, 0, "127.0.0.1"));
	}*/
}



//
// Destructor
//
JoinGame::~JoinGame()
{
	dkfDeleteFont(&font);
	m_games.clear();
	if (masterServerID > 0) bb_peerDelete(masterServerID);
	masterServerID = -1;
}


//
// Afficher (on a des special cases icite)
//
void JoinGame::renderUnique()
{
	//--- On affiche 20 game à la fois, on va dessiner le tableau first
	//--- Colones
	glBegin(GL_QUADS);
		glColor4f(1,1,1,.25f);
		glVertex2i(25, 100);
		glVertex2i(25, 450);
		glVertex2i(290, 450);
		glVertex2i(290, 100);

		glColor4f(.5f,.5f,.5f,.25f);
		glVertex2i(300, 100);
		glVertex2i(300, 450);
		glVertex2i(390, 450);
		glVertex2i(390, 100);

		glColor4f(1,1,1,.25f);
		glVertex2i(400, 100);
		glVertex2i(400, 450);
		glVertex2i(490, 450);
		glVertex2i(490, 100);

		glColor4f(.5f,.5f,.5f,.25f);
		glVertex2i(500, 100);
		glVertex2i(500, 450);
		glVertex2i(590, 450);
		glVertex2i(590, 100);

		glColor4f(1,1,1,.25f);
		glVertex2i(600, 100);
		glVertex2i(600, 450);
		glVertex2i(640, 450);
		glVertex2i(640, 100);

		glColor4f(.5f,.5f,.5f,.25f);
		glVertex2i(650, 100);
		glVertex2i(650, 450);
		glVertex2i(725, 450);
		glVertex2i(725, 100);
	glEnd();

	glBegin(GL_QUADS);
		glColor4f(0,0,0,.15f);
		for (int i=scroll_pos%2;i<14;i+=2)
		{
			glVertex2i(25, 100 + i*25);
			glVertex2i(25, 100 + i*25+25);
			glVertex2i(725, 100 + i*25+25);
			glVertex2i(725, 100 + i*25);
		}
		if (scroll_selected != -1 &&
			scroll_selected >= scroll_pos &&
			scroll_selected < scroll_pos + 14)
		{
			glColor4f(.5f,1,0,.75f);
			glVertex2i(25, 100 + (scroll_selected - scroll_pos) *25);
			glVertex2i(25, 100 + (scroll_selected - scroll_pos) *25+25);
			glVertex2i(725, 100 + (scroll_selected - scroll_pos) *25+25);
			glVertex2i(725, 100 + (scroll_selected - scroll_pos) *25);
		}
		if (scroll_mouseOver != -1 &&
			scroll_mouseOver >= scroll_pos &&
			scroll_mouseOver < scroll_pos + 14)
		{
			glColor4f(1,1,.5f,.15f);
			glVertex2i(25, 100 + (scroll_mouseOver - scroll_pos) *25);
			glVertex2i(25, 100 + (scroll_mouseOver - scroll_pos) *25+25);
			glVertex2i(725, 100 + (scroll_mouseOver - scroll_pos) *25+25);
			glVertex2i(725, 100 + (scroll_mouseOver - scroll_pos) *25);
		}
	glEnd();

	// On list les games
	CGameInfo gameInfo;
	for (int i=0;i<14;++i)
	{
		glColor3f(1,1,1);
		if (i + scroll_pos < (int)m_games.size())
		{
			gameInfo = m_games[i+scroll_pos];
			printLeftText(30, 100 + (float)i*25, 25, CString((char*)gameInfo.gameName.c_str()));
			printRightText(385, 100 + (float)i*25, 25, CString("%i / %i", gameInfo.nbPlayer, gameInfo.maxPlayer));
			printLeftText(405, 100 + (float)i*25, 25, CString((char*)gameInfo.gameType.c_str()));
			printLeftText(505, 100 + (float)i*25, 25, CString((char*)gameInfo.mapName.c_str()));
			if (gameInfo.ping < 0)
			{
				if (animSearchingPing < 10)
					printRightText(635, 100 + (float)i*25, 25, CString("\x2") + "-");
				else if (animSearchingPing < 20)
					printRightText(635, 100 + (float)i*25, 25, CString("\x2") + "\\");
				else if (animSearchingPing < 30)
					printRightText(635, 100 + (float)i*25, 25, CString("\x2") + "|");
				else
					printRightText(635, 100 + (float)i*25, 25, CString("\x2") + "/");

			}
			else if (gameInfo.ping <= 100)
			{
				printRightText(635, 100 + (float)i*25, 25, CString("\x2") + gameInfo.ping/*"Good"*/);
			}
			else if (gameInfo.ping < 200)
			{
				printRightText(635, 100 + (float)i*25, 25, CString("\x9") + gameInfo.ping/*"Avrg"*/);
			}
			else if (gameInfo.ping < 999)
			{
				printRightText(635, 100 + (float)i*25, 25, CString("\x4") + gameInfo.ping/*"Bad"*/);
			}
			else
			{
				printRightText(635, 100 + (float)i*25, 25, CString("\x5") + "???");
			}

			//--- Finalement, les icones
			glPushAttrib(GL_ENABLE_BIT);
				glBindTexture(GL_TEXTURE_2D, tex_icones);
				glEnable(GL_TEXTURE_2D);
				glColor3f(1,1,1);
				glPushMatrix();
					glTranslatef(655, 100 + (float)i*25 + 4,0);
					glBegin(GL_QUADS);
						if (gameInfo.flag & ICONE_DEDICATED)
						{
							glTexCoord2f(0,1);
							glVertex2i(0,0);
							glTexCoord2f(0,.5f);
							glVertex2i(0,16);
							glTexCoord2f(.5f,.5f);
							glVertex2i(16,16);
							glTexCoord2f(.5f,1);
							glVertex2i(16,0);
						}
						if (gameInfo.flag & ICONE_PASSWORDED)
						{
							glTexCoord2f(.5f,1);
							glVertex2i(16,0);
							glTexCoord2f(.5f,.5f);
							glVertex2i(16,16);
							glTexCoord2f(1,.5f);
							glVertex2i(32,16);
							glTexCoord2f(1,1);
							glVertex2i(32,0);
						}
						if (gameInfo.flag & ICONE_FRIENDLYFIRE)
						{
							glTexCoord2f(0,.5f);
							glVertex2i(32,0);
							glTexCoord2f(0,0);
							glVertex2i(32,16);
							glTexCoord2f(.5f,0);
							glVertex2i(48,16);
							glTexCoord2f(.5f,.5f);
							glVertex2i(48,0);
						}
						if (gameInfo.flag & ICONE_3DVIEW)
						{
							glTexCoord2f(.5f,.5f);
							glVertex2i(48,0);
							glTexCoord2f(.5f,0);
							glVertex2i(48,16);
							glTexCoord2f(1,0);
							glVertex2i(64,16);
							glTexCoord2f(1,.5f);
							glVertex2i(64,0);
						}
					glEnd();
				glPopMatrix();
			glPopAttrib();
		}
		else
		{
			break;
		}
	}

	// Le nb de game
	glColor3f(1,1,1);
	printRightText(790, 25, 25, CString(gameVar.lang_nbGamesRefreshed.s, (int)m_games.size(), nbTotalGame));

	// Le temps avant next refresh
	if (refreshDelay > 0)
	{
		glColor3f(1,1,1);
		printLeftText(200, 25, 25, CString(gameVar.lang_canRefreshIn.s, (int)(refreshDelay+.5f)));
	}

	//--- Si on n'a pas trouvé de game
	if (nbTotalGame == 0 && !problemConnectingToMaster && m_games.size() == 0)
	{
		glColor3f(1,1,1);
		printCenterText(400, 300, 30, gameVar.lang_noGameFound);
	}
	else if (problemConnectingToMaster && m_games.size() == 0)
	{
		glColor3f(1,.5f,0);
		printCenterText(400, 300, 30, gameVar.lang_problemConnectingToMaster);
		printCenterText(400, 330, 30, CString("Check for recent events on www.rndlabs.ca"));
	}

	// On render le scroll button
	//--- Le nb de page en tout
	float nbPage = (float)m_games.size() / 14.0f;
	if (nbPage > 1.0f)
	{
		float scroll_percent = ((float)scroll_pos / ((float)m_games.size() - 14));

		//--- On calcul la grosseur que prendra notre scroll button
		float scrollBar_size = 350.0f / nbPage;
		scrollBar_size = (float)((int)scrollBar_size);
		if (scrollBar_size < 35) scrollBar_size = 35;

		//--- Sa position
		float scrollBar_pos = (350.0f - scrollBar_size) * scroll_percent;

		scrollBar_pos = (float)((int)scrollBar_pos);

		//--- On l'affiche ici
		glBegin(GL_QUADS);
			glColor4f(0,0,0, .75f);
			glVertex2f(735, 100);
			glVertex2f(735, 100 + 350);
			glVertex2f(735+30, 100 + 350);
			glVertex2f(735+30, 100);
			glColor4f(.5f,1,0, .75f);
			glVertex2f(735+5, 100 + scrollBar_pos+5);
			glVertex2f(735+5, 100 + scrollBar_pos + scrollBar_size-5);
			glVertex2f(735+30-5, 100 + scrollBar_pos + scrollBar_size-5);
			glVertex2f(735+30-5, 100 + scrollBar_pos+5);
		glEnd();
	}
}


void JoinGame::updateUnique(float delay)
{
	animSearchingPing++;
	if (animSearchingPing>=40) animSearchingPing = 0;

	if (firstTime)
	{
		// 10sec between each refresh (minimum)
		refreshDelay = 1;

	//	stSqlQuery sqlQuery;
	//	sqlQuery.GameID = GAME_ID_BV2;
	//	strcpy(sqlQuery.QueryString, "Select * from BV2;");

		masterServerID = bb_peerSend(MASTER_ADRESS, 11112, /*(char*)(&sqlQuery)*/0, BV2_LIST/*SQL_QUERY*/, 0/*sizeof(stSqlQuery)*/, true);
		sendTimeOut = 2;
		console->add(CString("\x3 Master Server ID : %i", masterServerID));
		if (masterServerID == 0) 
		{
			sendTimeOut = 0;
			console->add("\x4 Error > can not connect to master");
			console->add(CString("\x9 BaboNetError : %s", bb_peerGetLastError()));
			masterServerID = -1;
			stoped = false;
			scene->masterReady = false;
			bJustRefreshed = false;
		}
		firstTime = false;
	}



//	console->add(CString("%i", masterServerID));

	clickDelay -= delay;

	if (sendTimeOut > 0)
	{
		sendTimeOut -= delay;
		if (sendTimeOut < 0)
		{
			sendTimeOut = 0;
			if (masterServerID > 0 && bJustRefreshed)
			{
				bb_peerDelete(masterServerID);
			}
		}
	}

	//--- On disable le refresh si on ne peut pas l'utiliser
	if (masterServerID == -1 && refreshDelay == 0)
	{
		btn_refresh->m_rect[1] = 25;
		btn_refresh->m_rect[3] = 50;
	}
	else
	{
		// Haha on le cache (DUCK TAPE!!!)
		btn_refresh->m_rect[1] = -200;
		btn_refresh->m_rect[3] = -175;
	}

	if (dkiGetMouseWheelVel() < 0)
	{
		scroll_pos += 3;
	}
	if (dkiGetMouseWheelVel() > 0)
	{
		scroll_pos -= 3;
	}

	// Clamp scroll
	if (scroll_pos + 14 > (int)m_games.size())
	{
		scroll_pos = (int)m_games.size() - 14;
	}
	if (scroll_pos < 0) scroll_pos = 0;

	// On check pour le mouse over
	CVector2i mousePos = dkwGetCursorPos();
	CVector2i res = dkwGetResolution();
	mousePos[0] = (int)((800.0f / (float)res[0]) * (float)mousePos[0]);
	mousePos[1] = (int)((600.0f / (float)res[1]) * (float)mousePos[1]);
	if (mousePos[0] > 25 && mousePos[0] < 725 &&
		mousePos[1] > 100 && mousePos[1] < 450)
	{
		scroll_mouseOver = scroll_pos + (mousePos[1]-100) / 25;
	}
	else scroll_mouseOver = -1;
	if (scroll_mouseOver < 0) scroll_mouseOver = -1;
	if (scroll_mouseOver >= (int)m_games.size()) scroll_mouseOver = -1;
	if (scroll_mouseOver != -1)
	{
		if (dkiGetState(DKI_MOUSE_BUTTON1) == DKI_DOWN)
		{
			if (oldMousePos == mousePos && clickDelay > 0)
			{
				onClick(btn_launch);
			}
			else
			{
				oldMousePos = mousePos;
				clickDelay = .5f;
				scroll_selected = scroll_mouseOver;
				txt_gameIP->set((char*)m_games[scroll_selected].ipAdress.c_str());
				txt_gameIP->changeCaption(CString((char*)m_games[scroll_selected].ipAdress.c_str()));
				gameVar.cl_port = m_games[scroll_selected].port;
				gamePassword = m_games[scroll_selected].password.c_str();
			}
		}
	}

	// On update le scroll button
	//--- Le nb de page en tout
	float nbPage = (float)m_games.size() / 14.0f;
	if (nbPage > 1.0f)
	{
		float scroll_percent = ((float)scroll_pos / ((float)m_games.size() - 14));

		//--- On calcul la grosseur que prendra notre scroll button
		float scrollBar_size = 350.0f / nbPage;
		scrollBar_size = (float)((int)scrollBar_size);
		if (scrollBar_size < 35) scrollBar_size = 35;

		//--- Sa position
		float scrollBar_pos = (350.0f - scrollBar_size) * scroll_percent;

		scrollBar_pos = (float)((int)scrollBar_pos);

		//--- On check pour la mouse ici
		for(int i=0;i<1;++i)
		{
			if (dkiGetState(DKI_MOUSE_BUTTON1) &&
				mousePos[0] > 735 &&
				mousePos[0] < 735+30 &&
				mousePos[1] > 100 &&
				mousePos[1] < 100+350)
			{
				if (mousePos[1] <= 100 + (int)scrollBar_size/2)
				{
					scroll_pos = 0;
					break;
				}
				if (mousePos[1] >= 100+350 - (int)scrollBar_size/2)
				{
					scroll_pos = (int)m_games.size() - 14;
					break;
				}

				float clickPosPercent = (float)(mousePos[1] - (100+(int)scrollBar_size/2)) / (350.0f - scrollBar_size);
				scroll_pos = (int)(clickPosPercent * ((float)m_games.size() - 14));
			}
			// Clamp scroll
			if (scroll_pos + 14 > (int)m_games.size())
			{
				scroll_pos = (int)m_games.size() - 14;
			}
			if (scroll_pos < 0) scroll_pos = 0;
		}
	}
}



//
// Si on clic sur un bouton
//
void JoinGame::onClick(Control * control)
{
	if (control == btn_launch)
	{
		if (masterServerID != -1 && !stoped)
		{
			// STOP IT
			bb_peerSend(masterServerID, 0, STOP_QUERY, 0, true);
			bb_peerDelete(masterServerID);
			stoped = true;
		}
		gameVar.cl_lastUsedIP = txt_gameIP->s;

		//--- Retreive the password for this game
		gameVar.cl_password = "";
		for (int i=0;i<(int)m_games.size();++i)
		{
			if (gameVar.cl_lastUsedIP == m_games[i].ipAdress.c_str())
			{
				gameVar.cl_password = m_games[i].password.c_str();
				break;
			}
		}
		if (gameVar.cl_password.isNull())
		{
			console->sendCommand(CString("connect %s %i", gameVar.cl_lastUsedIP.s, gameVar.cl_port));
		}
		else
		{
			rootMenu->currentMenu = MENU_PASSWORD;
		}
		return;
	}
	if (control == btn_back)
	{
		if (masterServerID != -1 && !stoped) 
		{
			// STOP IT
			bb_peerSend(masterServerID, 0, STOP_QUERY, 0, true);
			bb_peerDelete(masterServerID);
			stoped = true;
		}
		rootMenu->currentMenu = MENU_MAIN;
		return;
	}
	if (control == btn_quickRefresh)
	{
	/*	for (int i=0;i<(int)m_games.size();++i)
		{
			//--- On send les ping
			m_games[i].pingPeerID = bb_peerSend((char*)(m_games[i].ipAdress.c_str()), m_games[i].port, 0, 1, 0, true);
			m_games[i].ping = -2;
			if (m_games[i].pingPeerID == 0)
			{
				//--- Oups
			}
			else
			{
				m_games[i].pingFrame = scene->frameID;
				//--- On le delete tout suite, puis apres on attends juste confirmation du delete
				bb_peerDelete(m_games[i].pingPeerID);
			}
		}*/
		return;
	}
	if (control == btn_refresh)
	{
		//--- Thats it, on clear toute sti
		m_games.clear();

		// On a déjà une connection avec le master, faq mange un char
		if (masterServerID != -1 || refreshDelay > 0) return;
		problemConnectingToMaster = false;

		//--- Clear notre liste
		bJustRefreshed = true;
	//	m_games.clear();

		//--- On stop current request au master, et on en spawn un nouveau
	//	if (masterServerID > 0) bb_peerDelete(masterServerID);
	//	masterServerID = -1;

		//--- On send notre request au master server
		if (chk_network->getChoice() == gameVar.lang_internet)
		{

			// 10sec between each refresh (minimum)
			refreshDelay = 1;

		//	stSqlQuery sqlQuery;
		//	sqlQuery.GameID = GAME_ID_BV2;
		//	strcpy(sqlQuery.QueryString, "Select * from BV2;");

			masterServerID = bb_peerSend(MASTER_ADRESS, 11113, /*(char*)(&sqlQuery)*/0, BV2_LIST/*SQL_QUERY*/, 0/*sizeof(stSqlQuery)*/, true);
			sendTimeOut = 2;
		//	console->add(CString("\x3 Master Server ID : %i", masterServerID));
			if (masterServerID == 0) 
			{
				sendTimeOut = 0;
				console->add(CString("\x3 Master Server ID : %i", masterServerID));
				console->add("\x4 Error > can not connect to master");
				console->add(CString("\x9 BaboNetError : %s", bb_peerGetLastError()));
				masterServerID = -1;
				stoped = false;
				scene->masterReady = false;
				bJustRefreshed = false;
			}
			else if (masterServerID == -1)
			{
				sendTimeOut = 0;
				console->add(CString("\x3 Master Server ID : %i", masterServerID));
				console->add("\x4 Error > can not connect to master");
				console->add(CString("\x9 BaboNetError : %s", bb_peerGetLastError()));
				masterServerID = -1;
				stoped = false;
				scene->masterReady = false;
				bJustRefreshed = false;
			}
		}
		else if (chk_network->getChoice() == gameVar.lang_localNetwork)
		{
			//--- On send un broadcast query
			long bv2ID = UNIQUE_BV2_ID;
			bb_peerSend(NET_BROADCAST, (char*)(&bv2ID), NET_BROADCAST_QUERY, sizeof(long), true); // Last param ignored
		}
	}
	// Pour sorter
	if (control == btn_gameName)
	{
		if (CGameInfo::sortBy == SORT_GAME_NAME)
		{
			std::reverse(m_games.begin(), m_games.end());
			return;
		}
		CGameInfo::sortBy = SORT_GAME_NAME;
		std::sort(m_games.begin(), m_games.end());
	}
	if (control == btn_nbPlayer)
	{
		if (CGameInfo::sortBy == SORT_NB_PLAYER)
		{
			std::reverse(m_games.begin(), m_games.end());
			return;
		}
		CGameInfo::sortBy = SORT_NB_PLAYER;
		std::sort(m_games.begin(), m_games.end());
	}
	if (control == btn_gameType)
	{
		if (CGameInfo::sortBy == SORT_GAME_TYPE)
		{
			std::reverse(m_games.begin(), m_games.end());
			return;
		}
		CGameInfo::sortBy = SORT_GAME_TYPE;
		std::sort(m_games.begin(), m_games.end());
	}
	if (control == btn_mapName)
	{
		if (CGameInfo::sortBy == SORT_MAP_NAME)
		{
			std::reverse(m_games.begin(), m_games.end());
			return;
		}
		CGameInfo::sortBy = SORT_MAP_NAME;
		std::sort(m_games.begin(), m_games.end());
	}
	if (control == btn_ping)
	{
		if (CGameInfo::sortBy == SORT_PING)
		{
			std::reverse(m_games.begin(), m_games.end());
			return;
		}
		CGameInfo::sortBy = SORT_PING;
		std::sort(m_games.begin(), m_games.end());
	}
}



//
// Recv data from the master server (list game !)
//
void JoinGame::RecvDataFromMaster(char* dataBuffer, int typeID, long frameID)
{
	if (bJustRefreshed)
	{
		bJustRefreshed = false;
	//	m_games.clear();
	}
	stBV2row bv2row;
	memcpy(&bv2row, dataBuffer, sizeof(stBV2row));

	m_games.push_back(CGameInfo(
		bv2row.serverName, 
		bv2row.password, 
		bv2row.nbPlayer, 
		bv2row.maxPlayer, 
		bv2row.map, 
		bv2row.gameType, 
		-2, 
		bv2row.flags, 
		bv2row.ip,
		bv2row.port));

	m_games[m_games.size()-1].pingPeerID = bb_peerSend(bv2row.ip, bv2row.port/* + 1*/, 0, 1, 0, true);
	if (m_games[m_games.size()-1].pingPeerID == 0)
	{
		//--- Oups
	//	console->add("\x4> Cannot send ping to game");
	}
	else
	{
		m_games[m_games.size()-1].pingFrame = frameID;
		//--- On le delete tout suite, puis apres on attends juste confirmation du delete
		bb_peerDelete(m_games[m_games.size()-1].pingPeerID);
	}

	//--- Si on attein le max des games, on peur deleter le peer
	if ((int)m_games.size() == nbTotalGame && masterServerID != -1)
	{
		bb_peerDelete(masterServerID, true);
		masterServerID = -1;
		scene->masterReady = true;
	}
}



//
// On passe le frameID aussi pour pinger la game hehe
//
void JoinGame::RecvDataFromLan(char* bv2RowBuffer, long frameID)
{
	if (chk_network->getChoice() != gameVar.lang_localNetwork) return;

	if (bJustRefreshed)
	{
		bJustRefreshed = false;
	//	m_games.clear();
	}
	stBV2row bv2row;
	memcpy(&bv2row, bv2RowBuffer, sizeof(stBV2row));

	m_games.push_back(CGameInfo(
		bv2row.serverName, 
		bv2row.password, 
		bv2row.nbPlayer, 
		bv2row.maxPlayer, 
		bv2row.map, 
		bv2row.gameType, 
		999, 
		bv2row.flags, 
		bv2row.ip,
		bv2row.port));

	//--- On le ping asteur
	m_games[m_games.size()-1].pingPeerID = bb_peerSend(bv2row.ip, bv2row.port/* + 1*/, 0, 1, 0, true);
	if (m_games[m_games.size()-1].pingPeerID == 0)
	{
		//--- Oups
	}
	else
	{
		m_games[m_games.size()-1].pingFrame = frameID;
		//--- On le delete tout suite, puis apres on attends juste confirmation du delete
		bb_peerDelete(m_games[m_games.size()-1].pingPeerID);
	}

	// On n'a pas vraiment de max de game icite à atteindre
}
#endif

