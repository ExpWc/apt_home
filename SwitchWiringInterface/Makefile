FILES = \
	SwitchWiringInterface.cpp
                
DEMOFILES = \
	main.cpp

OBJS = $(patsubst %.c, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES += $(foreach n,$(dir $(FILES)),-I $(n))\
	#-I /home/V1.0.0/SwitchWiringInterface/cpprestsdk-master/Release/include\
	#-I /home/SwitchWiring/cpprestsdk-master/Release/include\


EXENAME = libSwitchWiringInterface.so
BIN = ./$(EXENAME)

CPPFLAGS = -g -Wall -O0
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer -std=c++11
CPPFLAGS  +=  -Wall -Wundef -Wpointer-arith
# -fvisibility=hidden
LIBS = \
	-lcrypto -lcpprest -lssl -lboost_system -lpthread

DEMOLIBS = \
	-lSwitchWiringInterface
	
LIBPATH = -L. \
	  #-L $(AUTO_HOME)/lib

	  
.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	echo "Linking" $(BIN)
	$(CXX) $(CPPFLAGS) -std=c++11 -shared -fPIC -o $@ $^ ${INCLUDES} $(LIBS)  $(LIBPATH) #$(LIBS) 
	cp ./$(EXENAME) $(AUTO_HOME)/lib
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
	@echo "rm "$(OBJS) $(BIN)
	rm -f *.o *.so $(BIN)

