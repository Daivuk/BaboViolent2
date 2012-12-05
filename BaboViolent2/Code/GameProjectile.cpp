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

#include "Game.h"
#include "Scene.h"
#include "Console.h"

extern Scene * scene;

long Projectile::uniqueProjectileID = 0;


//
// Constructeur
//
Projectile::Projectile(CVector3f & position, CVector3f & vel, char pFromID, int pProjectileType, bool pRemoteEntity, long pUniqueProjectileID)
{
	fromID = pFromID;

	uniqueID = pUniqueProjectileID;


	timeSinceThrown = 0.0f;

	projectileType = pProjectileType;
	currentCF.position = position;
	currentCF.vel = vel;
#ifndef CONSOLE
	spawnParticleTime = 0;
#endif
	damageTime = 0;


	lastCF = currentCF;
	netCF0.reset();
	netCF1.reset();
	cFProgression = 0;

	stickToPlayer = -1;
	stickFor = 0;

	whenToShoot = 0;

	remoteEntity = pRemoteEntity; // Ça c'est server only
	needToBeDeleted = false;
	reallyNeedToBeDeleted = false;
	movementLock = false;

#ifndef CONSOLE
	rotation = 0;
#endif
	projectileID = 0;

	switch (projectileType)
	{
	case PROJECTILE_ROCKET:
		{
#ifndef CONSOLE
			rotateVel = 360;
#endif
			duration = 10; // 10 sec
			// On calcul l'angle que la rocket devrait avoir (dépendanment de sa vel, qui est l'Orientation)
			CVector3f dirVect = vel;
			dirVect[2] = 0; // L'orientation est juste en Z
			normalize(dirVect);
			float dotWithY = dot(CVector3f(0,1,0),dirVect);
			float dotWithX = dot(CVector3f(1,0,0),dirVect);
			currentCF.angle = acosf(dotWithY)*TO_DEGREE;
			if (dotWithX > 0) currentCF.angle = -currentCF.angle;
			// La rocket démarre plus vite
			currentCF.vel *= 2.5f;
#ifndef CONSOLE
			if (remoteEntity)
			{
				for (int i=0;i<10;++i)
				{
					// On spawn des particules dans son cul (une par frame)
					dkpCreateParticleEx(
						position-vel, //CVector3f & positionFrom,
						position-vel, //CVector3f & positionTo,
						-vel, //CVector3f & direction,
						1, //float speedFrom,
						2, //float speedTo,
						0, //float pitchFrom,
						45, //float pitchTo,
						.05f, //float startSizeFrom,
						.25f, //float startSizeTo,
						.25f, //float endSizeFrom,
						.45f, //float endSizeTo,
						.5f, //float durationFrom,
						2, //float durationTo,
						CColor4f(.5f,.5f,.5f,1), //CColor4f & startColorFrom,
						CColor4f(.5f,.5f,.5f,1), //CColor4f & startColorTo,
						CColor4f(.5f,.5f,.5f,0), //CColor4f & endColorFrom,
						CColor4f(.5f,.5f,.5f,0), //CColor4f & endColorTo,
						0, //float angleFrom,
						360, //float angleTo,
						-30, //float angleSpeedFrom,
						30, //float angleSpeedTo,
						0, //float gravityInfluence,
						.25f, //float airResistanceInfluence,
						5, //unsigned int particleCountFrom,
						5, //unsigned int particleCountTo,
						&gameVar.tex_smoke1,
						0, //int textureFrameCount,
						DKP_SRC_ALPHA, //unsigned int srcBlend,
						DKP_ONE_MINUS_SRC_ALPHA);//unsigned int dstBlend);
					// On spawn des particules dans son cul (une par frame)
					dkpCreateParticleEx(
						position, //CVector3f & positionFrom,
						position, //CVector3f & positionTo,
						vel, //CVector3f & direction,
						1, //float speedFrom,
						2, //float speedTo,
						0, //float pitchFrom,
						45, //float pitchTo,
						.05f, //float startSizeFrom,
						.25f, //float startSizeTo,
						.25f, //float endSizeFrom,
						.45f, //float endSizeTo,
						.5f, //float durationFrom,
						2, //float durationTo,
						CColor4f(.5f,.5f,.5f,1), //CColor4f & startColorFrom,
						CColor4f(.5f,.5f,.5f,1), //CColor4f & startColorTo,
						CColor4f(.5f,.5f,.5f,0), //CColor4f & endColorFrom,
						CColor4f(.5f,.5f,.5f,0), //CColor4f & endColorTo,
						0, //float angleFrom,
						360, //float angleTo,
						-30, //float angleSpeedFrom,
						30, //float angleSpeedTo,
						0, //float gravityInfluence,
						.25f, //float airResistanceInfluence,
						5, //unsigned int particleCountFrom,
						5, //unsigned int particleCountTo,
						&gameVar.tex_smoke1,
						0, //int textureFrameCount,
						DKP_SRC_ALPHA, //unsigned int srcBlend,
						DKP_ONE_MINUS_SRC_ALPHA);//unsigned int dstBlend);
				}
			}
#endif
			break;
		}
	case PROJECTILE_GRENADE:
		{
#ifndef CONSOLE
			rotateVel = 360;
#endif
			duration = 2;
			// La grenade démarre plus vite
			currentCF.vel *= 5;
			currentCF.vel[2] += 5; // Pas trop apique, on veut pogner les murs
			break;
		}
	case PROJECTILE_COCKTAIL_MOLOTOV:
		{
#ifndef CONSOLE
			rotateVel = 360;
#endif
			duration = 10;
			currentCF.vel *= 6;
			currentCF.vel[2] += 2;
			break;
		}
	case PROJECTILE_LIFE_PACK:
		{
#ifndef CONSOLE
			rotateVel = rand(-90.0f, 90.0f);
#endif
		//	currentCF.vel.set(0,0,1);
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
			duration = 20;
			break;
		}
	case PROJECTILE_DROPED_WEAPON:
		{
#ifndef CONSOLE
			rotateVel = rand(-90.0f, 90.0f);
#endif
		//	currentCF.vel.set(0,0,1);
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
			duration = 30;
			break;
		}
	case PROJECTILE_DROPED_GRENADE:
		{
#ifndef CONSOLE
			rotateVel = rand(-90.0f, 90.0f);
#endif
		//	currentCF.vel.set(0,0,1);
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-45.0f, 45.0f), CVector3f(1,0,0));
		//	currentCF.vel = rotateAboutAxis(currentCF.vel, rand(-0.0f, 360.0f), CVector3f(0,0,1)) * 3;
			duration = 25;
			break;
		}
	case PROJECTILE_FLAME:
		{
#ifndef CONSOLE
			rotateVel = 0;
#endif
			duration = 10;
			break;
		}
	case PROJECTILE_GIB:
		{
	//		isClientOnly
			break;
		}
	}
}



