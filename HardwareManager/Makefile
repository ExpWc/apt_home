FILES = \
	HardwareManager.cpp \
	ParseXmlApi.cpp 

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I$(HRP_HOME)/tinyxml
#INCLUDES += -I../AutoTestInterface/Source
INCLUDES += -I$(HRP_HOME)/HiReport
INCLUDES += -I$(AUTO_HOME)/CCard
INCLUDES += -I./
INCLUDES += -I$(AUTO_HOME)/DataManager
INCLUDES += -I$(AUTO_HOME)/HiLog

EXENAME = libHardwareManager.so
BIN = \
     ../lib/$(EXENAME) 

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
-lHiLog

LIBPATH = \
-L$(HRP_HOME)/lib\
-L$(AUTO_HOME)/lib\

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

.PHONY:test
test:
	$(CXX) -g -O0 -o test main.cpp ParseXmlApi.cpp -lHardwareManager -lDataManager -lHiLog $(INCLUDES) $(LIBPATH)  

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
	rm -f test
