FILES = \
	RFMCardManager.cpp \
	ParseXml.cpp \

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))


INCLUDES += -I $(AUTO_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/HiReport
INCLUDES += -I $(AUTO_HOME)/BaseRFMCard
INCLUDES += -I $(AUTO_HOME)/DataManager
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(HRP_HOME)/tinyxml
INCLUDES += -I $(HRP_HOME)/HiReport


EXENAME = libAutoTestRFMCardManager.so

BIN = \
../HiAutoTestServer/bin/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith   
#-Werror


LIBS = \
-ltinyxml \
-lpthread \
-lHiReport \
-lCCard \
-lDataManager \
-lAutoTestBaseRFMCard \
-lHiLog

LIBPATH = \
-L . \
-L $(AUTO_HOME)/lib \
-L $(HRP_HOME)/lib 

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
.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
