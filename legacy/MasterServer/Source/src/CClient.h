#ifndef CLIENT_H
#define CLIENT_H

#ifdef WIN32
	#include "winsock2.h"
#else
	#include "LinuxHeader.h"
#endif

#include "md5class.h"
#include "cPacket.h"
#include "cConnection.h"

#define		BBNET_ERROR			-999999999

#define RND_KEY		"RND1"
#define KEY_SIZE	9				//4 byte for version, 4 bytes packetID, 1 byte for numbers of packets


class cClient
{
private:

	#ifdef USING_LOG
		FILE * LogFile;
	#endif

	
		//une struct qu'on va se servir pour envoyer du data
	struct stPacket			
	{
		unsigned short	Size;		//le Size du packet TOTAL
		unsigned short	TypeID;		//le type du packet
        char			*data;		//un array dynamique qui tien le data a envoyer
	};
	

	//TCP streaming
	stPacket		lastPacket;			// garde le data du packet qui se fait recevoir
	unsigned short	bytesRemaining;		// garde le nombre de byte qui reste a recevoir du lastPacket
	char			lastHeader[4];		// va garder le dernier header ( pour les receptions partielle )
	char			lastKey[9];			// keep the last key we are bulding
	bool			WaitingForHeader;	// est-ce que c un header qu'on attend
	bool			PartialHeader;		// savoir si on est entrain de completer un header partiel
	bool			WaitingForKey;		// are we waiting for the rndlabs key
	bool			PartialKey;			// are we rebuilding a partial key
	char			NbPacket;			// how many packets is there before next key

	//UDP
	//stPacket		UDPlastPacket;			// garde le data du packet qui se fait recevoir
	//unsigned short	UDPbytesRemaining;		// garde le nombre de byte qui reste a recevoir du lastPacket
	//char			UDPlastHeader[4];		// va garder le dernier header ( pour les receptions partielle )
	//bool			UDPWaitingForHeader;	// est-ce que c un header qu'on attend
	//bool			UDPPartialHeader;		// savoir si on est entrain de completer un header partiel
	



	char			Key[5];				// holds the key
	cPacket			*PacketToKill;		// garde le packet a enlever du queue des ReceivedPackets

	bool			isServer;			// garde si oui ou non on est en mode serveur


	bool			GetPendingID(char *pid);				// returns true if hash are different
	void			GetLastPacketID(char *pid);	// returns hashed packetID

public:

	unsigned long	PendingID;			// we keep the ID of the incoming packet
	unsigned long	LastPacketID;		// last emited packet ID

	unsigned short	DataRate;			//at what rate a client is being sent informations (default 1400)
	bool			UDPenabled;			//savoir si on a une gestion de UDP a faire

	//FILE *fp;							//pour debugger
	
	cConnection		*Connection;		//pointeur sur notre objet qui fait la connection
	bool			isConnected;		//va etre a true juste si une connexion a pu setablir

	unsigned long	BytesSent;
	unsigned long	BytesReceived;

	fd_set			master;				// for when we are a client
	fd_set			masterS;			// for when we are server and talking with this client
	fd_set			read_fds;
	fd_set			write_fds;
	int				fdmax;
	timeval			Timeout;

	cPacket			*PacketsToSend;		// liste des packet a envoyer en TCP
	cPacket			*UDPPacketsToSend;	// liste des packet a envoyer en UDP

	cPacket			*ReceivedPackets;	// garde les packet pret a etre pris par bb_serverReceive ou bb_clientReveive
	
	char			LastError[256];		// garde la derniere erreur cote serveur
	char			LastMessage[256];	// garde le dernier message cote serveur
	
	int				UDPfd;			// garde le file descriptor pour communiquer en udp
	int				FileDescriptor;	// garde le file descriptor creer par le socket pour communiquer avec ce client
	int				FileDescriptorS;// garde le file descriptor creer par le socket pour communiquer avec ce client, persu du serveur
	unsigned long	NetID;			// un id unique du client
	sockaddr_in		ipAdress;		// l'adresse IP du client
	sockaddr_in		RemoteIP;		// adresse IP du host auquel on veut se connecter
	unsigned short	UDPport;		// the port used for UDP packets

	cClient			*Next;			// prochain client dans liste
	cClient			*Previous;		// client precedent

	cClient();
	cClient(sockaddr_in *ip, int fileDescriptor,unsigned long netID);			//pour un serveur qui recois un client
	cClient(const char *HostIP, unsigned short port,unsigned long clientID);	//pour un client qui veut se connecter au serveur
	~cClient();


	void			CloseSocket(int socketFD);						//closes a socket
	int				ReceivePacketsFromServer();						//permet de lire ce qui sen vient du serveur, 0 tout est beau, 1 erreur, 2 = connection perdu
	int				SendPacketsToServer();							//permet denvoyer les packets au serveur
	
	int				UpdateConnection(float elapsed);				//donne de lattention au connection process
	
	long			IsReadyToSend();								//to know if the server can send data to this client, returns BBNET_ERROR in case of an error, 1 in case of a 'yes', and 0 in case of a 'No'
	long			IsReadyToReceive();								//to know if the server can receive data from this client, returns BBNET_ERROR in case of an error, 1 in case of a 'yes', and 0 in case of a 'No'


	void			ReceiveDatagram(int nbytes,char *buf);			//permet de recevoir un udp packet
	int				ReceiveStream(int nbytes,char *buf);			//va permettre de recevoir les packets de facon streamer TCP, 0 tout est beau, 1 = potential hacker
	void			AddReceivedPacket(cPacket *newPacket);			//ajoute un packet pret a etre lu a la liste des packet recu
	void			CreatePacket(cPacket *newPacket,bool isUDP=false);	//permet de creer un packet a envoyer, par defaut c TCP, sinon c UDP
	void			Disconnect();									//disconnect le client
	int				Send(unsigned long &nbByte);					//va envoyer les packets dans la liste PacketsToSend, retourn 1 si un packet n'a pas pu etre envoyer en 5 essaies, TCP
	int				SendUDP(int UDPFD,unsigned long &nbByte);		//envoie les packet UDP dans la liste de UDPPacketsToSend

	cPacket*		GetReadyPacket();								//pogne un packet qui est pret a etre envoyer aux clients, retourne 0 si yen a pu

	char*			GetLastError()		{	return LastError;		}
	char*			GetLastMessage()	{	return LastMessage;		}


};


#endif
