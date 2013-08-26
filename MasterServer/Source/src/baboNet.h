/////////////////////////////////////////////////
// Filename : baboNet.h
// Creation date : 2 juin 2006
// Author : Marc Durocher
// Description : baboNet.dll interface (network DLL)
////////////////////////////////////////////////

#ifndef BB_BBNET_H
#define BB_BBNET_H

//les includes
#ifdef WIN32
	#include "io.h"
	#include "stdio.h"
#else
	#include "stdio.h"
#endif


//les defines

	//Return values
	#define		CLIENT_CONNECT_FAILURE		1
	#define		SERVER_CLOSED_CONNECTION	2
	#define		CLIENT_CONNECT_SUCCESS		3

	#define		BBNET_ERROR			-999999999

	//Les protocol
	#define		NET_TCP				0
	#define		NET_UDP				1

	//Broadcast Peer
	#define		NET_BROADCAST		-1

	//pour les update
	#define		UPDATE_SEND_RECV	0
	#define		UPDATE_SEND			1
	#define		UPDATE_RECV			2

	//Les Settings changeable avec bb_enable
	#define		NET_ACCEPT_CLIENTS	0x01	//est-ce qu'on accept des nouveaux clients ? (Client/Server only)
	#define		NET_PUBLISH_PEERS	0x02	//disabled par defaut, si enabled, le "fake" server va communiquer les infos sur le ip/port de chaque peers quia entrer en communication avec

/*
#ifdef WIN32
	#ifndef DLL_EXPORTS
		#define BBNET_DLL_API __declspec(dllexport)
	#else
		#define BBNET_DLL_API __declspec(dllimport)
	#endif
#else
	//#define BBNET_DLL_API extern "C"
	#define BBNET_DLL_API
#endif
*/

#ifdef WIN32

	#define amax max	//by-pass an STL bug on linux
	
	#ifndef DLL_EXPORTS
		#define BBNET_DLL_API(p) __declspec(dllexport) p
	#else
		#define BBNET_DLL_API(p) __declspec(dllimport) p
	#endif
#else
	#define BBNET_DLL_API(p) p
#endif