//
// Son update
//
void Projectile::update(float delay, Map* map)
{
#ifndef CONSOLE
	rotation += delay * rotateVel; // 1 tour à la seconde :D
	while (rotation >= 360) rotation -= 360;
	while (rotation < 0) rotation += 360;
#endif

	lastCF = currentCF; // On garde une copie du dernier coordFrame
	currentCF.frameID++; // Ça ça reste inchangé

	timeSinceThrown += delay;

#ifndef CONSOLE
	if (remoteEntity)
	{
		// Là on va créer une genre d'interpolation (cubic spline (bezier), pour être plus précis)
	//	currentCF.interpolate(cFProgression, netCF0, netCF1, delay); //--- Client side maintenant

		if (projectileType == PROJECTILE_ROCKET)
		{
		//	scene->client->game->trails.push_back(new Trail(lastCF.position, currentCF.position, 1.5f, CVector4f(.6f,.6f,.6f,.5f), 4));
//rotation
			
			float colorArg1 = 1.0;
			float colorArg2 = 1.0;
			float colorArg3 = 1.0;
			if((scene->client->game->gameType == 1 || scene->client->game->gameType == 2) && scene->client->game->players[fromID])
				//If we're CTF or TDM, our zooka trails should be coloured
			{
				if(scene->client->game->players[fromID]->teamID == PLAYER_TEAM_BLUE)
				{
					colorArg3 = 1.0f;
					colorArg1 = colorArg2 = 0.25f;
				}
				else if(scene->client->game->players[fromID]->teamID == PLAYER_TEAM_RED)
				{
					colorArg1 = 1.0f;
					colorArg2 = colorArg3 = 0.25f;
				}
			}
			// On spawn des particules dans son cul (une par frame)
			dkpCreateParticle(	currentCF.position.s,//float *position,
								(-currentCF.vel*0/*.25f*/).s,//float *vel,
								CVector4f(colorArg1,colorArg2,colorArg3,0.75f).s,//float *startColor,
								CVector4f(colorArg1,colorArg2,colorArg3,0.0f).s,//float *endColor,
								.125f,//float startSize,
								rand(.6f,1.0f),//float endSize,
								gameVar.r_reducedParticles ? .5f : 5.0f,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								rand(0.0f, 30.0f),//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
								0);//int transitionFunc);

			// On spawn des particules dans son cul (une par frame)
		/*	dkpCreateParticle(	currentCF.position.s,//float *position,
								(-currentCF.vel*.015f).s,//float *vel,
								CVector4f(1,1,1,.75f).s,//float *startColor,
								CVector4f(1,1,1,0.0f).s,//float *endColor,
								.125f,//float startSize,
								rand(.6f,1.0f),//float endSize,
								5.0f,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								rand(0.0f, 30.0f),//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
								0);//int transitionFunc);*/
		}

		if (projectileType == PROJECTILE_GRENADE)
		{
			//--- Depending of the team, the color change
#ifdef _PRO_
			if ((scene->client->game->gameType == GAME_TYPE_DM) || (scene->client->game->gameType == GAME_TYPE_SND))
#else
         if (scene->client->game->gameType == GAME_TYPE_DM)
#endif
			{
				// On spawn des particules dans son cul (une par frame)
				dkpCreateParticle(	currentCF.position.s,//float *position,
									(-currentCF.vel*0/*.25f*/).s,//float *vel,
									CVector4f(1,1,1,.25f).s,//float *startColor,
									CVector4f(1,1,1,0.0f).s,//float *endColor,
									.125f,//float startSize,
									rand(.2f,.2f),//float endSize,
									gameVar.r_reducedParticles ? 1.0f : 2.0f,//float duration,,//float duration,
									0,//float gravityInfluence,
									0,//float airResistanceInfluence,
									rand(0.0f, 30.0f),//float rotationSpeed,
									gameVar.tex_shotGlow,//unsigned int texture,
									DKP_SRC_ALPHA,//unsigned int srcBlend,
									DKP_ONE/*_MINUS_SRC_ALPHA*/,//unsigned int dstBlend,
									0);//int transitionFunc);
			}
			else if (scene->client->game->players[fromID])
			{
				if (scene->client->game->players[fromID]->teamID == PLAYER_TEAM_RED)
				{
					// On spawn des particules dans son cul (une par frame)
					dkpCreateParticle(	currentCF.position.s,//float *position,
										(-currentCF.vel*0/*.25f*/).s,//float *vel,
										CVector4f(1,.25f,.25f,.25f).s,//float *startColor,
										CVector4f(1,.25f,.25f,0.0f).s,//float *endColor,
										.125f,//float startSize,
										rand(.2f,.2f),//float endSize,
										gameVar.r_reducedParticles ? 1.0f : 2.0f,//float duration,
										0,//float gravityInfluence,
										0,//float airResistanceInfluence,
										rand(0.0f, 30.0f),//float rotationSpeed,
										gameVar.tex_shotGlow,//unsigned int texture,
										DKP_SRC_ALPHA,//unsigned int srcBlend,
										DKP_ONE/*_MINUS_SRC_ALPHA*/,//unsigned int dstBlend,
										0);//int transitionFunc);
				}
				else if (scene->client->game->players[fromID]->teamID == PLAYER_TEAM_BLUE)
				{
					// On spawn des particules dans son cul (une par frame)
					dkpCreateParticle(	currentCF.position.s,//float *position,
										(-currentCF.vel*0/*.25f*/).s,//float *vel,
										CVector4f(.25f,.25f,1,.25f).s,//float *startColor,
										CVector4f(.25f,.25f,1,0.0f).s,//float *endColor,
										.125f,//float startSize,
										rand(.2f,.2f),//float endSize,
										gameVar.r_reducedParticles ? 1.0f : 2.0f,//float duration,
										0,//float gravityInfluence,
										0,//float airResistanceInfluence,
										rand(0.0f, 30.0f),//float rotationSpeed,
										gameVar.tex_shotGlow,//unsigned int texture,
										DKP_SRC_ALPHA,//unsigned int srcBlend,
										DKP_ONE/*_MINUS_SRC_ALPHA*/,//unsigned int dstBlend,
										0);//int transitionFunc);
				}
			}
		}

		if (projectileType == PROJECTILE_COCKTAIL_MOLOTOV)
		{
			CVector3f fireDirection(0,0,1);
			fireDirection = rotateAboutAxis(fireDirection, rotation, CVector3f(currentCF.vel[0], currentCF.vel[1], 0));
			// On spawn des particules de feu dans son cul (une par frame)
			dkpCreateParticle(	currentCF.position.s,//float *position,
								(fireDirection*.15f).s,//float *vel,
								CVector4f(1,.75f,0,1.0f).s,//float *startColor,
								CVector4f(1,.75f,0,0.0f).s,//float *endColor,
								.25f,//float startSize,
								.025f,//float endSize,
								0.25f,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								rand(0.0f, 30.0f),//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE,//unsigned int dstBlend,
								0);//int transitionFunc);
		}

		if (projectileType == PROJECTILE_FLAME)
		{
			spawnParticleTime++;
			if (spawnParticleTime >= 30) spawnParticleTime = 0;

			if (spawnParticleTime%3 == 0)
			{
				// On spawn des particules de feu dans son cul (une par frame)
				dkpCreateParticle(	(currentCF.position+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *position,
									(CVector3f(0,0,1)).s,//float *vel,
									rand(CVector4f(1,0,0,0.0f),CVector4f(1,.75f,0,0.0f)).s,//float *startColor,
									CVector4f(1,.75f,0,1.0f).s,//float *endColor,
									.3f,//float startSize,
									.0f,//float endSize,
									1.0f,//float duration,
									0,//float gravityInfluence,
									0,//float airResistanceInfluence,
									rand(0.0f, 30.0f),//float rotationSpeed,
									gameVar.tex_smoke1,//unsigned int texture,
									DKP_SRC_ALPHA,//unsigned int srcBlend,
									DKP_ONE,//unsigned int dstBlend,
									0);//int transitionFunc);
				// On spawn des particules de feu dans son cul (une par frame)
				dkpCreateParticle(	(currentCF.position+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *position,
									(CVector3f(0,0,1)+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *vel,
									rand(CVector4f(1,0,0,1.0f),CVector4f(1,.75f,0,1.0f)).s,//float *startColor,
									CVector4f(1,.75f,0,0.0f).s,//float *endColor,
									.0f,//float startSize,
									.3f,//float endSize,
									1.0f,//float duration,
									0,//float gravityInfluence,
									0,//float airResistanceInfluence,
									rand(0.0f, 30.0f),//float rotationSpeed,
									gameVar.tex_smoke1,//unsigned int texture,
									DKP_SRC_ALPHA,//unsigned int srcBlend,
									DKP_ONE,//unsigned int dstBlend,
									0);//int transitionFunc);
			}
			if (spawnParticleTime%10 == 0)
			{
				// On spawn des particules de feu dans son cul (une par frame)
				dkpCreateParticle(	(currentCF.position).s,//float *position,
									(CVector3f(-.5f,0,.5f)).s,//float *vel,
									CVector4f(.5f,.5f,.5f,0.5f).s,//float *startColor,
									CVector4f(.5f,.5f,.5f,0.0f).s,//float *endColor,
									.15f,//float startSize,
									1.0,//float endSize,
									3.0f,//float duration,
									0,//float gravityInfluence,
									0,//float airResistanceInfluence,
									rand(0.0f, 30.0f),//float rotationSpeed,
									gameVar.tex_smoke1,//unsigned int texture,
									DKP_SRC_ALPHA,//unsigned int srcBlend,
									DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
									0);//int transitionFunc);
			}
		}
	}
//	else
//	{
#endif
		// On la clamp quand meme (1 sqrtf par rocket, pas si pire..)
		float speed = currentCF.vel.length();
		if (projectileType == PROJECTILE_ROCKET)
		{
			if (speed > 10) // 10 = fast enough :P
			{
				currentCF.vel /= speed;
				speed = 10;
				currentCF.vel *= speed;
			}

			// On déplace avec la velocity
			currentCF.position += currentCF.vel * delay;

			// On incrémente la vel, la rocket à fuse en sale! (accélération exponentiel!)
			currentCF.vel += currentCF.vel * delay * 3;
		}

		if (projectileType == PROJECTILE_COCKTAIL_MOLOTOV)
		{
			// On déplace avec la velocity
			currentCF.position += currentCF.vel * delay;

			// On affecte la gravitée!
			currentCF.vel[2] -= 9.8f * delay;
		}

		if (projectileType == PROJECTILE_FLAME && !movementLock)
		{
			// On déplace avec la velocity
			currentCF.position += currentCF.vel * delay;

			// On affecte la gravitée!
			currentCF.vel[2] -= 9.8f * delay;
		}

		//--- Le feu est pogné sur un player
		if (projectileType == PROJECTILE_FLAME)
		{

			if (!remoteEntity)
			{
				if (stickToPlayer >= 0 && scene->server)
				{
					if (scene->server->game->players[stickToPlayer])
					{
						if (scene->server->game->players[stickToPlayer]->status == PLAYER_STATUS_DEAD)
						{
							stickToPlayer = -1;
						}
						else
						{
							currentCF.position = scene->server->game->players[stickToPlayer]->currentCF.position;
						}
					}
					stickFor -= delay;
					if (stickFor <= 0) 
					{
						stickFor = 0;
						stickToPlayer = -1;
						net_svcl_flame_stick_to_player flameStickToPlayer;
						flameStickToPlayer.playerID = -1;
						flameStickToPlayer.projectileID = projectileID;
						bb_serverSend((char*)&flameStickToPlayer, sizeof(net_svcl_flame_stick_to_player), NET_SVCL_FLAME_STICK_TO_PLAYER, 0);
						movementLock = false;
						stickFor = 1.0f; // 1 sec sans retoucher à un autre joueur (quand meme là)
					//	net_svcl_flame_stick_to_player flameStickToPlayer;
						flameStickToPlayer.playerID = -1;
						flameStickToPlayer.projectileID = projectileID;
						bb_serverSend((char*)&flameStickToPlayer, sizeof(net_svcl_flame_stick_to_player), NET_SVCL_FLAME_STICK_TO_PLAYER, 0);
					}
				}
				if (stickToPlayer == -1)
				{

					//console->add( CString("sticktoplayer = %i on flameID = %i",stickToPlayer,this->uniqueProjectileID));

					stickFor -= delay;
					if (stickFor <= 0)
					{
						stickFor = 0;
						Player * playerInRadius = scene->server->game->playerInRadius(currentCF.position,.5f , this->timeSinceThrown > 0.5f ? -1 : this->fromID);
						if (playerInRadius)
						{
							movementLock = true;
							stickToPlayer = playerInRadius->playerID;
							stickFor = 3;
							net_svcl_flame_stick_to_player flameStickToPlayer;
							flameStickToPlayer.playerID = playerInRadius->playerID;
							flameStickToPlayer.projectileID = projectileID;
							bb_serverSend((char*)&flameStickToPlayer, sizeof(net_svcl_flame_stick_to_player), NET_SVCL_FLAME_STICK_TO_PLAYER, 0);
						}
					}
				}

				damageTime++;
				if (damageTime >= 20)
				{
					damageTime = 0;
					if (scene->server) 
					{
						scene->server->game->radiusHit(currentCF.position, .5f, fromID, WEAPON_COCKTAIL_MOLOTOV);
					}
				}
			}
			else
			{
				if (stickToPlayer >= 0)
				{
					//console->add( CString("sticktoplayer = %i on flameID = %i",stickToPlayer,this->uniqueProjectileID));
#ifndef CONSOLE
					if (scene->client->game->players[stickToPlayer])
					{
						if (scene->client->game->players[stickToPlayer]->status == PLAYER_STATUS_DEAD)
						{
							stickToPlayer = -1;
							//--- Oups pas bon!?
						}
						else
						{
							currentCF.position = scene->client->game->players[stickToPlayer]->currentCF.position;
						}
					}
#endif
				}
			}
		}

		// On check les collisions
		if (map && projectileType == PROJECTILE_FLAME && !movementLock)
		{
			// On test si on ne pogne pas un babo!
			CVector3f p1 = lastCF.position;
			CVector3f p2 = currentCF.position;
			CVector3f normal;
			if (map->rayTest(p1, p2, normal))
			{
				// On lock les mouvements du feu
				movementLock = true;

				// On se cré un spot par terre, pis on lock les mouvement du feu là
				currentCF.position = p2 + normal*.1f;
			}
		}

		if (speed > .5f || currentCF.position[2] > .2f)
		{
			if (projectileType == PROJECTILE_GRENADE || projectileType == PROJECTILE_LIFE_PACK || projectileType == PROJECTILE_DROPED_WEAPON || projectileType == PROJECTILE_DROPED_GRENADE)
			{
				// On déplace avec la velocity
				currentCF.position += currentCF.vel * delay;

				// On affecte la gravitée!
				currentCF.vel[2] -= 9.8f * delay; // (suposont q'un babo fait 50cm de diamètre)
			}

			if (map && projectileType == PROJECTILE_GRENADE || projectileType == PROJECTILE_LIFE_PACK || projectileType == PROJECTILE_DROPED_WEAPON || projectileType == PROJECTILE_DROPED_GRENADE)
			{
				CVector3f p1 = lastCF.position;
				CVector3f p2 = currentCF.position;
				CVector3f normal;
				if (map->rayTest(p1, p2, normal))
				{
					// On dit à tout le monde de jouer le son (pour l'instant juste server side)
				//	net_svcl_play_sound playSound;
				//	playSound.position[0] = (unsigned char)p2[0];
				//	playSound.position[1] = (unsigned char)p2[1];
				//	playSound.position[2] = (unsigned char)p2[2];
				//	playSound.volume = 200;
				//	playSound.range = 1;
				//	playSound.soundID = SOUND_GRENADE_REBOUND;
				//	bb_serverSend((char*)&playSound, sizeof(net_svcl_play_sound), NET_SVCL_PLAY_SOUND, 0);
					if (remoteEntity)
					{
						//--- Play the sound
#ifndef CONSOLE
						dksPlay3DSound(gameVar.sfx_grenadeRebond, -1, 1, p2, 200);
#endif
					}
					currentCF.position = p2 + normal*.01f;
					currentCF.vel = reflect(currentCF.vel, normal);
					currentCF.vel *= .65f;
				}
			}
		}
		else
		{
			currentCF.vel.set(0,0,0);
		}

		// C le server et lui seul qui décide quand il est temps de mettre fin à ses jours
		if (!remoteEntity)
		{
			duration -= delay;
			if (duration <= 0) 
			{

				// KATABOOM
				if (projectileType == PROJECTILE_GRENADE)
				{
					if(needToBeDeleted) return;

                    needToBeDeleted = true;
					
					// On se cré DA explosion :P
					net_svcl_explosion explosion;
					explosion.position[0] = currentCF.position[0];
					explosion.position[1] = currentCF.position[1];
					explosion.position[2] = currentCF.position[2];
					explosion.playerID = (char)(-1);
					explosion.normal[0] = 0;
					explosion.normal[1] = 0;
					explosion.normal[2] = 1;
					explosion.radius = 1.5f;
					bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
					if (scene->server) scene->server->game->radiusHit(currentCF.position, 3, fromID, WEAPON_GRENADE);

					// On spawn du feu
				/*	net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.playerID = fromID;
					playerProjectile.nuzzleID = 0;
					playerProjectile.position[0] = currentCF.position[0];
					playerProjectile.position[1] = currentCF.position[1];
					playerProjectile.position[2] = currentCF.position[2];
					playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
					playerProjectile.projectileType = PROJECTILE_FLAME;
					playerProjectile.vel[0] = 0;
					playerProjectile.vel[1] = 0;
					playerProjectile.vel[2] = 0;
					scene->server->game->spawnProjectile(playerProjectile,true);*/

				/*	for (int i=0;i<3;++i)
					{
					//	console->add("Creating flame");
						net_clsv_svcl_player_projectile playerProjectile;
						playerProjectile.playerID = fromID;
						playerProjectile.nuzzleID = 0;
						playerProjectile.position[0] = currentCF.position[0];
						playerProjectile.position[1] = currentCF.position[1];
						playerProjectile.position[2] = currentCF.position[2];
						playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
						playerProjectile.projectileType = PROJECTILE_FLAME;
						CVector3f vel = rand(CVector3f(-2,-2,0),CVector3f(2,2,1));
						playerProjectile.vel[0] = vel[0];
						playerProjectile.vel[1] = vel[1];
						playerProjectile.vel[2] = vel[2];
						scene->server->game->spawnProjectile(playerProjectile,true);
					}*/
					return;
				}
				else
				{
					needToBeDeleted = true;
				}
				return;
			}
		}

		// On check les collisions
		float zookaRadius = 3.0;
		if (gameVar.sv_zookaRemoteDet && gameVar.sv_serverType == 1)
			zookaRadius = gameVar.sv_zookaRadius;
		if (gameVar.sv_serverType = 1)
		{
			gameVar.weapons[WEAPON_BAZOOKA]->damage = gameVar.sv_zookaDamage;
		}
		else
		{
			gameVar.weapons[WEAPON_BAZOOKA]->damage = 0.75f;
		}
		if (map && projectileType == PROJECTILE_ROCKET && !remoteEntity && !needToBeDeleted)
		{
			// On test si on ne pogne pas un babo!
			Player * playerInRadius = (scene->server)?scene->server->game->playerInRadius(currentCF.position, .25f):0;
			if (playerInRadius) if (playerInRadius->playerID == fromID) playerInRadius = 0;
			if (playerInRadius)
			{
				scene->server->game->players[fromID]->rocketInAir = false;
				scene->server->game->players[fromID]->detonateRocket = false;
				// On frappe un mec !!! KKAAAABBOOOUUMM PLEIN DE SANG MOUHOUHAHAHAHHA
				needToBeDeleted = true;
				// On se cré DA explosion :P
				net_svcl_explosion explosion;
				explosion.position[0] = playerInRadius->currentCF.position[0];
				explosion.position[1] = playerInRadius->currentCF.position[1];
				explosion.position[2] = playerInRadius->currentCF.position[2];
				explosion.normal[0] = 0;
				explosion.normal[1] = 0;
				explosion.normal[2] = 1;
				explosion.radius = zookaRadius;
				explosion.playerID = fromID;
				bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
				if (scene->server) scene->server->game->radiusHit(playerInRadius->currentCF.position, zookaRadius, fromID, WEAPON_BAZOOKA);
				return;
			}
			else
			{
				CVector3f p1 = lastCF.position;
				CVector3f p2 = currentCF.position;
				CVector3f normal;
				if (map->rayTest(p1, p2, normal) || scene->server->game->players[fromID]->detonateRocket)
				{
					scene->server->game->players[fromID]->rocketInAir = false;
					scene->server->game->players[fromID]->detonateRocket = false;
					p2 += normal * .1f;
					// On frappe un mur !!! KKAAAABBOOOUUMM
					needToBeDeleted = true;
					// On se cré DA explosion :P
					net_svcl_explosion explosion;
					explosion.position[0] = p2[0];
					explosion.position[1] = p2[1];
					explosion.position[2] = p2[2];
					explosion.normal[0] = normal[0];
					explosion.normal[1] = normal[1];
					explosion.normal[2] = normal[2];
					explosion.radius = zookaRadius;
					explosion.playerID = fromID;
					bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
					if (scene->server) scene->server->game->radiusHit(p2, zookaRadius, fromID, WEAPON_BAZOOKA);

					// On spawn du feu
				/*	net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.playerID = fromID;
					playerProjectile.nuzzleID = 0;
					playerProjectile.position[0] = p2[0];
					playerProjectile.position[1] = p2[1];
					playerProjectile.position[2] = p2[2];
					playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
					playerProjectile.projectileType = PROJECTILE_FLAME;
					playerProjectile.vel[0] = 0;
					playerProjectile.vel[1] = 0;
					playerProjectile.vel[2] = 0;
					scene->server->game->spawnProjectile(playerProjectile,true);*/

				/*	for (int i=0;i<3;++i)
					{
					//	console->add("Creating flame");
						net_clsv_svcl_player_projectile playerProjectile;
						playerProjectile.playerID = fromID;
						playerProjectile.nuzzleID = 0;
						playerProjectile.position[0] = p2[0];
						playerProjectile.position[1] = p2[1];
						playerProjectile.position[2] = p2[2];
						playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
						playerProjectile.projectileType = PROJECTILE_FLAME;
						CVector3f vel = reflect(currentCF.vel*.15f,normal) + rand(CVector3f(-1,-1,0),CVector3f(1,1,1));
						playerProjectile.vel[0] = vel[0];
						playerProjectile.vel[1] = vel[1];
						playerProjectile.vel[2] = vel[2];
						scene->server->game->spawnProjectile(playerProjectile,true);
					}*/
					return;
				}
			}
		}

		// On check les collisions
		if (map && projectileType == PROJECTILE_COCKTAIL_MOLOTOV && !remoteEntity && !needToBeDeleted)
		{
			// On test si on ne pogne pas un babo!
			Player * playerInRadius = (scene->server)?scene->server->game->playerInRadius(currentCF.position, .25f, (int)fromID):0;
			if (playerInRadius)
			{
				if (playerInRadius->playerID == fromID) playerInRadius = 0;
			}
			if (playerInRadius)
			{
				//console->add( CString("from id : %i",playerInRadius->playerID));
				// On frappe un mec !!! Flak MOLOTOV PARTY!
				needToBeDeleted = true;

				// On se cré DA FLAME explosion :P
				net_svcl_play_sound playSound;
				playSound.position[0] = (unsigned char)currentCF.position[0];
				playSound.position[1] = (unsigned char)currentCF.position[1];
				playSound.position[2] = (unsigned char)currentCF.position[2];
				playSound.volume = 250;
				playSound.range = 5;
				playSound.soundID = SOUND_MOLOTOV;
				bb_serverSend((char*)&playSound, sizeof(net_svcl_play_sound), NET_SVCL_PLAY_SOUND, 0);

				// On spawn du feu
				net_clsv_svcl_player_projectile playerProjectile;
				playerProjectile.playerID = fromID;
				playerProjectile.nuzzleID = 0;
				playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
				playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
				playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
				playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
				playerProjectile.projectileType = PROJECTILE_FLAME;
				playerProjectile.vel[0] = 0;
				playerProjectile.vel[1] = 0;
				playerProjectile.vel[2] = 0;
				scene->server->game->spawnProjectile(playerProjectile,true);

				//for (int i=0;i<1;++i)
				//{
				//	console->add("Creating flame");
					//net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.playerID = fromID;
					playerProjectile.nuzzleID = 0;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
					playerProjectile.projectileType = PROJECTILE_FLAME;
					CVector3f vel = currentCF.vel*.5f + rand(CVector3f(-1,-1,1),CVector3f(1,1,2));
					playerProjectile.vel[0] = 0;(char)(vel[0] * 10);
					playerProjectile.vel[1] = 0;(char)(vel[1] * 10);
					playerProjectile.vel[2] = 0;(char)(vel[2] * 10);
					scene->server->game->spawnProjectile(playerProjectile,true);
				//}
				return;
			}
			else
			{
				CVector3f p1 = lastCF.position;
				CVector3f p2 = currentCF.position;
				CVector3f normal;
				if (map->rayTest(p1, p2, normal))
				{
					currentCF.position = p2 + normal*.1f;

					// On frappe un mur ou un plancher, Molotov Party time
					needToBeDeleted = true;
					// On se cré DA FLAME explosion :P
					net_svcl_play_sound playSound;
					playSound.position[0] = (unsigned char)p2[0];
					playSound.position[1] = (unsigned char)p2[1];
					playSound.position[2] = (unsigned char)p2[2];
					playSound.volume = 250;
					playSound.range = 5;
					playSound.soundID = SOUND_MOLOTOV;
					bb_serverSend((char*)&playSound, sizeof(net_svcl_play_sound), NET_SVCL_PLAY_SOUND, 0);

					// On spawn du feu
					net_clsv_svcl_player_projectile playerProjectile;
					playerProjectile.playerID = fromID;
					playerProjectile.nuzzleID = 0;
					playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
					playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
					playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
					playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
					playerProjectile.projectileType = PROJECTILE_FLAME;
					playerProjectile.vel[0] = 0;
					playerProjectile.vel[1] = 0;
					playerProjectile.vel[2] = 0;
					scene->server->game->spawnProjectile(playerProjectile,true);
					for (int i=0;i<1;++i)
					{
					//	console->add("Creating flame");
						net_clsv_svcl_player_projectile playerProjectile;
						playerProjectile.playerID = fromID;
						playerProjectile.nuzzleID = 0;
						playerProjectile.position[0] = (short)(currentCF.position[0] * 100);
						playerProjectile.position[1] = (short)(currentCF.position[1] * 100);
						playerProjectile.position[2] = (short)(currentCF.position[2] * 100);
						playerProjectile.weaponID = 0;//WEAPON_COCKTAIL_MOLOTOV;
						playerProjectile.projectileType = PROJECTILE_FLAME;
						CVector3f vel = reflect(currentCF.vel*.5f, normal) + rand(CVector3f(-1,-1,0),CVector3f(1,1,1));
						playerProjectile.vel[0] = (char)(vel[0] * 10);
						playerProjectile.vel[1] = (char)(vel[1] * 10);
						playerProjectile.vel[2] = (char)(vel[2] * 10);
						scene->server->game->spawnProjectile(playerProjectile,true);
					}
					return;
				}
			}
		}

		// On ramasse de la vie
		if (projectileType == PROJECTILE_LIFE_PACK && !remoteEntity  && !needToBeDeleted)
		{
			// On test si on ne pogne pas un babo!
			Player * playerInRadius = (scene->server)?scene->server->game->playerInRadius(CVector3f(currentCF.position[0], currentCF.position[1], .25f), .25f):0;
			if (playerInRadius)
			{
				// On lui donne de la vie yééé
				playerInRadius->life += .5f;
				if (playerInRadius->life > 1) playerInRadius->life = 1;
				needToBeDeleted = true;
				net_svcl_pickup_item pickupItem;
				pickupItem.playerID = playerInRadius->playerID;
				pickupItem.itemType = ITEM_LIFE_PACK;
				pickupItem.itemFlag = 0;
				bb_serverSend((char*)&pickupItem, sizeof(net_svcl_pickup_item), NET_SVCL_PICKUP_ITEM, 0);
				return;
			}
		}

		// On ramasse les grenades
		if (projectileType == PROJECTILE_DROPED_GRENADE && !remoteEntity && !needToBeDeleted)
		{
			// On test si on ne pogne pas un babo!
			Player * playerInRadius = (scene->server)?scene->server->game->playerInRadius(CVector3f(currentCF.position[0], currentCF.position[1], .25f), .25f):0;
			if (playerInRadius)
			{
				// On lui donne la grenade
				playerInRadius->nbGrenadeLeft += 1;
				if (playerInRadius->nbGrenadeLeft > 3) playerInRadius->nbGrenadeLeft = 3;
				needToBeDeleted = true;
				net_svcl_pickup_item pickupItem;
				pickupItem.playerID = playerInRadius->playerID;
				pickupItem.itemType = ITEM_GRENADE;
				pickupItem.itemFlag = 0;
				bb_serverSend((char*)&pickupItem, sizeof(net_svcl_pickup_item), NET_SVCL_PICKUP_ITEM, 0);
				return;
			}
		}
#ifndef CONSOLE
//	}
#endif
}


#ifndef CONSOLE
//
// Pour l'afficher (client Only)
//
void Projectile::render()
{
	// Les effects de la rocket
	if (projectileType == PROJECTILE_ROCKET)
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glPushMatrix();
				// Shot glow
				glTranslatef(currentCF.position[0], currentCF.position[1], 0);
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
				glColor4f(1,1,1,rand(.05f, .25f));
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(-2.5f,2.5f,0);
					glTexCoord2f(0,0);
					glVertex3f(-2.5f,-2.5f,0);
					glTexCoord2f(1,0);
					glVertex3f(2.5f,-2.5f,0);
					glTexCoord2f(1,1);
					glVertex3f(2.5f,2.5f,0);
				glEnd();
				glEnable(GL_DEPTH_TEST);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(currentCF.position[0], currentCF.position[1], currentCF.position[2]);
				glRotatef(currentCF.angle, 0, 0, 1);
				glRotatef(rand(.0f, 360.0f), 0, 1, 0);
				glScalef(.5f, .5f, .5f);
				glEnable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_nuzzleFlash);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glDisable(GL_CULL_FACE);
				glDisable(GL_FOG);
				glColor4f(1,1,1,1.0f);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0);
					glVertex3f(-.25f,0,0);
					glTexCoord2f(0,1);
					glVertex3f(-.25f,-1,0);
					glTexCoord2f(1,1);
					glVertex3f(.25f,-1,0);
					glTexCoord2f(1,0);
					glVertex3f(.25f,0,0);

					glTexCoord2f(0,0);
					glVertex3f(0,0,.25f);
					glTexCoord2f(0,1);
					glVertex3f(0,-1,.25f);
					glTexCoord2f(1,1);
					glVertex3f(0,-1,-.25f);
					glTexCoord2f(1,0);
					glVertex3f(0,0,-.25f);
				glEnd();
			glPopMatrix();
		glPopAttrib();
#endif
	}

	// Les effects du molotov
	if (projectileType == PROJECTILE_COCKTAIL_MOLOTOV)
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glPushMatrix();
				// Le glow lumineux
				glTranslatef(currentCF.position[0], currentCF.position[1], 0);
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
				glColor4f(1,1,1,rand(.05f, .25f));
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(-2.5f,2.5f,0);
					glTexCoord2f(0,0);
					glVertex3f(-2.5f,-2.5f,0);
					glTexCoord2f(1,0);
					glVertex3f(2.5f,-2.5f,0);
					glTexCoord2f(1,1);
					glVertex3f(2.5f,2.5f,0);
				glEnd();
				glEnable(GL_DEPTH_TEST);
			glPopMatrix();
		glPopAttrib();
#endif
	}

	if (projectileType != PROJECTILE_FLAME)
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_LIGHTING);
			glPushMatrix();
				glTranslatef(currentCF.position[0], currentCF.position[1], currentCF.position[2]);
				if (projectileType == PROJECTILE_ROCKET) 
				{
					glRotatef(currentCF.angle, 0, 0, 1);
					glScalef(.0025f,.0025f,.0025f);
					dkoRender(gameVar.dko_rocket); // Voilà!
				}
				if (projectileType == PROJECTILE_GRENADE) 
				{
					glRotatef(rotation, currentCF.vel[0], currentCF.vel[1], 0);
					glScalef(.0025f,.0025f,.0025f);
					dkoRender(gameVar.dko_grenade); // Voilà!
				}
				if (projectileType == PROJECTILE_COCKTAIL_MOLOTOV) 
				{
					glRotatef(rotation, currentCF.vel[0], currentCF.vel[1], 0);
					glScalef(.0025f,.0025f,.0025f);
					dkoRender(gameVar.dko_cocktailMolotov); // Voilà!
				}
				if (projectileType == PROJECTILE_DROPED_GRENADE) 
				{
				//	glTranslatef(0,0,.30f);
					glScalef(.0025f,.0025f,.0025f);
					dkoRender(gameVar.dko_grenade); // Voilà!
				}
				if (projectileType == PROJECTILE_LIFE_PACK) 
				{
					glTranslatef(0,0,-.20f);
					glScalef(.0025f,.0025f,.0025f);
					dkoRender(gameVar.dko_lifePack); // Voilà!
				}
				if (projectileType == PROJECTILE_DROPED_WEAPON) 
				{
					glTranslatef(0,0,-.30f);
					glRotatef(rotation, 0, 0, 1);
					glScalef(.005f,.005f,.005f);
					if (fromID >= 0) 
					{
						if (gameVar.weapons[fromID]) dkoRender(gameVar.weapons[fromID]->dkoModel); // Voilà!
					}
				}
			glPopMatrix();
		glPopAttrib();
