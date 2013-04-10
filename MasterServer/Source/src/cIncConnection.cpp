#include "cIncConnection.h"



cIncConnection::cIncConnection(unsigned long id,unsigned char *netBitField,bool udpenabled,int tcpListener,int *nbClient,int *maxClient, char *lastError)
{
	ID				=	id;

	Next			=	0;
	Previous		=	0;

	UDPport			=	0;
	NewFD			=	0;

	NetBitField		=	netBitField;
	TCPlistener		=	tcpListener;

	NbClients		=	nbClient;
	MaxClients		=	maxClient;

	LastError		=	lastError;

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	isConnected		=	false;

	Timeout.tv_sec	=	0;
	Timeout.tv_usec	=	0;

	State			=	0;

	UDPenabled		=	udpenabled;

	Pid				=	0;

	FD_ZERO(&master);
	FD_ZERO(&fdwrite);
}


int cIncConnection::Update()
{


	//si on accept les nouveaux clients
	if(*NetBitField & NET_ACCEPT_CLIENTS)
	{

		if(!NewFD) //si la connection n'a pas encore ete accepter
		{
				// handle new connections
			sockaddr_in remoteaddr;		// new client address
			
			#ifdef WIN32
				int addrlen = sizeof(sockaddr);
			#else
				socklen_t addrlen = sizeof(sockaddr);
			#endif


			//on accept la connection TCP
			if ((NewFD = (int)accept(TCPlistener, (sockaddr *)&remoteaddr,&addrlen)) == -1)
			{
				printf("Error : Problem accept()ing new TCP connection\n");
				//sprintf(LastError,"Error : Problem accept()ing new TCP connection");
				isConnected = false;
				return -1;
			}

			IP = remoteaddr;		//la connection a reussi on garde son IP ici
			FD_SET(NewFD,&master);	//on lajoute au set

			return 0;

		}
		else
		{
			if(*NbClients==*MaxClients)
			{
				sprintf(LastMessage,"Denied new connection, maximum number of clients reached");

				FD_CLR(NewFD,&master);
				CloseSocket(NewFD);

				isConnected = false;
				return 0;
			}

			//connection a ete etablie on va voir si on est pret a negocier
			fdwrite =	master;

			if (select(NewFD+1, NULL, &fdwrite, NULL, &Timeout) == -1)
			{
				printf(LastError,"Error : Problem select()ing while cIncConnection::Update()");
				return -1; //erreur
			}
			Timeout.tv_sec	=	0;
			Timeout.tv_usec =	0;

			switch(State)
			{
				case 0: //on doit envoyer le id dla new connection au client
				{
					if(FD_ISSET(NewFD,&fdwrite))
					{
						int nbytes	=	0;
						int	sent	=	0;
						char udp	=	UDPenabled ? 1 : 0;

						//generate random junk data
						
						char junk[33];
						sprintf( junk , "%i%i%i" , rand() % 1000 + 1 , rand() % 100 + 1000, rand() % 1000 + 10000 );
			
						CMD5 md5(junk);
						sprintf(junk , "%s",md5.getMD5Digest());
						md5.setPlainText(0);

						char buf[37]; //conn data(5) + md5 junk(28) + hidden packetid at end(4)

						memcpy(buf,&ID,sizeof(unsigned long));
						memcpy(buf + sizeof(unsigned long),&udp,sizeof(char));
						memcpy(buf + sizeof(unsigned long) + sizeof(char),&junk,sizeof(char) * 32);

						//random starting packetID
						srand(time(0));
						Pid = rand() % 32000;
						memcpy(buf + sizeof(unsigned long) + sizeof(char) + sizeof(char) * 28,&Pid,sizeof(unsigned long));

						memcpy( &Pid , &(buf[33]) , sizeof(unsigned long) );

						while(sent < 37)
						{
							nbytes = send(NewFD,buf + sent,37-sent,0);
							if(nbytes <= 0)
							{
								//probleme a envoyer le id
								printf("Problem send()ing connID to client\n");
								//sprintf(LastError,"Problem send()ing connID to client");

								//on elimine le client, il se reconnectera simplement
								FD_CLR(NewFD,&master);
								CloseSocket(NewFD);
								isConnected		=	false;
								return -2;
							}

							sent += nbytes;
							
						}

						if(UDPenabled)
						{
							State = 1; //on se met en attente de reception du port
							return 0;
						}
						else
						{
							isConnected = true;
							return 1;
						}
						return 0;
					}
					return 0;
				}

				case 1: //on attend que le port udp soit rentrer
				{
					if(UDPport != 0)
					{
						//on a notre port!
						State = 2;
					}
					return 0;
				}

				case 2: //on envoie la reponse comme quoi on a recu son port UDP
				{
					if(FD_ISSET(NewFD,&fdwrite))
					{
						int nbytes	=	0;
						int	sent	=	0;

						while(sent < 4)
						{
							nbytes = send(NewFD,((char*)&ID) + sent,sizeof(ID)-sent,0);
							if(nbytes <= 0)
							{
								//probleme a envoyer le id
								sprintf(LastError,"Problem send()ing connID to client");
								return -1;
							}

							sent += nbytes;
							
						}

						//fp = fopen("_sdebug.txt","a");
						//fprintf(fp,"On a fini denvoyer la reponse au client, on le create",LastError);
						//fclose(fp);

						//on a terminer le connection process
						isConnected = true;
						return 1;
					}
					return 0;
				}
			}
		}
	}
	else
	{
		sockaddr_in remoteaddr;				// new client address
		#ifdef WIN32
			int addrlen = sizeof(sockaddr);
		#else
			socklen_t addrlen = sizeof(sockaddr);
		#endif


		sprintf(LastMessage,"Denied new connection, NET_ACCEPT_CLIENTS is off");
		if ((NewFD = (int)accept(TCPlistener, (sockaddr *)&remoteaddr,&addrlen)) == -1)
		{
			sprintf(LastError,"Error : Problem accept()ing for denying new connection");
			isConnected = false;
		}
		CloseSocket(NewFD);
		isConnected = false;
		return 0;
	}
	return 0;
}

void cIncConnection::CloseSocket(int socketFD)
{

#ifdef WIN32
	closesocket(socketFD);
#else
	close(socketFD);
#endif

}
