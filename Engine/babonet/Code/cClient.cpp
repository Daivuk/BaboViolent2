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

#include "cClient.h"




cClient::cClient()
{
	UDPfd				=	0;
	UDPport				=	0;
	FileDescriptor		=	0;
	Connection			=	0;
	UDPenabled			=	false;

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	isConnected			=	false;
	
	NetID				=	0;

	//TCP
	lastPacket.Size		=	0;
	lastPacket.data		=	0;
	bytesRemaining		=	KEY_SIZE;	//for incoming key
	WaitingForHeader	=	true;
	WaitingForKey		=	true;
	PartialHeader		=	false;
	NbPacket			=	0;
	PendingID			=	1;
	PartialKey			=	false;

	LastPacketID		=	0;
	
	
	ReceivedPackets		=	0;
	PacketsToSend		=	0;
	UDPPacketsToSend	=	0;
	PacketToKill		=	0;

	DataRate			=	1000;

	Next				=	0;
	Previous			=	0;

	BytesSent			=	0;
	BytesReceived		=	0;

	sprintf(Key,RND_KEY);

}

//client persu par le server
cClient::cClient(sockaddr_in *ip, int fileDescriptor,UINT4 netID)
{
	UDPfd				=	0;
	UDPport				=	0;
	FileDescriptor		=	fileDescriptor;
	ipAdress			=	*ip;
	UDPenabled			=	false;

	//clear master set
	FD_ZERO(&masterS);

	//add the new file descriptor to this clients fd
	FD_SET( (unsigned int)(FileDescriptor) , &masterS );

	//Unused: char *IP			=	inet_ntoa(ipAdress.sin_addr);
	//Unused: unsigned short port		=	ntohs(ipAdress.sin_port);


	isConnected			=	false;
	Connection			=	0;

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	NetID				=	netID;

	DataRate			=	1000;

	//TCP
	lastPacket.Size		=	0;
	lastPacket.data		=	0;
	bytesRemaining		=	KEY_SIZE;	//for incoming key
	//bytesRemaining		=	TCP_HEADER_SIZE;		//pour le header qui sen vient
	WaitingForHeader	=	true;
	WaitingForKey		=	true;
	PartialHeader		=	false;
	LastPacketID		=	0;
	NbPacket			=	0;
	PendingID			=	1;
	PartialKey			=	false;

	//UDP
	//UDPlastPacket.Size		=	0;
	//UDPlastPacket.data		=	0;
	//UDPbytesRemaining		=	HEADER_SIZE;		//pour le header qui sen vient
	//UDPWaitingForHeader		=	true;
	//UDPPartialHeader		=	false;

	//par defaut on a pas de packet a envoyer
	PacketsToSend		=	0;
	UDPPacketsToSend	=	0;
	ReceivedPackets		=	0;
	PacketToKill		=	0;

	Next				=	0;
	Previous			=	0;

	BytesSent			=	0;
	BytesReceived		=	0;

	sprintf(Key,RND_KEY);
}

//client qui veut se connecter a un serveur
cClient::cClient(const char *HostIP, unsigned short port,UINT4 netID)
{

	UDPenabled			=	false;
	BytesSent			=	0;
	BytesReceived		=	0;

	sprintf(LastMessage,"");
	sprintf(LastError,"");

	isConnected			=	false;
	Connection			=	0;

	RemoteIP.sin_addr.s_addr	=	inet_addr(HostIP);
	RemoteIP.sin_family			=	AF_INET;
	RemoteIP.sin_port			=	htons(port);
	memset(&(RemoteIP.sin_zero), '\0', 8);

	FileDescriptor	=	0;
	UDPfd			=	0;
	UDPport			=	0;
	NetID			=	netID;

	//clear master set
	FD_ZERO(&master);


	//TCP
	lastPacket.Size		=	0;
	lastPacket.data		=	0;
	bytesRemaining		=	KEY_SIZE;	//for incoming key
	WaitingForHeader	=	true;
	WaitingForKey		=	true;
	PartialHeader		=	false;
	NbPacket			=	0;
	PendingID			=	1;
	PartialKey			=	false;

	LastPacketID		=	0;
	
	
	//UDP
	//UDPlastPacket.Size		=	0;
	//UDPlastPacket.data		=	0;
	//UDPbytesRemaining		=	HEADER_SIZE;		//pour le header qui sen vient
	//UDPWaitingForHeader		=	true;
	//UDPPartialHeader		=	false;
	
	//par defaut on a pas de packet a envoyer
	PacketsToSend		=	0;
	UDPPacketsToSend	=	0;

	DataRate			=	1000;
	
	ReceivedPackets		=	0;
	PacketToKill		=	0;

	Next				=	0;
	Previous			=	0;

	sprintf(Key,RND_KEY);

	#ifdef USING_LOG
		LogFile = fopen( "log.txt" , "a" );
		fprintf( LogFile , "Going to start connection sequence\n");
		fclose(LogFile);
	#endif

	//on part la thread qui nous connect//////
	Connection			=	new cConnection(&UDPenabled,&Timeout,&RemoteIP,&FileDescriptor,&UDPfd,LastError,&master,&read_fds,&write_fds,&fdmax,&LastPacketID);
}

