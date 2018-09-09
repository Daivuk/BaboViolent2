#ifndef _CClient_H___
#define _CClient_H___

#include <iostream>
#include "sqlite3.h"
#include "UpdateStructs.h"
#include "baboNet.h"

class CClient
{
public:

	float	mfElaspedSinceLastAck;
	bool	Acked;				// if true, we can send next data to this client, if not, we wait until we get the ack

	CClient	*Next;
	CClient	*Previous;

	char*	CurrentVersion;		//version we will be updating too	

	sqlite3	*db;			//pointer on our database
	char	**azResult;		//SQL query result
	int		nRow;			//number of current row in azResult
	int		nColumn;		//number of column in azResult
	int		CurrentRow;		//tell us what current row are we currently on
	bool	Chunking;		//are we currently sending a file to the client
	long	Size;			//keep the size of the file
	long	Sent;			//keep number of byte we sent
	FILE	*CurrentFile;	//pointer on current file
	char	*Buffer;		//buffer that will be sent to network

	float	TimeOut;		//timeout before the user is forced to disconnect


	unsigned long bbID;	//id returned by babonet
	char	IP[16];

	char	Version[6];	//the clients version running on his machine

	bool	Updating;	//client is currently being updated
	bool	NeedList;	//the client needs to get the list of files he needs ?
	bool	OnlyDB;		//the client only needs the db

	CClient();
	CClient(unsigned long id,char *ip,sqlite3 *db,char *version);

	~CClient();


	void	Update( float elapsed );	//to update a client

	void	QueryFileList();
};



#endif
