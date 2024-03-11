ARCH = $(shell arch)
ifeq ($(ARCH), x86_64)
ARCHFLAG := -m64
else
ARCHFLAG := -m32
endif

SIMPOLEINSTALLDIR=$(SIMPOLE_HOME)

INCLUDES = -I.
INCLUDES += -I$(HRP_HOME)/tinyxml
INCLUDES += -I$(HRP_HOME)/HiReport
INCLUDES += -I ./RTCTimer
INCLUDES += -I ./StepInstance
INCLUDES += -I $(AUTO_HOME)/HiICDManager
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Block
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Field
INCLUDES += -I $(AUTO_HOME)/HardwareManager
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/DataManager
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(AUTO_HOME)/HiStorageManager
INCLUDES += -I $(AUTO_HOME)/AutoTestInterface
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/libSimulinkOffset
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMParaComLib
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMGlobalVarLib
INCLUDES += -I $(SIMPOLEINSTALLDIR)/include/HiMVarComLib
INCLUDES += -I $(AUTO_HOME)/StepBase

LIBS = -lrt \
-lpthread \
-ltinyxml \
-lHiReport \
-lICDManager \
-lHardwareManager \
-lCCard \
-lHiLog \
-ldl \
-lAutoTestInterface \
-lHiMParaCom \
-lHiMGlobalVar \
-lHiMVarCom \
-lSimulinkOffset \
-lstdc++ \
-lCurveManager\
-lStepBase\
-lStorageManager 


LIBPATH = -L. \
-L $(HRP_HOME)/lib \
-L $(SIMPOLE_HOME)/include/lib/ \
-L $(AUTO_HOME)/lib 

CXX := g++ $(ARCHFLAG)
CXXFLAGS := -g -O0 -Wall -fPIC $(INCLUDES)

BINDIR=$(AUTO_HOME)/lib
TARGET=libCaseManager.so

SOURCE :=  $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(TARGET)
$(TARGET):  $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	cp $(TARGET) $(BINDIR)
*.o:$(SOURCE)
	@echo 'Building target: $@'
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
.PHONY:clean
clean:
	rm $(TARGET) $(OBJS) *.o -rf

.PHONY:demo
demo:main.cpp
	g++ main.cpp -o main $(CXXFLAGS) $(INCLUDES) $(LIBPATH) $(LIBS) -lCaseManager 
