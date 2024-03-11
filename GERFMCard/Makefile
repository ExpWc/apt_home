 
FILES = \
	GERFMCard.cpp \
	DataBuffer.cpp \
	Ini.cpp

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES += $(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I $(AUTO_HOME)/HiReport
INCLUDES += -I $(AUTO_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/DataManager
INCLUDES += -I $(AUTO_HOME)/HiLog
INCLUDES += -I $(AUTO_HOME)/HiStorageManager
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/BaseRFMCard
INCLUDES += -I /usr/lib64/rfm2g
INCLUDES += -I $(HRP_HOME)/HiReport
INCLUDES += -I $(HRP_HOME)/tinyxml


EXENAME = libAutoTestRFMCard.so
BIN = \
    ../HiAutoTestServer/bin/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith
#-Werror


LIBS = \
	-lHiReport \
	-ltinyxml \
    -lDataManager \
    -lHiLog \
	-lStorageManager \
	-lCCard \
	-lAutoTestBaseRFMCard \
    -lrfm2g 


LIBPATH = \
	-L $(AUTO_HOME)/lib \
	-L $(HRP_HOME)/lib  \
	-L /usr/lib64/rfm2g \
	-L /usr/lib/rfm2g 


.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CPPFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
	#cp $(BIN) ../HardwareManager -f
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFPPLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)
.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
