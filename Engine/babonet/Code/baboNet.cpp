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

/////////////////////////////////////////////////
// Filename : BaboNet.cpp
// Creation date : 2 juin 2006
// Author : Marc Durocher
// Description : BaboNet network library main code
////////////////////////////////////////////////

#ifdef WIN32
	#define WIN32_MEAN_AND_LEAN
	#pragma comment (lib,"ws2_32.lib")
	#pragma comment (lib,"iphlpapi.lib")
	#include "winsock2.h"
	#include <iphlpapi.h> // this is used for getting mac adress
	#if defined(_DEBUG) && defined(USE_VLD)
		#include <vld.h>
	#endif
#endif
#ifdef __MACOSX__
	#include <net/route.h>
	#include <netinet/in.h>
	#include <net/if.h>
	#include <net/if_dl.h>
	#include <ifaddrs.h>
#endif
#ifndef WIN32
	#include "LinuxHeader.h"
#endif

#include "baboNet.h"		//les fonctions visible au user
#include "cServer.h"
#include "cClient.h"		//nos objets clients
#include "cPacket.h"		//nos packet
#include "cPeer2Peer.h"		//lobjet qui gere toute ce quier peer2peer



	//FILE *fp;			//un fichier de debug

	//une struct qu'on va se servir pour envoyer/recevoir du data
	struct stPacket
	{
		unsigned short	Size;		//le Size du *data
		unsigned short  TypeID;		//savoir c quel type de data
        char			*data;		//un array dynamique qui tien le data a envoyer
	};


//quelques proprietes globale

	
	float			ConnCheck=0;		// va mesurer le delay avant qu'on update les incoming connections
	UINT4		LastClientID=0;		// keep last emitted client ID

	cServer			*Server=0;		// notre objet serveur
	cClient			*MainClients=0;		// nos clients locals
	cPeer2Peer		*P2P=0;			// objet qui gere toute ce quier P2P

	unsigned char		NetBitField=0;		// pour les settings Enable/Disable

	char			Version[5];		// stock la current version dla bbnet

// debugging
#ifdef USING_LOG
	FILE * LogFile=0;
#endif

#ifdef __MACOSX__		
static struct ifaddrs * ifdevices = 0;
static char iface[8];
#endif

cClient *getClientByID(UINT4 clientID)
{
	for(cClient *c = MainClients;c;c=c->Next)
	{
		if(c->NetID	== clientID) return c;
	}

	return 0;
}

void CloseSocket(int socketFD)
{
	#ifdef WIN32
		closesocket(socketFD);
	#else
		close(socketFD);
	#endif
}


int bb_init()
{

#ifdef USING_LOG
	// clear the log file
	LogFile = fopen( "log.txt" , "w" );
	fclose(LogFile);
#endif

	// Version courrante de la bbnet
		sprintf(Version,"4.0");

	//if we are running on windows, we have some initialisation to make
	#ifdef WIN32
		// Tiens les infos sur winsock, dont la version
			WSADATA		WinSockInfo;
				
			int error = WSAStartup (0x0202, &WinSockInfo);  
            	
		// Si il y a une erreur
			if (error)
			{
				//sprintf(LastError,"Error : Problem loading Winsock infos");
				return 1;
			}
	
		// On check si c'est la bonne version de winsock
			if (WinSockInfo.wVersion != 0x0202)
			{
				//sprintf(LastError,"Error : Wrong winsock version ( %s )",WinSockInfo.wVersion);
				//sinon on sort
				WSACleanup();

				#ifdef USING_LOG
					LogFile = fopen( "log.txt" , "a" );
					fprintf( LogFile , "Old version of Winsock detected\n");
					fclose(LogFile);
				#endif

				return 1;
			}

		// On load les info
			#ifdef USING_LOG
				LogFile = fopen( "log.txt" , "a" );
				fprintf( LogFile , "Successfully started Winsock\n");
				fclose(LogFile);
			#endif

	#endif

		Server		=	0;
		MainClients	=	0;
		P2P			=	0;
		
#ifdef __MACOSX__
		getifaddrs(& ifdevices);
		struct ifaddrs * a = ifdevices;
		while(a)
		{
			if(a->ifa_addr->sa_family == AF_INET)
			{
				if(strstr(a->ifa_name, "en"))
				{
					strncpy(iface, a->ifa_name, sizeof(iface));
					break;
				}
			}
			a =	 a->ifa_next;
		}
#endif
 	

		//UDPserver	=	new cUDPserver();

		//on set notre fichier debug
		//fp = fopen("_netDebug.txt","w");
		//fclose(fp);

		// Tout est beau on retourne 0
		return 0;
}

