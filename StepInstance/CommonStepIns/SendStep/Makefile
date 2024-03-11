
FILES = \
	SendStep.cpp \


OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))
INCLUDES += -I $(HRP_HOME)/HiReport
INCLUDES += -I $(HRP_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(AUTO_HOME)/StepBase
INCLUDES += -I $(AUTO_HOME)/HiICDManager
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Block
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Field
INCLUDES += -I $(AUTO_HOME)/HardwareManager
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/DataManager

EXENAME = libSendStep.so
BIN = \
     $(AUTO_HOME)/HiAutoTestServer/bin/StepLib/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith
#-Werror

#LD_LIB = 

LIBS = \
	-lHiReport \
    -ltinyxml \
    -lHiLog \
	-lStepBase \
	-lICDManager \
	-lHardwareManager \
	

LIBPATH = \
	-L $(HRP_HOME)/lib \
	-L $(AUTO_HOME)/lib \


.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)

.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)
	mkdir -p $(AUTO_HOME)/HiAutoTestServer/bin/StepLib/

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
