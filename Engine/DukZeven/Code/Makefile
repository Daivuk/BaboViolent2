BABODIR=../../../BaboViolent2
FMOD_HEAD=$(BABODIR)/../../fmodstudioapi10901linux/api/lowlevel/inc
FMOD_LIB=$(BABODIR)../../fmodstudioapi10901linux/api/lowlevel/lib/x86_64
INC= -I../inc -I$(BABODIR)/inc -I.
FLAGS= -D USE_FMODEX -D LINUX64

dbg64: DBG=-g
dbg64: all

rel64: DBG=-O2
rel64: all desymb

console: dkc.so dksvar.so dkt.so copy

all: dki.so dkw.so dks.so dkc.so dksvar.so dkgl.so dkt.so dkf.so dkp.so 

dki.so: CVector.o dki.o
	g++ $(DBG) $(FLAGS) -shared -lSDL -Wl,-soname,libdki.so -o libdki.so dki.o CVector.o
dki.o:	dki.h CVector.h dkii.h dki.cpp dkw.h
	g++ $(DBG) $(FLAGS) -fPIC -o dki.o -c dki.cpp $(INC)
dkw.so: CVector.o dkw.o
	g++ $(DBG) $(FLAGS) -shared -lSDL -lGL -Wl,-soname,libdkw.so -o libdkw.so dkw.o CVector.o
dks.so:	CString.o dks.o
	g++ $(DBG) $(FLAGS) -I$(FMOD_HEAD) -L$(FMOD_LIB) -shared -lfmod -Wl,-soname,libdks.so -o libdks.so dks.o CString.o
dkc.so: dkc.o
	g++ $(DBG) $(FLAGS) -shared -Wl,-soname,libdkc.so -o libdkc.so dkc.o
dksvar.so: CVector.o CString.o dksvar.o CSystemVariable.o
	g++ $(DBG) $(FLAGS) -shared -Wl,-soname,libdksvar.so -o libdksvar.so CVector.o CString.o dksvar.o CSystemVariable.o
dkgl.so: CVector.o CString.o dkgl.o
	g++ $(DBG) $(FLAGS) -shared -lGL -Wl,-soname,libdkgl.so -o libdkgl.so CVector.o CString.o dkgl.o
dkgl.o: dkgl.cpp CVector.h dkgl.h dkgli.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkgl.o -c dkgl.cpp $(INC)

dkt.so: dkt.o CVector.o CString.o
	g++ $(DBG) $(FLAGS) -shared -lGL -Wl,-soname,libdkt.so -o libdkt.so dkt.o CVector.o CString.o

dkt.o:	dkt.cpp dkt.h dkti.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkt.o -c dkt.cpp -I$(INC)
dkf.so:	dkf.o CFont.o CString.o CVector.o
	g++ $(DBG) $(FLAGS) -shared -lGL -Wl,-soname,libdkf.so -o libdkf.so dkf.o CFont.o CString.o CVector.o

dkp.so:	dkp.o CParticle.o CMatrix.o CVector.o
	g++ $(DBG) $(FLAGS) -shared -Wl,-soname,libdkp.so -o libdkp.so CParticle.o CMatrix.o CVector.o dkp.o

dkp.o:	dkpi.h CMatrix.h dkp.cpp CVector.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkp.o -c dkp.cpp $(INC)
CVector.o: CVector.cpp CVector.h
	g++ $(DBG) $(FLAGS) -fPIC -o CVector.o -c CVector.cpp $(INC)
CString.o: CString.cpp CString.h
	g++ $(DBG) $(FLAGS) -fPIC -o CString.o -c CString.cpp $(INC)
CFont.o: CFont.cpp CFont.h
	g++ $(DBG) $(FLAGS) -fPIC -o CFont.o -c CFont.cpp $(INC)
dkf.o:	dkf.cpp dkf.h dkfi.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkf.o -c dkf.cpp $(INC)
CMatrix.o: CMatrix.cpp CMatrix.h
	g++ $(DBG) $(FLAGS) -fPIC -o CMatrix.o -c CMatrix.cpp $(INC)
CParticle.o: CParticle.cpp CParticle.h
	g++ $(DBG) $(FLAGS) -fPIC -o CParticle.o -c CParticle.cpp $(INC)
CSystemVariable.o: CSystemVariable.cpp CSystemVariable.h dksvari.h dksvardef.h
	g++ $(DBG) $(FLAGS) -fPIC -o CSystemVariable.o -c CSystemVariable.cpp $(INC)
dksvar.o: dksvar.cpp dksvar.h dksvari.h CString.h CSystemVariable.h dksvardef.h
	g++ $(DBG) $(FLAGS) -fPIC -o dksvar.o -c dksvar.cpp $(INC)
dkc.o:	dkc.cpp dkci.h dkc.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkc.o -c dkc.cpp $(INC) -I$(INC)
dks.o: dks.cpp dksi.h dks.h CVector.h 
	g++ $(DBG) $(FLAGS) -fPIC -o dks.o -c dks.cpp $(INC) -I$(FMOD_HEAD)
dkw.o:	dkw.cpp CVector.cpp dkw.h CVector.h
	g++ $(DBG) $(FLAGS) -fPIC -o dkw.o -c dkw.cpp $(INC)

desymb:
	strip -s *.so

clean:
	rm -f *.o *.so

install:
	sh install.sh

copy:
	mv *.so $(BABODIR)/Content/
