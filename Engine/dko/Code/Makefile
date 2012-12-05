INC=-I. -I../inc -I../../babonet/Code -I../../DukZeven/inc
LIB=-L ../../DukZeven/Code

rel64: FLAGS= -D LINUX64 -O2 -fPIC
rel64: all desymb

dbg64: FLAGS= -D LINUX64 -g -fPIC
dbg64: all


all:	dko.so

dko.so: CVector.o CdkoMaterial.o CdkoMesh.o CdkoModel.o CFace.o COctree.o eHierarchic.o ePTexture.o dko.o
	g++ $(FLAGS) -shared -Wl,-soname,libdko.so.0.1 -o libdko.so.0.1 $(LIB) -lGL CVector.o CdkoMaterial.o CdkoMesh.o CdkoModel.o CFace.o COctree.o eHierarchic.o ePTexture.o dko.o

CVector.o: CVector.cpp CVector.h
	g++ $(FLAGS) -o CVector.o -c CVector.cpp $(INC)
CdkoMaterial.o: CdkoMaterial.h CdkoMaterial.cpp dkoInner.h
	g++ $(FLAGS) -o CdkoMaterial.o -c CdkoMaterial.cpp $(INC)
CdkoMesh.o: CdkoModel.h CdkoMesh.h dkoInner.h CdkoMaterial.h CdkoMesh.cpp
	g++ $(FLAGS) -o CdkoMesh.o -c CdkoMesh.cpp $(INC)
CdkoModel.o: eHierarchic.h CdkoMaterial.h COctree.h dkoInner.h CdkoModel.h CdkoMesh.h CdkoModel.cpp
	g++ $(FLAGS) -o CdkoModel.o -c CdkoModel.cpp $(INC)
CFace.o: CFace.cpp CFace.h CVector.h
	g++ $(FLAGS) -o CFace.o -c CFace.cpp $(INC)
COctree.o: COctree.h CFace.h CVector.h dkoInner.h COctree.cpp
	g++ $(FLAGS) -o COctree.o -c COctree.cpp $(INC)
eHierarchic.o: eHierarchic.cpp eHierarchic.h CFace.h
	g++ $(FLAGS) -o eHierarchic.o -c eHierarchic.cpp $(INC)
ePTexture.o: ePTexture.h ePTexture.cpp
	g++ $(FLAGS) -o ePTexture.o -c ePTexture.cpp $(INC)
dko.o: dkoInner.h CdkoMesh.h dko.cpp
	g++ $(FLAGS) -o dko.o -c dko.cpp $(INC)

desymb:
	strip -s *.so.0.1

clean:
	rm *.o *.so.0.1

install:
	sh install.sh

