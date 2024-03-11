FILES = \
	CStorageManager.cpp \
	CStorageRule.cpp \
	Ini.cpp \
	TimeManager.cpp \
	parse_xml_api.cpp \
	

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I$(HRP_HOME)/tinyxml
INCLUDES += -I$(HRP_HOME)/HiReport
INCLUDES += -I../CCard
INCLUDES += -I./
INCLUDES += -I../DataManager
INCLUDES += -I../HiLog
INCLUDES += -I../ICDManager
#INCLUDES += -I../AutoTestInterface/Source


EXENAME = libStorageManager.so
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
-lICDManager \
-lHiLog \
#-lAutoTestInterface


LIBPATH = -L $(AUTO_HOME)/lib \
-L $(HRP_HOME)/lib \
-L ../lib 

.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
	cp $(EXENAME)  ../lib/
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)
.PHONY:test
test:
	$(CXX) -g -O0 -o test main.cpp -lHardwareManager -lDataManager -lHiLog -lStorageManager $(INCLUDES) -I../BaseSerialCard/ -I../Analysis422/ $(LIBPATH)  

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
	rm -f test
