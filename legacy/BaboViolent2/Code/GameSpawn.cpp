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
#include "Console.h"
#include "GameVar.h"
#ifndef DEDICATED_SERVER
#include "Client.h"



//
// Pour spawner du sang quand on touche quelqu'un
//
void Game::spawnBlood(CVector3f & position, float damage)
{
	float bloodColor;

	if (damage > 2.0) damage = 2.0;

	for (int i=0;i<(int)(damage*10);++i)
	{
		bloodColor = rand(0.3f, 1.0f);

		dkpCreateParticleEx(
			position, //CVector3f & positionFrom,
			position, //CVector3f & positionTo,
			CVector3f(0,0,1), //CVector3f & direction,
			damage, //float speedFrom,
			damage*2, //float speedTo,
			0, //float pitchFrom,
			90, //float pitchTo,
			.25f, //float startSizeFrom,
			.25f, //float startSizeTo,
			.25f, //float endSizeFrom,
			.25f, //float endSizeTo,
			2, //float durationFrom,
			2, //float durationTo,
			CColor4f(bloodColor,0,0,1), //CColor4f & startColorFrom,
			CColor4f(bloodColor,0,0,1), //CColor4f & startColorTo,
			CColor4f(bloodColor,0,0,0), //CColor4f & endColorFrom,
			CColor4f(bloodColor,0,0,0), //CColor4f & endColorTo,
			0, //float angleFrom,
			360, //float angleTo,
			-30, //float angleSpeedFrom,
			30, //float angleSpeedTo,
			.1f, //float gravityInfluence,
			0, //float airResistanceInfluence,
			1, //unsigned int particleCountFrom,
			1, //unsigned int particleCountTo,
			gameVar.tex_blood + rand()%10,
			0, //int textureFrameCount,
			DKP_SRC_ALPHA, //unsigned int srcBlend,
			DKP_ONE_MINUS_SRC_ALPHA);//unsigned int dstBlend);

		// On cré les marks de sang au sol
		CVector3f pos(1,0,0);
		pos = rotateAboutAxis(pos, rand(0.0f, 360.0f), CVector3f(0,0,1));
		float distance = rand(0.0f, damage*2.5f);
		float sizeMax = (1-(distance)/2.5f) * 1.0f;
		pos *= distance;
		pos += position;
		pos[2] = 0;
		floorMarks[getNextFloorMark()].set(pos,rand(0.0f, 360.0f),rand(.05f,sizeMax),30,distance*0.5f,gameVar.tex_blood[rand()%10], CVector4f(rand(.25f,0.5f),0,0,rand(.5f,1.0f)));
	}
}

#ifdef _PRO_
void Game::spawnBloodMinibot(CVector3f & position, float damage)
{
	float bloodColor;

	if (damage > 2.0) damage = 2.0;

	for (int i=0;i<(int)(damage*10);++i)
	{
		bloodColor = rand(0.3f, 1.0f);

		dkpCreateParticleEx(
			position, //CVector3f & positionFrom,
			position, //CVector3f & positionTo,
			CVector3f(0,0,1), //CVector3f & direction,
			damage, //float speedFrom,
			damage*2, //float speedTo,
			0, //float pitchFrom,
			90, //float pitchTo,
			.25f, //float startSizeFrom,
			.25f, //float startSizeTo,
			.25f, //float endSizeFrom,
			.25f, //float endSizeTo,
			2, //float durationFrom,
			2, //float durationTo,
			CColor4f(bloodColor,0,0,1), //CColor4f & startColorFrom,
			CColor4f(bloodColor,0,0,1), //CColor4f & startColorTo,
			CColor4f(bloodColor,0,0,0), //CColor4f & endColorFrom,
			CColor4f(bloodColor,0,0,0), //CColor4f & endColorTo,
			0, //float angleFrom,
			360, //float angleTo,
			-30, //float angleSpeedFrom,
			30, //float angleSpeedTo,
			.1f, //float gravityInfluence,
			0, //float airResistanceInfluence,
			1, //unsigned int particleCountFrom,
			1, //unsigned int particleCountTo,
			gameVar.tex_blood + rand()%10,
			0, //int textureFrameCount,
			DKP_SRC_ALPHA, //unsigned int srcBlend,
			DKP_ONE);//unsigned int dstBlend);

		// On cré les marks de sang au sol
		CVector3f pos(1,0,0);
		pos = rotateAboutAxis(pos, rand(0.0f, 360.0f), CVector3f(0,0,1));
		float distance = rand(0.0f, damage*2.5f);
		float sizeMax = (1-(distance)/2.5f) * 1.0f;
		pos *= distance;
		pos += position;
		pos[2] = 0;
		floorMarks[getNextFloorMark()].set(pos,rand(0.0f, 360.0f),rand(.05f,sizeMax),30,distance*0.5f,gameVar.tex_blood[rand()%10], CVector4f(rand(0.5f,1.0f),0,0,rand(.5f,1.0f)));
	}
}
#endif
#endif