char* bb_getVersion()
{
	return Version;
}

int	bb_serverCreate(bool UDPenabled,int maxClients,unsigned short listenPort)
{
	//si un serveur etait existant on va le deleter dabord
	if(Server) delete Server;

	
	//UDPserver->BindPort(listenPort);
	Server = new cServer(UDPenabled,maxClients,listenPort);
	//UDPserver->Server	=	Server;
	
	if(listenPort==11111)
	{
		sprintf(Server->LastError,"Error spawning server, invalid port, 11111 is bbnet reserved");
		delete Server;
		Server=0;
		return 1;
	}


	return 0;
}

int	bb_serverSetClientRate(int nbBytes,UINT4 baboNetID)
{
	if(!Server) return 1;

	Server->getClientByID(baboNetID)->DataRate	=	(unsigned short)nbBytes;

	return 0;
}

char *bb_serverGetLastError()
{
	if(Server) return Server->getLastError();
	return 0;
}

char *bb_serverGetLastMessage()
{
	if(Server) return Server->getLastMessage();
	return 0;
}

UINT4 bb_serverGetBytesSent()
{
	if(Server)
	{
		return Server->BytesSent;
	}
	return 0;
}

UINT4 bb_serverGetBytesReceived()
{
	if(Server)
	{
		return Server->BytesReceived;
	}
	return 0;
}


INT4 bb_serverUpdate(float elapsed,int updateMsg,char* newIP)
{
	sprintf(Server->LastMessage,"");
	sprintf(Server->LastError,"");



	if(!Server) return -999999999;

	ConnCheck += elapsed;

	switch(updateMsg)
	{
		case UPDATE_SEND_RECV: //on update les Send ET les Receive
		{
			//envoyer le stock TCP/UDP
			if(Server->SendPacketsToClients()) return BBNET_ERROR;
			
			//recevoir TCP/UDP
			INT4 r = Server->ReceivePacketsFromClients();
			if(r) return r;

			
			//on update les connections
			if(ConnCheck >= 0.5f)
			{
				ConnCheck = 0;
				
				if(Server->PendingConnections)
				{
					return Server->UpdateConnections(newIP);
				}
			}

			return 0;
		}
		case UPDATE_SEND: //on update que les send
		{
			//on envoie le stock TCP/UDP
			if(Server->SendPacketsToClients()) return -999999999;
			return 0;
		}
		case UPDATE_RECV: //on update que les recv
		{
			//on recois le stock TCP/UDP
			INT4 r = Server->ReceivePacketsFromClients();
			if(r) return r;

			
			//on update les connections
			if(ConnCheck >= 0.5f)
			{
				ConnCheck = 0;
				
				if(Server->PendingConnections)
				{
					return Server->UpdateConnections(newIP);
				}
			}


			return 0;
		}
	}
	return 0;
}


int bb_serverSend(char* dataToSend,int dataSize,int typeID,INT4 destination,int protocol)
{
	if(!Server)
	{
		//sprintf(LastError,"Error : No server created");
		return 1;
	}

	Server->Send(dataToSend,dataSize,typeID,destination,protocol);

	return 0;
}

char* bb_serverReceive(UINT4 & fromID,int & typeID, int * size)
{

	cPacket* p=0;

	//on va checker les packet qui vienne des client qui sont pret a etre recu
	for(cClient *C=Server->Clients;C;C=C->Next)
	{
		p = C->GetReadyPacket();
		if(p)
		{
			fromID = C->NetID;
			typeID = p->TypeID;
			if(size)
			{
				*size = p->Size;
			}
			return p->Data == 0 ? (char *)1 : p->Data;
		}
	}

	return 0;
}

