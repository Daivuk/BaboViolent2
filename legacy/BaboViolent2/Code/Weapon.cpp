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

#include "Weapon.h"
#include "GameVar.h"
#include "Player.h"
#include "Map.h"
#include "Game.h"
#include "Scene.h"

extern Scene * scene;



//
// Constructeur
//
Weapon::Weapon(CString dkoFilename, CString soundFilename, float pFireDelay, CString pWeaponName, float pDamage, float pImp, int pNbShot, float pReculVel, float pStartImp, int pWeaponID, int pProjectileType)
{
	currentFireDelay = 0;
	charge = 0;
	m_owner = 0;
	justCharged = 0;
	isInstance = false;

	chainOverHeat = 1;
	overHeated = false;

	shotInc = 0;
	fullReload = true;
	weaponName = pWeaponName;

	fireDelay = pFireDelay;
	currentFireDelay = 0; // On est ready à tirer au début

#ifndef DEDICATED_SERVER
	dkoFile = dkoFilename;
	soundFile = soundFilename;
#endif
	damage = pDamage;
	impressision = pImp;
	nbShot = pNbShot;
	reculVel = pReculVel;
	startImp = pStartImp;
	weaponID = pWeaponID;
	projectileType = pProjectileType;

	/* Maybe PM/cnik can do this later, current problems are:
		- doesn't work for projectile weps like zook
		- needs other vars for stuff like overheat
		- values should be reset to defaults on all disconnects or connects, otherwise it becomes an easy hacking tool
	CString names[] = {"SMG", "Shotgun", "Sniper", "DMG", "CG", "Bazooka", "Photon", "FT", "Grenade", "Molotov", "Knives", "Nuke", "Shield"};
	CString var_name = CString("sv_weapon%s_", names[weaponID].s);

	dksvarRegister(CString("%s%s [float : default %1f]", var_name.s, "damage", damage), &damage, 0, 0, LIMIT_MIN, true);
	dksvarRegister(CString("%s%s [float : default %1f]", var_name.s, "recoil", reculVel), &reculVel, 0, 0, LIMIT_MIN, true);
	dksvarRegister(CString("%s%s [float : default %1f]", var_name.s, "Imp", impressision), &impressision, 0, 0, LIMIT_MIN, true);
	dksvarRegister(CString("%s%s [float : default %1f]", var_name.s, "startImp", startImp), &startImp, 0, 0, LIMIT_MIN, true);
	dksvarRegister(CString("%s%s [float : default %1f]", var_name.s, "fireDelay", fireDelay), &fireDelay, 0, 0, LIMIT_MIN, true);
	dksvarRegister(CString("%s%s [int : default %i]", var_name.s, "shots", nbShot), &nbShot, 0, 0, LIMIT_MIN, true);;
	*/
}

Weapon::Weapon(Weapon * pWeapon)
{
    int i;

	shotInc = 0;
	fullReload = true;
	isInstance = true;
	currentFireDelay = 0;

	chainOverHeat = 1;
	overHeated = false;
	m_owner = 0;
	justCharged = 0;
	charge = 0;

	weaponName = pWeapon->weaponName;
#ifndef DEDICATED_SERVER
	dkoModel = pWeapon->dkoModel;
	dkoAlternative = pWeapon->dkoAlternative;
	modelAnim = 0;
#endif
	fireDelay = pWeapon->fireDelay;
	currentFireDelay = 0;
#ifndef DEDICATED_SERVER
	firingNuzzle = 0;
#endif
	damage = pWeapon->damage;
	impressision = pWeapon->impressision;
	nbShot = pWeapon->nbShot;
	reculVel = pWeapon->reculVel;
	currentImp = startImp = pWeapon->startImp;
	weaponID = pWeapon->weaponID;
	projectileType = pWeapon->projectileType;
#ifndef DEDICATED_SERVER
	sfx_sound = pWeapon->sfx_sound;
	for (i=0;i<(int)pWeapon->nuzzleFlashes.size();++i)
	{
		nuzzleFlashes.push_back(new NuzzleFlash(pWeapon->nuzzleFlashes[i]));
	}
	for (i=0;i<(int)pWeapon->ejectingBrass.size();++i)
	{
		ejectingBrass.push_back(new NuzzleFlash(pWeapon->ejectingBrass[i]));
	}
#endif
}


