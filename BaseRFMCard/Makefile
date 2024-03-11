FILES = \
	BaseRFMCard.cpp \


OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I $(HRP_HOME)/tinyxml
INCLUDES += -I $(HRP_HOME)/HiReport
INCLUDES += -I $(AUTO_HOME)/CCard
INCLUDES += -I $(AUTO_HOME)/HiReport
INCLUDES += -I $(AUTO_HOME)/tinyxml
INCLUDES += -I $(AUTO_HOME)/DataManager
INCLUDES += -I $(AUTO_HOME)/HiLog


EXENAME = libAutoTestBaseRFMCard.a
BIN = \
     ../lib/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith  

LIBS +=-lCCard \
	

LIBPATH +=-L$(AUTO_HOME)/lib \


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
