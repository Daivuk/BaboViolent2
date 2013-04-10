#ifndef _CONNECTION__
#define	_CONNECTION__

#include <fcntl.h>
#include "CThread.h"

#ifdef WIN32
	#define amax max
	#include "Winsock2.h"
	#include "ws2tcpip.h"
#else
	#include "LinuxHeader.h"
#endif

#include "cUDPpacket.h"
#include "cPacket.h"

class cConnection : public CThread
{
private:

	#ifdef USING_LOG
		FILE * LogFile;
	#endif

	float			ConTimeout;			//timeout for the connection, if it reached 5, we kill the thread connect()ing
	int			ConnectResult;			// 0 = success  1 = echec
	bool			isConnecting;		// currently connecting of not

public:


	float			LastCheck;			//savoir le nombre de temps qui s'est ecouler

	char			RecvBuf[64];		//buffer qui va recevoir le incoming data
	int			ToRecv;					//nombre de byte a recevoir

	int			State;					//savoir on est rendu ou avec notre connection

	unsigned long		connID;			//garde le ID de la remote connection
	unsigned long		*Lpid;			//LastPAcketID
	//les info du client qui essaye de se connecter

	timeval		*Timeout;			//pointeur sur une struct qui tien le timeout
	sockaddr_in *RemoteIP;			//pointeur sur les infos du host auquel on se connect
	int			*UDPfd;				//pointeur sur le filedescriptor UDP du client
	int			*FileDescriptor;	//pointeur sur le filedescriptor du client
	bool		isServer;			//est-ce que notre client est serveur ?
	char		*LastError;			//derniere erreur cote client
	fd_set		*Master;
	fd_set		*Write;
	fd_set		*Read;
	int			*FDmax;
	bool		*UDPenabled;


	cConnection(bool *UDPenabled,timeval *Timeout,sockaddr_in *remoteIP,int *filedescriptor,int *udpFD,char *lastError,fd_set *master,fd_set *read,fd_set *write,int *fdmax,unsigned long *LastPacketID);
	

	void		execute(void *pArg);			// the connection process
	int			Update(float elapsed);			// donne de lattention anotre connection process
	//int 			UpdateConnecting(float elapsed);	//update the connection process
	int			PrepareSocket();			//va setter les param de notre socket TCP
	int			PrepareUDP();				//socket UDP
	void			CloseSocket(int fd);			//closes a socket (cross platform)
};

#endif