int cClient::UpdateConnection(float elapsed)
{
	if(Connection)
	{
		int r = Connection->Update(elapsed);
		
		if(r==2) // la connection s'est fait!
		{
			PendingID = LastPacketID + 1;

			delete Connection; 
			Connection = 0;

			sprintf(LastMessage,"Connection to server succesfull FD : %i UDPfd : %i",FileDescriptor,UDPfd);

			return r;
		}

		if(r==1) //une erreur c produite durant la connection
		{
			//printf(">>>>>>>>>>>>>>Error while connecting\n");
			//sprintf(LastMessage,"Error while connecting...aborting, see bb_clientGetLastError()");

			sprintf(LastError, Connection->LastError);
			delete Connection;
			Connection = 0;
			return r;
		}
	}

	return 0;
}


int	cClient::ReceivePacketsFromServer()
{
    char buffer[3072];

	read_fds = master; // copy it

	if (select(fdmax+1, &read_fds, NULL, NULL, &Timeout) == -1)
	{
		//sprintf(LastError,"Error : Problem select()ing while cClient::ReceivePacketsFromServer() WSA : %i",WSAGetLastError());
		sprintf(LastError,"Error : client, error while selecting, err no. %d", 
		#ifdef WIN32
		WSAGetLastError()
		#else
		errno
		#endif
		);
        return 1;
   	 }

	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;

	if(UDPenabled)
	{
		
		int isReady = 20;

		while(isReady--)
		{
		
			//est-ce que notre client est pret a recevoir du data UDP
			if (FD_ISSET(UDPfd, &read_fds))
			{
				//on va recevoir des packets
				sockaddr_in remoteIP;
				#ifdef WIN32
					int len	= sizeof(sockaddr_in);
				#else
					socklen_t len	= sizeof(sockaddr_in);
				#endif

//				char		buf[2048];	//on se garde un buffer de 2048 byte
				int			nbytes=0;	//nombre de byte recu

				//if((nbytes = recvfrom(UDPfd, buf, sizeof(buf),0,(sockaddr*)&remoteIP,&len)) <= 0)
                if((nbytes = recvfrom(UDPfd, buffer, 2048, 0, (sockaddr*)&remoteIP, &len)) <= 0)
				{
					//un probleme est survenu
					if(nbytes == 0)
					{
						//le server nous a deconnecter
						Disconnect();
						sprintf(LastMessage,"Server disconnected");
						return 2;
					}
					else
					{
						//sprintf(LastError,"Error : Problem recv()ing from server UDP fd : %i WSA : %i",UDPfd,WSAGetLastError());
						isReady = false;
						continue;
					}
				}
				else
				{
					//ici on est pret a lire les packets
					ReceiveDatagram(nbytes, buffer);

					BytesReceived	+=	nbytes;

					//on va rechecker si on est ok
					if (select(fdmax+1, &read_fds, NULL, NULL, &Timeout) == -1)
					{
						//sprintf(LastError,"Error : Problem select()ing while cClient::ReceivePacketsFromServer() WSA : %i",WSAGetLastError());
						sprintf(LastError,"Error : client, error while selecting (#2)");
						return 1;
					}
					Timeout.tv_sec	=	0;
					Timeout.tv_usec =	0;
				}
			}
			else
			{
				isReady = false;
			}
		}
	}

	//est-ce que notre client est pret a recevoir du data TCP
    if (FD_ISSET(FileDescriptor, &read_fds))
	{
		//on va recevoir des packets
		//char		*buf = new char[3072];	//on se garde un buffer de 2048 byte
		int			nbytes=0;	//nombre de byte recu
		
		if((nbytes = recv(FileDescriptor, buffer, 3072,0)) <= 0)
		{
			//un probleme est survenu
			if(nbytes == 0)
			{
				//le server nous a deconnecter
				Disconnect();
				sprintf(LastMessage,"Server disconnected");
                    //delete [] buf;
				return 2;
			}
			else
			{
				sprintf(LastError,"Error : Problem recv()ing packets from server TCP");
                    //delete [] buf;
				return 1;
			}	
		}
		else
		{
			BytesReceived	+=	nbytes;

			//ici on est pret a lire les packets
			if(ReceiveStream(nbytes, buffer))
			{
				//hacking
				Disconnect();
				sprintf(LastMessage,"Disconnected for potential hacking");
                    //delete [] buf;
				return 2;
			}
		}
            //delete [] buf;
	}


	return 0;
	
}