//
// Spawner un player
//
bool Game::spawnPlayer(int playerID)
{
	if (players[playerID] && map)
	{
		if (players[playerID]->teamID == PLAYER_TEAM_BLUE ||
			players[playerID]->teamID == PLAYER_TEAM_RED)
		{

#ifdef _PRO_
			// On lui trouve une place libre loin des ennemies
         if (gameType == GAME_TYPE_SND)
			{
				float currentScore = 0;
				int bestFound = 0;
				if (map->dm_spawns.size() > 0)
				{
               // Player has a spawnslot, spawn him there
               if (players[playerID]->spawnSlot != -1)
               {
                  bestFound = players[playerID]->spawnSlot;
               }
               else
               {
					// Find us a free slot                  
               for (int i=0;i<MAX_PLAYER;++i)
					{						
						int nbPlayer = 0;
                  bool spawnUsed = false;
						
                  // Pour chaque spawn point on check pour chaque player
						for (int j=0;j<MAX_PLAYER;++j)
						{
							if (players[j] && (j != playerID) && (players[j]->spawnSlot == i) && 
                        ((players[j]->teamID == PLAYER_TEAM_BLUE) || (players[j]->teamID == PLAYER_TEAM_RED)))
							{
                        spawnUsed = true;
							}
						}

                  if (spawnUsed == false)
                  {
                  bestFound = i;
                  players[playerID]->spawnSlot = bestFound;
                  break;
                  }
               }
              
               }

               int spawnSize= 5;
               unsigned int spawnLocation = bestFound*spawnSize+ rand(0, spawnSize);
               if (spawnLocation > map->dm_spawns.size())
                  spawnLocation = (int)map->dm_spawns.size()-1;
               
					players[playerID]->spawn(CVector3f(map->dm_spawns[spawnLocation][0],map->dm_spawns[spawnLocation][1],.25f));
#ifndef DEDICATED_SERVER
					map->setCameraPos(players[playerID]->currentCF.position);
#endif
					return true;
            }
         }
#endif

			// On lui trouve une place libre loin des ennemies
         if (gameType == GAME_TYPE_DM)
			{
				float currentScore = 0;
				int bestFound = 0;
				if (map->dm_spawns.size() > 0)
				{
					for (int i=0;i<(int)map->dm_spawns.size();++i)
					{
						float nearestPlayer = 100000;
						int nbPlayer = 0;
						// Pour chaque spawn point on check pour chaque player
						for (int j=0;j<MAX_PLAYER;++j)
						{
							if (players[j] && j != playerID)
							{
								if (players[j]->status == PLAYER_STATUS_ALIVE)
								{
									nbPlayer++;
									float dis = distanceSquared(map->dm_spawns[i], players[j]->currentCF.position);
									if (dis < nearestPlayer) nearestPlayer = dis;
								}
							}
						}
						if (nearestPlayer > currentScore)
						{
							currentScore = nearestPlayer;
							bestFound = i;
						}
						if (nbPlayer == 0)
						{
							bestFound = rand()%(int)map->dm_spawns.size();
							break;
						}
					}
					players[playerID]->spawn(CVector3f(map->dm_spawns[bestFound][0],map->dm_spawns[bestFound][1],.25f));
#ifndef DEDICATED_SERVER
					map->setCameraPos(players[playerID]->currentCF.position);
#endif
					return true;
				}
			}

			// On lui trouve une place libre loin des ennemies
			if (gameType == GAME_TYPE_TDM || gameType == GAME_TYPE_CTF)
			{
				float currentScore = 0;
				int bestFound = 0;
				if (map->dm_spawns.size() > 0)
				{
					for (int i=0;i<(int)map->dm_spawns.size();++i)
					{
						float nearestPlayer = 100000;
						int nbPlayer = 0;
						// Pour chaque spawn point on check pour chaque player
						for (int j=0;j<MAX_PLAYER;++j)
						{
							if (players[j] && j != playerID)
							{
								if (players[j]->teamID != players[playerID]->teamID && players[j]->status == PLAYER_STATUS_ALIVE)
								{
									nbPlayer++;
									float dis = distanceSquared(map->dm_spawns[i], players[j]->currentCF.position);
									if (dis < nearestPlayer) nearestPlayer = dis;
								}
							}
						}
						if (nearestPlayer > currentScore)
						{
							currentScore = nearestPlayer;
							bestFound = i;
						}
						if (nbPlayer == 0)
						{
							bestFound = rand()%(int)map->dm_spawns.size();
							break;
						}
					}
               CVector3f spawnPosition(map->dm_spawns[bestFound][0],map->dm_spawns[bestFound][1],.25f);
               
#ifdef _PRO_
               if ((gameType == GAME_TYPE_CTF)&&(spawnType == SPAWN_TYPE_LADDER))
               {
               float timeElapsed = gameVar.sv_gameTimeLimit - gameTimeLeft;               
               if (timeElapsed < 10.0f)
                  {
                  spawnPosition = map->flagPodPos[players[playerID]->teamID];
                  }
               }
#endif               

					players[playerID]->spawn(spawnPosition);
#ifndef DEDICATED_SERVER
					map->setCameraPos(players[playerID]->currentCF.position);
#endif
					return true;
				}
			}
		/*	while (!found)
			{
				int index = rand()%(int)map->dm_spawns.size();
				int x = rand((int)0, (int)map->size[0]);
				int y = rand((int)0, (int)map->size[1]);
				if (map->cells[y*map->size[0]+x].passable)
				{
					found = true;
					players[playerID]->spawn(CVector3f((float)x+.5f,(float)y+.5f,.25f));
					if (players[playerID] == thisPlayer)
					{
						map->setCameraPos(players[playerID]->currentCF.position);
					}
				}
			}
			return true;*/
		}
	}
	return false;
}