int	bb_serverGetQueueCount(int protocol)
{
	return Server->GetQueueCount(protocol);
}

int	bb_clientSetRate(UINT4 clientID,int nbBytes)
{
	cClient *c = getClientByID(clientID);

	if(!c) return 1;

	c->DataRate	=	(unsigned short)nbBytes;

	return 0;
}


UINT4 bb_clientConnect(const char* HostIP,unsigned short Port)
{

	LastClientID++;

	if(MainClients)
	{
		//get the last client
		cClient *c=0;
		for(c=MainClients;c->Next;c=c->Next)
		{
		}

		c->Next		=	new cClient(HostIP,Port,LastClientID);
		c->Next->Previous = c;
	}
	else
	{
		MainClients = new cClient(HostIP,Port,LastClientID);
	}

	return LastClientID;
}


int bb_clientUpdate(UINT4 clientID,float elapsed,int updateMsg)
{
	cClient *c = getClientByID(clientID);

	if(!c)
	{
		return 1;
	}

	sprintf(c->LastMessage,"");
	sprintf(c->LastError,"");



	if(c->isConnected)
	{
		switch(updateMsg)
		{
			case UPDATE_SEND_RECV:
			{
				//on recois UDP du serveur
				//UDPserver->ReceiveFromSocket( Server ? 20 : 50 );

				//on recois TCP/UDP du serveur
				int r = c->ReceivePacketsFromServer();
				if(r) return r;
			
				//on envoi au serveur TCP/UDP
				if(c->SendPacketsToServer()) return 1;

				return 0;
			}
			case UPDATE_SEND:
			{
				//on envoi au serveur TCP/UDP
				if(c->SendPacketsToServer()) return 1;
				return 0;
			}
			case UPDATE_RECV:
			{
				//on recois UDP du serveur
				//UDPserver->ReceiveFromSocket( Server ? 20 : 40 );

				//on recois TCP/UDP du serveur
				int r = c->ReceivePacketsFromServer();
				if(r) return r;
				return 0;
			}
		}
	}
	else	//on est pas encore connecter
	{
		int r = c->UpdateConnection(elapsed);
		
		if(r==2)	//connection reussi!
		{
			c->isConnected = true;
			return 3;
		}

		if(r==1)	//erreur pendant la connection
		{
			c->isConnected = false;
			return 1;
		}
	}

	return 0;
}

UINT4 bb_clientGetBytesSent(UINT4 clientID)
{
	cClient *c = getClientByID(clientID);

	if(!c) return 0;

	return c->BytesSent;

}

UINT4 bb_clientGetBytesReceived(UINT4 clientID)
{
	cClient *c = getClientByID(clientID);

	if(c) return c->BytesReceived;
	return 0;
}

int	bb_clientSend(UINT4 clientID,char* dataToSend,int dataSize,int typeID,int protocol)
{
	cClient *c = getClientByID(clientID);

	if(!c)	return 1;

	c->CreatePacket(new cPacket(dataToSend,dataSize,(unsigned short)typeID),c->UDPenabled ? (protocol ? true : false) : false);

	return 0;
}


char* bb_clientReceive(UINT4 clientID,int *typeID)
{

	cClient *c = getClientByID(clientID);

	if(!c)
	{
		*typeID = 0;
		return 0;
	}

	cPacket* p=0;

	//on va checker les packet qui vienne des client qui sont pret a etre recu
	p = c->GetReadyPacket();
	if(p)
	{
		*typeID = p->TypeID;
		return p->Data == 0 ? (char *)1 : p->Data;
	}


	return 0;
}

static char serror[256] = "Error : Client is not availble";

char* bb_clientGetLastError(UINT4 clientID)
{

	cClient *c = getClientByID(clientID);

	if(!c)
	{
		return serror;
	}
	return c->GetLastError();
}