INT4 cClient::IsReadyToSend()
{

	fd_set writeFD;
	writeFD = masterS;

	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;


	if( select( this->FileDescriptor+1, NULL , &writeFD , NULL , &Timeout ) == -1 )
	{
		if( errno == 9 )
		{
			return BBNET_ERROR;
		}
		else
		{
			return 0;
		}
	}

	if( FD_ISSET( this->FileDescriptor , &writeFD ) )
	{
		// ready to send
		return 1;
	}
	else
	{
		// not ready to send
		return 0;
	}

	// Unreachable:
	//Timeout.tv_sec = 0;
	//Timeout.tv_usec = 0;

}

INT4 cClient::IsReadyToReceive()
{

	fd_set readFD;
	readFD = masterS;

	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;


	if( select( this->FileDescriptor+1, &readFD , NULL , NULL , &Timeout ) == -1 )
	{
		if( errno == 9 )
		{
			return BBNET_ERROR;
		}
		else
		{
			return 0;
		}
	}

	if( FD_ISSET( this->FileDescriptor , &readFD ) )
	{
		// ready to read
		return 1;
	}
	else
	{
		// not ready to read
		return 0;
	}

	// Unreachable:
	//Timeout.tv_sec = 0;
	//Timeout.tv_usec = 0;

}

void cClient::ReceiveDatagram(int nbytes,char *buf)
{
	int nread=0;
	while(nread < nbytes)
	{
		//on va extraire le header
		stHeader header;

		memcpy(&header,buf + nread,sizeof(stHeader));
		nread += sizeof(stHeader);

		//on create le packet avec le data en l'ajoutant a notre queue des packet recu
		AddReceivedPacket(new cPacket(buf + nread,header.Size,header.typeID));
		nread += header.Size;
	}


}

int cClient::SendPacketsToServer()
{
	
	write_fds = master; // copy it

	if (select(fdmax+1, NULL, &write_fds, NULL, &Timeout) == -1)
	{
		sprintf(LastError,"Error : Problem select()ing while SendPacketsToServer()");
		return 1;
	}
	
	Timeout.tv_sec	=	0;
	Timeout.tv_usec =	0;


	if(UDPenabled)
	{
		//si ya des packets UDP en attente on va les envoyer
		if(UDPPacketsToSend)
		{
			//est-ce que notre client est pret a envoyer du data UDP
			if (FD_ISSET(UDPfd, &write_fds))
			{
				for(cPacket *P=UDPPacketsToSend;P;)
				{
					//on envoie le packet
					//struct sockaddr_in remIP = RemoteIP;
					//unsigned short port = ntohs(remIP.sin_port);
					//remIP.sin_port = htons(port + 1);
					
					BytesSent += P->Size;
					P->SendUDP(UDPfd,&RemoteIP);

					//on peut retirer ce packet du queue
					if (P->Next) P->Next->Previous = P->Previous;
					if (P->Previous) P->Previous->Next = P->Next;
					if(P==UDPPacketsToSend)
					{
						UDPPacketsToSend = P->Next;
					}
					cPacket *toKill = P;
					P = P->Next;
					delete toKill;
					continue;
				}
			}
		}
	}

	//si ya des packets TCP en attente on va les envoyer
	if(PacketsToSend)
	{
		//est-ce que notre client est pret a envoyer du data TCP
		if (FD_ISSET(FileDescriptor, &write_fds))
		{
			Send(BytesSent);
		}
	}

	return 0;
}
void cClient::Disconnect()
{
	CloseSocket(FileDescriptor);
	if(UDPenabled) CloseSocket(UDPfd);
	UDPfd			=	0;
	FileDescriptor	=	0;
}

