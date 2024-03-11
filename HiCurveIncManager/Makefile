FILES = \
	./Curve/CCurve.cpp \
	./Curve/Constant.cpp \
	./Curve/SawtoothWave.cpp \
	./Curve/SineWave.cpp \
	./Curve/SlopeWave.cpp \
	./Curve/SquareWave.cpp \
	./Curve/TriangleWave.cpp \
	./Curve/CustomWave.cpp\
	./CurveInterface.cpp\
	parseXmlApi.cpp		
	

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I$(HRP_HOME)/tinyxml
INCLUDES += -I$(HRP_HOME)/HiReport
INCLUDES += -I$(AUTO_HOME)/CCard
INCLUDES += -I./
INCLUDES += -I$(AUTO_HOME)/DataManager
INCLUDES += -I$(AUTO_HOME)/HiLog
INCLUDES += -I$(AUTO_HOME)/AutoTestInterface/
INCLUDES += -I$(AUTO_HOME)/HiCurveIncManager/Curve
INCLUDES += -I$(AUTO_HOME)/HiStorageManager/
INCLUDES += -I $(SIMPOLE_HOME)/include/HiMGlobalVarLib
EXENAME = libCurveManager.so
BIN = \
     $(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith   
#-Werror

#LD_LIB = 

LIBS = \
-lpthread \
-ltinyxml \
-lHiReport \
-lCCard \
-ldl \
-lDataManager \
-lAutoTestInterface \
-lHiLog \
-lHiMParaCom \
-lHiMGlobalVar
#-lCaseManager 

LIBPATH = \
-L. \
-L $(AUTO_HOME)/lib \
-L $(SIMPOLE_HOME)/include/lib/ \
-L $(HRP_HOME)/lib \

.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS) -g -O0
	cp $(EXENAME)  ../lib/
	
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)

.PHONY:test
test:
	$(CXX) -g -O0 -o test main.cpp -lCurveManager -lpthread -ltinyxml -lHiReport -lDataManager -lAutoTestInterface -lHiLog -lCaseManager $(INCLUDES) $(LIBPATH)

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
	rm -f test
