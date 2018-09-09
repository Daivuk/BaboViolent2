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

#include "Scene.h"
#include "Console.h"
#ifndef DEDICATED_SERVER
#include "JoinGame.h"
#include "CStatus.h"
#endif
#include <fstream>


//extern unsigned long masterServerID;



#ifndef DEDICATED_SERVER
//
// Pour cr�r, join, disconnecter d'une game
//
void Scene::host(CString mapName)
{
	disconnect();
	console->add("\x3> Creating Server");
	server = new Server(new Game(mapName)); // On connect notre jeu au server
	console->add("\x3> Creating Client");
	client = new Client(new Game()); // On connect notre jeu au client
	client->isServer = true;

	// On start le server
	if (!server->host())
	{
		disconnect();
		console->add("\x4> Error creating server");
	//	menu->show();
		menuManager.root->visible = true;
		return;
	}
	else
	{
		console->add(CString("\x3> Server Created on port %i", gameVar.sv_port));
	}

	// Voil� maintenant on le join
	if (!client->join("127.0.0.1", gameVar.sv_port))
	{
		disconnect();
		console->add("\x4> Error connecting to server. Verify the IP adress");
	//	menu->show();
		menuManager.root->visible = true;
		return;
	}
	else
	{
	//	console->add("\x3> Connection successfull");
	//	menu->hide();
		menuManager.root->visible = false;
	}
}
#endif

void Scene::dedicate(CString mapName)
{
	disconnect();
	console->add("\x3> Creating Server");
	server = new Server(new Game(mapName)); // On connect notre jeu au server

	// On start le server
	if (!server->host())
	{
		disconnect();
		console->add("\x4> Error creating server");
#ifndef DEDICATED_SERVER
	//	menu->show();
		menuManager.root->visible = true;
#endif
	}
	else
	{
		console->add(CString("\x3> Server Created on port %i", gameVar.sv_port));
#ifndef DEDICATED_SERVER
	//	menu->hide();
		menuManager.root->visible = false;
#endif
	}
}

#ifndef DEDICATED_SERVER

void Scene::join(CString IPAddress, int port, CString password/*, CString adminRequest*/)
{
	disconnect();
	console->add("\x3> Creating Client");
	client = new Client(new Game()); // On connect notre jeu au client

/*	if (!adminRequest.isNull())
	{
		client->adminUserPassRequest = adminRequest;
		client->requestedAdmin = true;
	}*/

	// Voil� maintenant on le join
	if (!client->join(IPAddress, port, password))
	{
		disconnect();
		console->add("\x4> Error connecting to server. Verify the IP address");
	//	menu->show();
		menuManager.root->visible = true;
	}
	else
	{
	//	console->add("\x3> Connection successfull");
	//	menu->hide();
		menuManager.root->visible = false;
	}
}



void Scene::edit(CString command)
{
	CString mapName = command.getFirstToken(' ');
	int w,h;
	CString token = command.getFirstToken(' ');
	if (token.isNull()) w = 0;
	else w = token.toInt();
	token = command.getFirstToken(' ');
	if (token.isNull()) h = 0;
	else h = token.toInt();
	disconnect();
	console->add("\x3> Creating Editor");
//	ZEVEN_SAFE_DELETE(menuManager.root);
	editor = new Editor(mapName, font, w, h); // On connect notre jeu au client
//	menu->hide();
	menuManager.root->visible = false;
}
#endif


void Scene::disconnect()
{
#ifndef DEDICATED_SERVER
	if (client && !server) dksvarLoadConfigSVOnly("main/bv2.cfg");

	status->set(CStatus::ONLINE);

	bool wrongVersion = false;
	if (client) wrongVersion = client->wrongVersionReason;
	console->add("\x3> Disconnecting from current server");
	if (client) console->add("\x3> Shutdowning Client");
	ZEVEN_SAFE_DELETE(client);
#endif
	bool wasServer = false;
	if (server) {console->add("\x3> Shutdowning Server");wasServer=true;}
	ZEVEN_SAFE_DELETE(server);
#ifndef DEDICATED_SERVER
	if (editor) console->add("\x3> Shutdowning Editor");
	ZEVEN_SAFE_DELETE(editor);
	console->unlock(); // Petit bug quand on chattait ;)
//	menu->show();
	menuManager.root->visible = true;
//	if (wrongVersion) menu->currentMenu = MENU_WRONG_VERSION;
#endif
}



// Pour chatter
void Scene::sayall(CString message)
{
#ifndef DEDICATED_SERVER
	if (client) client->sayall(message);
	else 
#endif
		if (server) server->sayall(message);
}