int cClient::ReceiveStream(int nbytes,char *buf)
{
	int nread=0; //nombre de byte qu'on a lu du recv()

	//tant quia des byte a lire
	while(nread<nbytes)
	{
	
		if(WaitingForKey)
		{
			//key partial
			if(nbytes-nread < KEY_SIZE)
			{
				//partial key, lets take what we can
				memcpy(lastKey,buf+nread,nbytes-nread);

				//let's check if what we partialy have is valid
				int nb = nbytes-nread;
				if(memcmp(lastKey, "RND1", nb > 4 ? 4 : nb ))
				{
					//rnd key is corrupted, potential hacking
					Disconnect();
				}
											
                bytesRemaining = bytesRemaining - (unsigned short)(nbytes-nread);
				nread += nbytes-nread;
				PartialKey = true;

                		return 0;
			}
			//rest of the key is in the packet
			else
			{
				//we are terminating a key
				if(PartialKey)
				{
					//lets complete that key
					memcpy(lastKey + (KEY_SIZE - bytesRemaining),buf+nread,bytesRemaining);
					nread += bytesRemaining;

					bytesRemaining = TCP_HEADER_SIZE;
					WaitingForKey = false;
					WaitingForHeader = true;

					PartialKey = false;
				}
				else
				{
					//we take the key from the buffer as its all available
					memcpy(lastKey,buf+nread,bytesRemaining);
					nread += bytesRemaining;
					bytesRemaining = TCP_HEADER_SIZE;
					WaitingForKey = false;

				}

				//Key is complete, lets analyze it
				char key[5];
				char pid[5];	//packet ID
				memcpy(key, lastKey, sizeof(UINT4));
				key[4] = '\0';
				if(stricmp("RND1",key)) return 1;	//RndLabs key is corrupted, potential hacker

				memcpy(pid, lastKey + sizeof(UINT4), sizeof(UINT4));
                               			
				if(GetPendingID(pid)) return 1;		//potential hacker
				PendingID++;

				//grab le nombre de packet a recevoir
				memcpy(&NbPacket,lastKey + (KEY_SIZE - 1),sizeof(char));

			}
		}
	
		if(WaitingForHeader)
		{
			//on test si c un partial header ou complete
			if(nbytes-nread < TCP_HEADER_SIZE)
			{
				//header partiel
				memcpy(&lastHeader[0],&buf[nread],nbytes-nread);
                bytesRemaining = bytesRemaining - (unsigned short)(nbytes-nread);
				nread += nbytes-nread;
				PartialHeader = true;

				//fp = fopen("_sdebug.txt","a");
				//fprintf(fp,"On attend un header partiel\n");
				//fclose(fp);

			}
			else
			{
				//on va lire le header qui sen vient
				if(PartialHeader)
				{
					//fp = fopen("_sdebug.txt","a");
					//fprintf(fp,"On tente de reconstituer un header partiel\n");
					//fclose(fp);

					memcpy(&lastHeader[TCP_HEADER_SIZE - bytesRemaining],buf + nread,bytesRemaining);
					memcpy(&lastPacket.Size,&lastHeader,2);
					memcpy(&lastPacket.TypeID,&(lastHeader[2]),2);
					nread += bytesRemaining;
					PartialHeader = false;
					WaitingForHeader = false;
					bytesRemaining = lastPacket.Size;
					if(lastPacket.Size > 0)	//si notre packet a du data
					{
						lastPacket.data = new char[lastPacket.Size];
					}
					else	//si notre packet a suelement un type id
					{
						lastPacket.data = 0;
						WaitingForHeader = true;
						bytesRemaining = TCP_HEADER_SIZE;

						//on va l'ajouter au queue des packet disponible
						AddReceivedPacket(new cPacket(lastPacket.data,lastPacket.Size,lastPacket.TypeID));
					
							lastPacket.Size = 0;
							delete [] lastPacket.data;
							lastPacket.data = 0;

						NbPacket--;
						
						//all sub packets have been received in the master packet
						if(!NbPacket)
						{
							WaitingForKey = true;
							bytesRemaining = KEY_SIZE;
						}
					}
				}
				else
				{
					memcpy(&lastHeader[TCP_HEADER_SIZE - bytesRemaining],buf + nread,bytesRemaining);
					memcpy(&lastPacket.Size,&lastHeader,2);
					memcpy(&lastPacket.TypeID,&(lastHeader[2]),2);
					nread += bytesRemaining;
					WaitingForHeader = false;
					bytesRemaining = lastPacket.Size;
					if(lastPacket.Size > 0)	//si notre packet a du data
					{
						lastPacket.data = new char[lastPacket.Size];
					}
					else	//si notre packet a suelement un type id
					{
						lastPacket.data = 0;
						WaitingForHeader = true;
						bytesRemaining = TCP_HEADER_SIZE;

						//on va l'ajouter au queue des packet disponible
						AddReceivedPacket(new cPacket(lastPacket.data,lastPacket.Size,lastPacket.TypeID));
					
							lastPacket.Size = 0;
							delete [] lastPacket.data;
							lastPacket.data = 0;

						NbPacket--;
						
						//all sub packets have been received in the master packet
						if(!NbPacket)
						{
							WaitingForKey = true;
							bytesRemaining = KEY_SIZE;
						}
					}
				}
			}
		}
		else
		{
			//est-ce que le reste du packet est dans le buffer ?
			if(nread + bytesRemaining <= nbytes)
			{
				//on pogne le reste du data directement
				memcpy(lastPacket.data + (lastPacket.Size - bytesRemaining),buf + nread,bytesRemaining);
				WaitingForHeader = true;
				nread += bytesRemaining;
				bytesRemaining = TCP_HEADER_SIZE; //pour le prochain header

				//ici le packet est complet
					
				//on va l'ajouter au queue des packet disponible
				AddReceivedPacket(new cPacket(lastPacket.data,lastPacket.Size,lastPacket.TypeID));
					
					lastPacket.Size = 0;
					delete [] lastPacket.data;
					lastPacket.data = 0;

				NbPacket--;
				
				//all sub packets have been received in the master packet
				if(!NbPacket)
				{
					WaitingForKey = true;
					bytesRemaining = KEY_SIZE;
				}
			}
			else
			{
				//on prend ce qu'on peut
				memcpy(lastPacket.data,buf + nread,nbytes - nread);
                bytesRemaining = bytesRemaining - (unsigned short)(nbytes - nread);
				nread = nbytes;

				//fp = fopen("_sdebug.txt","a");
				//fprintf(fp,"On est en presence d'un packet data partiel\n");
				//fclose(fp);
			}
		}
	}

	return 0;
}


