#include "CClient.h"

CClient::CClient()
{
	Next = 0;
	Previous = 0;
	
	bbID = 0;
	sprintf(Version,"");

	Updating = false;
	NeedList = false;

	CurrentRow = 0;

	Chunking = false;

	Sent = 0;
	Size = 0;
	
	CurrentFile = 0;
	Buffer = new char[2000];

	TimeOut = 0;
	CurrentVersion = 0;

	azResult = 0;
	Acked = true;

	mfElaspedSinceLastAck = 0.0f;
	OnlyDB = false;
}

CClient::CClient(unsigned long id,char *ip,sqlite3 *DB,char *version)
{
	Next = 0;
	Previous = 0;
	
	bbID = id;
	sprintf(IP,ip);
	sprintf(Version,"");

	CurrentVersion = version; //version we will be updating to

	Updating = false;
	NeedList = false;

	db = DB;

	CurrentRow = 0;
	Chunking = false;

	Sent = 0;
	Size = 0;
	
	CurrentFile = 0;
	Buffer = new char[2000];

	TimeOut = 0;
	azResult = 0;
	Acked = true;
	
	mfElaspedSinceLastAck = 0.0f;
	OnlyDB = false;
}

CClient::~ CClient()
{
	if(azResult) sqlite3_free_table(azResult);
	delete [] Buffer;
}

void CClient::Update( float elapsed )
{
	if(!Updating) return;

	if(NeedList)
	{
		NeedList = false;

		//let's query the file list the client needs
		QueryFileList();
	}

	mfElaspedSinceLastAck += elapsed;

	//has 1 second passed since last ack ?
	if ( !Acked && mfElaspedSinceLastAck > 1.0f )
	{
		return;
	}

	if(nRow > CurrentRow)
	{
		if(Chunking) //are we already sending a file to the client ?
		{
			//copy the file into the buffer.
			fread(Buffer,((Size-Sent) > 2000 ? 2000 : (Size-Sent)),1,CurrentFile);

			//send a file chunk
			stFileChunk fc;
			fc.ChunkSize = (unsigned short)((Size-Sent) > 2000 ? 2000 : (Size-Sent));
			
			char * tbuf = new char[2002];
			memcpy( tbuf, &fc.ChunkSize , sizeof(unsigned short) );
			memcpy( tbuf + sizeof(unsigned short), Buffer , fc.ChunkSize);
			bb_serverSend(tbuf, 2 + fc.ChunkSize,FILE_CHUNK,bbID,NET_TCP);
			delete [] tbuf;

			Sent += fc.ChunkSize;
	
			//are we finished sending the file ?
			TimeOut = 0;
			if(Sent==Size)
			{
				Chunking = false;
				Sent = 0;
				Size = 0;
				fclose(CurrentFile);
				CurrentRow++;
			}

			//we are waiting for the next ack
			Acked = false;

		}
		else //here we have finish sending a file, so we open next one
		{
			//check to see if its an added file or a file removal
			int action = atoi(azResult[ACTION]);

			stFileHeader fh; //our file header

			if(!action) //adding a file
			{
			
				//first open the file and seek to the end to get its size
				printf(" > Sending file '%s'\n",azResult[FILENAME]);
				CurrentFile = fopen(azResult[FILENAME],"rb");

				//file didnt exist
				if(!CurrentFile)
				{
					printf("WARNING : file '%s' couldnt be found for an update\n",azResult[FILENAME]);
					Chunking = false;
					Sent = 0;
					Size = 0;
					CurrentRow++;
					return;
				}
		
				// obtain file size.
				fseek (CurrentFile , 0 , SEEK_END);
				Size = ftell(CurrentFile);
				rewind(CurrentFile);
				
	
				//ready to send the file
				//send header first
				fh.Action = 0;
				sprintf(fh.Filename,azResult[FILENAME]);
				fh.Filesize = Size;

				Chunking = true; //tell the client we are actually sending him file chunks on next update(s)
				Sent = 0;
				Acked = false; //we need an ack before sending him chunks
				//LastAcked = 0;
			}
			else
			{
				//deleting a file
				fh.Action = 1;
				
				sprintf(fh.Filename,azResult[FILENAME]);
				fh.Filesize = 0;

				Chunking = false; //tell the client we are not sending him file chunks on next update(s)
				Sent = 0;
				
				CurrentRow++;

				Acked = false; //we need an ack before continuing on next file
				//LastAcked = 0;
			}

			//tell the client about the file we are sending/asking him to delete
			bb_serverSend((char*)&fh,sizeof(stFileHeader),FILE_HEADER,bbID,NET_TCP);
			
		}
	}
	else
	{	//update is finished, tell the client, his new version and that update is finished
		
		if(nRow == CurrentRow)
		{
			CurrentRow++;
			printf(" > Update for Client %li is finished\n",bbID);
			
			if( !OnlyDB )
			{
				stVersion ver;
				sprintf(ver.Version,CurrentVersion);
				bb_serverSend((char*)&ver,sizeof(stVersion),VERSION,bbID,NET_TCP);
			}
			
			bb_serverSend(0,0,END_OF_UPDATE,bbID,NET_TCP);
		}		
	}
}

void CClient::QueryFileList()
{
	//Quelques infos sur la table qu'on va loader
	char	*zErrMsg;		// Contient un message d'erreur s'il y a lieu
	char	SQL[300];

	if( OnlyDB )
	{
		sprintf(SQL,"Select * from FileList Where FileName = 'bv2.db';");
	}
	else
	{
		sprintf(SQL,"Select * from FileList Where Version > %f;",atof(Version));
	}
	
	int dbResult = sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	//printf("Error : %s, dbResult : %i, nRow: %i\n",zErrMsg,dbResult,nRow);

	if(nRow<1)
	{
		Updating = false;
		azResult = 0;//sqlite3_free_table(azResult);
	}

	//printf("Client with ID %li needs to update %i file(s) SQL : %s\n",bbID,nRow,SQL);

	CurrentRow = 0;

}
