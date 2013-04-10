bin_PROGRAMS = linuxmaster


# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
linuxmaster_LDFLAGS = $(all_libraries) 
linuxmaster_SOURCES = cBV2game.cpp cBV2game.h CClient.cpp CClient.h \
	cMasterServer.cpp cMasterServer.h cNetManager.cpp cNetManager.h CServer.cpp CServer.h \
	main.cpp cMSstruct.h
lib_LIBRARIES = libBaboNet.a libDKC.a
libBaboNet_a_SOURCES = baboNet.cpp baboNet.h cClient.cpp cClient.h \
	cConnection.cpp cConnection.h cDNSquery.cpp cDNSquery.h cIncConnection.cpp \
	cIncConnection.h cPacket.cpp cPacket.h cPeer2Peer.cpp cPeer2Peer.h cPeer.cpp cPeer.h \
	cServer.cpp cServer.h cUDPpacket.cpp cUDPpacket.h global.h LinuxHeader.h md5c.c \
	md5class.cpp md5class.h MD5.h CThread.cpp CThread.h
linuxmaster_LDADD = $(top_builddir)/src/libDKC.a \
	$(top_builddir)/src/libBaboNet.a
libDKC_a_SOURCES = dkc.cpp dkc.h dkci.h
