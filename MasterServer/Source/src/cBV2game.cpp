#include "cBV2game.h"


cBV2game::cBV2game()
{
	Next		=	0;
	Previous	=	0;

	LastCheck	=	0;
	//ID			=	0;
	//ServerID	=	0;
}

cBV2game::cBV2game(char *ip,stBV2row & gameinfos, unsigned long baboID)
{
	Next		=	0;
	Previous	=	0;

	LastCheck	=	0;
	//ServerID	=	0;

	BaboID		=	baboID;

	memcpy(&GameInfos,&gameinfos,sizeof(stBV2row));
	
	if(GameInfos.Priority == 0)
	{
		sprintf(GameInfos.ip,ip);
	}
}

int cBV2game::Update(float elapsed)
{
	LastCheck += elapsed;

	if(LastCheck >= (GameInfos.Priority ? GAME_TIMEOUT * 5 : GAME_TIMEOUT))
	{
		//si on arrive a une minute, la game est morte
		return 1;
	}

	return 0;
}
