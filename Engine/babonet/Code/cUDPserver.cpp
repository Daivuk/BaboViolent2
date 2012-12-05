#include "cUDPserver.h"


cUDPserver::cUDPserver()
{
	//on va setter notre socket principal
	UDPfd	=	0;
	BCfd	=	0;

	P2P		=	0;
	Server	=	0;
	Client	=	0;

	

	SetDefaultSocket();

	

	Timeout.tv_sec	=	0;
	Timeout.tv_usec	=	0;

}

int cUDPserver::SetDefaultSocket()
{
	
	//fp = fopen("_netDebug.txt","a");
	//fprintf(fp,"> On va setter le default socket du serveur UDP\n");
	//fclose(fp);


	if(UDPfd)	closesocket(UDPfd);
	if(BCfd)	closesocket(BCfd);

	//get UDP socket
    if ((UDPfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		//sprintf(LastError,"Error : Problem Creating UDPfd Socket UDP");
        return 1;
    }

	if ((BCfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		//sprintf(LastError,"Error : Problem Creating UDPfd Socket UDP");
        return 1;
    }

	char yes = '1';

	// lose the pesky "address already in use" error message
    if (setsockopt(UDPfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		//sprintf(LastError,"Error : setsockopt() failed UDP");
        return 1;
    }
	
	if (setsockopt(BCfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		//sprintf(LastError,"Error : setsockopt() failed UDP");
        return 1;
    }

	int Tio = 100;

	if(setsockopt(UDPfd, SOL_SOCKET, SO_SNDTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_SNDTIMEO UDP WSA : %i",UDPfd,Error);
		return 1;
	}

	if(setsockopt(BCfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)))
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_BROADCAST UDP WSA : %i",UDPfd,Error);
		return 1;
	}

	if(setsockopt(UDPfd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO UDP WSA : %i",UDPfd,Error);
		return 1;
	}

	//on va binder le broadcast socket
	sockaddr_in ipAdress;
    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr	=	INADDR_ANY;
    ipAdress.sin_port			=	htons(BB_BROADCAST_PORT);
    memset(&(ipAdress.sin_zero), '\0', 8);

	if (bind(BCfd, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		int i = WSAGetLastError();//sprintf(LastError,"Error : Problem on bind()ing the UDPfd UDP");
		return 1;
    }

	FD_ZERO(&master);
	FD_SET(UDPfd,&master);
	FD_SET(BCfd,&master);

	fdmax = max(UDPfd,BCfd);


	//fp = fopen("_netDebug.txt","a");
	//fprintf(fp,"	>> Setup default socket du serveur UDP - OK\n");
	//fclose(fp);


	return 0;

	//if(setsockopt(UDPfd, IPPROTO_IP, IP_DONTFRAGMENT,&yes,sizeof(yes)) == -1)
	//{
	//	sprintf(LastError,"Error : Problem on setsockopt() for the ClientSocket IP_DONTFRAGMENT UDP");
	//	return 1;
	//}
}

int cUDPserver::BindPort(unsigned short port)
{
	//pour ce mettre en "fake server"
	//if(UDPfd)	closesocket(UDPfd);

	SetDefaultSocket();


	//Setup les infos sur notre connection
	sockaddr_in ipAdress;
    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr	=	INADDR_ANY;
    ipAdress.sin_port			=	htons(port);
    memset(&(ipAdress.sin_zero), '\0', 8);

	//BCport = port;


	//bind UDPfd
    if (bind(UDPfd, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		int i = WSAGetLastError();//sprintf(LastError,"Error : Problem on bind()ing the UDPfd UDP");
		return 1;
    }

    // add the UDPfd to the master set
    //FD_SET(UDPfd, &master);


	P2P->BindedPort	=	port;

	//par defaut on ne publie pas les peers aux autres
	//NetBitField		&=		~NET_PUBLISH_PEERS;

	return 0;
}

int cUDPserver::ReceiveFromSocket(int nbTimes)
{
	
	while(nbTimes--)
	{
		fd_set fdread = master;

		if (select(fdmax+1, &fdread, NULL, NULL, &Timeout) == -1)
		{
			//sprintf(LastError,"Error : Problem select()ing while ReceivingFromPeers() WSA : %i",WSAGetLastError());
			return 1;
		}

		if(FD_ISSET(UDPfd,&fdread))
		{
			int				nbytes	=	0;
			int				len		=	sizeof(sockaddr);
			char			buf[1000];
			sockaddr_in		fromIP;

			if((nbytes = recvfrom(UDPfd,buf,sizeof(buf),0,(sockaddr*)&fromIP,&len)) < 0)
			{
				int i = WSAGetLastError();
				return 1;
			}
			else
			{
				char ip[16];
				sprintf(ip,inet_ntoa(fromIP.sin_addr));
				unsigned short port = ntohs(fromIP.sin_port);

				//fp = fopen("_netDebug.txt","a");
				//fprintf(fp,"> Receiving packets from IP : %s:%i\n",ip,port);
				//fclose(fp);
					
				
				int nread=0;
				while(nread < nbytes) //tant quia du data a lire
				{

					//on va extraire l'interface id du packet
                	char interfaceID=0;
					memcpy(&interfaceID,buf + nread,sizeof(char));
					nread += sizeof(char);
					
					if(interfaceID == 0 || interfaceID == 1) //packet dedier a la p2p
					{
						//on va extraire le packet
						cUDPpacket *packet = ExtractP2Ppacket(buf,&nread);


						//on va debugger ce qu'on recoit comme packet et de qui
						if(packet->TypeID == 1 && interfaceID == 1)
						{
							//fp = fopen("_netDebug.txt","a");
							//fprintf(fp,"	>> RECEIVING PING\n",ip,port);
							//fclose(fp);
						}
						else
						{
							//fp = fopen("_netDebug.txt","a");
							//fprintf(fp,"	>> Packet P2P : Interfaceid:%i ID:%i TypeID:%i Size:%i\n",interfaceID,packet->ID,packet->TypeID,packet->Size);
							//fclose(fp);
						}



						if(P2P) P2P->ReceiveDatagram(packet,fromIP,interfaceID);
						delete packet;
					}
					else
					{
						cPacket *packet = ExtractCSpacket(buf,&nread);


						//on va debugger ce qu'on recoit comme packet et de qui
						/*fp = fopen("_netDebug.txt","a");
						fprintf(fp,"	>> Packet C/S : Interfaceid : %i,TypeID : %i,Size : %i\n",interfaceID,packet->TypeID,packet->Size);
						fclose(fp);*/


						if(interfaceID == INTERFACE_SERVER)
						{
							if(Server)
							{
								Server->ReceiveDatagram(packet,fromIP);
							}
						}
						else if(interfaceID == INTERFACE_CLIENT)
						{	
							if(Client)
							{
								Client->AddReceivedPacket(new cPacket(packet));
							}
						}

						delete packet;
					}	
				}
			}
		}
		else if(FD_ISSET(BCfd,&fdread))	//on recoit un broadcast
		{
			int				nbytes	=	0;
			int				len		=	sizeof(sockaddr);
			char			buf[1000];
			sockaddr_in		fromIP;

			if((nbytes = recvfrom(BCfd,buf,sizeof(buf),0,(sockaddr*)&fromIP,&len)) < 0)
			{
				int i = WSAGetLastError();
				return 1;
			}
			else
			{
				char ip[16];
				sprintf(ip,inet_ntoa(fromIP.sin_addr));
				unsigned short port = ntohs(fromIP.sin_port);

				//fp = fopen("_netDebug.txt","a");
				//fprintf(fp,"> Receiving packets from IP : %s:%i\n",ip,port);
				//fclose(fp);
					
				
				int nread=0;
				while(nread < nbytes) //tant quia du data a lire
				{

					//on va extraire l'interface id du packet
                	char interfaceID=0;
					memcpy(&interfaceID,buf + nread,sizeof(char));
					nread += sizeof(char);
					
					//on va extraire le packet
					cUDPpacket *packet = ExtractP2Ppacket(buf,&nread);


					if(P2P)
					{
						if(!P2P->BCPeer)
						{
							sockaddr_in newIp;
							newIp.sin_family			=	AF_INET;
							newIp.sin_port				=	htons(BB_BROADCAST_PORT);
							newIp.sin_addr.s_addr		=	INADDR_BROADCAST;
							memset(newIp.sin_zero,'\0',8);

							P2P->BCPeer = new cPeer(newIp,0);
						}

						P2P->BCPeer->AddReceivedPacket(new cUDPpacket(packet));
					}
					delete packet;
				}
			}
		}
		else
		{
			nbTimes = 0;
		}
	}
	return 0;
}

cUDPpacket* cUDPserver::ExtractP2Ppacket(char* buf,int *nread)
{
	cUDPpacket *packet = new cUDPpacket();
	
	//on va extraire le ID du packet
	memcpy(&(packet->ID),buf + (*nread),sizeof(UINT4));
	(*nread) += sizeof(UINT4);

	//on va extraire le type de packet
	memcpy(&(packet->TypeID),buf + (*nread),sizeof(unsigned short));
	(*nread) += sizeof(unsigned short);

	//on extrait le size du packet
	memcpy(&(packet->Size),buf + (*nread),sizeof(unsigned short));
	(*nread) += sizeof(unsigned short);

	//on creer un espace memoire suffisant
	if(packet->Size)
	{
		packet->Data	=	new char[packet->Size];

		//on copy le data dans le nouvel espace memoire
		memcpy(packet->Data,buf + (*nread),packet->Size);
		(*nread) += packet->Size;
	}
	else
	{
		packet->Data	= 0;
	}


	return packet;
}

cPacket* cUDPserver::ExtractCSpacket(char* buf,int *nread)
{
	cPacket *packet = new cPacket();

	//on va extraire le header du packet
	stHeader header;

	memcpy(&header,buf + (*nread),sizeof(stHeader));
	(*nread) += sizeof(stHeader);

	packet->Size	=	header.Size;
	packet->TypeID	=	header.typeID;

	if(packet->Size)
	{
		packet->Data	=	new char[packet->Size];
		memcpy(packet->Data,buf + (*nread),packet->Size);
		(*nread) += packet->Size;
	}
	else
	{
		packet->Data	=	0;
	}

	return packet;
}