#ifndef DEDICATED_SERVER
void Weapon::loadModels()
{
	firingNuzzle = 0;
	dkoModel = dkoLoadFile(dkoFile.s);
	dkoAlternative = 0;
	if (weaponID == WEAPON_SHIELD)
	{
		dkoAlternative = dkoLoadFile("main/models/ShieldMagnet.DKO");
	}
	modelAnim = 0;

	sfx_sound = dksCreateSoundFromFile(soundFile.s, false);

	int i=0;
	unsigned int flashID;
	CVector3f pos;
	CMatrix3x3f mat;
	do
	{
		i++;
		flashID = dkoGetDummy(CString("flash%i",i).s, dkoModel);
		if (flashID)
		{
			dkoGetDummyPosition(flashID, dkoModel, pos.s);
			dkoGetDummyMatrix(flashID, dkoModel, mat.s);
			nuzzleFlashes.push_back(new NuzzleFlash(mat, pos));
		}
	} while (flashID);

	i = 0;
	do
	{
		i++;
		flashID = dkoGetDummy(CString("eject%i",i).s, dkoModel);
		if (flashID)
		{
			dkoGetDummyPosition(flashID, dkoModel, pos.s);
			dkoGetDummyMatrix(flashID, dkoModel, mat.s);
			ejectingBrass.push_back(new NuzzleFlash(mat, pos));
		}
	} while (flashID);
}


void NuzzleFlash::render()
{
	if (delay > 0)
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
			glDisable(GL_FOG);
			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glPushMatrix();
				// Shot glow
				glTranslatef(position[0], position[1], position[2]-(.5f/.005f));
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
				glColor4f(1,1,1,(delay/NUZZLE_DELAY)*(delay/NUZZLE_DELAY) * .25f);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(-500,500,0);
					glTexCoord2f(0,0);
					glVertex3f(-500,-500,0);
					glTexCoord2f(1,0);
					glVertex3f(500,-500,0);
					glTexCoord2f(1,1);
					glVertex3f(500,500,0);
				glEnd();
				glEnable(GL_DEPTH_TEST);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(position[0], position[1], position[2]);
				glScalef((1-delay/NUZZLE_DELAY)*2+.5f, (1-delay/NUZZLE_DELAY)*2+.5f, (1-delay/NUZZLE_DELAY)*2+.5f);
				glRotatef(angle, 0, 1, 0);
				glEnable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, gameVar.tex_nuzzleFlash);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glDisable(GL_CULL_FACE);
				glColor4f(1,1,1,delay/NUZZLE_DELAY);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(-50,100,0);
					glTexCoord2f(0,0);
					glVertex3f(-50,0,0);
					glTexCoord2f(1,0);
					glVertex3f(50,0,0);
					glTexCoord2f(1,1);
					glVertex3f(50,100,0);

					glTexCoord2f(0,1);
					glVertex3f(0,100,50);
					glTexCoord2f(0,0);
					glVertex3f(0,0,50);
					glTexCoord2f(1,0);
					glVertex3f(0,0,-50);
					glTexCoord2f(1,1);
					glVertex3f(0,100,-50);
				glEnd();
			glPopMatrix();
		glPopAttrib();
#endif
	}
}
#endif


//
// Destructeur
//
Weapon::~Weapon()
{
#ifndef DEDICATED_SERVER
int i;
	if (!isInstance)
	{
		dkoDeleteModel(&dkoModel);
		dksDeleteSound(sfx_sound);
		if (dkoAlternative) dkoDeleteModel(&dkoAlternative);
	}
	for ( i=0;i<(int)nuzzleFlashes.size();++i)
	{
		delete nuzzleFlashes[i];
	}
	nuzzleFlashes.clear();
	for (i=0;i<(int)ejectingBrass.size();++i)
	{
		delete ejectingBrass[i];
	}
	ejectingBrass.clear();
#endif
}



