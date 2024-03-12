
# ar command
AR=ar
ARFLAGS=-rvu

LIB=libCCard.so



all:$(LIB)


$(LIB): CCard.o
	@echo "Linking libCard.a"
	$(CXX) -shared -o $@ $?
	cp libCCard.so ../lib/
	cp libCCard.so ../HiAutoTestServer/bin
	cp libCCard.so ../HardwareManager/

CCard.o:CCard.cpp
	@echo "compiling " $<
	g++  -fPIC -g3 -O0 -c CCard.cpp	-I $(HRP_HOME)/HiReport -I $(AUTO_HOME)/DataManager -I $(AUTO_HOME)/HiLog -L$(HRP_HOME)/lib -L $(AUTO_HOME)/lib -lHiReport -lDataManager -lHiLog

clean :
	@rm -f *.o *~ libCCard.so ../lib/libCCard.so