//fonction prototypes---------------------------------------------------------------------------------------------
	
	
	//Startup / Shutdown
	BBNET_DLL_API(int)			bb_init();																		//init la bbnet et winsock
	BBNET_DLL_API(void)			bb_shutdown();																	//ferme bbnet et winsock
	
	
	//Peer-2-Peer functions	
	BBNET_DLL_API(int)			bb_peerUpdate(float elapsed,bool & isNew);															//donne de lattention a la partie p2p de la net... important pour que les message senvoie, la fonction retourne le id dun nouveau peer avec qui on peut communiquer ou un peer qui vient de se delter, voir isNew, 0 si tout va bien et -1 on error
	BBNET_DLL_API(int)			bb_peerBindPort(unsigned short listenPort);															//cette fonction permet decouter les packets UDP sur un port, premordial pour "faker" un serveur, 0 on success, 1 on error
	BBNET_DLL_API(int)			bb_peerSend(long peerID,char *data,int typeID,int dataSize,bool safe=false);						//peerID est lidentificateur du peer, un peerID de -1 signifie broadcast packet,safe/unsafe si on tien a ce que le message se rende.
	BBNET_DLL_API(int)			bb_peerSend(char *IPdomain,unsigned short port,char *data,int typeID,int dataSize,bool safe=false);	//IP ou DomainName/port du peer avec qui on veux communiquer,safe/unsafe si on tien a ce que le message se rende, timeout avant de discarter le message safe qui c pas rendu, si c le cas. Ca retourne un index pour communiquer avec ce peer par la suite, ou -1 on error
	BBNET_DLL_API(char*)		bb_peerReceive(long *fromPeerID,int *typeID);														//va recevoir un packet de tel tel peer
	BBNET_DLL_API(int)			bb_peerGetIPport(unsigned long babonetID,char *IP,unsigned short *Port);							//permet d'obtenir le IP et le Port d'un Peer	
	BBNET_DLL_API(int)			bb_peerDelete(unsigned long baboNetID,bool instant=false);											//permet de retirer un peer dla liste des peers, si instant, le peer n'attendra pas 
	BBNET_DLL_API(int)			bb_peerShutdown();																					//clear la memoire utiliser par la p2p
	BBNET_DLL_API(char*)		bb_peerGetLastError();																				//permet d'obtenir une version textuelle de la derniere erreur
	BBNET_DLL_API(char*)		bb_peerGetLastMessage();																			//permet d'obtenir une version textuelle du dernier message
	BBNET_DLL_API(unsigned long)bb_peerGetBytesSent();																				//retourne le nombre de bytes envoyer par notre peer
	BBNET_DLL_API(unsigned long)bb_peerGetBytesReceived();																			//retourne le nombre de bytes envoyer par notre peer


	//Server functions
	BBNET_DLL_API(long)			bb_serverUpdate(float elapsed,int updateMsg=0,char* NewIP=0);									//donne de l'attention au serveur, retourne -baboNetID dun client disconnecter, retourne baboNetID si un client s'est connecter, retourne 0 on succes, retourn -999999999 on error, le updateMSG : 0=Send/Receive 1=Send 2=Receive
	BBNET_DLL_API(int)			bb_serverCreate(bool UDPenabled=false,int maxClients=64,unsigned short listenPort=11112);		//permet de spawner un serveur, premier param pour si on est UDP/TCP ou TCP only
	BBNET_DLL_API(int)			bb_serverSend(char* dataToSend,int dataSize,int typeID,long destination=0,int protocol=0);		//permet d'envoyer des donnees du serveur vers les clients, destination 0 = tout le monde, n = client n, protocol 0 = tcp(safe) 1=Udp(unsafe)
	BBNET_DLL_API(char*)		bb_serverReceive(unsigned long & babonetID,int & typeID);										//permet de recevoir le data disponible pour le server
	BBNET_DLL_API(char*)		bb_serverGetLastError();																		//retourne une version textuel de la derniere erreur, cote serveur
	BBNET_DLL_API(char*)		bb_serverGetLastMessage();																		//retourne une version textuel de ce qui c passer au dernier cycle, cote serveur
	BBNET_DLL_API(int)			bb_serverDisconnectClient(unsigned long baboNetID);												//permet de deconnecter un client du serveur
	BBNET_DLL_API(int)			bb_serverShutdown();																			//permet de fermer le serveur
	BBNET_DLL_API(int)			bb_serverGetQueueCount(int protocol);															//permet de savoir combien de packet sont encore dans le queue
	BBNET_DLL_API(unsigned long)bb_serverGetBytesSent();																		//retourne le nombre de bytes envoyer par notre serveur
	BBNET_DLL_API(unsigned long)bb_serverGetBytesReceived();																	//retourne le nombre de bytes envoyer par notre serveur
	BBNET_DLL_API(int)			bb_serverSetClientRate(int nbBytes,unsigned long baboNetID);									//set the max packet size for a send to client

	//Client functions
	BBNET_DLL_API(int)			bb_clientUpdate(unsigned long clientID,float elapsed,int updateMsg=0);							//donne de l'attention au client,retourne 3 lorsque la connection est etablie, retourne 2 si le serveur a disconnecter,retourne 1 si un probleme, 0 on succes, voir le serverUpdate pour le updateMsg : updateMsg est ignorer pendant la connection
	BBNET_DLL_API(unsigned long)bb_clientConnect(const char* HostIP,unsigned short Port);										//permet de connecter notre client a un serveur
	BBNET_DLL_API(int)			bb_clientSend(unsigned long clientID,char* dataToSend,int dataSize,int typeID,int protocol=0);	//permet d'envoyer des donnees du client vers le serveur, protocol 0 = TCP(safe) 1 = UDP(unsafe)
	BBNET_DLL_API(char*)		bb_clientReceive(unsigned long clientID,int *typeID);
	BBNET_DLL_API(char*)		bb_clientGetLastError(unsigned long clientID);													//retourne une version textuel de la derniere erreur, cote client
	BBNET_DLL_API(char*)		bb_clientGetLastMessage(unsigned long clientID);												//retourne une version textuel de ce qui c passer au dernier cycle, cote client
	BBNET_DLL_API(int)			bb_clientDisconnect(unsigned long clientID);													//permet de disconnecter notre client du serveur
	BBNET_DLL_API(unsigned long)bb_clientGetBytesSent(unsigned long clientID);													//retourne le nombre de bytes envoyer par notre client
	BBNET_DLL_API(unsigned long)bb_clientGetBytesReceived(unsigned long clientID);												//retourne le nombre de bytes envoyer par notre client
	BBNET_DLL_API(int)			bb_clientSetRate(unsigned long clientID,int nbBytes);											//set the max packet sizes for send to server

	//Misc
	BBNET_DLL_API(char*)		bb_getVersion();																//retourne la version de la babonet
	BBNET_DLL_API(char*)		bb_getMyIP();																	//permet d'obtenir l'adresse ip de l'ordi courant
	BBNET_DLL_API(void)			bb_getMyMAC( unsigned char * AddrOut );													//get current mac adress of the first adapter
	BBNET_DLL_API(void)			bb_enable(unsigned char netFlags);												//permet d'enabler des settings de la baboNet ( voir #defines )
	BBNET_DLL_API(void)			bb_disable(unsigned char netFlags);


#endif
