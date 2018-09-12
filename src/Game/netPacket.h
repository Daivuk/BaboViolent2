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

#ifndef NETPACKET_H
#define NETPACKET_H



#include "cMSstruct.h"
#include <stdint.h>

// Quand le client recois un ping, il renvoit un pong
#define NET_CLSV_PONG 1
struct net_clsv_pong
{
	char playerID; // Le ID du joueur concern�
//	char bidon[31];
};

// Le client est pret � spawner (apparaitre)
// Il va envoyer une request au server, et ce dernier va d�cider O� il spawn
#define NET_CLSV_SPAWN_REQUEST 2
struct net_clsv_spawn_request
{
	char playerID; // Le ID du joueur concern�
	char weaponID; // Le ID du gun avec lequel spawner
	char meleeID;

	// Skin info
	char skin[7];

	//--- Les couleurs custom du babo
	unsigned char redDecal[3];
	unsigned char greenDecal[3];
	unsigned char blueDecal[3];
};

// Le client tire du fusil (activit� commune chez les cocassien)
// Le server et les clients connaissent le fusil utilis� par le joueur, donc pas besoin de l'envoyer
#define NET_CLSV_PLAYER_SHOOT 3
struct net_clsv_player_shoot
{
	char playerID; // Le ID du joueur
	char weaponID; // Le ID du type de gun
	char nuzzleID; // le ID du nuzzle du fusil qui l'a tir�
	short p1[3]; // Le point du d�but du ray
	short p2[3]; // Le point de la fin du ray
};

// La version du server est accept� par le client
#define NET_CLSV_GAMEVERSION_ACCEPTED 4
struct net_clsv_gameversion_accepted
{
	char playerID;		// Le ID du joueur
	char password[16];	// Password
};

// On demande au server de pickuper un item par terre
#define NET_CLSV_PICKUP_REQUEST 5
struct net_clsv_pickup_request
{
	char playerID; // Le ID du joueur en question
};

// On demande au server d'�tre admin!
#define NET_CLSV_ADMIN_REQUEST 6
#if defined(_PRO_)
struct net_clsv_admin_request
{
	char login[33];		//md5
	char password[33];	//md5
};
#endif

// Vote
#define NET_CLSV_VOTE 7
struct net_clsv_vote
{
	bool value;
	char playerID;
};

// On map list request
#define NET_CLSV_MAP_LIST_REQUEST 8
struct net_clsv_map_list_request
{
	char playerID;
	bool all;
};

// Le server accept une new connection, il envoit � tout le monde le ID du joueur
#define NET_SVCL_NEWPLAYER 101
struct net_svcl_newplayer
{
	char newPlayerID; // Le ID du nouveau Joueur (de 0 � 31)
	int32_t baboNetID;
};

// Le server envoit ses info aux nouveaux clients
// Suivi de �a, il va lui envoyer la liste de tout les joueurs
// � partir de ce moment l�, le joueur devra recevoir tout les messages du server
// Pour garder l'�tat du server � jour
#define NET_SVCL_SERVER_INFO 102
struct net_svcl_server_info
{
	int32_t mapSeed; // Le seed de la map, pour le random
	char mapName[16]; // 15 + '\0'

	// Le type de parti
	char gameType;

	// Les score
	short blueScore;
	short redScore;
	short blueWin;
	short redWin;
};

// Le server fou le camp, il le dit � tout le monde (en moins quil plante l�)
#define NET_SVCL_SERVER_DISCONNECT 103
/*struct net_svcl_server_disconnect
{
	// Pas grand truc � mettre ici ! Me semble que le message est clair ;)
};*/

// Le client fou le camp, le server le sait tout suite, et le shoot au autres 
// (en moins quil plante l�)
#define NET_SVCL_PLAYER_DISCONNECT 104
struct net_svcl_player_disconnect
{
	char playerID; // Le ID du joueur
};

// Le client fou le camp, le server le sait tout suite, et le shoot au autres 
// (en moins quil plante l�)
#define NET_SVCL_PLAYER_ENUM_STATE 105
struct net_svcl_player_enum_state
{
	char playerID; // Le ID du joueur
	char playerName[31+1]; // Le nom du joueur, 31 + \0 caract�res
	char teamID; // Son team
	char status; // Son status
	short kills;
	short deaths;
	short score; // Son score
	short returns;
	short flagAttempts;
	short damage;
	float life; // Sa vie
	float dmg;
	char weaponID; // Le gun qu'il a
	char playerIP[16];
	int32_t babonetID;

	// Skin info
	char skin[7];

