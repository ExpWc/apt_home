# ar command

FILES = \
BaseStep.cpp \
BaseICDStep.cpp \
parseXmlApi.cpp \
BaseCommunicationStep.cpp \


OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))
INCLUDES += -I $(HRP_HOME)/HiReport
INCLUDES += -I $(HRP_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(AUTO_HOME)/HiICDManager
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Block
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Field



EXENAME = libStepBase.a
BIN = \
     ../lib/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith  

LIBS = \
	-lHiReport \
	-ltinyxml \
	-lHiLog \
	-lICDManager

	
LIBPATH = \
	-L $(AUTO_HOME)/lib \
	-L $(HRP_HOME)/lib \


.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	ar -rvu $@ $? 
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)
.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)

