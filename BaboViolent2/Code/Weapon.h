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

#ifndef WEAPON_H
#define WEAPON_H


#include "Zeven.h"
#include "netPacket.h"
#include <vector>


class Player;

#ifndef CONSOLE
#define NUZZLE_DELAY .10f
#endif

#define PROJECTILE_DIRECT 1
#define PROJECTILE_ROCKET 2
#define PROJECTILE_GRENADE 3
#define PROJECTILE_LIFE_PACK 4
#define PROJECTILE_DROPED_WEAPON 5
#define PROJECTILE_DROPED_GRENADE 6
#define PROJECTILE_COCKTAIL_MOLOTOV 7
#define PROJECTILE_FLAME 8
#define PROJECTILE_GIB 9
#define PROJECTILE_NONE 10
#define PROJECTILE_PHOTON 11

#ifndef CONSOLE
struct NuzzleFlash
{
	CMatrix3x3f matrix;
	CVector3f position;
	float delay; // le delait d'affichage du feu
	float angle; // l'angle du nuzzle flash, ça c random
	NuzzleFlash(CMatrix3x3f & pMatrix, CVector3f & pPosition)
	{
		delay = 0;
		angle = 0;
		matrix = pMatrix;
		position = pPosition;
	}
	NuzzleFlash(NuzzleFlash * nuzzleFlash)
	{
		delay = 0;
		angle = 0;
		matrix = nuzzleFlash->matrix;
		position = nuzzleFlash->position;
	}
	void update(float pDelay)
	{
		delay -= pDelay;
		if (delay <= 0) delay = 0;
	}
	void shoot()
	{
		delay = NUZZLE_DELAY;
		angle = rand(0.0f, 360.0f);
	}
	void render();
};
#endif


class Weapon
{
public:
#ifndef CONSOLE
	// Son model DKO
	unsigned int dkoModel;
	unsigned int dkoAlternative;
#endif
	float modelAnim; //--- Used in some specific case
	long nukeFrameID;

	// pour le shotty
	int shotInc;
	bool fullReload;
	
	// Pour le photon rifle
	float charge;
	float justCharged;
#ifndef CONSOLE
	// Sa liste de nuzzle (il peut en avoir plusieurs)
	std::vector<NuzzleFlash*> nuzzleFlashes;
	std::vector<NuzzleFlash*> ejectingBrass;

	// À quel nuzzle on est rendu à tirer
	int firingNuzzle;
#endif
	
	// Le fire delay
	float fireDelay;
	float currentFireDelay;

#ifndef CONSOLE
	// For delayed loading of models
	CString dkoFile;
	CString soundFile;

	// Le son
	FSOUND_SAMPLE * sfx_sound;
#endif

	// Si il est juste une instance, on ne delete pas ses ressource
	bool isInstance;

	// Le nom du gun
	CString weaponName;

	// Le damage
	float damage;
	
	// L'impressision
	float impressision;
	float startImp;
	float currentImp;

	// Le nb de shot qu'un tire envoit (shotgun = 6)
	int nbShot;

	// Le recul de quand on tire
	float reculVel;

	// Le ID du gun
	int weaponID;

	//Last position shot from
	CVector3f shotFrom;

	// Le type de projectile que l'ont tire
	int projectileType;

	// Chaingun overheat
	float chainOverHeat;
	bool overHeated;

	Player* m_owner;

public:
	// Constructeur
	Weapon(CString dkoFilename, CString soundFilename, float pFireDelay, CString pWeaponName, float pDamage, float pImp, int pNbShot, float pReculVel, float pStartImp, int pWeaponID, int pProjectileType);
	Weapon(Weapon * pWeapon);

	// Destructeur
	virtual ~Weapon();

	// On tire
#ifndef CONSOLE
	void loadModels();

	void shoot(Player * owner);
	void shoot(net_svcl_player_shoot & playerShoot, Player * owner);
	void shootMelee(Player * owner);
#endif

	// Le server shot le player Melee
	void shootMeleeSV(Player * owner);

	// On l'update
	void update(float delay);

#ifndef CONSOLE
	// On l'affiche
	void render();
#endif
};


#endif