	//--- Les couleurs custom du babo
	unsigned char redDecal[3];
	unsigned char greenDecal[3];
	unsigned char blueDecal[3];
};

// Le server envoit un ping � toute les seconde � tout les joueurs
#define NET_SVCL_PING 106
struct net_svcl_ping
{
	char playerID; // Le ID du joueur concerv�
//	char bidon[31];
};

// Le server envoit � chaque seconde le ping de toute le monde � tout le monde
#define NET_SVCL_PLAYER_PING 107
struct net_svcl_player_ping
{
	char playerID; // Le ID du joueur
	short ping; // Son ping avec le server, en miliseconde
};

// Le server re�ois la request de spawner du joueur, et renvoit � TOUT le monde
// sa position de spawn
#define NET_SVCL_PLAYER_SPAWN 108
struct net_svcl_player_spawn
{
	char playerID; // Le ID du joueur
	char weaponID; // Le ID du gun avec lequel spawner
	char meleeID; // Le ID du melee gun avec lequel spawner
	short position[3]; // La position o� il spawn

	// Skin info
	char skin[7];

	//--- Les couleurs custom du babo
	unsigned char redDecal[3];
	unsigned char greenDecal[3];
	unsigned char blueDecal[3];
};

// Le server modifie une variable sv_, il va l'envoyer � tout le monde
// Les sv ce sont les seuls qui affectent le gameplay et que le server garde l'exclusivit�e
#define NET_SVCL_SV_CHANGE 109
struct net_svcl_sv_change
{
	char svChange[79+1]; // 80 c'est assez
};

// Un client a tir�, le server a effectu� la collision et renvoi le r�sultat aux autres joueurs
#define NET_SVCL_PLAYER_SHOOT 110
struct net_svcl_player_shoot
{
	char playerID; // Le ID du joueur qui l'a tir�
	char hitPlayerID; // Le ID du joueur qu'on a touch�, si -1 on a touch� un mur
	char nuzzleID; // le ID du nuzzle du fusil qui l'a tir� (pour savoir o� spawner le feu)
	char weaponID; // Le ID du type de gun
	short p1[3]; // Le point du d�but du ray
	short p2[3]; // Le point de la fin du ray (point d'impact)
	char normal[3]; // La normal de l'impact
};

// Pour dire qu'on supprime un projectile
#define NET_SVCL_DELETE_PROJECTILE 111
struct net_svcl_delete_projectile
{
	int32_t projectileID; // Son ID dans le vector
};

// La position d'un projectile (uniquement control� par le server)
#define NET_SVCL_PROJECTILE_COORD_FRAME 112
struct net_svcl_projectile_coord_frame
{
	int32_t uniqueID; // Le ID unique du projectile
	short projectileID; // Le ID du projectile concern�
	int32_t frameID; // Le frame auquel �a a �t� envoy� (on en a de besoin pour cr�er des belles interpolations)
	short position[3]; // Sa position
	char vel[3]; // Sa velocity
//	int32_t uniqueProjectileID;
	// Sa rotation sur l'axe est calcul� c�t� client, vu que c uniquement visuel
};

// Pour spawner une explosion
#define NET_SVCL_EXPLOSION 113
struct net_svcl_explosion
{
	float position[3]; // La position de l'explosion dans map
	float normal[3]; // L'orientation de l'explosion
	float radius; // Sa puissance !! (�a va aussi faire shaker la vue :P)
	char playerID;
	// On ne dit pas qui l'a provoqu� et tout, c'est le server qui va faire les hits
};

// Si on a touch� un joueur l'hors d'une explosion par exemple
#define NET_SVCL_PLAYER_HIT 114
struct net_svcl_player_hit
{
	char playerID; // Le joueur touch�
	char fromID; // De qui �a vient
	char weaponID; // Le type d'arme utilis�
	float damage; // ne pas oublier le damage inflig� ! ** New, la vie restante **
	char vel[3]; // La velocity qu'on recoit par le coup
};

// Le server emet un son et veut que les clients le jou
#define NET_SVCL_PLAY_SOUND 115
struct net_svcl_play_sound
{
	char soundID;
	unsigned char volume;
	char range;
	unsigned char position[3];
};

// La version du server
#define NET_SVCL_GAMEVERSION 116
struct net_svcl_gameversion
{
	uint32_t gameVersion;
};

// Pour synchroniser le temps des horloges du jeu
#define NET_SVCL_SYNCHRONIZE_TIMER 117
struct net_svcl_synchronize_timer
{
	int32_t frameID;
	float gameTimeLeft;
	float roundTimeLeft;
};