#endif
	}
	else
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glPushMatrix();
				// Shot glow
				glTranslatef(currentCF.position[0], currentCF.position[1], 0);
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
				glColor4f(1,.75f,0,rand(.10f, .15f)*(1-currentCF.position[2]));
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(-1.0f,1.0f,0);
					glTexCoord2f(0,0);
					glVertex3f(-1.0f,-1.0f,0);
					glTexCoord2f(1,0);
					glVertex3f(1.0f,-1.0f,0);
					glTexCoord2f(1,1);
					glVertex3f(1.0f,1.0f,0);
				glEnd();
				glEnable(GL_DEPTH_TEST);
			glPopMatrix();
		glPopAttrib();
#endif
	}
}

void Projectile::renderShadow()
{
#ifndef _DX_
	// On render son shadow :)
	glPushMatrix();
		glTranslatef(currentCF.position[0]+.1f, currentCF.position[1]-.1f, 0.025f);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);
			glVertex2f(-.25f, .25f);
			glTexCoord2f(0,0);
			glVertex2f(-.25f, -.25f);
			glTexCoord2f(1,0);
			glVertex2f(.25f, -.25f);
			glTexCoord2f(1,1);
			glVertex2f(.25f, .25f);
		glEnd();
	glPopMatrix();
#endif
}
#endif


