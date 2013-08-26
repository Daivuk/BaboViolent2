#ifndef _NET_MANAGER_H
#define _NET_MANAGER_H

//le net manager va gerer notre objet Server, ou notre client, ou les 2
//et communiquer le data necessaire aux bonnes entites presente

#include <map>
#include "cMSstruct.h"
#include "cMasterServer.h"
#include "string.h"


class cNetManager
{
private:

	class RemoteCacheListReq
	{
	public:
		RemoteCacheListReq(unsigned long _fromID, short _reqNum)
		{
			fromID = _fromID;
			reqNum = _reqNum;
		}

		unsigned long fromID;
		short reqNum;
	};

	typedef std::map<unsigned long, RemoteCacheListReq> RemoteCacheListReqMap;
	typedef std::pair<unsigned long, RemoteCacheListReq> RemoteCacheListReqPair;

	RemoteCacheListReqMap cacheRequests;

public:
	

	cMasterServer	*Server;			//notre serveur

	cNetManager();
	~cNetManager();


	void	Init();																					//initialise la babonet

	void	SpawnServer(unsigned short listenPort=11114);							//permet de spawner le master server
	//void	Connect(char* ip,unsigned short port);

	void	RetreiveData();
	void	ReceiveServerPacket(char *data,int typeID,unsigned long fromID);

	bool	Update(float elapsed);	//permet de donner du temps a la network

};

#endif