// Pour Changer l'�tat d'un flag
#define NET_SVCL_CHANGE_FLAG_STATE 118
struct net_svcl_change_flag_state
{
	char flagID; // 0 ou 1
	char newFlagState; // Son nouvel �tat
	char playerID; // Le ID du player qui a effectu� l'action
};

// Un joueur est mort ou disconnect�, il laisse tomber le flag
// Le server communique alors la position exacte aux autres players
#define NET_SVCL_DROP_FLAG 119
struct net_svcl_drop_flag
{
	char flagID;
	float position[3];
};

// Un joueur join la game, on send juste le enum state
#define NET_SVCL_FLAG_ENUM 120
struct net_svcl_flag_enum
{
	char flagState[2];
	float positionBlue[3];
	float positionRed[3];
};

// On change le state du round
#define NET_SVCL_GAME_STATE 121
struct net_svcl_round_state
{
	char newState;
	char reInit; // Pour restarter le round � neuf ou en parti (trace de sang, vie, etc)
};

// On change le type de game
#define NET_SVCL_CHANGE_GAME_TYPE 122
struct net_svcl_change_game_type
{
	char newGameType;
};

// Le server change de map, il le dit aux autres !
#define NET_SVCL_MAP_CHANGE 123
struct net_svcl_map_change
{
	char mapName[16]; // 15 + '\0'
	char gameType; // Le type dla game
};

// Un joueur ramasse un item, on le dit � tout le monde
#define NET_SVCL_PICKUP_ITEM 124
struct net_svcl_pickup_item
{
	char playerID;
	char itemType;
	char itemFlag;
};

// Un joueur passe sur une flame, la flame se colle sur lui
#define NET_SVCL_FLAME_STICK_TO_PLAYER 125
struct net_svcl_flame_stick_to_player
{
	short projectileID; // Le ID unique du projectile
	char playerID; // Le ID du joueur sur qui �a stick
};

// La console envoit les messages console aux admin
#define NET_SVCL_CONSOLE 126

// Le server accept le user/pass du admin
#define NET_SVCL_ADMIN_ACCEPTED 127

// Le server averti qu'il va y avoir un auto-balance dans 15secondes.
#define NET_SVCL_AUTOBALANCE 128

// Le server met fin au vote
#define NET_SVCL_END_VOTE 129

// Le server update le voting status
#define NET_SVCL_UPDATE_VOTE 130
struct net_svcl_update_vote
{
	char nbYes;
	char nbNo;
};

// Le server shoot le r�sultat des votes
#define NET_SVCL_VOTE_RESULT 131
struct net_svcl_vote_result
{
	bool passed;
};

#define NET_SVCL_MSG 132
struct net_svcl_msg
{
	char msgDest; // where msg should be displayed, 0 - chat
	char teamID; // -2 - all, -1 - spectators, 1 - blue, 2 - red
	char message[49+80+1]; // Null terminated string. De 79 + \0 caract�res
};

#define NET_SVCL_PLAYER_UPDATE_STATS 133
struct net_svcl_player_update_stats
{
	char playerID;
	short kills;
	short deaths;
	short score; // Son score
	short returns;
	short flagAttempts;
	float timePlayedCurGame;
};

#define NET_SVCL_BAD_CHECKSUM_ENTITY 134
struct net_svcl_bad_checksum_entity
{
	int id;
	char name[32];
	char playerIP[16];
};

#define NET_SVCL_BAD_CHECKSUM_INFO 135
struct net_svcl_bad_checksum_info
{
	int number;
};


// Le client recois son ID, il envoit ses info (player name, etc), 
// et le server le renvois aux autres
#define NET_CLSV_SVCL_PLAYER_INFO 201
struct net_clsv_svcl_player_info
{
	char playerID; // Le ID du joueur
	char playerIP[16];
	char playerName[31+1]; // Le nom du joueur, 31 + \0 caract�res
	char username[21];		// Account username
	char password[32];		// Account password (MD5)
	char macAddr[20]; // player's mac adress, mouhouha
};

// Le client �cris un message, l'envoit au server
#define NET_CLSV_SVCL_CHAT 202
struct net_clsv_svcl_chat
{
	char teamID; // -1 for all, >= 0 for team ID
	char message[49+80+1]; // Null terminated string. De 79 + \0 caract�res
};

// Le client veux changer de team, il le demande d'abords au server
#define NET_CLSV_SVCL_TEAM_REQUEST 203
struct net_clsv_svcl_team_request
{
	char playerID; // Son ID
	char teamRequested; // L'etat quil demande
};

