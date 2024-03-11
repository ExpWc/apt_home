FILES = \
	SimpoleInterfaceLinux.cpp
                
DEMOFILES = \
	main.cpp
OBJS = $(patsubst %.c, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))\
		-I /usr/include
	 #-I /home/V1.0.0/HiCaseManager/StepInstance/SimpoleInterfaceStep/Source/cpprestsdk-master/Release/include\
     #    -I ../driver/cpprestsdk-master/Release/include  \

EXENAME = libSimpoleInterfaceLinux.so
BIN = ./$(EXENAME)

CPPFLAGS= -g #-Wall -O0
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer -std=c++11
CPPFLAGS  +=  -Wall -Wundef -Wpointer-arith -fPIC 
#-fvisibility=hidden
#LD_LIB = 

LIBS = \
	-lcrypto -lcpprest -lssl -lboost_system -lpthread

DEMOLIBS = \
	-lSimpoleInterfaceLinux 
LIBPATH = -L. \
	-L $(AUTO_HOME)/lib \
	-L /usr/lib/




.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	echo "Linking" $(BIN)
	$(CXX) -std=c++11 -shared -fPIC -o $@ $^ $(INCLUDES)    $(LIBS)  $(LIBPATH)
#$(CXX) -shared -fPIC -o $@ $^ -Bstatic $(LIBPATH) $(LIBS) 
	cp ./$(EXENAME) $(AUTO_HOME)/lib
	cp ./$(EXENAME) ../Lib
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $< -o $@ 

.PHONY:demo
demo: $(DEMOFILES)
	$(CXX) -std=c++11 $(INCLUDES)  -o $@  $(DEMOFILES) $(LIBS) $(DEMOLIBS) $(LIBPATH)
	@echo "Compiled finished"

DIRS :
	@mkdir -p $(DIR)

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f *.o *.so

