#include "cServer.h"


cServer::cServer()
{
	Clients		=	0;
	maxClients	=	0;
	nbClient	=	0;
	UDPfd		=	0;
	UDPenabled = false;

	ipAdress.sin_port = htons(11112);
	ListenPort	=	11112;

	if(PrepareHosting())
	{
		sprintf(LastMessage,"Error spawning server - See bb_getLastError");
		return;
	}

	sprintf(LastMessage,"Spawned server succesfully");

	BytesSent		=	0;
	BytesReceived	=	0;

	
	
}

cServer::cServer(bool udpenabled,int maxclients,unsigned short port)
{

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	Clients		=	0;
	maxClients	=	maxclients;
	nbClient	=	0;

	ListenPort	=	port;
	UDPfd		=	0;

	UDPenabled	=	udpenabled;

	ipAdress.sin_port = htons(port);

	FD_ZERO(&UDPmaster);	// clear the master and temp sets
	FD_ZERO(&master);	// clear the master and temp sets
    	FD_ZERO(&read_fds);	// temp fd_set pour lire
	FD_ZERO(&write_fds);	// temp fd_set pour ecrire

	PendingConnections	=	0;

	//reset le max file descriptor index
	fdmax		=	0;

	//on a pas encore de new connections
	NewConnID	=	0;

	//pas de NetID emis encore
	LastNetID	=	0;

	// on va setter le timeout par defaut a 0
	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;

	Listener = 0;
	
	
	if(PrepareHosting())
	{
		sprintf(LastMessage,"Error spawning server - See bb_serverGetLastError");
		return;
	}

	sprintf(LastMessage,"Spawned server succesfully");

	BytesSent		=	0;
	BytesReceived	=	0;

;

}

