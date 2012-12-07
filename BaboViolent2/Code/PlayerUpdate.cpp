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

#include "Player.h"
#include "netPacket.h"
#include "GameVar.h"
#include "Console.h"
#include "Game.h"
#include "Scene.h"


extern Scene * scene;

void Player::updatePing(float delay)
{
	nextPingLogTime -= delay;
	if (nextPingLogTime < 0.0f)
	{
		if (pingLogID >= PING_LOG_SIZE)
			pingLogID = 0;
		pingLog[pingLogID] = ping;
		pingSum += pingLog[pingLogID];
		if (pingLogID + 1 < PING_LOG_SIZE)
			pingSum -= pingLog[pingLogID+1];
		else
			pingSum -= pingLog[0];
		avgPing = pingSum / PING_LOG_SIZE;
		if(avgPing < 1)
			avgPing = 1;
		nextPingLogTime = pingLogInterval;
		pingLogID++;
	}
}

//
// Update
//
void Player::update(float delay)
{
	updatePing(delay);

	if (teamID != PLAYER_TEAM_SPECTATOR)
		timeIdle += delay;
	else
		timeIdle = 0.0f;

	if (babySitTime >= 0.0f)
		babySitTime -= delay;

	float lenShootShakeDis = shootShakeDis.length();
	if (lenShootShakeDis > 0)
	{
		normalize(shootShakeDis);
		lenShootShakeDis -= delay;
		if (lenShootShakeDis < 0) lenShootShakeDis = 0;
		shootShakeDis *= lenShootShakeDis;
	}

	mfElapsedSinceLastShot += delay;
	mfCFTimer += delay;

	if (protection > 0)
	{
		protection -= delay;
		if (protection < 0) protection = 0;
	}

	if (immuneTime > 0)
	{
		immuneTime -= delay;
		if (immuneTime < 0) immuneTime = 0;
	}

	frameSinceLast++; // For server hacking prevention

	if (fireFrameDelay > 0) fireFrameDelay--;

	lastCF = currentCF; // On garde une copie du dernier coordFrame
	currentCF.frameID++; // Ça ça reste inchangé

#ifdef _PRO_
	if (minibot)
	{
		minibot->lastCF = minibot->currentCF;
		minibot->currentCF.frameID++;
	}
#endif

	// Rapid-fire hack detection
	secondPassed += delay;
	if (secondPassed > 3.0f && shotCount > 1) {
		// Calculate shots per second, subtracting one shot
		// to allow for lag
		shotsPerSecond = (shotCount - 1) / secondPassed;
		secondPassed = 0;
	}

	if (screenHit > 0)
	{
		screenHit -= delay*.25f;
		if (screenHit < 0) screenHit = 0;
	}

	if (firedShowDelay > 0) 
	{
		firedShowDelay -= delay;
		if (firedShowDelay < 0) firedShowDelay = 0;
	}

	if (grenadeDelay > 0)
	{
		grenadeDelay -= delay;
		if (grenadeDelay < 0) grenadeDelay = 0;
	}

	if (meleeDelay > 0)
	{
		meleeDelay -= delay;
		if (meleeDelay < 0) meleeDelay = 0;
	}


	// ca c pour le reload complet du shotty...jme demande ben ou a part ici ca devrait aller......dans shoot c appelé seulement quand ya un mouse HOLD....faq c la meilleur place que j'ai trouver la
	if (weapon)
	{
		if (weapon->fullReload && weapon->weaponID == WEAPON_SHOTGUN)
		{
			if (weapon->shotInc > 0)
			{
				// si ca fait un sizieme du temps alloué pour reloader
				if ((int)(weapon->currentFireDelay/3*100) % 17 == 0)
				{
					weapon->shotInc--;
#ifndef CONSOLE
					dksPlay3DSound(scene->client->sfxShotyReload, -1, 5, currentCF.position,230);
#endif
				}
			}
			else
			{
				weapon->fullReload = false;
			}
		}
	}

#ifndef CONSOLE
	// On check si on change de nom (on fait ça uniquement si on est à la fin d'un round (tout le temps en fin de compte))
	if (isThisPlayer)
	{
		if (name != gameVar.cl_playerName)
		{
			// On le clamp à 31 caracter
			if (gameVar.cl_playerName.len() > 31) gameVar.cl_playerName.resize(31);
			net_clsv_svcl_player_change_name playerChangeName;
			memcpy(playerChangeName.playerName, gameVar.cl_playerName.s, gameVar.cl_playerName.len()+1);
			playerChangeName.playerID = playerID;
			bb_clientSend(scene->client->uniqueClientID, (char*)&playerChangeName, sizeof(net_clsv_svcl_player_change_name), NET_CLSV_SVCL_PLAYER_CHANGE_NAME);
			name = gameVar.cl_playerName;
		}
	}
#endif

	// On update son gun
	if (weapon) weapon->update(delay);
	if (meleeWeapon) meleeWeapon->update(delay);

	if (status == PLAYER_STATUS_DEAD)
	{
		timeDead += delay;
	}

	timeInServer+=delay;

	if (status == PLAYER_STATUS_ALIVE)
	{
		timeAlive += delay;
		timePlayedCurGame += delay;

#ifdef _PRO_
		if (minibot)
		{
			//--- Control it if it's server, else do as a remote entity
			if (game->isServerGame)
			{
				// On déplace avec la velocity
				minibot->currentCF.position += minibot->currentCF.vel * delay;

				// On ralenti sa vel
				float size = minibot->currentCF.vel.length();
				if (size > 0)
				{
					size -= delay * 8;
					if (size < 0) size = 0;
					normalize(minibot->currentCF.vel);
					minibot->currentCF.vel = minibot->currentCF.vel * size;
				}

				// Un ajustement obligatoire
				minibot->currentCF.position[2] = .15f;

				// On gère les inputs1
				minibot->Think(delay);
			}
			else
			{
				// Là on va créer une genre d'interpolation
				minibot->currentCF.interpolate(minibot->cFProgression, minibot->netCF0, minibot->netCF1, delay);

				// Un ajustement obligatoire (sa hauteur)
				minibot->currentCF.position[2] = .15f;
			}
			minibot->m_seekingTime -= delay;
			if(minibot->m_seekingTime <= 0 && minibot->owner->meleeWeapon && minibot->owner->meleeWeapon->weaponID == WEAPON_MINIBOT)
			{
#ifndef CONSOLE
				dksPlay3DSound(dksCreateSoundFromFile("main/sounds/BaboCreve3.wav", false), -1, 5, minibot->currentCF.position,64);
				game->spawnBloodMinibot(minibot->currentCF.position, .5f);
#endif
				delete minibot;
				minibot = 0;
			}
		}
#endif

		if (remoteEntity)
		{
			// Là on va créer une genre d'interpolation
			currentCF.interpolate(cFProgression, netCF0, netCF1, delay);

			// Un ajustement obligatoire (sa hauteur)
			currentCF.position[2] = .25f;
		}
#ifndef CONSOLE
		else
		{
			// On déplace avec la velocity
			currentCF.position += currentCF.vel * delay;

			// On ralenti sa vel
			float size = currentCF.vel.length();
			if (size > 0)
			{
				if (game->map->theme == THEME_SNOW && gameVar.sv_slideOnIce)
				{
					if (game->map->cells[(int)(currentCF.position[1] - .5f) * game->map->size[0] + (int)(currentCF.position[0] - .5f)].splater[0] > .5f)
					{
						size -= delay * 1;
					}
					else
					{
						size -= delay * 4;
					}
				}
				else
				{
					size -= delay * 4;
				}
				if (size < 0) size = 0;
				normalize(currentCF.vel);
				currentCF.vel = currentCF.vel * size;
			} 

			// Un ajustement obligatoire
			currentCF.position[2] = .25f;

			// On gère les inputs
			if (isThisPlayer && !console->isActive() && !writting && !game->showMenu && !menuManager.root->visible)
			{
				controlIt(delay);
			}

			// On envoit aux autres
			if (isThisPlayer)
			{
				// Bon, on envoit cette position aux autres joueurs, la vitesse d'envoit c'est dépendant de son ping
				sendPosFrame++;
				if (sendPosFrame >= avgPing && sendPosFrame >= gameVar.sv_minSendInterval && status == PLAYER_STATUS_ALIVE)
				{
					// On essait de rester constant
					net_clsv_svcl_player_coord_frame playerCoordFrame;
					playerCoordFrame.frameID = currentCF.frameID;
				//	playerCoordFrame.angle = currentCF.angle;
					playerCoordFrame.playerID = playerID;
					playerCoordFrame.position[0] = (short)(currentCF.position[0] * 100);
					playerCoordFrame.position[1] = (short)(currentCF.position[1] * 100);
					playerCoordFrame.position[2] = (short)(currentCF.position[2] * 100);
					playerCoordFrame.vel[0] = (char)(currentCF.vel[0] * 10);
					playerCoordFrame.vel[1] = (char)(currentCF.vel[1] * 10);
					playerCoordFrame.vel[2] = (char)(currentCF.vel[2] * 10);
					playerCoordFrame.mousePos[0] = (short)(currentCF.mousePosOnMap[0] * 100);
					playerCoordFrame.mousePos[1] = (short)(currentCF.mousePosOnMap[1] * 100);
					playerCoordFrame.mousePos[2] = (short)(currentCF.mousePosOnMap[2] * 100);
					playerCoordFrame.babonetID = babonetID;
#ifdef _PRO_
					playerCoordFrame.camPosZ = (int)game->map->camPos[2];
#endif
					bb_clientSend(scene->client->uniqueClientID, (char*)&playerCoordFrame, sizeof(net_clsv_svcl_player_coord_frame), NET_CLSV_SVCL_PLAYER_COORD_FRAME, NET_UDP);

					sendPosFrame = 0;
				}
			}

			// On est vivant, donc on a 10sec avant de pouvoir respawner
			timeToSpawn = gameVar.sv_timeToSpawn;
		}
#endif
		
		// On l'oriente
		{
#ifndef CONSOLE
			CVector3f shotOrigin;
			if (weapon->nuzzleFlashes.size() > 0)
				shotOrigin = weapon->nuzzleFlashes[weapon->firingNuzzle]->position * .005f;
			else
				shotOrigin = currentCF.position;
			shotOrigin = rotateAboutAxis(shotOrigin, currentCF.angle, CVector3f(0,0,1)) + currentCF.position;
			CVector3f dirVect = currentCF.mousePosOnMap - shotOrigin;
			CVector3f dirVectAlt = currentCF.mousePosOnMap - currentCF.position;
			if(!gameVar.cl_preciseCursor || weapon->weaponID == WEAPON_DUAL_MACHINE_GUN || dirVectAlt.length() <= 1.5f)
				dirVect = dirVectAlt;
#else
			CVector3f dirVect = currentCF.mousePosOnMap - currentCF.position;
#endif
			dirVect[2] = 0;

			normalize(dirVect);
			float dotWithY = dirVect[1];//dotting with (0,1,0) just gives the y coord
			float dotWithX = dirVect[0];//dotting with (1,0,0) just gives the x coord
			currentCF.angle = acosf(dotWithY)*TO_DEGREE;
			if (dotWithX > 0) currentCF.angle = -currentCF.angle;

			// Bon, on fait rouler la bouboule
			if (lastCF.position != currentCF.position)
			{
				CVector3f mouvement = currentCF.position - lastCF.position;
				float angle = PI*mouvement.length();
				CVector3f right = cross(mouvement, CVector3f(0,0,1));
				normalize(right);
				matrix.RotateArbitrary(-angle*TO_DEGREE, right);

				// On la normalize (parce que la boule à rapetisse :|)
				matrix.normalize();
			}
		}

#ifdef _PRO_
		if (minibot)
		{
			// On l'oriente
			CVector3f dirVect = minibot->currentCF.mousePosOnMap - minibot->currentCF.position;
			dirVect[2] = 0;
			normalize(dirVect);
			float dotWithY = dot(CVector3f(0,1,0),dirVect);
			float dotWithX = dot(CVector3f(1,0,0),dirVect);
			minibot->currentCF.angle = acosf(dotWithY)*TO_DEGREE;
			if (dotWithX > 0) minibot->currentCF.angle = -minibot->currentCF.angle;

			// Bon, on fait rouler la bouboule
			if (minibot->lastCF.position != minibot->currentCF.position)
			{
				CVector3f mouvement = minibot->currentCF.position - minibot->lastCF.position;
				float angle = .5f*PI*(mouvement.length()) * 4.0f;
				CVector3f right = cross(mouvement, CVector3f(0,0,1));
				normalize(right);
				minibot->matrix.RotateArbitrary(-angle*TO_DEGREE, right);

				// On la normalize (parce que la boule à rapetisse :|)
				minibot->matrix.normalize();
			}
		}
#endif
	}
	else if ((
		teamID == PLAYER_TEAM_BLUE || 
		teamID == PLAYER_TEAM_RED) &&
		status == PLAYER_STATUS_DEAD && timeToSpawn >= 0)
	{
		timeToSpawn -= delay;
		if (timeToSpawn <= 0)
		{
			// On ne veut pas afficher des négatif ;)
			timeToSpawn = 0;

#ifndef CONSOLE
			// Seuleument si c'est notre joueur, sinon on s'en caliss
			if (isThisPlayer)
			{
				// On check si on peut requester le spawn, sauf si on est en s&d (là c le server qui choisi ;))
#ifdef _PRO_
            if ((gameVar.sv_forceRespawn || ((dkiGetState(gameVar.k_shoot) == DKI_DOWN && !scene->client->showMenu) && !scene->client->chatting.haveFocus())) && !spawnRequested)
#else
            if ((gameVar.sv_forceRespawn || ((dkiGetState(gameVar.k_shoot) == DKI_DOWN && !scene->client->showMenu) && !scene->client->chatting.haveFocus())) && !spawnRequested && game->gameType != GAME_TYPE_SND)
#endif				
				{

         #ifdef _PRO_
            if (gameVar.sv_subGameType == SUBGAMETYPE_RANDOMWEAPON)
            {
            nextSpawnWeapon = rand(0, WEAPON_FLAME_THROWER+1);
            if (rand(-1, 1))
               nextMeleeWeapon = WEAPON_KNIVES;
            else
               nextMeleeWeapon = WEAPON_SHIELD;
            }

         #endif

					// Ici on le call juste une fois, isshh sinon ça sera pas trop bon...
					// On request to spawn
					spawnRequested = true;
					net_clsv_spawn_request spawnRequest;
					spawnRequest.playerID = playerID;
					spawnRequest.weaponID = nextSpawnWeapon;
					spawnRequest.meleeID = nextMeleeWeapon;
					skin = gameVar.cl_skin;
					redDecal = gameVar.cl_redDecal;
					greenDecal = gameVar.cl_greenDecal;
					blueDecal = gameVar.cl_blueDecal;
					memcpy(spawnRequest.skin, skin.s, (skin.len() <= 6)?skin.len()+1:7);
					spawnRequest.blueDecal[0] = (unsigned char)(blueDecal[0] * 255.0f);
					spawnRequest.blueDecal[1] = (unsigned char)(blueDecal[1] * 255.0f);
					spawnRequest.blueDecal[2] = (unsigned char)(blueDecal[2] * 255.0f);
					spawnRequest.greenDecal[0] = (unsigned char)(greenDecal[0] * 255.0f);
					spawnRequest.greenDecal[1] = (unsigned char)(greenDecal[1] * 255.0f);
					spawnRequest.greenDecal[2] = (unsigned char)(greenDecal[2] * 255.0f);
					spawnRequest.redDecal[0] = (unsigned char)(redDecal[0] * 255.0f);
					spawnRequest.redDecal[1] = (unsigned char)(redDecal[1] * 255.0f);
					spawnRequest.redDecal[2] = (unsigned char)(redDecal[2] * 255.0f);
					bb_clientSend(scene->client->uniqueClientID, (char*)&spawnRequest, sizeof(net_clsv_spawn_request), NET_CLSV_SPAWN_REQUEST);
				}
			}
#endif
		}
	}
}