#ifndef DEDICATED_SERVER
//
// Pour spawner des particules sur le murs l'hors d'un impact
//
void Game::spawnImpact(CVector3f & p1, CVector3f & p2, CVector3f & normal, Weapon*weapon, float damage, int team)
{
	// Bon, ben on spawn des particules là
	CVector3f front = normal;
	CVector3f right, up;
	createRightUpVectors(right, front, up);
	CMatrix3x3f mat;
	mat.setRight(right);
	mat.setFront(front);
	mat.setUp(up);
	mat.RotateAboutFront(rand(0.0f, 360.0f));
	mat.RotateAboutRight(rand(-30.0f, 30.0f));

	int type = 0;
	if (weapon->weaponID == WEAPON_PHOTON_RIFLE) type = 1;

	// On se cré une trail yo yea
	if (type == 0)
	{
		trails.push_back(new Trail(p1, p2, damage, CVector4f((team==PLAYER_TEAM_RED)?.9f:.5f,.5f,(team==PLAYER_TEAM_BLUE)?.9f:.5f,1), damage*4, 0));
	}
	else if (type == 1)
	{
		if (weapon->weaponID == WEAPON_PHOTON_RIFLE) damage = 2.0f;
		trails.push_back(new Trail(p1, p2, damage, CVector4f((team==PLAYER_TEAM_RED)?.9f:.25f,.25f,(team==PLAYER_TEAM_BLUE)?.9f:.25f,1), damage*4, 0));
		trails.push_back(new Trail(p1, p2, damage / 4, CVector4f((team==PLAYER_TEAM_RED)?.9f:.25f,.25f,(team==PLAYER_TEAM_BLUE)?.9f:.25f,1), damage, 1));
		trails.push_back(new Trail(p1, p2, damage / 8, CVector4f((team==PLAYER_TEAM_RED)?.9f:.25f,.25f,(team==PLAYER_TEAM_BLUE)?.9f:.25f,1), damage, 1));
		trails.push_back(new Trail(p1, p2, damage / 16, CVector4f((team==PLAYER_TEAM_RED)?.9f:.25f,.25f,(team==PLAYER_TEAM_BLUE)?.9f:.25f,1), damage, 1));
	}

	gameVar.ro_hitPoint = p2;

	dkpCreateParticle(	p2.s,//float *position,
						(mat.getFront()*.0f).s,//float *vel,
						CVector4f(1,1,0,1).s,//float *startColor,
						CVector4f(1,1,0,0.0f).s,//float *endColor,
						.15f,//float startSize,
						.15f,//float endSize,
						.3f,//float duration,
						-.1f,//float gravityInfluence,
						0,//float airResistanceInfluence,
						0,//float rotationSpeed,
						gameVar.tex_shotGlow,//unsigned int texture,
						DKP_SRC_ALPHA,//unsigned int srcBlend,
						DKP_ONE,//unsigned int dstBlend,
						0);//int transitionFunc);

	dkpCreateParticle(	p2.s,//float *position,
						(mat.getFront()*.25f).s,//float *vel,
						CVector4f(1,1,1,.5f).s,//float *startColor,
						CVector4f(1,1,1,0.0f).s,//float *endColor,
						0,//float startSize,
						.5f,//float endSize,
						1,//float duration,
						-.1f,//float gravityInfluence,
						0,//float airResistanceInfluence,
						90,//float rotationSpeed,
						gameVar.tex_smoke1,//unsigned int texture,
						DKP_SRC_ALPHA,//unsigned int srcBlend,
						DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
						0);//int transitionFunc);
	dkpCreateParticle(	p2.s,//float *position,
						(mat.getFront()*2.0f).s,//float *vel,
						CVector4f(1,1,1,.75f).s,//float *startColor,
						CVector4f(1,1,1,0.0f).s,//float *endColor,
						.125f,//float startSize,
						.25f,//float endSize,
						.5f,//float duration,
						0,//float gravityInfluence,
						0,//float airResistanceInfluence,
						90,//float rotationSpeed,
						gameVar.tex_smoke1,//unsigned int texture,
						DKP_SRC_ALPHA,//unsigned int srcBlend,
						DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
						0);//int transitionFunc);
	dkpCreateParticle(	p2.s,//float *position,
						(mat.getFront()*4.0f).s,//float *vel,
						CVector4f(1,1,1,.75f).s,//float *startColor,
						CVector4f(1,1,1,0.0f).s,//float *endColor,
						.125f,//float startSize,
						.25f,//float endSize,
						.25f,//float duration,
						0,//float gravityInfluence,
						0,//float airResistanceInfluence,
						90,//float rotationSpeed,
						gameVar.tex_smoke1,//unsigned int texture,
						DKP_SRC_ALPHA,//unsigned int srcBlend,
						DKP_ONE_MINUS_SRC_ALPHA,//unsigned int dstBlend,
						0);//int transitionFunc);

	// on ricoche!
	dksPlay3DSound(gameVar.sfx_ric[rand()%5], -1, 2, p2, 150);
}
#endif


