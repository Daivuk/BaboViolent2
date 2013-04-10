#include "cPeer2Peer.h"


cPeer2Peer::cPeer2Peer()
{
	PeerList		=	0;
	BCPeer			=	0;
	BCfd			=	0;
	UDPfd			=	0;
	fdmax			=	0;

	LastPeerID		=	0;
	BindedPort		=	0;
	NetBitField		=	0;

	Timeout.tv_sec	=	0;
	Timeout.tv_usec	=	0;

	FD_ZERO(&master);
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	//FD_SET(UDPfd,&master);
	//FD_SET(BCfd,&master);

	BytesSent		=	0;
	BytesReceived	=	0;

	DNSqueries		=	0;

	//fonction qui va initialiser les 2 socketa
	PrepareSockets();

}

void cPeer2Peer::CloseSocket(int socketFD)
{

#ifdef WIN32
	closesocket(socketFD);
#else
	close(socketFD);
#endif

}

int cPeer2Peer::PrepareSockets()
{

	if(UDPfd)	CloseSocket(UDPfd);
	if(BCfd)	CloseSocket(BCfd);

	//get UDP socket
    if ((UDPfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		sprintf(LastError,"Error : Problem Creating UDPfd Socket UDP");
        return 1;
    }

	if ((BCfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		sprintf(LastError,"Error : Problem Creating BCfd Socket UDP");
        return 1;
    }

	#ifdef WIN32
		char yes = '1';
	#else
		int yes = 1;
	#endif

	// lose the pesky "address already in use" error message
    if (setsockopt(UDPfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		sprintf(LastError,"Error : setsockopt() failed UDPfd");
        return 1;
    }
	
	if (setsockopt(BCfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		sprintf(LastError,"Error : setsockopt() failed BCfd");
        return 1;
    }

/*	int Tio = 10;

	if(setsockopt(UDPfd, SOL_SOCKET, SO_SNDTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_SNDTIMEO UDP WSA : %i",UDPfd,Error);
		return 1;
	} */

	if(setsockopt(BCfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)))
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_BROADCAST UDP WSA : %i",UDPfd,Error);
		return 1;
	}

/*	if(setsockopt(UDPfd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
	{
		//int Error = WSAGetLastError();
		//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO UDP WSA : %i",UDPfd,Error);
		return 1;
	}
*/

	//on va binder le broadcast socket
	sockaddr_in ipAdress;
    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr	=	INADDR_ANY;
    ipAdress.sin_port			=	htons(BB_BROADCAST_PORT);
    memset(&(ipAdress.sin_zero), '\0', 8);

	if (bind(BCfd, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		//int i = WSAGetLastError();//sprintf(LastError,"Error : Problem on bind()ing the UDPfd UDP");
		return 1;
    }

	FD_ZERO(&master);
	FD_SET(UDPfd,&master);
	FD_SET(BCfd,&master);

	fdmax = amax(UDPfd,BCfd);

	return 0; //toute est beau
}

int cPeer2Peer::BindPort(unsigned short port)
{

	//pour ce mettre en "fake server"


	if(port == 11111)
	{
		sprintf(LastError,"Invalid Port, 11111 is baboNet reserved!");
		return 1;
	}
	
	//on reset les socket de base (broadcast + communication)	
	if(PrepareSockets())
	{
		sprintf(LastError,"Error while cP2P::PrepareSockets()");
		return 1;
	}

	//Setup les infos sur notre connection
	sockaddr_in ipAdress;
    ipAdress.sin_family			=	AF_INET;
	ipAdress.sin_addr.s_addr	=	INADDR_ANY;
    ipAdress.sin_port			=	htons(port);
    memset(&(ipAdress.sin_zero), '\0', 8);

	//bind UDPfd
    if (bind(UDPfd, (sockaddr *)&ipAdress, sizeof(sockaddr)) == -1)
	{
		//int i = WSAGetLastError();//sprintf(LastError,"Error : Problem on bind()ing the UDPfd UDP");
		return 1;
	}

	BindedPort	=	port;

	//par defaut on ne publie pas les peers aux autres
	NetBitField		&=		~NET_PUBLISH_PEERS;

	return 0;
}



int cPeer2Peer::CreatePeer(struct sockaddr_in ip,bool *exist)
{
	//on va checker si le peer n'est pas deja dans notre liste
	for(cPeer *P = PeerList;P;P=P->Next)
	{
		//if(!stricmp(inet_ntoa(P->IpAdress.sin_addr),inet_ntoa(ip.sin_addr)))
		if(P->IpAdress.sin_addr.s_addr == ip.sin_addr.s_addr)
		{
			if(ntohs(P->IpAdress.sin_port) == ntohs(ip.sin_port))
			{
				*exist = true;
				return P->ID; //le peer existe deja
			}
		}
	}

	LastPeerID++; //on va pogner le new ID du peer

	if(PeerList)
	{
		//on se rend a la fin des peers
		cPeer *Pe = 0;
		for(Pe = PeerList;Pe->Next;Pe=Pe->Next)
		{
		}

		Pe->Next = new cPeer(ip,LastPeerID);
		Pe->Next->Previous = Pe;

	}
	else
	{ 
		PeerList	=	new cPeer(ip,LastPeerID);
	}

	return LastPeerID;

}

char* cPeer2Peer::GetBroadcastAdress()
{
		
	struct sockaddr_in ipAdress;
	struct sockaddr_in NetMask;
	struct sockaddr_in Subnet;
	struct sockaddr_in Broadcast;


	char buffer[80];
	gethostname(buffer,sizeof(buffer));
	hostent *phe = gethostbyname(buffer);
    memcpy(&ipAdress.sin_addr, phe->h_addr_list[0], sizeof(struct in_addr));
	
	//on va checker c quoi le subnet mask
		char *IP = inet_ntoa(ipAdress.sin_addr);	//pogne le IP
		char Class[4]; //on va garder les 3 premier chiffres qui definisse la classe du network ici

		//check les 3 premier chiffres
		int i=0;
		while(IP[i] != '.')
		{
			Class[i] = IP[i];
			i++;
		}
		Class[i] = '\0';
		int ClassNum = atoi(Class);
		

		//est-ce que c un reseau de class A
		if(ClassNum <= 126)
		{
			NetMask.sin_addr.s_addr		=	inet_addr("255.0.0.0");
		}
		else if(ClassNum <= 191) //class B network
		{
			NetMask.sin_addr.s_addr		=	inet_addr("255.255.0.0");
		}
		else	//192 en montant, class C network
		{
			NetMask.sin_addr.s_addr		=	inet_addr("255.255.255.0");
		}

		//on va calculer le braodcast adress
		Subnet.sin_addr.s_addr			=	ipAdress.sin_addr.s_addr & NetMask.sin_addr.s_addr;
		Broadcast.sin_addr.s_addr		=	Subnet.sin_addr.s_addr | ~NetMask.sin_addr.s_addr;

		return inet_ntoa(Broadcast.sin_addr);

}

int cPeer2Peer::SendTo(long peerID,cUDPpacket *newPacket,unsigned long exception)
{


	if(peerID==-1) //est-ce qu'on est en presence d'un send de broadcast ?
	{
        if(!BCPeer)
		{
			sockaddr_in newIp;
			newIp.sin_family			=	AF_INET;
			newIp.sin_port				=	htons(BB_BROADCAST_PORT);
			newIp.sin_addr.s_addr		=	inet_addr(GetBroadcastAdress());
			memset(newIp.sin_zero,'\0',8);

			BCPeer = new cPeer(newIp,0);
		}
		newPacket->Safe = false; //on force le broadcast packet a etre unsafe
		newPacket->InterfaceID	=	INTERFACE_P2P;
		BCPeer->CreatePacket(newPacket);
		
		return 0;
	}



	if(peerID)	//si on c fait specifier un peer
	{
		//on va pogner le bon peer
		for(cPeer *P=PeerList;P;P=P->Next)
		{
			if(P->ID==(unsigned long)peerID) //on a trouver notre peer
			{
				P->CreatePacket(newPacket);
				return 0;
			}
		}

		sprintf(LastError,"Invalid peer ID : %i",(int)peerID);
		return 1;
	}
	else	//sinon on va envoyer a tout le monde
	{
		//pour chaque peer
		for(cPeer *P=PeerList;P;P=P->Next)
		{
			if(P->ID == exception) continue;
			P->CreatePacket(newPacket);
		}
	}

	return 0;
}

int	cPeer2Peer::SendToPeers(float elapsed)
{
	//on va envoyer le stock a chaque peer
	for(cPeer *P = PeerList;P;P=P->Next)
	{
		if(P->IpAdress.sin_addr.s_addr == 0)
		{
			continue;
		}

		if(P->SendPackets(UDPfd,elapsed,master,BytesSent))
		{
			//sprintf(LastError,"Error while sending packets to peers WSA : %i",WSAGetLastError());
			return 1;
		}
	}

	if(BCPeer)
	{
		BCPeer->SendPackets(BCfd,elapsed,master,BytesSent);
	}


	return 0;
}

int cPeer2Peer::ReceiveFromPeers()
{
	//on va prendre du data 30 fois
	int isReady =	30;
	//int newPeer	=	0;	//est-ce qu'on a un new peer

	while(--isReady)
	{
		fdread = master;

		if (select(fdmax+1, &fdread, NULL, NULL, &Timeout) == -1)
		{
			//sprintf(LastError,"Error : Problem select()ing while ReceivingFromPeers() WSA : %i",WSAGetLastError());
			return 1;
		}

		Timeout.tv_sec	=	0;
		Timeout.tv_usec =	0;

		//est-ce quon est pret a lire un packet udp 
		if(FD_ISSET(UDPfd,&fdread))
		{
			int				nbytes	=	0;
			
			#ifdef WIN32
				int	len	=	sizeof(sockaddr);
			#else
				socklen_t len	=	sizeof(sockaddr);
			#endif

			char			buf[1024];
			sockaddr_in		fromIP;

			if((nbytes = recvfrom(UDPfd,buf,sizeof(buf),0,(sockaddr*)&fromIP,&len)) < 0)
			{
				//sprintf(LastError,"Problem recvfrom()ing WSA : %i",WSAGetLastError());
				return 1;
			}
			else
			{
				//cPeer *P	=	GetPeerByIP(fromIP);
		
				int nread=0;
				while(nread < nbytes) //tant quia du data a lire
				{
					cUDPpacket *packet = ExtractPacket(buf,&nread);

					BytesReceived += packet->Size;
					
					ReceiveDatagram(packet,fromIP);


					delete packet;
				}
			}
		}
		else
		{
			isReady = 1;
		}


		//on check si on a un broadcast packet
		if(FD_ISSET(BCfd,&fdread))
		{
			int				nbytes	=	0;

			#ifdef WIN32
				int	len =	sizeof(sockaddr);
			#else
				socklen_t len = sizeof(sockaddr);
			#endif

			char			buf[1024];
			sockaddr_in		fromIP;

			if((nbytes = recvfrom(BCfd,buf,sizeof(buf),0,(sockaddr*)&fromIP,&len)) < 0)
			{
				//sprintf(LastError,"Problem recvfrom()ing BCpeer WSA : %i",WSAGetLastError());
				return 1;
			}
			else
			{
				if(!BCPeer)
				{
					sockaddr_in newIp;
					newIp.sin_family			=	AF_INET;
					newIp.sin_port				=	htons(BB_BROADCAST_PORT);
					newIp.sin_addr.s_addr		=	inet_addr(GetBroadcastAdress());
					memset(newIp.sin_zero,'\0',8);

					BCPeer = new cPeer(newIp,0);
				}

				int nread=0;
				while(nread < nbytes) //tant quia du data a lire
				{
					cUDPpacket *packet = ExtractPacket(buf,&nread);
					BytesReceived += packet->Size;
					
					BCPeer->AddReceivedPacket(new cUDPpacket(packet));				

					delete packet;
				}
			}
		}
	}
	return 0;
}

void cPeer2Peer::ReceiveDatagram(cUDPpacket *packet,sockaddr_in fromIP)
{
	cPeer		*P		=	0;
	BytesReceived		+=	packet->Size;


	if(packet->InterfaceID == 0 && packet->TypeID==TYPE_CONFIRM)
	{
		//on recois la confirmation, on va mettre a jour le IP/Port
		//fp = fopen("_netDebug.txt","a");
		//fprintf(fp,"	> RECEPTION DUNE CONFIRMATION DE %s:%i\n",inet_ntoa(fromIP.sin_addr),ntohs(fromIP.sin_port));
		//fclose(fp);


		//on va extraire le id
		unsigned long id;
		memcpy(&id,packet->Data,sizeof(unsigned long));

		P = GetPeerByID(id);
		
		if(!P) return; //on a recu une confirmation dun peer qui vient de se faire delter

		P->AddAck(packet->ID);

		if(!P->Confirmed)
		{
			P->IpAdress = fromIP;
			P->PendingID++;
			P->Confirmed = true;
		}

		return;
	}
	else
	{
		P = GetPeerByIP(fromIP);
	}

	//on check si le peer exist, si oui, tout est beau, sinon, on en create SI le packetID == 1
	if(P)
	{
		switch(packet->InterfaceID)
		{
			case INTERFACE_SYSTEM:
			{
				AnalyzeSystemDatagram(packet,P);
				break;
			}
			case INTERFACE_P2P:
			{
				if(P->Need2Delete)
				{
					//P->AddAck(packet->ID);
				}
				else
				{
					P->AddReceivedPacket(new cUDPpacket(packet));
				}
				break;
			}
		}
	}
	else //ici le peer ne fais pas partie dla liste des peer
	{
		if(packet->ID==1) //si c le premier packet qu'on recoit, on va creer un peer et commencer a parler avec
		{
			bool exist=false;
			int newID = CreatePeer(fromIP,&exist);

			//char ip[16];
			//unsigned short port=0;

			//sprintf(ip,inet_ntoa(fromIP.sin_addr));
			//port = ntohs(fromIP.sin_port);

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"> CREATION DUN PEER VIA RECEPTION DE PACKET %s:%i     exist = %i\n",ip,port,(int)exist);
			//fclose(fp);
			
			//si le peer existe, on est en presence d'un dupliquer du premier packet avant la publoication du peer
			if(exist) return;
			
			P = GetPeerByID(newID);
			P->Need2Publish = true;
			//P->Confirmed = true;

			switch(packet->InterfaceID)
			{
				case INTERFACE_SYSTEM:
				{
					AnalyzeSystemDatagram(packet,P);
					break;
				}
				case INTERFACE_P2P:
				{
					if(P->Need2Delete)
					{
						//P->AddAck(packet->ID);
					}
					else
					{
						P->AddReceivedPacket(new cUDPpacket(packet));
					}
					break;
				}
			}

			//si on a un port binder, on va devoir envoyer le peer aux autres
			if(BindedPort && (NetBitField & NET_PUBLISH_PEERS))
			{
				cUDPpacket *newPacket	=	new cUDPpacket();
				newPacket->ID			=	0;
				newPacket->InterfaceID	=	INTERFACE_SYSTEM;
				newPacket->TypeID		=	TYPE_PEER;

				newPacket->Safe			=	true;
				newPacket->Sent			=	false;
				newPacket->Data			=	new char[18];
				
				//pogne le IP
				char ip[16];
				sprintf(ip,inet_ntoa(fromIP.sin_addr));
				
				//pogne le port
				unsigned short port = ntohs(fromIP.sin_port);
				
				//on parse le data
				memcpy(newPacket->Data,ip,16);
				memcpy(newPacket->Data + 16,&port,2);
		        
				SendTo(0,newPacket,newID);
			}
		}
	}
}


cUDPpacket* cPeer2Peer::ExtractPacket(char* buf,int *nread)
{
	cUDPpacket *packet = new cUDPpacket();

	//on extract l'interface ID
	memcpy(&(packet->InterfaceID),buf + (*nread),sizeof(char));
	(*nread) += sizeof(char);
	
	//on va extraire le ID du packet
	memcpy(&(packet->ID),buf + (*nread),sizeof(unsigned long));
	(*nread) += sizeof(unsigned long);

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

cPeer* cPeer2Peer::GetPeerByIP(sockaddr_in ip)
{
	for(cPeer *P=PeerList;P;P=P->Next)
	{
		if(!stricmp(inet_ntoa(ip.sin_addr),inet_ntoa(P->IpAdress.sin_addr)))
		{
			if(ntohs(ip.sin_port) == ntohs(P->IpAdress.sin_port))	return P;
		}
	}
	return 0;
}

cPeer* cPeer2Peer::GetPeerByIPport(char *ip,unsigned short port)
{
	for(cPeer *P=PeerList;P;P=P->Next)
	{
		if(!stricmp(ip,inet_ntoa(P->IpAdress.sin_addr)))
		{
			if(port == ntohs(P->IpAdress.sin_port))	return P;
		}
	}
	return 0;
}

cPeer* cPeer2Peer::GetPeerByID(unsigned long id)
{
	for(cPeer *P=PeerList;P;P=P->Next)
	{
		if(P->ID == id) return P;
	}
	return 0;
}

void cPeer2Peer::AnalyzeSystemDatagram(char *buffer,int *nread,char *fromIP,unsigned short port)
{
	AnalyzeSystemDatagram(ExtractPacket(buffer,nread),GetPeerByIPport(fromIP,port));
}

void cPeer2Peer::AnalyzeSystemDatagram(cUDPpacket *packet,cPeer *fromPeer)
{
    //le packet ici est pret a se faire lire
	switch(packet->TypeID)
	{
		case TYPE_ACK:
		{
			//on va dire a nos packet lesquels on ete recu
			ApplyAcks(packet->Data,packet->Size,fromPeer);
			break;
		}
		case TYPE_PEER:
		{
			if(fromPeer->Need2Delete) return;

			fromPeer->AddAck(packet->ID);

			//on va creer un nouveau peer quia besoin d'etre publisher
			char ip[16];
			unsigned short port;
			memcpy(ip,packet->Data,16);
			memcpy(&port,packet->Data + 16,2);

			sockaddr_in ipAdress;
			
			ipAdress.sin_family			=	AF_INET;
			ipAdress.sin_addr.s_addr	=	inet_addr(ip);
			ipAdress.sin_port			=	htons(port);
			memset(&(ipAdress.sin_zero), '\0', 8);

			bool exist = false;
            int newID = CreatePeer(ipAdress,&exist);

			cPeer *P = GetPeerByID(newID);

			P->Need2Publish = true;

			break;
		}
		case TYPE_DEMAND:
		{
			//on recois une demande confirmation dun peer, on lui renvoie simplement le contenu de ce quil avait afin quil enregistre le bon port

			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"	>ON RECOIT UNE DEMANDE DE %s:%i \n",inet_ntoa(fromPeer->IpAdress.sin_addr),ntohs(fromPeer->IpAdress.sin_port));
			//fclose(fp);
			

			fromPeer->AddAck(packet->ID);
			if(fromPeer->Confirmed) return;
			
			//fp = fopen("_netDebug.txt","a");
			//fprintf(fp,"	>ON ENVOIE UNE CONFIRMATION \n");
			//fclose(fp);

			SendTo(fromPeer->ID,new cUDPpacket(0,0,packet->Data,TYPE_CONFIRM,sizeof(unsigned long),true));
			fromPeer->PendingID++; //on va lui dire quon attend maintenant apres le packet 2
			fromPeer->Confirmed = true;

			break;
		}
	}
}

void cPeer2Peer::ApplyAcks(char *ackBuffer,int bufferSize,cPeer* fromPeer)
{
	//on va se faire un array de ID quiont ete acked
	unsigned long *acks = new unsigned long[bufferSize/4];
	memcpy(acks,ackBuffer,bufferSize);

	//pour chaque packet on va checker si y se fait acker
	for(cUDPpacket *P = fromPeer->PacketsToSend;P;P=P->Next)
	{
		for(int i=0;i<bufferSize/4;i++)
		{
			if(acks[i] == P->ID) //on a trouver un packet qui est acker
			{
				//on indique que le packet est recu
				P->Ack	=	true;
				break;
			}
		}
	}

    delete	[]	acks;
}

int cPeer2Peer::CheckUnpublishedPeers(float elapsed,bool &isNew)
{
	for(cPeer *P=PeerList;P;P=P->Next)
	{
		if(P->Need2Delete)
		{
			P->Timeout += elapsed;

			if(P->Timeout<2)
			{
				//on check si on remplie les conditions pour mourir
				if(P->PacketsToSend || P->nbAcks) continue;

				//sinon on peut le deleter
				unsigned long id = P->ID;
				
				isNew = false;

				//fp = fopen("_netDebug.txt","a");
				//fprintf(fp,"		>>> PEER DELETED %s:%i\n",inet_ntoa(P->IpAdress.sin_addr),ntohs(P->IpAdress.sin_port));
				//fclose(fp);
				DeletePeer(P->ID);

				return (int)id;
			}
			else
			{
			
				//sinon on peut le deleter
				unsigned long id = P->ID;
				

				isNew = false;

				//fp = fopen("_netDebug.txt","a");
				//fprintf(fp,"		>>> PEER DELETED %s:%i WITH TIMEOUT\n",inet_ntoa(P->IpAdress.sin_addr),ntohs(P->IpAdress.sin_port));
				//fclose(fp);
				DeletePeer(P->ID);

				return (int)id;
			}
		}

		if(P->Need2Publish)
		{
			P->Need2Publish = false;

			isNew = true;

			return P->ID;
		}
	}

	//on va updater la liste des DNSquery
	for(cDNSquery *q = DNSqueries;q;q=q->Next)
	{
		int i = q->Update();

		if(i>0)
		{
			//on a confirmer le IP du domain
			isNew = true;
			unsigned long id = q->PeerID;

			//on va updater le peer
			cPeer *p = GetPeerByID(id);
			
			if(p)
			{
				p->IpAdress.sin_addr = q->IP.sin_addr;
			}

			if(q->Next)		q->Next->Previous = q->Previous;
			if(q->Previous) q->Previous->Next = q->Next;

			if(DNSqueries == q) DNSqueries = q->Next;

			delete q;
			return 0;
            
		}
		else if(i<0)
		{
			//on a confirmer le IP du domain est PAS BON
			isNew = false;
			unsigned long id = q->PeerID;

			if(q->Next)		q->Next->Previous = q->Previous;
			if(q->Previous) q->Previous->Next = q->Next;

			if(DNSqueries == q) DNSqueries = q->Next;

			DeletePeer(id);

			delete q;
			return (int)id;
		}
	}


	return 0;
}

int cPeer2Peer::AddDNSquery(char *domain,unsigned long id)
{
	//on check si ya deja des query
	if(DNSqueries)
	{
		cDNSquery *q = 0;
		for(q = DNSqueries;q->Next;q=q->Next)
		{
			if(!stricmp(q->Domain,domain)) return -1;
		}

		//on va creer un new peer pour la dnsquery
		q->Next = new cDNSquery(domain,id);
		q->Next->Previous = q;
	}
	else
	{
		DNSqueries = new cDNSquery(domain,id);
	}

	return 0;

}

int	cPeer2Peer::DeletePeer(unsigned long peerID)
{
	for(cPeer *P = PeerList;P;P=P->Next)
	{
		if(P->ID == peerID) //on a trouver le peer a deleter
		{
			if(P->Next)		P->Next->Previous = P->Previous;
			if(P->Previous) P->Previous->Next = P->Next;

			if(P==PeerList) PeerList=PeerList->Next;
			delete P;

			return 0;
		}
	}

	return 1;
}


cPeer2Peer::~cPeer2Peer()
{
	//on ferme notre socket de communication
	CloseSocket(UDPfd);

    //on va deleter tout les peers
	cPeer *toKill = 0;
	for(cPeer *P = PeerList;P;delete toKill)
	{
		toKill = P;
		P = P->Next;
	}

	if(BCPeer) delete BCPeer;


}