#ifndef CONSOLE
//
// Pour le controller (ça c'est client side only, on ne gère pas le mouvement des autres players comme ça)
//
void Player::controlIt(float delay)
{
	// On gère les inputs

	// Si on est en mode scope (FPS), on tourne la tête avec la mouse
	if (scopeMode)
	{
		CVector2i mouseVel = dkiGetMouseVel();
		float angle = -(float)mouseVel[0];
		angle *= .2f;

		CVector3f dir = currentCF.mousePosOnMap - currentCF.position;
		normalize(dir);
		dir = rotateAboutAxis(dir, angle, CVector3f(0,0,1));
		dir *= 10;
		currentCF.mousePosOnMap = currentCF.position + dir;
	}


	float accel = 12.5f;
	if (game->map->cells[(int)(currentCF.position[1] - .5f) * game->map->size[0] + (int)(currentCF.position[0] - .5f)].splater[0] > .5f && game->map->theme == THEME_SNOW && gameVar.sv_slideOnIce)
	{
		accel = 4.0f;
	}


	// Si on est en mode scope, on se déplace dapres l'orientation local
	if (scopeMode)
	{
		CVector3f front = currentCF.mousePosOnMap - currentCF.position;
		normalize(front);
		CVector3f right = cross(front, CVector3f(0,0,1));
		/*if (!(weapon->weaponID == WEAPON_PHOTON_RIFLE && dkiGetState(gameVar.k_shoot)))
		{*/
			if (dkiGetState(gameVar.k_moveUp))
			{
				currentCF.vel += front * delay * accel;
			}
			if (dkiGetState(gameVar.k_moveDown))
			{
				currentCF.vel -= front * delay * accel;
			}
			if (dkiGetState(gameVar.k_moveRight))
			{
				currentCF.vel += right * delay * accel;
			}
			if (dkiGetState(gameVar.k_moveLeft))
			{
				currentCF.vel -= right * delay * accel;
			}
		//}
	}
	else // Sinon absolu
	{
		/*if (!(weapon->weaponID == WEAPON_PHOTON_RIFLE && dkiGetState(gameVar.k_shoot)))
		{*/
			if (gameVar.cl_enableXBox360Controller)
			{
				CVector3f joyVel = dkiGetJoy();
				joyVel[2] = 0;
				joyVel[1] = -joyVel[1];

				if (fabsf(joyVel[0]) < .1f) joyVel[0] = 0;
				if (fabsf(joyVel[1]) < .1f) joyVel[1] = 0;

				joyVel *= 1.5f;
				if (joyVel[0] < -1) joyVel[0] = -1;
				if (joyVel[1] < -1) joyVel[1] = -1;
				if (joyVel[0] > 1) joyVel[0] = 1;
				if (joyVel[1] > 1) joyVel[1] = 1;

				currentCF.vel[0] += joyVel[0] * delay * accel;
				currentCF.vel[1] += joyVel[1] * delay * accel;
			}
			else
			{
				if (dkiGetState(gameVar.k_moveUp))
				{
					currentCF.vel[1] += delay * accel;
				}
				if (dkiGetState(gameVar.k_moveDown))
				{
					currentCF.vel[1] -= delay * accel;
				}
				if (dkiGetState(gameVar.k_moveRight))
				{
					currentCF.vel[0] += delay * accel;
				}
				if (dkiGetState(gameVar.k_moveLeft))
				{
					currentCF.vel[0] -= delay * accel;
				}
			}
		//}
	}

	// Si on tire
	if (!dkiGetState(gameVar.k_shoot))
	{
		if (weapon)
		{
			weapon->charge = 0;
		}
	}
	if (dkiGetState(gameVar.k_shoot) == DKI_DOWN) initedMouseClic = true;
	if (dkiGetState(gameVar.k_shoot) && initedMouseClic)
	{
		if (weapon && grenadeDelay == 0 && meleeDelay == 0) 
		{
			firedShowDelay = 2; // Ça c'est le ping sur la map qu'on voit quand L'autre tire

			//--- Est-ce qu'on est sniper et en scope mode?
			if (weapon->weaponID == WEAPON_SNIPER && game->map->camPos[2] >= 10)
			{
				//--- On shoot une deuxième fois pour faire plus de damage en scope mode
				weapon->nbShot = 3;
			}

			weapon->shoot(this);

			if (weapon->weaponID == WEAPON_SNIPER)
			{
				//--- On shoot une deuxième fois pour faire plus de damage en scope mode
				weapon->nbShot = 2;
			}
		}
	}

	// SECONDARY FIRE (Melee weapon)
#ifdef _PRO_
	if (!minibot)
#endif
	if (dkiGetState(gameVar.k_melee) && grenadeDelay == 0 && meleeDelay == 0 && gameVar.sv_enableSecondary)
	{
		if (meleeWeapon && grenadeDelay == 0) 
		{
			firedShowDelay = 2; // Ça c'est le ping sur la map qu'on voit quand L'autre tire

			//--- On shoot ça
			net_clsv_svcl_player_shoot_melee playerShootMelee;
			playerShootMelee.playerID = playerID;
			bb_clientSend(scene->client->uniqueClientID, (char*)&playerShootMelee, sizeof(net_clsv_svcl_player_shoot_melee), NET_CLSV_SVCL_PLAYER_SHOOT_MELEE);

	//		meleeWeapon->shoot(this);

			meleeDelay = meleeWeapon->fireDelay;
		}
	}

	// On lance une nade
	if (dkiGetState(gameVar.k_throwGrenade) == DKI_DOWN && grenadeDelay == 0 && nbGrenadeLeft > 0 && meleeDelay == 0)
	{
		if (weapon)
		{
			if (weapon->currentFireDelay <= 0) // On n'est pas entrein de shoter avec un autre gun?
			{
				lastShootWasNade = true;
				nbGrenadeLeft--;
				grenadeDelay = gameVar.weapons[WEAPON_GRENADE]->fireDelay;
				// On pitch ça
				gameVar.weapons[WEAPON_GRENADE]->shoot(this);
				gameVar.weapons[WEAPON_GRENADE]->currentFireDelay = 0; // Il n'y a pas d'update sur ce gun
			}
		}
	}

	// On lance un cocktail molotov
	if (dkiGetState(gameVar.k_throwMolotov) == DKI_DOWN && grenadeDelay == 0 && nbMolotovLeft > 0 && gameVar.sv_enableMolotov)
	{
		if (weapon)
		{
			if (weapon->currentFireDelay <= 0) // On n'est pas entrein de shoter avec un autre gun?
			{
				lastShootWasNade = false;
				nbMolotovLeft--;
				grenadeDelay = gameVar.weapons[WEAPON_COCKTAIL_MOLOTOV]->fireDelay;
				// On pitch ça
				gameVar.weapons[WEAPON_COCKTAIL_MOLOTOV]->shoot(this);
				gameVar.weapons[WEAPON_COCKTAIL_MOLOTOV]->currentFireDelay = 0; // Il n'y a pas d'update sur ce gun
			}
		}
	}

	// On switch en scope mode (sniper only) (cétait juste un test à chier ça)
/*	if (dkiGetState(DKI_MOUSE_BUTTON2) == DKI_DOWN)
	{
		scopeMode = !scopeMode;
	}*/

	// On pickup un item par terre
	if (dkiGetState(gameVar.k_pickUp) == DKI_DOWN)
	{
		net_clsv_pickup_request pickupRequest;
		pickupRequest.playerID = playerID;
		bb_clientSend(scene->client->uniqueClientID, (char*)&pickupRequest, sizeof(net_clsv_pickup_request), NET_CLSV_PICKUP_REQUEST);
	}

	// On clamp sa vel /* Upgrade, faster ! haha */
	float size = currentCF.vel.length();
	if (size > 3.25f)
	{
		normalize(currentCF.vel);
		currentCF.vel = currentCF.vel * 3.25f;
	}
}
#endif