//
// On spawn un projectile!
//
bool Game::spawnProjectile(net_clsv_svcl_player_projectile & playerProjectile, bool imServer)
{
//	if (playerProjectile.projectileType == PROJECTILE_FLAME) return;
	// On le push toujours à la fin du vector, si on respect bien ça les clients devraient tous les avoir
	// dans l'ordre
	if (imServer)
	{
		++(Projectile::uniqueProjectileID);
		playerProjectile.uniqueID = Projectile::uniqueProjectileID;

		if (playerProjectile.projectileType == PROJECTILE_GRENADE)
		{
			if (players[playerProjectile.playerID]->nbGrenadeLeft <= 0) return false;
			if (players[playerProjectile.playerID]) players[playerProjectile.playerID]->nbGrenadeLeft--;
		}
		if (playerProjectile.projectileType == PROJECTILE_COCKTAIL_MOLOTOV)
		{
			if (players[playerProjectile.playerID]->nbMolotovLeft <= 0) return false;
			if (players[playerProjectile.playerID]) players[playerProjectile.playerID]->nbMolotovLeft--;
		}
		if (playerProjectile.projectileType == PROJECTILE_DROPED_WEAPON)
		{
			CVector3f position;
			position[0] = (float)playerProjectile.position[0] / 100.0f;
			position[1] = (float)playerProjectile.position[1] / 100.0f;
			position[2] = (float)playerProjectile.position[2] / 100.0f;
			CVector3f vel;
			vel[0] = (float)playerProjectile.vel[0] / 10.0f;
			vel[1] = (float)playerProjectile.vel[1] / 10.0f;
			vel[2] = (float)playerProjectile.vel[2] / 10.0f;
			projectiles.push_back(new Projectile(position, vel, playerProjectile.weaponID, playerProjectile.projectileType, false,Projectile::uniqueProjectileID));
		}
		else
		{
			CVector3f position;
			position[0] = (float)playerProjectile.position[0] / 100.0f;
			position[1] = (float)playerProjectile.position[1] / 100.0f;
			position[2] = (float)playerProjectile.position[2] / 100.0f;
			CVector3f vel;
			vel[0] = (float)playerProjectile.vel[0] / 10.0f;
			vel[1] = (float)playerProjectile.vel[1] / 10.0f;
			vel[2] = (float)playerProjectile.vel[2] / 10.0f;
			projectiles.push_back(new Projectile(position, vel, playerProjectile.playerID, playerProjectile.projectileType, false,Projectile::uniqueProjectileID));
		}
		projectiles[projectiles.size()-1]->projectileID = (short)projectiles.size()-1;

		if (playerProjectile.projectileType == PROJECTILE_FLAME)
		{
			Projectile * projectile = projectiles[projectiles.size()-1];

			// On demande au server de créer une instance d'une flame
			net_clsv_svcl_player_projectile playerProjectile;
			playerProjectile.playerID = projectile->fromID;
			playerProjectile.nuzzleID = 0;
			playerProjectile.projectileType = projectile->projectileType;
			playerProjectile.weaponID = WEAPON_COCKTAIL_MOLOTOV;
			playerProjectile.position[0] = (short)(projectile->currentCF.position[0] * 100);
			playerProjectile.position[1] = (short)(projectile->currentCF.position[1] * 100);
			playerProjectile.position[2] = (short)(projectile->currentCF.position[2] * 100);
			playerProjectile.vel[0] = (char)(projectile->currentCF.vel[0] * 10);
			playerProjectile.vel[1] = (char)(projectile->currentCF.vel[1] * 10);
			playerProjectile.vel[2] = (char)(projectile->currentCF.vel[2] * 10);
			playerProjectile.uniqueID = projectile->uniqueID;
			bb_serverSend((char*)&playerProjectile,sizeof(net_clsv_svcl_player_projectile),NET_CLSV_SVCL_PLAYER_PROJECTILE,0);
		}
	}
#ifndef DEDICATED_SERVER
	else
	{
		if (playerProjectile.projectileType == PROJECTILE_DROPED_WEAPON)
		{
			CVector3f position;
			position[0] = (float)playerProjectile.position[0] / 100.0f;
			position[1] = (float)playerProjectile.position[1] / 100.0f;
			position[2] = (float)playerProjectile.position[2] / 100.0f;
			CVector3f vel;
			vel[0] = (float)playerProjectile.vel[0] / 10.0f;
			vel[1] = (float)playerProjectile.vel[1] / 10.0f;
			vel[2] = (float)playerProjectile.vel[2] / 10.0f;
			projectiles.push_back(new Projectile(position, vel, playerProjectile.weaponID, playerProjectile.projectileType, true,playerProjectile.uniqueID));
		}
		else
		{
			CVector3f position;
			position[0] = (float)playerProjectile.position[0] / 100.0f;
			position[1] = (float)playerProjectile.position[1] / 100.0f;
			position[2] = (float)playerProjectile.position[2] / 100.0f;
			CVector3f vel;
			vel[0] = (float)playerProjectile.vel[0] / 10.0f;
			vel[1] = (float)playerProjectile.vel[1] / 10.0f;
			vel[2] = (float)playerProjectile.vel[2] / 10.0f;
			projectiles.push_back(new Projectile(position, vel, playerProjectile.playerID, playerProjectile.projectileType, true,playerProjectile.uniqueID));
		}
		projectiles[projectiles.size()-1]->projectileID = (short)projectiles.size()-1;
	//	projectiles[projectiles.size()-1]->remoteEntity = false;
	//	projectiles[projectiles.size()-1]->uniqueID = playerProjectile.uniqueID;
	}
#endif

	return true;
}