#ifndef DEDICATED_SERVER
//
// On tire
//
void Weapon::shoot(Player * owner)
{
	if (weaponID == WEAPON_BAZOOKA && owner->rocketInAir && currentFireDelay <= fireDelay - 0.25f)
	{
		//we get the direction right so this doesnt create a bug with older servers
		if (nuzzleFlashes.size() > 0)
		{
			owner->currentCF.position[2] = .25f;
			gameVar.dkpp_firingSmoke.positionFrom = nuzzleFlashes[firingNuzzle]->position * .005f;
			gameVar.dkpp_firingSmoke.positionFrom = rotateAboutAxis(gameVar.dkpp_firingSmoke.positionFrom, owner->currentCF.angle, CVector3f(0,0,1)) + owner->currentCF.position;
			gameVar.dkpp_firingSmoke.positionFrom[2] -= owner->currentCF.position[2];
			gameVar.dkpp_firingSmoke.positionTo = gameVar.dkpp_firingSmoke.positionFrom;
			gameVar.dkpp_firingSmoke.direction.set(0,1,0);
			gameVar.dkpp_firingSmoke.direction = rotateAboutAxis(gameVar.dkpp_firingSmoke.direction, owner->currentCF.angle, CVector3f(0,0,1));
			gameVar.dkpp_firingSmoke.pitchTo = 45;
		}
		owner->game->shoot(gameVar.dkpp_firingSmoke.positionFrom, gameVar.dkpp_firingSmoke.direction, 0, damage, owner, projectileType);
		return;
	}
	if (currentFireDelay <= 0 && !overHeated)
	{
		if (weaponID == WEAPON_PHOTON_RIFLE && charge < 0.5f)
		{
			if (charge == 0)
			{
				if (justCharged == 0)
				{
					justCharged = 1;
					//--- We start the charge, tell the other to play charge sound
					net_svcl_play_sound playSound;
					playSound.position[0] = (unsigned char)(owner->currentCF.position[0]);
					playSound.position[1] = (unsigned char)(owner->currentCF.position[1]);
					playSound.position[2] = (unsigned char)(owner->currentCF.position[2]);
					playSound.soundID = SOUND_PHOTON_START;
					playSound.volume = 150;
					playSound.range = 5;
					bb_clientSend(scene->client->uniqueClientID, (char*)(&playSound), sizeof(net_svcl_play_sound), NET_SVCL_PLAY_SOUND);
					dksPlay3DSound(gameVar.sfx_photonStart, -1, 5, owner->currentCF.position, 150);
				}
			}
			charge += .033333f;
			return;
		}
		charge = 0;
		modelAnim = 0;
		chainOverHeat -= .052f;      
		if (chainOverHeat < 0)
		{
			chainOverHeat = 0;
			if (weaponID == WEAPON_CHAIN_GUN)
			{
				// Un ptit son pour agrémenter tout ça!
				net_svcl_play_sound playSound;
				playSound.position[0] = (unsigned char)(owner->currentCF.position[0]);
				playSound.position[1] = (unsigned char)(owner->currentCF.position[1]);
				playSound.position[2] = (unsigned char)(owner->currentCF.position[2]);
				playSound.soundID = SOUND_OVERHEAT;
				playSound.volume = 150;
				playSound.range = 5;
				bb_clientSend(scene->client->uniqueClientID, (char*)(&playSound), sizeof(net_svcl_play_sound), NET_SVCL_PLAY_SOUND);
				dksPlay3DSound(gameVar.sfx_overHeat, -1, 5, owner->currentCF.position, 150);
				overHeated = true;

				//--- Un ti peu de fume?
			}
		}

		firingNuzzle++;
		if (firingNuzzle >= (int)nuzzleFlashes.size()) firingNuzzle = 0;
		currentFireDelay = fireDelay;

		//--- Si on est shotgun, pis que ça fait 6 shot, on reload
		shotInc++;
		if (shotInc >= 6 && weaponID == WEAPON_SHOTGUN)
		{
			if (gameVar.sv_enableShotgunReload) 
			{
				currentFireDelay = 3;
				fullReload = true;
			}
			else
				shotInc = 0;
		}

		if (nuzzleFlashes.size() > 0)
		{
		/*	if (weaponID == WEAPON_FLAME_THROWER)
			{
				nuzzleFlashes[firingNuzzle]->shoot();
				owner->currentCF.position[2] = .25f;
				gameVar.dkpp_firingSmoke.positionFrom = nuzzleFlashes[firingNuzzle]->position * .005f;
				gameVar.dkpp_firingSmoke.positionFrom = rotateAboutAxis(gameVar.dkpp_firingSmoke.positionFrom, owner->currentCF.angle, CVector3f(0,0,1)) + owner->currentCF.position;
				gameVar.dkpp_firingSmoke.positionFrom[2] -= owner->currentCF.position[2];
				gameVar.dkpp_firingSmoke.positionTo = gameVar.dkpp_firingSmoke.positionFrom;
				gameVar.dkpp_firingSmoke.direction.set(0,1,0);
				gameVar.dkpp_firingSmoke.direction = rotateAboutAxis(gameVar.dkpp_firingSmoke.direction, owner->currentCF.angle, CVector3f(0,0,1));
				gameVar.dkpp_firingSmoke.pitchTo = 45;
			}
			else
			{*/
				if (weaponID != WEAPON_FLAME_THROWER) nuzzleFlashes[firingNuzzle]->shoot();
				owner->currentCF.position[2] = .25f;
				gameVar.dkpp_firingSmoke.positionFrom = nuzzleFlashes[firingNuzzle]->position * .005f;
				gameVar.dkpp_firingSmoke.positionFrom = rotateAboutAxis(gameVar.dkpp_firingSmoke.positionFrom, owner->currentCF.angle, CVector3f(0,0,1)) + owner->currentCF.position;
				gameVar.dkpp_firingSmoke.positionFrom[2] -= owner->currentCF.position[2];
				gameVar.dkpp_firingSmoke.positionTo = gameVar.dkpp_firingSmoke.positionFrom;
				gameVar.dkpp_firingSmoke.direction.set(0,1,0);
				gameVar.dkpp_firingSmoke.direction = rotateAboutAxis(gameVar.dkpp_firingSmoke.direction, owner->currentCF.angle, CVector3f(0,0,1));
				gameVar.dkpp_firingSmoke.pitchTo = 45;
		//	}

			// On subit un recul
			owner->currentCF.vel -= gameVar.dkpp_firingSmoke.direction * reculVel;

			// On entends ça
			if (owner->fireFrameDelay == 0)
			{
				dksPlay3DSound(sfx_sound, -1, 5, owner->currentCF.position,255);
				owner->fireFrameDelay = 2;
			}

			// On en shot le nb qui faut
#ifdef _PRO_
			owner->shootShakeDis = -gameVar.dkpp_firingSmoke.direction * reculVel * .5f;
			//for (int i=0;i<nbShot;++i)
			{
				owner->game->shoot(gameVar.dkpp_firingSmoke.positionFrom, gameVar.dkpp_firingSmoke.direction, 0, damage, owner, projectileType);
			}			
#else
			{
				currentImp += 3;
				if (currentImp > impressision) currentImp = impressision;
				owner->shootShakeDis = -gameVar.dkpp_firingSmoke.direction * reculVel * .5f;
				for (int i=0;i<nbShot;++i)
				{
					owner->game->shoot(gameVar.dkpp_firingSmoke.positionFrom, gameVar.dkpp_firingSmoke.direction, currentImp, damage, owner, projectileType);
				}
			}
#endif


			if (projectileType == PROJECTILE_DIRECT && weaponID != WEAPON_FLAME_THROWER && weaponID != WEAPON_PHOTON_RIFLE)
			{
				CVector3f pos = rotateAboutAxis(ejectingBrass[firingNuzzle]->position * .005f, owner->currentCF.angle, CVector3f(0,0,1)) + owner->currentCF.position - CVector3f(0,0,.25f);
				CVector3f dir = rotateAboutAxis(ejectingBrass[firingNuzzle]->matrix.getUp(), owner->currentCF.angle, CVector3f(0,0,1));
				CVector3f right = rotateAboutAxis(ejectingBrass[firingNuzzle]->matrix.getRight(), owner->currentCF.angle, CVector3f(0,0,1));
				gameVar.dkpp_firingSmoke.positionFrom = pos;
				gameVar.dkpp_firingSmoke.positionTo = pos;
				gameVar.dkpp_firingSmoke.direction = dir;
				gameVar.dkpp_firingSmoke.pitchTo = 0;
				dkpCreateParticleExP(gameVar.dkpp_firingSmoke);
				if (gameVar.r_showCasing) owner->game->douilles.push_back(new Douille(pos, dir*(damage+1), right));
			}

			dkpCreateParticleExP(gameVar.dkpp_firingSmoke);
		}
		else
		{
			if (weaponID == WEAPON_KNIVES || weaponID == WEAPON_NUCLEAR)
			{
				// On shoot melee
				net_clsv_svcl_player_shoot_melee playerShootMelee;
				playerShootMelee.playerID = owner->playerID;
				bb_clientSend(scene->client->uniqueClientID, (char*)&playerShootMelee, sizeof(net_clsv_svcl_player_shoot_melee), NET_CLSV_SVCL_PLAYER_SHOOT_MELEE);

				// On entends ça
				if (owner->fireFrameDelay == 0)
				{
					dksPlay3DSound(sfx_sound, -1, 5, owner->currentCF.position,255);
					owner->fireFrameDelay = 2;
					modelAnim = 0;
					currentFireDelay = fireDelay;
				}
			}
		}
	}
}