char* bb_clientGetLastMessage(UINT4 clientID)
{
	cClient *c = getClientByID(clientID);

	if(!c) return 0;
	return c->GetLastMessage();
}

void bb_enable(unsigned char netFlags)
{
	if(Server)
	{
		Server->NetBitField		|=		netFlags;
	}

	if(P2P)
	{
		P2P->NetBitField		|=		netFlags;
	}
}

void bb_disable(unsigned char netFlags)
{
	if(Server)
	{
		Server->NetBitField		&=		~netFlags;
	}

	if(P2P)
	{
		P2P->NetBitField		&=		~netFlags;
	}
}

void bb_shutdown()
{
	if(Server)		delete Server;
	Server		=	0;
	
	cClient *toKill = 0;
	for(cClient *c = MainClients;c;delete toKill)
	{
		toKill = c;
		c = c->Next;
	}

	MainClients	=	0;

	if(P2P)			delete P2P;
	P2P			=	0;
	
	#ifdef WIN32
		WSACleanup();
	#endif
	#ifdef __MACOSX__
	if(ifdevices)
		freeifaddrs(ifdevices);
	ifdevices = 0;
	#endif
}

int bb_clientDisconnect(UINT4 clientID)
{
	if(!MainClients) return 1;

	cClient *c = getClientByID(clientID);

	if(!c) return 1;

	c->Disconnect();


	if(c->Next)		c->Next->Previous = c->Previous;
	if(c->Previous)		c->Previous->Next = c->Next;


	if(c == MainClients) MainClients = MainClients->Next;

	delete c;
	
	return 0;
}

int bb_serverDisconnectClient(UINT4 ID)
{
	if(!Server) return 1;
	return Server->RemoveClient(Server->getClientByID(ID));
}

int bb_serverShutdown()
{
	if(!Server) return 1;
	delete Server;
	Server = 0;
	
	//if(MainClient) delete MainClient;
	//MainClient = 0;

	return 0;
}