#ifndef DEDICATED_SERVER
//
// On se cré une explosion dla mort mouhou :P
//
void Game::spawnExplosion(CVector3f & position, CVector3f & normal, float size)
{
	if (size >= 4.0f)
	{
		NukeFlash* nukeFlash = new NukeFlash();
		nukeFlash->position = position;
		nukeFlash->radius = size * 3;
		nikeFlashes.push_back(nukeFlash);
	}

	float trueSize = size;
	size *= 3;

	// Make it sounds and shake !!!
//	dksPlay3DSound(gameVar.sfx_explosion[0], -1, 10, position,255);
	int channel = FSOUND_PlaySoundEx(-1, gameVar.sfx_explosion[0], 0, TRUE);
	FSOUND_3D_SetMinMaxDistance(channel, 10, 10000000.0f);
	FSOUND_3D_SetAttributes(channel, position.s, 0);
	if (trueSize >= 4.0f)
	{
		FSOUND_SetFrequency(channel, 5000);
	}
	else
	{
		FSOUND_SetFrequency(channel, 22050);
	}
	FSOUND_SetVolume(channel, 255);
	FSOUND_SetPaused(channel, FALSE);

	float duration = size*.5f;

	float maxDuration = gameVar.r_reducedParticles ? 1.0f : 10.0f;
   
	if (duration > maxDuration) duration = maxDuration;

	dkpCreateParticleEx(
		position, //CVector3f & positionFrom,
		position, //CVector3f & positionTo,
		normal, //CVector3f & direction,
		.5f, //float speedFrom,
		.5f, //float speedTo,
		80, //float pitchFrom,
		90, //float pitchTo,
		size*.03f, //float startSizeFrom,
		size*.03f, //float startSizeTo,
		size*.2f, //float endSizeFrom,
		size*.2f, //float endSizeTo,
		duration*.5f, //float durationFrom,
		duration*.5f, //float durationTo,
		CColor4f(0,0,0,1), //CColor4f & startColorFrom,
		CColor4f(0,0,0,1), //CColor4f & startColorTo,
		CColor4f(.7f,.7f,.7f,0), //CColor4f & endColorFrom,
		CColor4f(.7f,.7f,.7f,0), //CColor4f & endColorTo,
		0, //float angleFrom,
		360, //float angleTo,
		0, //float angleSpeedFrom,
		30, //float angleSpeedTo,
		0,//.025f, //float gravityInfluence,
		1, //float airResistanceInfluence,
		20, //unsigned int particleCountFrom,
		20, //unsigned int particleCountTo,
		&(gameVar.tex_smoke1),
		0, //int textureFrameCount,
		DKP_SRC_ALPHA, //unsigned int srcBlend,
		DKP_ONE_MINUS_SRC_ALPHA);//unsigned int dstBlend);

	dkpCreateParticleEx(
		position, //CVector3f & positionFrom,
		position, //CVector3f & positionTo,
		normal, //CVector3f & direction,
		.05f, //float speedFrom,
		.55f, //float speedTo,
		0, //float pitchFrom,
		45, //float pitchTo,
		size*.03f, //float startSizeFrom,
		size*.03f, //float startSizeTo,
		size*.2f, //float endSizeFrom,
		size*.2f, //float endSizeTo,
		duration, //float durationFrom,
		duration, //float durationTo,
		CColor4f(0,0,0,1), //CColor4f & startColorFrom,
		CColor4f(0,0,0,1), //CColor4f & startColorTo,
		CColor4f(.7f,.7f,.7f,0), //CColor4f & endColorFrom,
		CColor4f(.7f,.7f,.7f,0), //CColor4f & endColorTo,
		0, //float angleFrom,
		360, //float angleTo,
		0, //float angleSpeedFrom,
		30, //float angleSpeedTo,
		0,//.025f, //float gravityInfluence,
		1, //float airResistanceInfluence,
		10, //unsigned int particleCountFrom,
		10, //unsigned int particleCountTo,
		&(gameVar.tex_smoke1),
		0, //int textureFrameCount,
		DKP_SRC_ALPHA, //unsigned int srcBlend,
		DKP_ONE_MINUS_SRC_ALPHA);//unsigned int dstBlend);

	dkpCreateParticleEx(
		position, //CVector3f & positionFrom,
		position, //CVector3f & positionTo,
		normal, //CVector3f & direction,
		1.5f, //float speedFrom,
		3.5f, //float speedTo,
		0, //float pitchFrom,
		90, //float pitchTo,
		size*.01f, //float startSizeFrom,
		size*.01f, //float startSizeTo,
		size*.02f, //float endSizeFrom,
		size*.02f, //float endSizeTo,
		duration*.1f, //float durationFrom,
		duration*.1f, //float durationTo,
		CColor4f(1,0,0,1), //CColor4f & startColorFrom,
		CColor4f(1,1,0,1), //CColor4f & startColorTo,
		CColor4f(1,1,0,0), //CColor4f & endColorFrom,
		CColor4f(1,1,0,0), //CColor4f & endColorTo,
		0, //float angleFrom,
		360, //float angleTo,
		0, //float angleSpeedFrom,
		30, //float angleSpeedTo,
		0, //float gravityInfluence,
		1, //float airResistanceInfluence,
		20, //unsigned int particleCountFrom,
		20, //unsigned int particleCountTo,
		&(gameVar.tex_smoke1),
		0, //int textureFrameCount,
		DKP_SRC_ALPHA, //unsigned int srcBlend,
		DKP_ONE);//unsigned int dstBlend);

	dkpCreateParticleEx(
		position, //CVector3f & positionFrom,
		position, //CVector3f & positionTo,
		normal, //CVector3f & direction,
		.05f, //float speedFrom,
		.35f, //float speedTo,
		0, //float pitchFrom,
		90, //float pitchTo,
		size*.05f, //float startSizeFrom,
		size*.05f, //float startSizeTo,
		size*.1f, //float endSizeFrom,
		size*.1f, //float endSizeTo,
		duration*.1f, //float durationFrom,
		duration*.1f, //float durationTo,
		CColor4f(1,0,0,1), //CColor4f & startColorFrom,
		CColor4f(1,1,0,1), //CColor4f & startColorTo,
		CColor4f(1,1,0,0), //CColor4f & endColorFrom,
		CColor4f(1,1,0,0), //CColor4f & endColorTo,
		0, //float angleFrom,
		360, //float angleTo,
		0, //float angleSpeedFrom,
		30, //float angleSpeedTo,
		0, //float gravityInfluence,
		1, //float airResistanceInfluence,
		5, //unsigned int particleCountFrom,
		5, //unsigned int particleCountTo,
		&(gameVar.tex_smoke1),
		0, //int textureFrameCount,
		DKP_SRC_ALPHA, //unsigned int srcBlend,
		DKP_ONE);//unsigned int dstBlend);

	dkpCreateParticleEx(
		position-CVector3f(trueSize*.35f,trueSize*.35f,0), //CVector3f & positionFrom,
		position+CVector3f(trueSize*.35f,trueSize*.35f,0), //CVector3f & positionTo,
		normal, //CVector3f & direction,
		0, //float speedFrom,
		0, //float speedTo,
		0, //float pitchFrom,
		90, //float pitchTo,
		trueSize*.25f, //float startSizeFrom,
		trueSize*.25f, //float startSizeTo,
		trueSize*.5f, //float endSizeFrom,
		trueSize*.5f, //float endSizeTo,
		.25f, //float durationFrom,
		.25f, //float durationTo,
		CColor4f(1,0,0,1), //CColor4f & startColorFrom,
		CColor4f(1,1,0,1), //CColor4f & startColorTo,
		CColor4f(1,0,0,0), //CColor4f & endColorFrom,
		CColor4f(1,1,0,0), //CColor4f & endColorTo,
		0, //float angleFrom,
		360, //float angleTo,
		-180, //float angleSpeedFrom,
		180, //float angleSpeedTo,
		0, //float gravityInfluence,
		1, //float airResistanceInfluence,
		8, //unsigned int particleCountFrom,
		8, //unsigned int particleCountTo,
		&(gameVar.tex_smoke1),
		0, //int textureFrameCount,
		DKP_SRC_ALPHA, //unsigned int srcBlend,
		DKP_ONE);//unsigned int dstBlend);

	floorMarks[getNextFloorMark()].set(CVector3f(position[0], position[1], 0),rand(0.0f, 360.0f),size*.18f,30,0,gameVar.tex_explosionMark, CVector4f(1,1,1,.5f));

	/*if (thisPlayer)
	{

		// La distance
		float dis = distance(position, thisPlayer->currentCF.position);
		if (dis < trueSize*2)
		{
			viewShake += (1-(dis / (trueSize*2))) * 2;
		}
	}// screen shake moved elsewhere (so that it shakes only when you are hit!) */
}
#endif