void cClient::AddReceivedPacket(cPacket *newPacket)
{
	if(ReceivedPackets)	//yavait deja des packet en attente de rammasser par le server ?
	{
		//on va l'ajouter au debut de la liste
		ReceivedPackets->Previous = newPacket;
		newPacket->Next = ReceivedPackets;
		ReceivedPackets = newPacket;
	}
	else
	{
		ReceivedPackets = newPacket;
	}
}

bool cClient::GetPendingID(char *pid)
{
	char digest[33];

	sprintf( digest , "%ld" , PendingID );
	//ltoa( PendingID , digest , 10 );

	//create md5 hash from current PendingID
	CMD5 md5(digest);
	sprintf(digest, "%s",md5.getMD5Digest());
	md5.setPlainText(0);

	char newID[5];

	memcpy( &(newID[0]) , digest + 1 , sizeof (char) );
	memcpy( &(newID[1]) , digest + 3 , sizeof (char) );
	memcpy( &(newID[2]) , digest + 5 , sizeof (char) );
	memcpy( &(newID[3]) , digest + 7 , sizeof (char) );
	newID[4] = '\0';
	pid[4] = '\0';

    return stricmp(pid , newID) ? true : false;
        

}

cPacket* cClient::GetReadyPacket()
{
	if(PacketToKill)
	{
		if(PacketToKill == ReceivedPackets) ReceivedPackets  = 0;
		delete PacketToKill;

		PacketToKill = 0;
	}

	if(!ReceivedPackets) return 0;

	cPacket *P=ReceivedPackets;
	
	while(P->Next)
	{
		P=P->Next;
	}

	PacketToKill = P;
	if(P->Previous)	P->Previous->Next = 0;
	return P;
}