#endif
// Le server shot le player Melee
void Weapon::shootMeleeSV(Player * owner)
{
	currentFireDelay = fireDelay;
	owner->fireFrameDelay = 2;
	switch (weaponID)
	{
	case WEAPON_KNIVES:
		//--- On tue toute dans un rayon de 1 :D
		owner->game->radiusHit(owner->currentCF.position, 1, owner->playerID, weaponID, true);
		break;
	case WEAPON_NUCLEAR:
//#ifndef DEDICATED_SERVER
		owner->SpawnNukeBotSV();
		nukeFrameID = 0;
//#endif
		break;
	case WEAPON_SHIELD:
		//--- Protect this player for 2 seconde
		owner->protection = 2;
		break;
#ifdef _PRO_
	case WEAPON_MINIBOT:
		if (!owner->minibot)
			owner->SpawnMiniBotSV();
		break;
#endif
	}
}
#ifndef DEDICATED_SERVER

// Le server shot le player Melee
void Weapon::shootMelee(Player * owner)
{
	if(owner->meleeWeapon->weaponID == WEAPON_SHIELD || 
		owner->meleeWeapon->weaponID == WEAPON_KNIVES || owner->minibot)
	{
		nukeFrameID = 0;
		dksPlay3DSound(sfx_sound, -1, 5, owner->currentCF.position,255);
		owner->fireFrameDelay = 2;
		currentFireDelay = fireDelay;
		modelAnim = 0;
	}
}

