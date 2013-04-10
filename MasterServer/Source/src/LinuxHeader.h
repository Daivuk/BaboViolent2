#ifndef _LINUX_HEADER_H
#define _LINUX_HEADER_H

	#include <sys/time.h>
	#include "memory.h"
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <sys/times.h>
	#include <sys/resource.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <iostream>
	#include <string.h>
	#include <pthread.h>
	#include <net/if.h>
	#include <sys/ioctl.h>
	#include <net/if_arp.h>
	#include <errno.h>
	#include <time.h>
	#include <ctime>

	// --- Floyd Davidson's macro  ---
	#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
	#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))

	
	#define stricmp strcasecmp
	#define strnicmp strncasecmp
	#define amax(X,Y) ((X) < (Y) ? (Y) : (X))

#endif
