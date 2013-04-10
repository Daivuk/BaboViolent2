#include "cConnection.h"
#include "stdio.h"

cConnection::cConnection(bool *udpenabled,timeval *timeout,sockaddr_in *remoteIP,int *filedescriptor,int *udpfd,char* lastError,fd_set *master,fd_set *read,fd_set *write,int *fd_max, unsigned long *lpid)
{
	//pas de resultat de connection pour le moment
	ConnectResult			=	0;

	//on set les infos
    	Timeout					=	timeout;
	RemoteIP				=	remoteIP;
	UDPfd					=	udpfd;
	FileDescriptor			=	filedescriptor;
	LastError				=	lastError;
	Master					=	master;
	Read					=	read;
	Write					=	write;
	FDmax					=	fd_max;
	UDPenabled				=	udpenabled;

	connID					=	0;

	State					=	0;

	//on met a 0 le recvbuf
	memset(RecvBuf, '\0', 10);

	ConTimeout = 0;

	Lpid = lpid;

	//fp = fopen("_cdebug.txt","w");
	//fclose(fp);

	//start();
}

int cConnection::PrepareSocket()
{
	//on va setter ce quier relatif aux socket qu'on a besoin pour communiquer avec le serveur

	#ifdef USING_LOG
		LogFile = fopen( "log.txt" , "a" );
		fprintf( LogFile , "Entering PrepareSocket\n");
		fclose(LogFile);
	#endif

	Timeout->tv_sec	=	0;
	Timeout->tv_usec =	0;

	FD_ZERO(Master);	// clear the master and temp sets
	FD_ZERO(Read);		// temp fd_set pour lire
	FD_ZERO(Write);		// temp fd_set pour ecrire


	//unsigned short port		=	ntohs(RemoteIP->sin_port);

	// on va tester si les donner du remote host on ete entree
	if(RemoteIP->sin_port == 0)
	{
		sprintf(LastError,"Error : Remote Host infos not set!");

		#ifdef USING_LOG
			LogFile = fopen( "log.txt" , "a" );
			fprintf( LogFile , "Remote infos not set\n");
			fclose(LogFile);
		#endif

		return 1;
	}


	//on va creer le socket TCP avec le quel on va communiquer avec le remote host
	if ((*FileDescriptor = (int)socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{

		#ifdef USING_LOG
			LogFile = fopen( "log.txt" , "a" );
			fprintf( LogFile , "Problem creating client socket \n");
			fclose(LogFile);
		#endif

		printf(">>>>>>>>>>>>>>problem creating socket for connection\n");
		sprintf(LastError,"Error : Problem creating ClientSocket TCP");

        return 1;
    }

	
	#ifndef WIN32
		int yes = 1;
	#else
		char yes ='1';
	#endif

    // lose the pesky "address already in use" error message
    if (setsockopt(*FileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
	{
		#ifdef USING_LOG
			LogFile = fopen( "log.txt" , "a" );
			fprintf( LogFile , "Problem with SO_REUSEADDR option \n");
			fclose(LogFile);
		#endif

        sprintf(LastError,"Error : Problem on setsockopt() for the ClientSocket SO_REUSEADDR TCP");
        return 1;
    }

	if(setsockopt(*FileDescriptor, IPPROTO_TCP, TCP_NODELAY,&yes,sizeof(yes)) == -1)
	{
		#ifdef USING_LOG
			LogFile = fopen( "log.txt" , "a" );
			fprintf( LogFile , "Problem with TCP_NODELAY option \n");
			fclose(LogFile);
		#endif

			sprintf(LastError,"Error : Problem on setsockopt() for the ClientSocket TCP_NODELAY");
        	return 1;
	}


	return 0;
}

int cConnection::PrepareUDP()
{
	#ifdef WIN32
		char yes = '1';
	#else
		int yes = 1;
	#endif

	int Tio = 10; //10 ms timeout

	//on va creer le socket UDP avec le quel on va communiquer avec le remote host
	if ((*UDPfd = (int)socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		sprintf(LastError,"Error : Problem creating ClientSocket UDP");
        return 1;
    }

	// lose the pesky "address already in use" error message
    if (setsockopt(*UDPfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
	{
        sprintf(LastError,"Error : Problem on setsockopt() for the ClientSocket SO_REUSEADDR UDP");
        return 1;
    }

// 	if(setsockopt(*UDPfd, SOL_SOCKET, SO_SNDTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
// 	{
// 		//int Error = WSAGetLastError();
// 		//sprintf(LastError,"Error : FD : %i setsockopt() SO_SNDTIMEO UDP WSA : %i",*UDPfd,Error);
// 		return 1;
// 	}
// 
// 	if(setsockopt(*UDPfd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tio,sizeof(Tio)) == -1)
// 	{
// 		//int Error = WSAGetLastError();
// 		//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO UDP WSA : %i",*UDPfd,Error);
// 		return 1;
// 	}

	return 0;
}

int cConnection::Update(float elapsed)
{
	
	//on va supdater selon notre state

	switch(State)
	{
		case 0: //on va preparer les socket, si tout est beau on va launcher la thread qui nous connect
		{
			if(PrepareSocket())
			{
				return 1;
			}

			#ifdef USING_LOG
				LogFile = fopen( "log.txt" , "a" );
				fprintf( LogFile , "Starting connection thread \n");
				fclose(LogFile);
			#endif

			//initiate the connection
			isConnecting = false;
			State = 1;
			start();


			return 0;

		}
		case 1:	//on attend apres la thread du connect() de finir
		{
			if(!(isRunning())) //has connection process finished ?
			{
				//on check le resultat du connect()
				if(ConnectResult == 1)
				{
					if( *FileDescriptor )
						CloseSocket(*FileDescriptor);

					return 1; //une erreur pendant la connection


					// the connection with the thread didnt work, lets try without
		
					//if (connect(*FileDescriptor, (sockaddr *)RemoteIP,sizeof(sockaddr)) == -1)
					//{
					//	
					//	//sprintf(LastError,"Error : Problem while trying to connect() TCP to remote host : WSAerr : %i",Error);
					//	
					//	//printf("error calling connect()\n");
					//	ConnectResult = 1; //probleme de connection

					//	#ifdef USING_LOG
					//		int Error = WSAGetLastError();
					//		LogFile = fopen( "log.txt" , "a" );
					//		fprintf( LogFile , "Problem during re-connection WSA : %i\n",Error);
					//		fclose(LogFile);
					//	#endif

					//	
					//	return 1;
					//}
				}

				//else
				{
					//tout est beau on est pret a passer au prochain state, reception du ID de connection
					State	=	2;
					connID	=	0;
					ToRecv	=	37;	//on a 37 byte a recevoir du serveur
	
					FD_SET(*FileDescriptor,Master);
					*FDmax	=	*FileDescriptor;
			

					return 0;
				}
			}
			else
			{
				ConTimeout += elapsed;

				//UpdateConnecting( elapsed );
				
				//10 seconds timeout reached ?
				if(ConTimeout > 10.0f)
				{
					printf("ConnTimout Reached on socket %i!\n",*FileDescriptor);
					//let's kill the socket
					CloseSocket(*FileDescriptor);
					//*FileDescriptor = 0;
					ConTimeout = 0;
					isConnecting = false;
					return 1;
				}
			}
			return 0;
		}
		case 2:	//on attend apres le ID de connection
		{
			*Read	=	*Master;
			Timeout->tv_sec	=	0;
			Timeout->tv_usec =	0;
			if (select((*FDmax)+1, Read, NULL, NULL, Timeout) == -1)
			{
				sprintf(LastError,"Error : Problem select()ing while cConnection::Update()");

				#ifdef USING_LOG
					LogFile = fopen( "log.txt" , "a" );
					fprintf( LogFile , "Error Select()ing in con state 2  \n");
					fclose(LogFile);
				#endif

				return 1; //erreur
			}
			Timeout->tv_sec	=	0;
			Timeout->tv_usec =	0;


			if(FD_ISSET(*FileDescriptor,Read))
			{
				
				//on va recevoir notre id de connection du server
				int r = recv(*FileDescriptor,RecvBuf + (37 - ToRecv),37 - (37 - ToRecv),0);

				//on check la valeur retourner par le recv()
				if(r <= 0)
				{
					#ifdef USING_LOG
						LogFile = fopen( "log.txt" , "a" );
						fprintf( LogFile , "Problem Receiving connID from server \n");
						fclose(LogFile);
					#endif

					sprintf(LastError,"Error : Problem Receiving connID from server r = %i",r);
					return 1;
				}

				ToRecv -= r;

				if(!ToRecv) //on a recu tout ce quon attendais
				{
					memcpy(&connID,RecvBuf,sizeof(connID));

					char udp=0;
					memcpy(&udp,RecvBuf + 4,sizeof(char));

					char junk[33];
					memcpy(&junk,RecvBuf + 5,sizeof(char) * 32);
					//junk[32] = '\0';

					memcpy(Lpid , &(junk[28]) , sizeof(unsigned long) );

					
// 					if(udp)
// 					{
// 
// 						*UDPenabled = true;
// 
// 						if(PrepareUDP())
// 						{
// 							sprintf(LastError,"Error : Problem Preapring UDP socket");
// 							return 1;
// 						}
// 						FD_SET(*UDPfd,Master);
// 						*FDmax	=	amax(*FileDescriptor,*UDPfd);
// 
// 						State = 3; //on va se mettre en mode qu'on renvoie le ID
// 					}
					//else
					//{
// 						int Tiio = 10;
// 
// 						if(setsockopt(*FileDescriptor, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tiio,sizeof(Tiio)) == -1)
// 						{
// 							//int Error = WSAGetLastError();
// 							//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO TCP WSA : %i",*FileDescriptor,Error);
// 							return 1;
// 						}

						*UDPenabled = false;
						return 2; //la connection s'est etablie!
					//}
					
					
				}
        	}

			return 0;
		}

		case 3:  //on send un udp packet au serveur
		{
			*Write = *Master;
			if (select((*FDmax)+1, NULL, Write, NULL, Timeout) == -1)
			{
				sprintf(LastError,"Error : Problem select()ing to send UDP Packet");
				return 1; //erreur
			}
			Timeout->tv_sec	=	0;
			Timeout->tv_usec =	0;

			if(FD_ISSET(*UDPfd,Write))
			{
				int len = sizeof(sockaddr);
				struct sockaddr_in	remIP	=	*RemoteIP;
				//unsigned short port = ntohs(remIP.sin_port);
				//remIP.sin_port = htons(port + 1);

				//char interfaceID = INTERFACE_SERVER;
				char buf[20];
				int nwrite=0;	//garde le nombre de byte ecrites
				
				//on copy linterface
				//memcpy(buf,(char*)&interfaceID,sizeof(char));
				//nwrite += sizeof(char);
				
				//on copy le header
				stHeader header;
				header.Size		=	sizeof(unsigned long);
				header.typeID	=	0;

				//copy le header
				memcpy(buf + nwrite,&header,sizeof(stHeader));
				nwrite += sizeof(stHeader);
                
				
				memcpy(buf + nwrite,&connID,sizeof(unsigned long));
				nwrite += sizeof(unsigned long);

				int sent=0;
				sent = sendto(*UDPfd,(char*)buf,nwrite,0,(sockaddr*)RemoteIP,len);
				if(sent<=0)
				{
					//sprintf(LastError,"Error sendto : WSA %i",WSAGetLastError());
					return 1;
				}

				//fp = fopen("_cdebug.txt","a");
				//fprintf(fp,"on a envoyer le packet udp au serveur",LastError);
				//fclose(fp);

				State		=	4;	//on va se mettre en attente de confirmation de reception
				LastCheck	=	0;
				ToRecv		=	4;
				memset(RecvBuf,'\0',10);
			}

			return 0;
		}
		case 4:
		{
			LastCheck += elapsed;
			if(LastCheck > 2) //ca fait plus de 2 secondes qu'on attend apres le ack, y c surement pas rendu
			{
				//fp = fopen("_cdebug.txt","a");
				//fprintf(fp,"Fais 2 seconde quon attend apres le ack\n");
				//fclose(fp);

				State = 3;
				return 0;
			}

			*Read	=	*Master;
			if (select((*FDmax)+1, Read, NULL, NULL, Timeout) == -1)
			{
				sprintf(LastError,"Error : Problem select()ing to get tcp ack");
				return 1; //erreur
			}
			Timeout->tv_sec	=	0;
			Timeout->tv_usec =	0;

			if(FD_ISSET(*FileDescriptor,Read))
			{

				//fp = fopen("_cdebug.txt","a");
				//fprintf(fp,"on va tenter de recevoir le ACK connid\n");
				//fclose(fp);

				//on va recevoir le ack
				int r = recv(*FileDescriptor,RecvBuf + (sizeof(connID) - ToRecv),sizeof(connID) - (4 - ToRecv),0);

				//fp = fopen("_cdebug.txt","a");
				//fprintf(fp,"r = %i\n",r);
				//fclose(fp);

				//on check la valeur retourner par le recv()
				if(r <= 0)
				{
					//sprintf(LastError,"Error : Problem Receiving ACK connID from server WSA : %i r = %i",WSAGetLastError(),r);
					return 1;
				}

				ToRecv -= r;
			}
		
			if(ToRecv<=0) //on a recu tout ce qu'on attendait
			{
// 				int Tiio = 10;
// 
// 				if(setsockopt(*FileDescriptor, SOL_SOCKET, SO_RCVTIMEO,(const char*)&Tiio,sizeof(Tiio)) == -1)
// 				{
// 					//int Error = WSAGetLastError();
// 					//sprintf(LastError,"Error : FD : %i setsockopt() SO_RCVTIMEO TCP WSA : %i",*FileDescriptor,Error);
// 					return 1;
// 				}


				return 2;
			}
			return 0;
		}

	}
	return 0;
}

void cConnection::execute(void *pArg)
{
	//on va se connecter en TCP au Remote Host
	if (connect(*FileDescriptor, (sockaddr *)RemoteIP,sizeof(sockaddr)) == -1)
	{

		//sprintf(LastError,"Error : Problem while trying to connect() TCP to remote host : WSAerr : %i",Error);
		
		//printf("error calling connect()\n");
		ConnectResult = 1; //probleme de connection

		#ifdef USING_LOG
			int Error = WSAGetLastError();
			LogFile = fopen( "log.txt" , "a" );
			fprintf( LogFile , "Problem during connection WSA : %i\n", Error);
			fclose(LogFile);
		#endif

		
		return;
	}

	ConnectResult = 0; 

	#ifdef USING_LOG
		LogFile = fopen( "log.txt" , "a" );
		fprintf( LogFile , "Connection successful \n");
		fclose(LogFile);
	#endif

	
	return ;
}

void cConnection::CloseSocket(int fd)
{
#ifdef WIN32
	closesocket(fd);
#else
	close(fd);
#endif
}