void Weapon::shoot(net_svcl_player_shoot & playerShoot, Player * owner)
{
	modelAnim = 0;

	if (weaponID != WEAPON_FLAME_THROWER) nuzzleFlashes[playerShoot.nuzzleID]->shoot();
/*	else
	{
		//--- Just spawn flames particule.
		return;
	}*/

	CVector3f p1;
	p1[0] = (float)playerShoot.p1[0] / 100.0f;
	p1[1] = (float)playerShoot.p1[1] / 100.0f;
	p1[2] = (float)playerShoot.p1[2] / 100.0f;
	CVector3f p2;
	p2[0] = (float)playerShoot.p2[0] / 100.0f;
	p2[1] = (float)playerShoot.p2[1] / 100.0f;
	p2[2] = (float)playerShoot.p2[2] / 100.0f;
	CVector3f normal;
	normal[0] = (float)playerShoot.normal[0] / 120.0f;
	normal[1] = (float)playerShoot.normal[1] / 120.0f;
	normal[2] = (float)playerShoot.normal[2] / 120.0f;
	gameVar.dkpp_firingSmoke.positionFrom = p1;
	gameVar.dkpp_firingSmoke.positionTo = gameVar.dkpp_firingSmoke.positionFrom;
	gameVar.dkpp_firingSmoke.direction.set(0,1,0);
	gameVar.dkpp_firingSmoke.direction = rotateAboutAxis(gameVar.dkpp_firingSmoke.direction, owner->currentCF.angle, CVector3f(0,0,1));
	gameVar.dkpp_firingSmoke.pitchTo = 45;

	// Un ptit son pour agrémenter tout ça!
	dksPlay3DSound(sfx_sound, -1, 5, gameVar.dkpp_firingSmoke.positionFrom,150);

	dkpCreateParticleExP(gameVar.dkpp_firingSmoke);

	if (projectileType == PROJECTILE_DIRECT && weaponID != WEAPON_FLAME_THROWER)
	{
		if (weaponID != WEAPON_PHOTON_RIFLE)
		{
			CVector3f pos = rotateAboutAxis(ejectingBrass[playerShoot.nuzzleID]->position * .005f, owner->currentCF.angle, CVector3f(0,0,1)) + owner->currentCF.position - CVector3f(0,0,.25f);
			CVector3f dir = rotateAboutAxis(ejectingBrass[playerShoot.nuzzleID]->matrix.getUp(), owner->currentCF.angle, CVector3f(0,0,1));
			CVector3f right = rotateAboutAxis(ejectingBrass[playerShoot.nuzzleID]->matrix.getRight(), owner->currentCF.angle, CVector3f(0,0,1));
			gameVar.dkpp_firingSmoke.positionFrom = pos;
			gameVar.dkpp_firingSmoke.positionTo = pos;
			gameVar.dkpp_firingSmoke.direction = dir;
			gameVar.dkpp_firingSmoke.pitchTo = 0;
			dkpCreateParticleExP(gameVar.dkpp_firingSmoke);
			if (gameVar.r_showCasing)  owner->game->douilles.push_back(new Douille(pos, dir*(damage+1), right));
		}
		owner->game->spawnImpact(p1, p2, normal, this, damage, owner->teamID);
	}
	else if (weaponID == WEAPON_FLAME_THROWER)
	{
		//--- Spawn fire
		CVector3f firePos;
		float percent = 0;
		for (float i=0;i<=1;i+=.05f)
		{
			firePos = p1 + (p2 - p1) * i;
			dkpCreateParticle(	(firePos+rand(CVector3f(-i*.3f,-i*.3f,0),CVector3f(i*.3f,i*.3f,0))).s,//float *position,
								(CVector3f(0,0,1) + normal).s,//float *vel,
								rand(CVector4f(i,0,1-i,0.0f),CVector4f(i,i*.75f,1-i,0.0f)).s,//float *startColor,
								CVector4f(i,i*.75f,1-i,1.0f).s,//float *endColor,
								.6f * (i*.5f + .5f),//float startSize,
								.0f,//float endSize,
								i/*.5f*/,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								rand(0.0f, 30.0f),//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE,//unsigned int dstBlend,
								0);//int transitionFunc);
			dkpCreateParticle(	(firePos+rand(CVector3f(-i*.3f,-i*.3f,0),CVector3f(i*.3f,i*.3f,0))).s,//float *position,
								(CVector3f(0,0,1) + normal+rand(CVector3f(-.20f,-.20f,0),CVector3f(.20f,.20f,0))).s,//float *vel,
								rand(CVector4f(i,0,1-i,1.0f),CVector4f(i,i*.75f,1-i,1.0f)).s,//float *startColor,
								CVector4f(i,i*.75f,1-i,0.0f).s,//float *endColor,
								.0f,//float startSize,
								.6f * (i*.5f + .5f),//float endSize,
								i/*.5f*/,//float duration,
								0,//float gravityInfluence,
								0,//float airResistanceInfluence,
								rand(0.0f, 30.0f),//float rotationSpeed,
								gameVar.tex_smoke1,//unsigned int texture,
								DKP_SRC_ALPHA,//unsigned int srcBlend,
								DKP_ONE,//unsigned int dstBlend,
								0);//int transitionFunc);
		}
	}
}
#endif