int cClient::Send(UINT4 &nbByte)
{
	//on va envoyer les packets en attente dans la liste PacketsTCP

	int	packed			=	0;
	char	*buf		=	new char[3072];
	char	NbPacket	=	0;


	if(PacketsToSend)
	{

		//on parse notre key
		memcpy(buf + packed,&Key,sizeof(char)*4);
            //packed += sizeof(sizeof(char)*4);
            packed += sizeof(UINT4);
	
		//on parse le packetID
		char pid[5];
		GetLastPacketID(pid);
		memcpy(buf + packed,&(pid),sizeof(char) * 4);
		packed += (sizeof(char) * 4) + 1; //+1 pour laisser 1 place pour le nombre de packet
	}
	

	cPacket *toKill=0;
	for(cPacket *p=PacketsToSend;p;delete toKill)
	{
		NbPacket++;

		stHeader header;

		header.Size		=	p->Size;
		header.typeID	=	p->TypeID;
				
		memcpy(buf + packed,&header,sizeof(stHeader));
		packed += sizeof(stHeader);

		//on parse le data
		if(header.Size)
		{
			memcpy(buf + packed,p->Data,header.Size);
			packed += header.Size;
		}

		
		if(packed >= DataRate) //on est pret a envoyer une premier bacth
		{
			//on copie le nombre de packet qui sen vient
			memcpy(&(buf[8]),&NbPacket,sizeof(char));

			int sent	=	0;
			
			while(sent < packed)
			{
				int iSent=0;
				iSent = send(FileDescriptor,buf + sent,packed - sent,0);

				if(iSent <= 0)
				{
					printf("Problem sending packets in cClient::Send() \n");
					//sprintf(LastError,"Problem sending packets in cClient::Send()");
					delete [] buf;
					return 1;
				}

				sent += iSent;
			}
			nbByte += packed;
			packed = 0;

			toKill = p;
			PacketsToSend=p=p->Next;
			
			delete [] buf;

			delete toKill;
			return 0;
		}

		toKill = p;
		PacketsToSend=p=p->Next;
	}

	if(packed)
	{

		memcpy(&(buf[8]),&NbPacket,sizeof(char));

		int sent	=	0;
		
		while(sent < packed)
		{
			int iSent=0;
			iSent = send(FileDescriptor,buf + sent,packed - sent,0);

			if(iSent <= 0)
			{
				printf("Problem sending packets in cClient::Send() \n");
				//sprintf(LastError,"Problem sending packets in cClient::Send()");
				delete [] buf;
				return 1;
			}

			sent += iSent;
		}

		nbByte += packed;
	}

	delete [] buf;
	return 0;

}

void cClient::GetLastPacketID(char *pid)
{
	++LastPacketID;

	char digest[33];

	sprintf( digest , "%ld" , LastPacketID );
//	ltoa( LastPacketID , digest , 10 );

	//create md5 hash from current PendingID
	CMD5 md5(digest);
	sprintf(digest, "%s",md5.getMD5Digest());
	md5.setPlainText(0);

	char newID[5];

	memcpy( &(newID[0]) , digest + 1 , sizeof (char) );
	memcpy( &(newID[1]) , digest + 3 , sizeof (char) );
	memcpy( &(newID[2]) , digest + 5 , sizeof (char) );
	memcpy( &(newID[3]) , digest + 7 , sizeof (char) );
	newID[4] = '\0';

	sprintf(pid , newID);

}