//
// pour updater le coordFrame avec celui du server
//
void Projectile::setCoordFrame(net_svcl_projectile_coord_frame & projectileCoordFrame)
{
	if (netCF1.frameID > projectileCoordFrame.frameID) return;

	// Notre dernier keyframe change pour celui qu'on est rendu
	netCF0 = currentCF;
	netCF0.frameID = netCF1.frameID; // On pogne le frameID de l'ancien packet par contre
	cFProgression = 0; // On commence au début de la courbe ;)

	// On donne la nouvelle velocity à notre entity
	currentCF.vel[0] = (float)projectileCoordFrame.vel[0] / 10.0f;
	currentCF.vel[1] = (float)projectileCoordFrame.vel[1] / 10.0f;
	currentCF.vel[2] = (float)projectileCoordFrame.vel[2] / 10.0f;

	// Son frame ID
	netCF1.frameID = projectileCoordFrame.frameID;

	// Va faloir interpoler ici et prédire (job's done!)
	netCF1.position[0] = (float)projectileCoordFrame.position[0] / 100.0f;
	netCF1.position[1] = (float)projectileCoordFrame.position[1] / 100.0f;
	netCF1.position[2] = (float)projectileCoordFrame.position[2] / 100.0f;

	// Sa velocity
	netCF1.vel[0] = (float)projectileCoordFrame.vel[0] / 10.0f;
	netCF1.vel[1] = (float)projectileCoordFrame.vel[1] / 10.0f;
	netCF1.vel[2] = (float)projectileCoordFrame.vel[2] / 10.0f;

	// Si notre frameID était à 0, on le copie direct
	if (netCF0.frameID == 0) 
	{
		netCF0 = netCF1;
	}
}