//
// On l'update
//
void Weapon::update(float delay)
{
	if (justCharged > 0) justCharged -= delay;
	if (justCharged < 0) justCharged = 0;
	if (m_owner) if (m_owner->status != PLAYER_STATUS_ALIVE) return;
	if (currentImp > startImp)
	{
		currentImp -= delay * 10;
		if (currentImp < startImp)
		{
			currentImp = startImp;
		}
	}
	//--- flame thrower need a tite flame bleu
	if (weaponID == WEAPON_FLAME_THROWER)
	{
		//--- On va oublier ça pour tout suite
	}
	if (currentFireDelay > 0)
	{
		currentFireDelay -= delay;
		if (weaponID == WEAPON_KNIVES)
		{
			if (currentFireDelay > fireDelay - .10f)
			{
				modelAnim = (1 - ((currentFireDelay - (fireDelay - .10f)) / .10f)) * 10.0f;
				if (modelAnim > 10) modelAnim = 10;
			}
			else if (currentFireDelay < .25f)
			{
				modelAnim = (currentFireDelay / .25f) * 10.0f;
				if (modelAnim < 0) modelAnim = 0;
			}
			else
			{
				modelAnim = 10;
			}
		}
		if (weaponID == WEAPON_SHIELD)
		{
			modelAnim = (3 - currentFireDelay) / 3.0f * 20.0f;
			if (modelAnim > 20) modelAnim = 20;
		}
		if (weaponID == WEAPON_NUCLEAR)
		{
			nukeFrameID++;
			if (nukeFrameID % 45 == 0 && m_owner->minibot)
			{
				if (!m_owner->game->isServerGame)
				{
#ifndef DEDICATED_SERVER
					dksPlay3DSound(sfx_sound, -1, 5, m_owner->minibot->currentCF.position,255);
#endif
				}
			}

			//--- On explose apres 2sec
			if (nukeFrameID >= 30 * gameVar.sv_nukeTimer && m_owner)
			{
				if (m_owner->game->isServerGame && m_owner->minibot)
				{
					//--- KABOOMMMMMMMMMMMMM MODA FUKA
					net_svcl_explosion explosion;
					explosion.position[0] = m_owner->minibot->currentCF.position[0];
					explosion.position[1] = m_owner->minibot->currentCF.position[1];
					explosion.position[2] = m_owner->minibot->currentCF.position[2];
					explosion.normal[0] = 0;
					explosion.normal[1] = 0;
					explosion.normal[2] = 1;
					explosion.playerID = m_owner->playerID;
					explosion.radius = gameVar.sv_nukeRadius;
					bb_serverSend((char*)&explosion, sizeof(net_svcl_explosion), NET_SVCL_EXPLOSION, 0);
					if (scene->server) scene->server->game->radiusHit(m_owner->minibot->currentCF.position, gameVar.sv_nukeRadius, m_owner->playerID, weaponID);
					delete m_owner->minibot;
					m_owner->minibot = 0;
				}
			}
		}
	}
	else
	{
		modelAnim = 0;
	}

	chainOverHeat += delay * .25f;
	if (chainOverHeat > 1)
	{
		chainOverHeat = 1;
		overHeated = false;
	}

	if (chainOverHeat > 0.5)
	{
		overHeated = false;
	}

	if (overHeated)
	{
	/*	gameVar.dkpp_firingSmoke.positionFrom = p1;
		gameVar.dkpp_firingSmoke.positionTo = gameVar.dkpp_firingSmoke.positionFrom;
		gameVar.dkpp_firingSmoke.direction.set(0,1,0);
		gameVar.dkpp_firingSmoke.direction = rotateAboutAxis(gameVar.dkpp_firingSmoke.direction, owner->currentCF.angle, CVector3f(0,0,1));
		gameVar.dkpp_firingSmoke.pitchTo = 45;
		dkpCreateParticleExP(gameVar.dkpp_firingSmoke);*/
	}
#ifndef DEDICATED_SERVER
	for (int i=0;i<(int)nuzzleFlashes.size();++i)
	{
		nuzzleFlashes[i]->update(delay);
	}
#endif
}