//le serveur veux envoyer a ce client
int cClient::SendUDP(int UDPFD,UINT4 &nbByte)
{
	//on va envoyer les packets en attente dans la liste UDPPacketsToSend

	cPacket *toKill=0;
	int		packed=0;		//garde le nombre de bytes dans le buffer
	char	buffer[1024];	//buffer quon va envoyer

	//on doit packer les packets
	for(cPacket *p=UDPPacketsToSend;p;delete toKill)
	{
		if(packed>=500) break; //si ca fais plus de 500 bytes qu'on pack, on sort
		
		//on setup une struct pour envoyer

		stPacket Packet;
		stHeader header;

		header.Size			=	p->Size;
		header.typeID		=	p->TypeID;
	
		//si c juste un typeID qu'on veut passer
		if(!header.Size)
		{
			Packet.data		=	new char[UDP_HEADER_SIZE];
			memcpy(Packet.data, &header, UDP_HEADER_SIZE);
		}
		else
		{
			Packet.data		=	new char[p->Size + UDP_HEADER_SIZE];

			memcpy(Packet.data, &header, UDP_HEADER_SIZE);
			memcpy(Packet.data + UDP_HEADER_SIZE, p->Data, header.Size);
		}
	

		memcpy(buffer + packed,Packet.data,p->Size + UDP_HEADER_SIZE);
		packed += p->Size + UDP_HEADER_SIZE;

		delete [] Packet.data;

		toKill = p;
		UDPPacketsToSend=p=p->Next;

	}

	sockaddr_in remip;
	remip = ipAdress;
	remip.sin_port = htons(UDPport);

	//on envoie le packet
	int sent = 0;	

	//tant qui reste du data on va envoyer le size
	int Remaining = packed;
	int total = Remaining;

	//on a 5 essaie sinon on sort en erreur
	int tries = 5;

	while(Remaining)
	{
		if(!tries)
		{
			return 1;
		}

		sent = sendto(UDPFD,(const char*)(buffer + (total-Remaining)),Remaining,0,(sockaddr*)&remip,sizeof(sockaddr));

		if(sent < 0)
		{
			//sprintf(LastError,"Error sending UDP packets to Client WSA : %i",WSAGetLastError());
			return 1;
		}
		
		Remaining -= sent;
		tries -= 1;
	}

	nbByte += packed;


	return 0;

}

void cClient::CreatePacket(cPacket *newPacket,bool isUDP)
{
	if(!isUDP)
	{
		//si yavait deja des packets a envoyer
		if(PacketsToSend)
		{
			//on move jusqua la fin des packets
			cPacket *P=0;
			for(P=PacketsToSend;P->Next;P=P->Next){}

			//on lajoute au queue
			P->Next = newPacket;
			newPacket->Previous = P;
		}
		else	//yavait pas de packet a envoyer
		{
			PacketsToSend = newPacket;
		}
	}
	else
	{
		//si yavait deja des packets a envoyer
		if(UDPPacketsToSend)
		{
			//on move jusqua la fin des packets
			cPacket *P=0;
			for(P=UDPPacketsToSend;P->Next;P=P->Next){}

			//on lajoute au queue
			P->Next = newPacket;
			newPacket->Previous = P;
		}
		else	//yavait pas de packet a envoyer
		{
			UDPPacketsToSend = newPacket;
		}
	}
}	

cClient::~cClient()
{
	//Active	=	false;

	//on va deleter les packets si yen avait
	if(PacketsToSend)
	{
		cPacket *toKill=0;
		for(cPacket *P=PacketsToSend;P;delete toKill)
		{
			toKill = P;
			P=P->Next;
		}
		PacketsToSend = 0;
	}

	//on va deleter les packets si yen avait
	if(UDPPacketsToSend)
	{
		cPacket *toKill=0;
		for(cPacket *P=UDPPacketsToSend;P;delete toKill)
		{
			toKill = P;
			P=P->Next;
		}
		UDPPacketsToSend = 0;
	}


	if(ReceivedPackets)
	{
		cPacket *toKill=0;
		for(cPacket *P=ReceivedPackets;P;delete toKill)
		{
			toKill = P;
			P=P->Next;
		}
		ReceivedPackets = 0;
	}

	Disconnect();
	

}

void cClient::CloseSocket(int socketFD)
{
	#ifdef WIN32
		closesocket(socketFD);
	#else
		close(socketFD);
	#endif
}