int cServer::PrepareHosting()
{

	if( Listener )
	{
		FD_CLR(Listener, &master);
		CloseSocket( Listener );
	}

    	// get the listener TCP
    	if ((Listener = (int)socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{
		printf("Error : Problem Creating Listener Socket TCP\n");
		//sprintf(LastError,"Error : Problem Creating Listener Socket TCP");
        	return 1;
    	}

	// lose the pesky "address already in use" error message
	#ifdef WIN32	
		char yes = '1';
	#else
		int yes = 1;
	#endif


   	 if (setsockopt(Listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		printf("Error : setsockopt() failed TCP SO_REUSEADDR \n");
		//sprintf(LastError,"Error : setsockopt() failed TCP");
        	return 1;
    	}

	if(UDPenabled)
	{
		//on va creer le socket UDP
		if ((UDPfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
		{
			sprintf(LastError,"Error : Problem creating ServerSocket UDP");
			return 1;
		}

		// lose the pesky "address already in use" error message
		if (setsockopt(UDPfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
		{
			sprintf(LastError,"Error : Problem on setsockopt() for the ServerSocket SO_REUSEADDR UDP");
			return 1;
		}

		//on set le timeout pour ce socket
/*		int Tio = 10;	//100 ms.

		if(setsockopt(UDPfd, SOL_SOCKET, SO_SNDTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
		{
			//int Error = WSAGetLastError();
			//sprintf(LastError,"Error : FD : %i setsockopt() SO_SNDTIMEO UDP WSA : %i",UDPfd,Error);
			return 1;
		}

		if(setsockopt(UDPfd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
		{
			//int Error = WSAGetLastError();
			//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO UDP WSA : %i",UDPfd,Error);
			return 1;
		}
*/
	}


	
	//Setup les infos sur notre connection
    	ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr		=	INADDR_ANY;
    	ipAdress.sin_port = htons(ListenPort);
    	memset(&(ipAdress.sin_zero), '\0', 8);

	// bind TCP listener
    	if (bind(Listener, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		printf("Error : Problem on bind()ing the Listener TCP\n");
		//sprintf(LastError,"Error : Problem on bind()ing the Listener TCP");
		return 1;
   	}

	if(UDPenabled)
	{
		//bind udp socket
		if (bind(UDPfd, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
		{
			sprintf(LastError,"Error : Problem on bind()ing the UDP socket");
			return 1;
		}
	}


	// listen TCP
    	if (listen(Listener, 10) == -1)
	{
		printf("Error : Problem on listen() TCP\n");
		//sprintf(LastError,"Error : Problem on listen() TCP");
        	return 1;
    	}

	// add the TCP listener to the master set
	FD_SET(Listener, &master);


	if(UDPenabled)
	{	// add the UDP socket to the master set
		FD_SET(UDPfd, &UDPmaster);
	}


	// keep track of the biggest TCP file descriptor
	if( Listener > fdmax ) fdmax = Listener;

	//par defaut on accept les nouveaux clients
	NetBitField		|=		NET_ACCEPT_CLIENTS;


	return 0;
}

int cServer::SendPacketsToClients()
{

	//on va checker qui quier pret pour ecrire
	
	fd_set writeUDP;
	if(UDPenabled)
	{	
		FD_ZERO(&writeUDP);
		writeUDP	=	UDPmaster;	// UDP set
	}

	write_fds	=	master;		// TCP set


	if (select(fdmax+1, NULL, &write_fds, NULL, &Timeout) == -1)
	{
		printf("Error : Problem select()ing while cServer::SendPacketsToClients() TCP \n");
		sprintf(LastError,"Error : Problem select()ing while cServer::SendPacketsToClients() TCP");
        	return 1; //erreur
   	 }

	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;


	//pour chaque client on va envoyer les packets quil a dans son queue
	for(cClient *C = Clients;C;C=C->Next)
	{
		if(UDPenabled)
		{
			//le client est pret a envoyer en UPD
			if (select(UDPfd+1, NULL, &writeUDP, NULL, &Timeout) == -1)
			{
				//sprintf(LastError,"Error : Problem select()ing while cServer::SendPacketsToClients() UDP WSA : %i",WSAGetLastError());
				return 1; //erreur
			}
			
			Timeout.tv_sec	=	0;
			Timeout.tv_usec =	0;


			if(FD_ISSET(UDPfd,&writeUDP))
			{
				if(C->SendUDP(UDPfd,BytesSent))
				{
					sprintf(LastError,"Number of tries exceeded, bypassing client for now");
					//continue;
				}
			}
		}

		//le client est pret a envoyer en tcp
		if(FD_ISSET(C->FileDescriptor,&write_fds))
		{
			if(C->Send(BytesSent))
			{
				//a problem occured while sending infos to the client, disconnect him
				printf(" problem sending packets to client ID %li ...disconnected \n",C->NetID);
				RemoveClient(C);
                return 0;
			}
		}
	}

	return 0; //tout est beau
}

int	cServer::ReceiveDatagram(cPacket *packet,sockaddr_in fromIP)
{
	//on va recevoir les packets quia a recevoir par clients
	cClient *c = getClientByIP(inet_ntoa(fromIP.sin_addr),ntohs(fromIP.sin_port));
	if(c)
	{
		c->AddReceivedPacket(new cPacket(packet));
	}
	else
	{

		//on pogne le id dla pending connection
		unsigned long pid;
		memcpy(&pid,packet->Data,sizeof(unsigned long));

		//rendu ici c que le udppacket etait pour une pending connection
		for(cIncConnection *IC = PendingConnections;IC;IC=IC->Next)
		{
			if(IC->ID == pid)
			{
				IC->UDPport		=	ntohs(fromIP.sin_port);
				break;
			}
		}
	}
	return 0;
}

long cServer::ReceivePacketsFromClients()
{

	
	
	//TCP part-----------------------------------
	read_fds = master; // copy it
	if (select(fdmax+1, &read_fds, NULL, NULL, &Timeout) == -1)
	{
		printf(" error selecting while cServer::ReceivePacketsFromClients() \n");
		//sprintf(LastError,"Error : Problem select()ing");
        	return -999999999;
    	}

	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;

    // run through the existing connections looking for data to read
    	for(int i = 0; i <= fdmax; i++)
	{
		if (FD_ISSET(i, &read_fds))
		{	
			if (i == Listener) //si c un des listener on va accepter la nouvelle connections
			{
				AcceptConnection(++NewConnID);
			} 
			else
			{
				int nbytes=0;		//garde le nombre de bytes retourner par recv()
				char buf[2048];		//buffer for client data
	
				// handle data from a client
				if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
				{
					// got error or connection closed by client
					if (nbytes == 0)
					{
						// connection closed
						sprintf(LastMessage,"Server : client on socket %i disconnected", i);
					} 
					else
					{
						//printf(" error recv()ing while cServer::ReceivePacketsFromClients() n = %i \n", nbytes);
						//sprintf(LastError,"Error : Problem recv()ing, client on socket %i will be disconnected WSA %i",i,WSAGetLastError());
					}
					getClientByFD((unsigned long)i)->CloseSocket(i); // bye!
					FD_CLR(i, &master); // remove from master set
		
					int disconnectedClient = getClientByFD((unsigned long)i)->NetID * -1;
		
					//on va enlever le client de la liste des clients
					RemoveClient(getClientByFD((unsigned long)i));

					fdmax = GetMaxFD();
		
					return disconnectedClient;
				}
				else
				{
					//on va recevoir les packets quia a recevoir par clients
					if(getClientByFD(i)->ReceiveStream(nbytes,buf))
					{
						//potential hacking detected
						sprintf(LastMessage,"Server : client disconnected due to potential hacking");
		
						getClientByFD(i)->CloseSocket(i); // bye!
						FD_CLR(i, &master); // remove from master set
		
						int disconnectedClient = getClientByFD((unsigned long)i)->NetID * -1;
		
						//on va enlever le client de la liste des clients
						RemoveClient(getClientByFD((unsigned long)i));

						fdmax = GetMaxFD();
		
						return disconnectedClient;
					}	
					BytesReceived += nbytes;
				}
			} 
		}
    	}

	//UDP part--------

	if(UDPenabled)
	{
		int isUDPready = 30;

		read_fds	=	UDPmaster;
		if (select(UDPfd+1, &read_fds, NULL, NULL, &Timeout) == -1)
		{
			sprintf(LastError,"Error : Problem select()ing for UDP");
			return -99999;
		}

		Timeout.tv_sec	=	0;
		Timeout.tv_usec =	0;



		while(isUDPready--)
		{
			if(FD_ISSET(UDPfd,&read_fds))
			{
			
				int nbytes=0;		//garde le nombre de bytes retourner par recv()
				char buf[2048];		//buffer for client data
				sockaddr_in remip;	//remoteip
				#ifdef WIN32
					int len = sizeof(sockaddr);
				#else
					socklen_t len = sizeof(sockaddr);
				#endif

				// handle data from a client
				if ((nbytes = recvfrom(UDPfd, buf, sizeof(buf), 0,(sockaddr*)&remip,&len)) <= 0)
				{
					// got error or connection closed by client
					if (nbytes == 0)
					{
						// connection closed
						sprintf(LastMessage,"Server : client on socket %i disconnected while recvfrom()", UDPfd);
					} 
					else
					{
						//sprintf(LastError,"Error : Problem recv()ingfrom, WSA : %i",WSAGetLastError());
						return -99999;
					}

				}
				else
				{
					BytesReceived += nbytes;

					//on va recevoir les packets quia a recevoir par clients
					cClient *c = getClientByIP(inet_ntoa(remip.sin_addr),ntohs(remip.sin_port));
					if(c)
					{
						c->ReceiveDatagram(nbytes,buf);
					}
					else
					{
					
						//on pogne le id dla pending connection
						unsigned long pid;
						memcpy(&pid,buf + sizeof(stHeader),sizeof(unsigned long));

						//rendu ici c que le udppacket etait pour une pending connection
						for(cIncConnection *IC = PendingConnections;IC;IC=IC->Next)
						{
							if(IC->ID == pid)
							{
								IC->UDPport		=	ntohs(remip.sin_port);
								break;
							}
						}
					}
				}

				read_fds = UDPmaster;
				if (select(UDPfd+1, &read_fds, NULL, NULL, &Timeout) == -1)
				{
					sprintf(LastError,"Error : Problem REselect()ing for UDP");
					return -99999;
				}

				Timeout.tv_sec	=	0;
				Timeout.tv_usec =	0;
			}
			else
			{
				isUDPready = false;
			}
		}
	}

	return 0;
}

long cServer::UpdateConnections(char *newIP)
{

	for(cIncConnection *c = PendingConnections;c;c=c->Next)
	{
		//on va mettre a jour les ocnnections entrentes
		int u = c->Update();

		if(u==-1)
		{
			//un probleme est survenu pendant que la connection se negociait
			
			//fp = fopen("_sdebug.txt","a");
			//fprintf(fp,"Probleme en updatant une connection : LE : %s",LastError);
			//fclose(fp);

			//on enleve la connection du queue
			if(c->Previous) c->Previous->Next	=	c->Next;
			if(c->Next)		c->Next->Previous	=	c->Previous;

			if(c==PendingConnections)
			{
				if(c->Next)	PendingConnections = c->Next;
				else		PendingConnections = 0;
			}

			delete c;

			return -999999999;
		}
		else if(u==-2)
		{
			if(c->Previous) c->Previous->Next	=	c->Next;
			if(c->Next)		c->Next->Previous	=	c->Previous;

			if(c==PendingConnections)
			{
				if(c->Next)	PendingConnections = c->Next;
				else		PendingConnections = 0;
			}

			delete c;
			return 0;
		}
		else if(u > 0)
		{
			long newID = CreateClient(&(c->IP),c->NewFD,c->UDPport, c->Pid);
			sprintf(LastMessage,"New client IP : %s Socket : %i UDPport : %i",inet_ntoa(c->IP.sin_addr),c->NewFD,c->UDPport);
			
			if(newIP)
			{
				sprintf(newIP,"%s",inet_ntoa(c->IP.sin_addr));
			}

			//on enleve la connection du queue
			if(c->Previous) c->Previous->Next	=	c->Next;
			if(c->Next)		c->Next->Previous	=	c->Previous;

			if(c==PendingConnections)
			{
				if(c->Next)	PendingConnections = c->Next;
				else		PendingConnections = 0;
			}

			delete c;
			


			return newID;
		}
	}
	return 0;
}

void cServer::CloseSocket(int socketFD)
{
	#ifdef WIN32
		closesocket(socketFD);
	#else
		close(socketFD);
	#endif
}

int cServer::CreateListener()
{
	//fp = fopen("_sdebug.txt","w");
	//fclose(fp);

	PendingConnections	=	0;

	FD_CLR(Listener,&master);	// clear out the Listener fd from the master set
	CloseSocket(Listener);

	// on va setter le timeout par defaut a 0
	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;

    // get the listener TCP
    if ((Listener = (int)socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{
		sprintf(LastError,"Error : Problem Creating Listener Socket TCP");
        return 1;
    }

	// lose the pesky "address already in use" error message
	char yes = '1';
    if (setsockopt(Listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		sprintf(LastError,"Error : setsockopt() failed TCP");
        return 1;
    }

		
	//Setup les infos sur notre connection
    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr	=	INADDR_ANY;
    ipAdress.sin_port = htons(ListenPort);
    memset(&(ipAdress.sin_zero), '\0', 8);

	// bind TCP listener
    if (bind(Listener, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		sprintf(LastError,"Error : Problem on bind()ing the Listener TCP");
		return 1;
    }

	// listen TCP
    if (listen(Listener, 32) == -1)
	{
		sprintf(LastError,"Error : Problem on listen() TCP");
        return 1;
    }

    // add the TCP listener to the master set
    FD_SET(Listener, &master);


	// keep track of the biggest TCP file descriptor
	fdmax = amax(Listener,fdmax);

	//par defaut on accept les nouveaux clients
	//NetBitField		|=		NET_ACCEPT_CLIENTS;
	
	return 0;
}

void cServer::AcceptConnection(unsigned long id)
{
	cIncConnection *ic=0;

	//ya deja des connections dans le queue ?
	if(PendingConnections)
	{
		//on se rend a fin du queue
		cIncConnection *c = 0;
		for(c = PendingConnections;c->Next;c=c->Next){}

		c->Next = new cIncConnection(id,&NetBitField,UDPenabled,Listener,&nbClient,&maxClients,LastError);
		c->Next->Previous = c;

		ic = c->Next;
	}
	else
	{
		//fp = fopen("_sdebug.txt","a");
		//fprintf(fp,"par une thread ak une connection id de %i\n",(int)id);
		//fclose(fp);

		PendingConnections = new cIncConnection(id,&NetBitField,UDPenabled,Listener,&nbClient,&maxClients,LastError);
		ic = PendingConnections;
	}

	//update the connection
	int u = ic->Update();

	// problem occured while accepting connection
	if( u )
	{
		printf("problem accepting new incoming connection\n");
	
		
		//on enleve la connection du queue
		if(ic->Previous) 	ic->Previous->Next	=	ic->Next;
		if(ic->Next)		ic->Next->Previous	=	ic->Previous;

		if(ic==PendingConnections)
		{
			if(ic->Next)	PendingConnections = ic->Next;
			else		PendingConnections = 0;
		}

		delete ic;
	}
}

long cServer::CreateClient(sockaddr_in *ip, int fileDescriptor,unsigned short udpPort, unsigned long pid)
{
	//connection TCP
	nbClient++;

	cClient *Cli=0;

	//on va a la fin de notre liste
	if(Clients)
	{
		cClient *C=0;
		for(C=Clients;C->Next;C=C->Next){}
		
		C->Next = new cClient(ip,fileDescriptor,++LastNetID);
		C->Next->Previous = C;

		Cli = C->Next;
	}
	else
	{
		Clients = new cClient(ip,fileDescriptor,++LastNetID);
		Cli	=	Clients;
	}

	#ifdef WIN32
		char yes = '1';
	#else
		int yes = 1;
	#endif
	
	if(setsockopt(fileDescriptor, IPPROTO_TCP, TCP_NODELAY,&yes,sizeof(yes)) == -1)
	{
		printf(" Error : Problem on setsockopt() for the new client socket TCP_NODELAY \n");
		//sprintf(LastError,"Error : Problem on setsockopt() for the new client socket TCP_NODELAY");
		return 1;
	}

	//on set le timeout pour ce socket
// 	int To = 10;	//100ms
// 	if(setsockopt(fileDescriptor, SOL_SOCKET, SO_SNDTIMEO,(char*)&To,sizeof(To)) == -1)
// 	{
// 		sprintf(LastError,"Error : Problem on setsockopt() for the new client socket SO_SNDTIMEO TCP");
// 		return 1;
// 	}
// 
// 	if(setsockopt(fileDescriptor, SOL_SOCKET, SO_RCVTIMEO,(char*)&To,sizeof(To)) == -1)
// 	{
// 		sprintf(LastError,"Error : Problem on setsockopt() for the new client socket SO_RCVTIMEO TCP");
// 		return 1;
// 	}

	Cli->UDPport	=	UDPenabled ? udpPort : 0;

	//on va ajouter le nouveau file descriptor au master set
	FD_SET(fileDescriptor,&master);

	if(fileDescriptor > fdmax) fdmax = fileDescriptor;

	Cli->LastPacketID = pid;
	Cli->PendingID = pid + 1;
	
	//tout est beau retourne le babonetID emis
	return LastNetID;
}

int cServer::Send(char *dataToSend,int dataSize,int typeID,long destination,int protocol)
{
	//envoie a toute les clients
	if(destination<1)
	{
		for(cClient *C=Clients;C;C=C->Next)
		{
			C->CreatePacket(new cPacket(dataToSend,dataSize,typeID),UDPenabled ? (protocol ? true : false) : false);
		}
	}
	else
	{
		//envoie a un client specifique
		cClient *client = getClientByID((unsigned long)destination);
		
		//si le netID du client est invalid
		if(!client)
		{
			sprintf(LastError,"Error : Invalid baboNet ID for Destination, packet was NOT sent");
			return 1;
		}

		client->CreatePacket(new cPacket(dataToSend,dataSize,typeID),UDPenabled ? (protocol ? true : false) : false);
	}

	return 0;
}

cClient* cServer::getClientByID(unsigned long netid)
{

	for(cClient *C=Clients;C;C=C->Next)
	{
		if(C->NetID==netid) return C;
	}
	
	return 0;
}

cClient* cServer::getClientByFD(int fd)
{
	for(cClient *C=Clients;C;C=C->Next)
	{
		if(C->FileDescriptor==fd)
		{
			return C;
		}
	}
	
	return 0;
}

//for UDP fd matchmaking only
cClient* cServer::getClientByIP(char *ip,unsigned short port)
{
	for(cClient *C=Clients;C;C=C->Next)
	{
		//le bon ip
		if(!stricmp(inet_ntoa(C->ipAdress.sin_addr),ip))
		{
			//on test maintenant le port
			if(C->UDPport == port)
			{
				return C;
			}
		}
	}
	return 0;
}

int cServer::RemoveClient(cClient *clientToKill)
{
	if(!clientToKill)
	{
		sprintf(LastError,"Could not disconnect client, bad baboNetID");
		return 1;
	}

	for(cClient *C=Clients;C;C=C->Next)
	{
		//on a trouver le client a retirer
		if(C==clientToKill)
		{
			//on va enlever le fd du master set
			FD_CLR(C->FileDescriptor,&master);
			//FD_CLR(C->UDPfd,&master);

			//on va fermer sa connection
			CloseSocket(C->FileDescriptor);
			//closesocket(C->UDPfd);

			nbClient--;

			if(C->Next)	C->Next->Previous = C->Previous;
			if(C->Previous) C->Previous->Next = C->Next;

			if(C == Clients)
			{
				Clients = Clients->Next;
			}
			
			delete C;
			return 0;

// 			if(clientToKill==Clients)	//si on est la tete de file
// 			{
// 				if(Clients->Next)
// 				{
// 					Clients = Clients->Next;
// 					delete Clients->Previous;
// 					Clients->Previous = 0;
// 					return 0;
// 				}
// 				else
// 				{
// 					delete Clients;
// 					Clients = 0;
// 					return 0;
// 				}
// 			}
// 			else
// 			{
// 				//si on est pas la queue
// 				if(C->Next)
// 				{
// 					C->Previous->Next = C->Next;
// 					C->Next->Previous = C->Previous;
// 					delete C;
// 					return 0;
// 				}
// 				else
// 				{
// 					C->Previous->Next = 0;
// 					delete C;
// 					return 0;
// 				}
// 			}
		}
	}
	return 0;
}

int cServer::GetMaxFD()
{
	int max=0;

	//check the listener
	if( Listener > max ) max = Listener;

	for( cClient *C = Clients ; C ; C=C->Next )
	{
		if(C->FileDescriptor > max) max = C->FileDescriptor;
	}

	return max;
}

int cServer::GetQueueCount(int protocol)
{
	int count=0;

	if(protocol == NET_UDP)
	{
		for(cClient *C = Clients;C;C=C->Next)
		{
			for(cPacket *P=C->UDPPacketsToSend;P;P=P->Next)
			{
				count++;
			}
		}

		return count;
	}

	if(protocol == NET_TCP)
	{
		for(cClient *C = Clients;C;C=C->Next)
		{
			for(cPacket *P=C->PacketsToSend;P;P=P->Next)
			{
				count++;
			}
		}

		return count;
	}

	return 0;
}

cServer::~cServer()
{
	
	//on va deleter toute les clients
	cClient *toKill=0;
	for(cClient *c = Clients;c;delete toKill)
	{
		toKill = c;
		c = c->Next;
	}
	

	//on va fermer les listener
	CloseSocket(Listener);
	if(UDPenabled) CloseSocket(UDPfd);

	Listener	=	0;
	UDPfd		=	0;


}