#ifndef DEDICATED_SERVER
//
// On l'affiche
//
void Weapon::render()
{
	if (weaponID == WEAPON_KNIVES)
	{
#ifndef _DX_
		glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.3f);
			dkoRender(dkoModel, modelAnim);
		glPopAttrib();
#endif
	}
	else if (weaponID == WEAPON_SHIELD)
	{
#ifndef _DX_
		dkoRender(dkoAlternative, modelAnim);
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
			glPushMatrix();
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				dkoRender(dkoModel, modelAnim);
				glDepthMask(GL_FALSE);
				for (float i=0;i<10;++i)
				{
				//	glScalef(1.05f,1.05f,1.05f);
					glRotatef(36,0,0,1);
					dkoRender(dkoModel, modelAnim);
				}
			glPopMatrix();
		glPopAttrib();
#endif
		if (modelAnim < 10)
		{
#ifndef _DX_
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
				glDisable(GL_FOG);
				glDisable(GL_LIGHTING);
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glPushMatrix();
					// Shot glow
					glEnable(GL_TEXTURE_2D);
					glDisable(GL_DEPTH_TEST);
					glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
					glColor4f(0,.9f,1,1 - (modelAnim/10.0f));
					glBegin(GL_QUADS);
						glTexCoord2f(0,1);
						glVertex3f(-25,25,0);
						glTexCoord2f(0,0);
						glVertex3f(-25,-25,0);
						glTexCoord2f(1,0);
						glVertex3f(25,-25,0);
						glTexCoord2f(1,1);
						glVertex3f(25,25,0);
					glEnd();
					glEnable(GL_DEPTH_TEST);
				glPopMatrix();
			glPopAttrib();
#endif
		}
	}
	else if (weaponID == WEAPON_NUCLEAR)
	{
		if (nukeFrameID % 45 < 23 && m_owner->minibot && this->currentFireDelay > 0)
		{
#ifndef _DX_
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
				glDisable(GL_FOG);
				glDisable(GL_LIGHTING);
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glPushMatrix();
					// Shot glow
					glEnable(GL_TEXTURE_2D);
					glDisable(GL_DEPTH_TEST);
					glBindTexture(GL_TEXTURE_2D, gameVar.tex_shotGlow);
					glColor4f(1,.25f,.25f,.5f);
					glBegin(GL_QUADS);
						glTexCoord2f(0,1);
						glVertex3f(-1000,1000,0);
						glTexCoord2f(0,0);
						glVertex3f(-1000,-1000,0);
						glTexCoord2f(1,0);
						glVertex3f(1000,-1000,0);
						glTexCoord2f(1,1);
						glVertex3f(1000,1000,0);
					glEnd();
					glEnable(GL_DEPTH_TEST);
				glPopMatrix();
			glPopAttrib();
#endif
		}
		dkoRender(dkoModel);
	}
	else
	{
		dkoRender(dkoModel);
		for (int i=0;i<(int)nuzzleFlashes.size();++i)
		{
			nuzzleFlashes[i]->render();
		}
	}
}
#endif