char *bb_getMyIP()
{
#ifdef WIN32
		struct sockaddr_in ipAdress;
		char buffer[80];
		gethostname(buffer,sizeof(buffer));
		hostent *phe = gethostbyname(buffer);
		memcpy(&ipAdress.sin_addr, phe->h_addr_list[0], sizeof(struct in_addr));
		//printf("%s\n",buffer);
		return inet_ntoa(ipAdress.sin_addr);
#elif __MACOSX__
    static char ip[32];
    struct sockaddr_in * sin;
    struct ifaddrs * a;

    memset(ip, 0, sizeof(ip));
    if(ifdevices)
    {
        a = ifdevices;
    	while(a)
    	{
    		if(a->ifa_addr->sa_family == AF_INET)
    		{
    			if(strcmp(a->ifa_name, iface) == 0)
    			{
    				sin = (struct sockaddr_in *) a->ifa_addr;
    				strncpy(ip, inet_ntoa(sin->sin_addr), sizeof(ip));
    				break;
    			}
    		}
    		a = a->ifa_next;
    	}
    }
    return ip;
    
#else
		unsigned char      *u;
		int                sockfd, size  = 1;
		struct ifreq       *ifr;
		struct ifconf      ifc;
		struct sockaddr_in sa;

		if (0 > (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
		{
			perror("Socket Creation");
			return 0;
		}
		
		ifc.ifc_len = IFRSIZE;
		ifc.ifc_req = NULL;
		
		do
		{
			++size;
			/* realloc buffer size until no overflow occurs  */
			if (NULL == (ifc.ifc_req = (ifreq*)realloc(ifc.ifc_req, IFRSIZE)))
			{
				perror("Realloc buffer size");
				return 0;
			}
	
			ifc.ifc_len = IFRSIZE;
			if (ioctl(sockfd, SIOCGIFCONF, &ifc))
			{
				perror("ioctl SIOCFIFCONF");
				return 0;
			}
		} while  (IFRSIZE <= ifc.ifc_len);

		
		ifr = ifc.ifc_req;
		int ii = 0;
		for (;(char *) ifr < (char *) ifc.ifc_req + ifc.ifc_len; ++ifr)
		{
		
			if (ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data)
			{
				continue;  /* duplicate, skip it */
			}
		
			if (ioctl(sockfd, SIOCGIFFLAGS, ifr))
			{
				continue;  /* failed to get flags, skip it */
			}

			ii++;
			
			if(ii==2)
			{
				CloseSocket(sockfd);
				return inet_ntoa(inaddrr(ifr_addr.sa_data));
			}

		}

		return 0;


	#endif


}

void bb_getMyMAC(unsigned char * AddrOut)
{
#ifdef WIN32

			IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
													// for up to 16 NICs
			DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

			DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
				AdapterInfo,                 // [out] buffer to receive data
				&dwBufLen);                  // [in] size of receive data buffer
			if(dwStatus != ERROR_SUCCESS)
			{
				printf("Problem getting MA\n");
			}

			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;	// Contains pointer to
															// current adapter info

			


			do
			{
				memcpy( AddrOut , pAdapterInfo->Address , 8 );
				//sprintf(AddrOut , "%s" , pAdapterInfo->Address );
				//PrintMACaddress(pAdapterInfo->Address); // Print MAC address
				pAdapterInfo = pAdapterInfo->Next;    // Progress through
			}
			while(pAdapterInfo);                    // Terminate if last adapter

		return;

#elif __MACOSX__
    struct sockaddr_dl * sdl;
    struct ifaddrs * a = ifdevices;
	
	while(a)
	{
		if(a->ifa_addr->sa_family == AF_LINK)
		{
			if(strcmp(a->ifa_name, iface) == 0)
			{
				sdl = (struct sockaddr_dl *) a->ifa_addr;
				if(memcmp(sdl->sdl_data, iface, sdl->sdl_nlen) == 0)
				{ 
					memcpy(AddrOut, sdl->sdl_data, sdl->sdl_alen);
					break;
				}
			}
		}
		a = a->ifa_next;
	}
	
#else

		// temporarly we dont do it on linux
	
		//unsigned char      *u;
		//int                sockfd, size  = 1;
		//struct ifreq       *ifr;
		//struct ifconf      ifc;
		//struct sockaddr_in sa;

		//if (0 > (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
		//{
		//	perror("Socket Creation");
		//	return 0;
		//}
		//
		//ifc.ifc_len = IFRSIZE;
		//ifc.ifc_req = NULL;
		//
		//do
		//{
		//	++size;
		//	/* realloc buffer size until no overflow occurs  */
		//	if (NULL == (ifc.ifc_req = (ifreq*)realloc(ifc.ifc_req, IFRSIZE)))
		//	{
		//		perror("Realloc buffer size");
		//		return 0;
		//	}
	
		//	ifc.ifc_len = IFRSIZE;
		//	if (ioctl(sockfd, SIOCGIFADDR, &ifc))
		//	{
		//		perror("ioctl SIOCGIFADDR");
		//		return 0;
		//	}
		//} while  (IFRSIZE <= ifc.ifc_len);

		//
		//ifr = ifc.ifc_req;
		//int ii = 0;
		//for (;(char *) ifr < (char *) ifc.ifc_req + ifc.ifc_len; ++ifr)
		//{
		//
		//	if (ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data)
		//	{
		//		continue;  /* duplicate, skip it */
		//	}
		//
		//	if (ioctl(sockfd, SIOCGIFFLAGS, ifr))
		//	{
		//		continue;  /* failed to get flags, skip it */
		//	}

		//	ii++;
		//	
		//	if(ii==2)
		//	{
		//		CloseSocket(sockfd);
		//		return inet_ntoa(inaddrr(ifr_addr.sa_data));
		//	}

		//}

		//return 0;


	#endif


}


int	bb_peerBindPort(unsigned short listenPort)
{

	if(!P2P)
	{
		P2P = new cPeer2Peer();
	}

	if(listenPort == 11111)
	{
		sprintf(P2P->LastError,"Invalid port, 11111 is baboNet reserved!");
	}

	P2P->BindPort(listenPort);

	/*UDPserver->BindPort(listenPort);
	
	FD_ZERO(&(P2P->master));
	FD_ZERO(&(P2P->BCmaster));
	FD_SET(UDPserver->UDPfd,&(P2P->master));
	FD_SET(UDPserver->BCfd,&(P2P->BCmaster));*/

	return 0;
}

int	bb_peerSend(char *IP,unsigned short port,char *data,int typeID,int dataSize,bool safe)
{

	if(!P2P)
	{
		P2P = new cPeer2Peer();
	}

	struct sockaddr_in ipAdress;

    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_port			=	htons(port);
	memset(&(ipAdress.sin_zero), '\0', 8);

	//on va tester si on doit pogner l'IP du string passer
	if(!(IP[0] >= 48 && IP[0] <= 57))
	{
		//si on est pas numeric, on part une thread qui va pogner le host by name
		
		ipAdress.sin_addr.s_addr = 0;

		bool exist=false;
		int newID = P2P->CreatePeer(ipAdress,&exist);

		if(P2P->AddDNSquery(IP,newID) == -1) return 0; //le domain etait deja parmi les query

		char newData[sizeof(UINT4)];
		UINT4 newid = newID;

		memcpy(newData,&newid,sizeof(UINT4));

        
		if(P2P->SendTo(newID,new cUDPpacket(0,0,(char*)newData,TYPE_DEMAND,sizeof(UINT4),true)))
		{
			//une erreur s'est produite
			return -1;
		}

		//on va envoyer le message au Peer
            if(P2P->SendTo(newID,new cUDPpacket(0,1,data,typeID,(unsigned short)(dataSize),safe)))
		{
			//une erreur s'est produite
			return -1;
		}

		return newID;
	}
	else
	{
		ipAdress.sin_addr.s_addr	=	inet_addr(IP);
	}



	bool exist=false;
    int newID = P2P->CreatePeer(ipAdress,&exist);

	//fp = fopen("_netDebug.txt","a");
	//fprintf(fp,"> CREATION DUN PEER VIA bb_peerSend %s:%i     exist = %i\n",IP,port,(int)exist);
	//fclose(fp);

	
	//on va envoyer un message au Peer dans lequel on pack simplement son new id
	if(!exist)
	{
		char newData[sizeof(UINT4)];
		UINT4 newid = newID;

		memcpy(newData,&newid,sizeof(UINT4));

        
		if(P2P->SendTo(newID,new cUDPpacket(0,0,(char*)newData,TYPE_DEMAND,sizeof(UINT4),true)))
		{
			//une erreur s'est produite
			return -1;
		}

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"	> ENVOIE D'UNE DEMANDE A %s:%i\n",IP,port,(int)exist);
			//fclose(fp);

	}


	//on va envoyer le message au Peer
        if(P2P->SendTo(newID,new cUDPpacket(0,1,data,typeID,(unsigned short)(dataSize),safe)))
	{
		//une erreur s'est produite
		return -1;
	}


	return exist ? 0 : newID;
	
}

