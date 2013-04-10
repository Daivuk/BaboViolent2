#ifndef _BV2_GAME_
#define _BV2_GAME_

#include "stdio.h"
#include "cMSstruct.h"

#ifndef WIN32
	#include "LinuxHeader.h"
#else
	#include "memory.h"
#endif

#define GAME_TIMEOUT	60		//timeout apres 1 minute

class cBV2game
{
public:

	//int		ID;				//garde le id unique dla game
	float		LastCheck;		//combien de temps qu'on a recu un update
	//unsigned short	ServerID;		//garde le ID du server s'il est babostats enabled

	//game infos
	stBV2row	GameInfos;		//keep all game infos here

	unsigned long	BaboID;

	cBV2game	*Next;
	cBV2game	*Previous;

	cBV2game();
	cBV2game(char *ip,stBV2row & gameinfos, unsigned long baboID);


	int	Update(float elapsed);


};



#endif
