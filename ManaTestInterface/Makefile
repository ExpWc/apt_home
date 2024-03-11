FILES = \
	ManualTestInterface.cpp \


OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

AUTOTESTINSTALLDIR=$(AUTO_HOME)
SIMPOLEINSTALLDIR=$(SIMPOLE_HOME)

INCLUDES += -I ./ \
				-I $(HRP_HOME)/tinyxml \
				-I $(AUTOTESTINSTALLDIR)/CCard \
				-I $(HRP_HOME)/HiReport \
				-I $(AUTOTESTINSTALLDIR)/DataManager \
				-I $(AUTOTESTINSTALLDIR)/HiICDManager/Block \
				-I $(AUTOTESTINSTALLDIR)/HiICDManager/Field \
				-I $(AUTOTESTINSTALLDIR)/HiICDManager \
				-I $(AUTOTESTINSTALLDIR)/HardwareManager \
				-I $(AUTOTESTINSTALLDIR)/HiLog \
				-I $(AUTOTESTINSTALLDIR)/HiStorageManager \
				-I $(SIMPOLEINSTALLDIR)/include/HiHWResource \
				-I $(SIMPOLEINSTALLDIR)/include/HiMGlobalVarLib \
				-I $(SIMPOLEINSTALLDIR)/include/HiMSched \
				-I $(SIMPOLEINSTALLDIR)/include/HiMParaComLib \
				-I $(SIMPOLEINSTALLDIR)/include/HiMVarComLib \
				-I $(SIMPOLEINSTALLDIR)/include/libSimulinkOffset \


EXENAME = libAutoTestInterface.so
BIN = \
     $(EXENAME)

CPPFLAGS=  -Wall -O2 -fPIC  #-g -O0
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith   

LIBS = \
-lHiReport -ltinyxml -lHardwareManager -lICDManager -lDataManager \
-lHiHWResource -lHiMVarCom -lSimulinkOffset -lHiMGlobalVar -lHiMParaCom  -lStorageManager #-lSimpoleInterfaceLinux -lCurveManager

LIBPATH = \
-L $(AUTO_HOME)/lib/ \
-L $(SIMPOLE_HOME)/include/lib/ \
-L $(HRP_HOME)/lib/ 

.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CPPFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
	cp $(EXENAME) $(AUTOTESTINSTALLDIR)/HiAutoTestServer/bin/ -f
	cp $(EXENAME) $(AUTOTESTINSTALLDIR)/lib/
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CPPFLAGS)  $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)
.PHONY:test
test:
	$(CXX) -g -O0 -o test Main.cpp -lAutoTestInterface $(INCLUDES) $(LIBPATH)  
.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
