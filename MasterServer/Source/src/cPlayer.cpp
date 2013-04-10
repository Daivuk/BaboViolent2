#include "cPlayer.h"


cPlayer::cPlayer()
{
	BabonetID	=	0;

	Next		=	0;
	Previous	=	0;
	sqlResult	=	0;
	nRow		=	0;
	nColumn		=	0;
	CurrentRow	=	0;

	isDead		=	false;

	//LastCheck	=	0;
}

cPlayer::cPlayer(unsigned long babonetID)
{
	BabonetID	=	babonetID;
	isDead		=	false;

	Next		=	0;
	Previous	=	0;

	sqlResult	=	0;
	nRow		=	0;
	nColumn		=	0;
	CurrentRow	=	0;
	//LastCheck	=	0;
}

int cPlayer::Update()
{

	if(CurrentRow == nRow)
	{
		nRow = 0;
		CurrentRow = 0;
		return 1;
	}



	//on va batir une struct pour le prochain enregistrement
	stBV2row row;

	row.flags			=	(unsigned short)atoi(sqlResult[BV2_FLAGS]);
	row.gameType		=	(char)atoi(sqlResult[BV2_GAMETYPE]);
	sprintf(row.ip,sqlResult[BV2_IP]);
	sprintf(row.map,sqlResult[BV2_MAP]);
	row.maxPlayer		=	(char)atoi(sqlResult[BV2_MAXPLAYERS]);
	row.nbPlayer		=	(char)atoi(sqlResult[BV2_NBPLAYERS]);
	sprintf(row.password,sqlResult[BV2_PASSWORD]);
	row.port			=	(unsigned short) atoi(sqlResult[BV2_PORT]);
	sprintf(row.serverName,sqlResult[BV2_SERVERNAME]);


	bb_peerSend(BabonetID,(char*)&row,BV2_ROW,sizeof(stBV2row),true);



	CurrentRow++;
	if(CurrentRow == nRow) return 1;
	return 0;
}

cPlayer::~cPlayer()
{
	if(sqlResult) sqlite3_free_table(sqlResult);
}
