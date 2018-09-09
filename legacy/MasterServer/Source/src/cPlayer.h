#ifndef _PLAYER_H
#define _PLAYER_H

#include <memory.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "baboNet.h"
#include "cMSstruct.h"

//client du master server
class cPlayer
{
private:
public:

	char				cdkey[33];		//keep the players cd key here
	char				username[16];		//player's username
	char				password[16];		//player's password

	int				nRow;			//garde le nombre denregistrement retourner par sa requete
	int				nColumn;		//garde le nombre de champs retourner par la requete
	char				**sqlResult;		//on va stocker le resultat de la requete ici
	int				CurrentRow;		//on garde quel enregistrement on est rendu a envoyer
	unsigned long			BabonetID;		//id unique retourner par la babonet lors de la connection


	cPlayer();
	cPlayer(unsigned long babonetID);

	~cPlayer();


	int					Update();		//envoie a la network le prochain enregistrement
};




#endif
