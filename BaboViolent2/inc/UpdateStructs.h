#ifndef _UPDATE_STRUCTS_
#define _UPDATE_STRUCTS_

	#define VERSION		1	//version packet
	#define STATUS		2	//a status packet is to get servers response (Need update, or not)
	#define FILE_HEADER	3	//a file header is FileName,FileSize
	#define	FILE_CHUNK	4	//a file chunk is a part of a binary file (ChunkSize,ChunkData)
	#define END_OF_UPDATE	5	//telling the client we have no more file to send him
	#define PING		6	//a ping query
	#define PONG		7	//a ping response
	#define	NEW_NODE	8	//a new server for updating
	#define LOAD_QUERY	9	//load query, an empty packet asking for a load score
	#define LOAD_SCORE	10	//gives to a client our current load
	#define ACK		11	//client ACK a packet, packet is empty, server just needs to need when to continu sending stuff
	#define DB_VERSION	12	//client's db version

	struct stLoadScore
	{
		unsigned char LoadScore;
	};

	struct stVersion
	{
		char Version[6];
	};

	struct stDBVersion
	{
		unsigned short Version;
	};

	struct stStatus
	{
		char Status;	//0 = up to date, 1 = need update
	};

	struct stFileHeader
	{
		char 		Filename[123];
		char		Action;		//0 = adding, 1 = deleting
		long 		Filesize;
	};

	#define CHUNK_SIZE 2000
	struct stFileChunk
	{
		unsigned short	ChunkSize;
		char			*ChunkData; //pointer to the actual data

		//default constructor
		stFileChunk()
		{
			ChunkSize = 0;
			ChunkData = new char[CHUNK_SIZE];
		}

		//default destructor
		~stFileChunk()
		{
			delete [] ChunkData;
		}
	};

	struct stNewNode
	{
		char IP[16];	//new node's IP
	};

	//Macros for file table field indexing 
//#ifndef WIN32//Linux	
	#define ACTION			CurrentRow*nColumn+8	//Action 0 = adding file 1 = removing file
	#define FORMACOS		CurrentRow*nColumn+9
	#define FORLINUX		CurrentRow*nColumn+10
	#define FORWIN32		CurrentRow*nColumn+11
	#define FORSERVER		CurrentRow*nColumn+12
	#define FILENAME		CurrentRow*nColumn+13
	#define FILEID			CurrentRow*nColumn+14
	#define FILEVERSION		CurrentRow*nColumn+15
//#else //Win32
//	#define ACTION			CurrentRow*nColumn+8	//Action 0 = adding file 1 = removing file
//	#define FILENAME		CurrentRow*nColumn+9
//	#define FILEID			CurrentRow*nColumn+10
//	#define FORLINUX		CurrentRow*nColumn+11
//	#define FORMACOS		CurrentRow*nColumn+12
//	#define FORSERVER		CurrentRow*nColumn+13
//	#define FILEVERSION		CurrentRow*nColumn+14
//	#define FORWIN32		CurrentRow*nColumn+15
//#endif



#endif