// La position du joueur
#define NET_CLSV_SVCL_PLAYER_COORD_FRAME 204
struct net_clsv_svcl_player_coord_frame
{
	char playerID; // Le ID du joueur concern�
	int32_t frameID; // Le frame auquel �a a �t� envoy� (on en a de besoin pour cr�er des belles interpolations)
//	float angle; // Par o� il regarde
	short position[3]; // Sa position
	char vel[3]; // Sa velocity
	short mousePos[3]; // La position o� il vise
	int32_t babonetID;
#if defined(_PRO_)
	int camPosZ;
#endif
	// Son orientation sera calcul� client side, vu que c pas full important c une boule
};

#if defined(_PRO_)
//--- mini bot creation
#define NET_SVCL_CREATE_MINIBOT 1001
struct net_svcl_create_minibot
{
	char playerID; //--- Player ID owning that bot
	short position[3]; //--- Bot position
	short mousePos[3]; //--- Where it aims
};

#define NET_SVCL_MINIBOT_COORD_FRAME 1002
struct net_svcl_minibot_coord_frame
{
	char playerID; // Le ID du joueur concern�
	int32_t frameID; // Le frame auquel �a a �t� envoy� (on en a de besoin pour cr�er des belles interpolations)
//	float angle; // Par o� il regarde
	short position[3]; // Sa position
	char vel[3]; // Sa velocity
	short mousePos[3]; // La position o� il vise
	int32_t babonetID; // ?? don't need that
};
#endif


// On change le nom du joueur pendant le round (devra attendre la fin, ou au prochain round)
// Techniquement �a ne devrait pas �tre allow�, mais �a va �tre hot
#define NET_CLSV_SVCL_PLAYER_CHANGE_NAME 205
struct net_clsv_svcl_player_change_name
{
	char playerID; // Le ID du joueur
	char playerName[31+1]; // Le nom du joueur, 31 + \0 caract�res
};

// Le player shoot un projectile, on demande au server de cr�er l'instance
#define NET_CLSV_SVCL_PLAYER_PROJECTILE 206
struct net_clsv_svcl_player_projectile
{
	char playerID; // Le ID du joueur
	char weaponID; // Le ID du type de gun qui a shoot� le projectile
	char nuzzleID; // Le ID du nuzzle du fusil qui a l'a tir�
	char projectileType; // Le type du projectile
	short position[3]; // La position initial du projectile
	char vel[3]; // La velocit�e initial du projectile
	int32_t uniqueID;
//	int32_t uniqueProjectileID;
};

// Le player shoot avec son melee
#define NET_CLSV_SVCL_PLAYER_SHOOT_MELEE 207
struct net_clsv_svcl_player_shoot_melee
{
	char playerID;
};

// On request un vote
#define NET_CLSV_SVCL_VOTE_REQUEST 208
struct net_clsv_svcl_vote_request
{
	char vote[79+1]; // La commande
	char playerID; // Le player ID
};

// On request map
#define NET_CLSV_MAP_REQUEST 209
struct net_clsv_map_request
{
	char mapName[16]; // 15 + '\0'
	uint32_t uniqueClientID;
};

// On request map
#define NET_SVCL_MAP_CHUNK 210
struct net_svcl_map_chunk
{
	unsigned short	size;
	char			data[250]; //250 bytes chunks
};

// On map list request
#define NET_SVCL_MAP_LIST 211
struct net_svcl_map_list
{
	char mapName[16]; // 15 + '\0'
};

// Le player shoot avec son melee
#define NET_CLSV_SVCL_PLAYER_UPDATE_SKIN 212
struct net_clsv_svcl_player_update_skin
{
	char playerID;

	// Skin info
	char skin[7];

	//--- Les couleurs custom du babo
	unsigned char redDecal[3];
	unsigned char greenDecal[3];
	unsigned char blueDecal[3];
};

// BROADCAST MESSAGE
#define UNIQUE_BV2_KEY 3333113333
#define NET_BROADCAST -1
#define NET_BROADCAST_QUERY 301
#define NET_BROADCAST_GAME_INFO 302

struct net_clsv_broadcast_query
{
	char key[12];	//unique bv2 key for broadcasting
};

struct net_svcl_broadcast_game_info
{
	char		key[16];	//unique bv2 key for broadcasting
	stBV2row	GameInfo;	
};



// things specific to the pro client
#if defined(_PRO_)

	#define NET_SVCL_HASH_SEED 404
	struct net_svcl_hash_seed
	{
		short s1;
		short s2;
		short s3;
		short s4;
	};

	#define NET_SVCL_HASH_SEED_REPLY 405

#endif


#endif

