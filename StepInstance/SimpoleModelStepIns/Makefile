
FILES = \
	CLoginStep.cpp \
	CloadModelStep.cpp \
	CBaseSimpoleInterfaceStep.cpp \
	CUnLoadModelStep.cpp \
	CUnLoadModelStep.cpp \
	CStartModelStep.cpp \
	CStopSimpoleModelStep.cpp \
	CPauseModelStep.cpp \
	CSetModelVarStep.cpp \
	CCompareModelVarStep.cpp \


OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

SIMPOLEINSTALLDIR=$(SIMPOLE_HOME)

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))
INCLUDES += -I $(HRP_HOME)/HiReport
INCLUDES += -I $(HRP_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(AUTO_HOME)/StepBase
INCLUDES += -I $(AUTO_HOME)/HiICDManager
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Block
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Field
INCLUDES += -I $(AUTO_HOME)/HardwareManager
INCLUDES += -I $(AUTO_HOME)/AutoTestInterface
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/DataManager
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiHWResource 
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMGlobalVarLib 
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMSched 
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMParaComLib 
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMVarComLib 
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/libSimulinkOffset 
INCLUDES += -I $(AUTO_HOME)/SwitchWiringInterface
INCLUDES += -I $(AUTO_HOME)/SimpoleInterface
INCLUDES += -I $(AUTO_HOME)/HiStorageManager
INCLUDES += -I $(AUTO_HOME)/HiCaseManager/StepInstance
INCLUDES += -I $(AUTO_HOME)/HiCaseManager/RTCTimer

EXENAME = libSimpoleStep.so
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
	-lHiHWResource \
	-lHiMVarCom \
	-lSimulinkOffset \
	-lHiMGlobalVar \
	-lHiMParaCom  \
	-lAutoTestInterface  \
	-lSimpoleInterfaceLinux  \
	-lStorageManager  \

LIBPATH = \
	-L $(HRP_HOME)/lib \
	-L $(AUTO_HOME)/lib \
	-L $(SIMPOLE_HOME)/include/lib/ \


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
