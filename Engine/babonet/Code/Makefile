BABODIR=../../../BaboViolent2
OUT=libbaboNet.so
INC=-I. -I../inc -I$(BABODIR)/Code -I$(BABODIR)/inc


rel64: DBG=-O2
rel64: FLAGS=-D LINUX64 -fPIC
rel64: all desymb copy


dbg64: DBG=-g
dbg64: FLAGS=-D LINUX64 -fPIC
dbg64: all


all:	babonet.so

babonet.so: cPacket.o cDNSquery.o cPeer.o cPeer2Peer.o cUDPpacket.o cClient.o cConnection.o CThread.o cIncConnection.o cServer.o md5c.o md5class.o baboNet.o
	g++ $(FLAGS) $(DBG) -shared -Wl,-soname,$(OUT) -o $(OUT) cPacket.o cDNSquery.o cPeer.o cPeer2Peer.o cUDPpacket.o cClient.o cConnection.o CThread.o cIncConnection.o cServer.o md5c.o md5class.o baboNet.o -lpthread
cPacket.o: cPacket.cpp cPacket.h
	g++ $(FLAGS) $(DBG) -o cPacket.o -c cPacket.cpp $(INC)
cDNSquery.o: cDNSquery.cpp cDNSquery.h
	g++ $(FLAGS) $(DBG) -o cDNSquery.o -c cDNSquery.cpp $(INC)
cPeer.o: cPeer.cpp cPeer.h
	g++ $(FLAGS) $(DBG) -o cPeer.o -c cPeer.cpp $(INC)
cPeer2Peer.o: cPeer2Peer.cpp cPeer2Peer.h
	g++ $(FLAGS) $(DBG) -o cPeer2Peer.o -c cPeer2Peer.cpp $(INC)
cUDPpacket.o: cUDPpacket.cpp cUDPpacket.h
	g++ $(FLAGS) $(DBG) -o cUDPpacket.o -c cUDPpacket.cpp $(INC)
cClient.o: cClient.cpp cClient.h
	g++ $(FLAGS) $(DBG) -o cClient.o -c cClient.cpp $(INC)
cConnection.o: cConnection.cpp cConnection.h
	g++ $(FLAGS) $(DBG) -o cConnection.o -c cConnection.cpp $(INC)
CThread.o: $(BABODIR)/Code/CThread.cpp $(BABODIR)/Code/CThread.h
	g++ $(FLAGS) $(DBG) -o CThread.o -c $(BABODIR)/Code/CThread.cpp $(INC)
cIncConnection.o: cIncConnection.cpp cIncConnection.h
	g++ $(FLAGS) $(DBG) -o cIncConnection.o -c cIncConnection.cpp $(INC)
cServer.o: cServer.cpp cServer.h
	g++ $(FLAGS) $(DBG) -o cServer.o -c cServer.cpp $(INC)
md5c.o:	md5c.c MD5.h global.h
	gcc $(FLAGS) $(DBG) -o md5c.o -c md5c.c $(INC)
md5class.o: md5class.cpp md5class.h
	g++ $(FLAGS) $(DBG) -o md5class.o -c md5class.cpp $(INC)
baboNet.o: baboNet.cpp baboNet.h
	g++ $(FLAGS) $(DBG) -o baboNet.o -c baboNet.cpp $(INC)

copy:
	mv $(OUT) $(BABODIR)/Content/

desymb:
	strip -s $(OUT)

clean:
	rm -f *.o $(OUT)

install:
	sh install.sh