#ifndef DEDICATED_SERVER
void Scene::sayteam(CString message)
{
	if (client) client->sayteam(message);
}
#endif


// pour kicker un FUCKING cheateux
void Scene::kick(CString playerName)
{
	if (server)
	{
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (server->game)
			{
				if (server->game->players[i])
				{
					if (textColorLess(server->game->players[i]->name) == textColorLess(playerName))
					{
						if( master ) master->RA_DisconnectedPlayer( textColorLess(playerName).s, server->game->players[i]->playerIP, (long)server->game->players[i]->playerID );

						bb_serverDisconnectClient(server->game->players[i]->babonetID);
						console->add(CString("\x3> Disconnecting client %s (%s), kicked by server", server->game->players[i]->name.s, server->game->players[i]->playerIP), true);
						ZEVEN_SAFE_DELETE(server->game->players[i]);
						net_svcl_player_disconnect playerDisconnect;
						playerDisconnect.playerID = (char)i;
						bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
						continue;
					}
				}
			}
		}
	}
}


void Scene::kick(int ID)
{
	if (server)
	{
		if (server->game)
		{
			if (server->game->players[ID])
			{
				if( master ) master->RA_DisconnectedPlayer( textColorLess(server->game->players[ID]->name).s, server->game->players[ID]->playerIP, (long)server->game->players[ID]->playerID );
				bb_serverDisconnectClient(server->game->players[ID]->babonetID);
				console->add(CString("\x3> Disconnecting client %s (%s), kicked by server", server->game->players[ID]->name.s, server->game->players[ID]->playerIP), true);
				ZEVEN_SAFE_DELETE(server->game->players[ID]);
				net_svcl_player_disconnect playerDisconnect;
				playerDisconnect.playerID = (char)ID;
				bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
			}
		}
	}
}

void Scene::ban(CString playerName)
{
	if (server)
	{
		for (int i=0;i<MAX_PLAYER;++i)
		{
			if (server->game)
				if (server->game->players[i])
			{
				if (textColorLess(server->game->players[i]->name) == textColorLess(playerName))
				{
					ban(server->game->players[i]->playerID);
				}
			}
		}
	}
}

void Scene::ban(int ID)
{
	if (server)
		if (server->game)
			if (server->game->players[ID])
	{

		if( master ) master->RA_DisconnectedPlayer( textColorLess(server->game->players[ID]->name).s, server->game->players[ID]->playerIP, (long)server->game->players[ID]->playerID );
		bb_serverDisconnectClient(server->game->players[ID]->babonetID);
		server->banList.push_back(std::pair<CString,CString>(server->game->players[ID]->name,server->game->players[ID]->playerIP) );
		
		std::ofstream file("main/banlist", std::ios::app | std::ios::binary);
		CString name = server->game->players[ID]->name;
		name.resize(32);
		file.write(name.s, sizeof(char)*32);
		file.write(server->game->players[ID]->playerIP, sizeof(char)*16);

		console->add(CString("\x3> Disconnecting client %s (%s), banned by server", server->game->players[ID]->name.s, server->game->players[ID]->playerIP), true);
		ZEVEN_SAFE_DELETE(server->game->players[ID]);
		net_svcl_player_disconnect playerDisconnect;
		playerDisconnect.playerID = (char)ID;
		bb_serverSend((char*)&playerDisconnect,sizeof(net_svcl_player_disconnect),NET_SVCL_PLAYER_DISCONNECT,0);
	}
}

void Scene::banIP(CString playerIP)
{
	if (server)
	{
		CString playerName("MANUAL-IP-BAN");
		server->banList.push_back(std::pair<CString,CString>(playerName,playerIP) );

		std::ofstream file("main/banlist", std::ios::app | std::ios::binary);
		playerName.resize(32);
		playerIP.resize(16);
		file.write(playerName.s, sizeof(char)*32);
		file.write(playerIP.s, sizeof(char)*16);
	}
}

void Scene::unban(int banID)
{
	if(server)
		if (server->game)
	{
		// First, remove entry
		server->banList.erase(server->banList.begin() + banID);

		// Then rewrite file
		std::ofstream file("main/banlist", std::ios::trunc | std::ios::binary);
		CString name;

		for(std::size_t i = 0; i < server->banList.size(); ++i)
		{
			// Ensure name is 32 characters
			name = server->banList[i].first;
			name.resize(32);
			
			// Write data
			file.write(name.s, sizeof(char)*32);
			file.write(server->banList[i].second.s, sizeof(char)*16);
		}
	}
}