int	bb_peerSend(INT4 peerID,char *data,int typeID,int dataSize,bool safe)
{
	if(!P2P)
	{
		P2P = new cPeer2Peer();
	}

	//on va envoyer le message au Peer
        if(P2P->SendTo(peerID,new cUDPpacket(0,1,data,typeID,(unsigned short)(dataSize),safe)))
	{
		//une erreur s'est produite
		return 1;
	}
	return 0;
}


char* bb_peerReceive(INT4 *fromPeerID,int *typeID)
{

	//on va checker si on a recu de quoi du broadcast
    if(P2P->BCPeer)
	{
		cUDPpacket *p = P2P->BCPeer->GetReadyPacket();

        if(p)
		{
			*fromPeerID =	-1;
			*typeID		=	p->TypeID;

			return p->Size ? p->Data : (char*)1;
		}
	}


	//pour chaque peer quia du stock de pret on va retourner du data
	for(cPeer *Peer = P2P->PeerList;Peer;Peer=Peer->Next)
	{
		if(Peer->Need2Publish) continue;

		cUDPpacket *p = Peer->GetReadyPacket();

        if(p)
		{
			*fromPeerID =	Peer->ID;
			*typeID		=	p->TypeID;

			return p->Size ? p->Data : (char*)1;
		}
	}

	return 0;
}

char* bb_peerGetLastError()
{
	return P2P->LastError;
}

char* bb_peerGetLastMessage()
{
	return P2P->LastMessage;
}

int bb_peerUpdate(float elapsed,bool & isNew)
{
	if(!P2P)
	{
		P2P = new cPeer2Peer();
	}
	
	//on va recevoir le stock upd
	if(P2P->ReceiveFromPeers()) return -1;

	//on va envoyer le stock UDP
	if(P2P->SendToPeers(elapsed)) return -1;

	return P2P->CheckUnpublishedPeers(elapsed,isNew);
}

int	bb_peerDelete(UINT4 baboNetID,bool instant)
{
	if(!P2P) return 1; //problem, la p2p n'est pas initialiser

	//on va marquer le peer comme quoi il doit se faire deleter
	if(instant)
	{
		P2P->DeletePeer(baboNetID);
		//fp = fopen("_netDebug.txt","a");
		//fprintf(fp,"		>>> DELETED PEER ID:%i INSTANT\n",baboNetID);
		//fclose(fp);
	}
	else
	{
		cPeer *p = P2P->GetPeerByID(baboNetID);
		if(p)
		{
			p->Need2Delete = true;

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"		>>> MARKING PEER %s:%i ID:%i for DELETION\n",inet_ntoa(p->IpAdress.sin_addr),ntohs(p->IpAdress.sin_port),baboNetID);
			//fclose(fp);
		}


	}
	
	return 0;
}

//int	bb_setBroadcastPort(unsigned short broadcastPort)
//{
//	if(!P2P)
//	{
//		P2P = new cPeer2Peer(&(UDPserver->UDPfd));
//		UDPserver->P2P = P2P;
//	}
//
//	if(!(P2P->BCPeer))
//	{
//		P2P->BCport			=	broadcastPort;
//
//		sockaddr_in newIp;
//		newIp.sin_family			=	AF_INET;
//		newIp.sin_port				=	htons(P2P->BCport);
//		newIp.sin_addr.s_addr		=	INADDR_BROADCAST;
//		memset(newIp.sin_zero,'\0',8);
//
//		P2P->BCPeer = new cPeer(newIp,0);
//	}
//	else
//	{
//		delete P2P->BCPeer;
//		
//		P2P->BCport					=	broadcastPort;
//
//		sockaddr_in newIp;
//		newIp.sin_family			=	AF_INET;
//		newIp.sin_port				=	htons(P2P->BCport);
//		newIp.sin_addr.s_addr		=	INADDR_BROADCAST;
//		memset(newIp.sin_zero,'\0',8);
//
//		P2P->BCPeer = new cPeer(newIp,0);
//	}
//
//	return 0;
//}

int bb_peerGetIPport(UINT4 babonetID,char *IP,unsigned short *Port)
{
	if(!P2P) return 1;

	for(cPeer *P = P2P->PeerList;P;P=P->Next)
	{
		if(P->ID == babonetID)
		{
			sprintf(IP,inet_ntoa(P->IpAdress.sin_addr));
			*Port	=	ntohs(P->IpAdress.sin_port);

			return 0;
		}
	}

	return 1;
}

UINT4 bb_peerGetBytesSent()
{
	if(P2P)
	{
		return P2P->BytesSent;
	}
	return 0;
}

UINT4 bb_peerGetBytesReceived()
{
	if(P2P)
	{
		return P2P->BytesReceived;
	}
	return 0;
}

int	bb_peerShutdown()
{
	if(P2P)
	{
		delete P2P;
		P2P = 0;
	}


	return 0;
}


