ARCH = $(shell arch)
ifeq ($(ARCH), x86_64)
ARCHFLAG := -m64
else
ARCHFLAG := -m32
endif

INCLUDES = -I.
INCLUDES += -I$(HRP_HOME)/tinyxml
INCLUDES += -I$(HRP_HOME)/HiReport
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Block
INCLUDES += -I $(AUTO_HOME)/HiICDManager/Field
INCLUDES += -I $(AUTO_HOME)/HiICDManager/XMLParser

LIBS = -lpthread \
-ltinyxml \
-lHiReport

LIBPATH = -L. \
-L$(AUTO_HOME)/lib \
-L ../lib \
-L $(HRP_HOME)/lib \

CXX := g++ $(ARCHFLAG)
CFLAGS := -g -O0 -Wall -fPIC $(INCLUDES) -std=c++0x 
#-std=c++11
CXXFLAGS := $(CFLAGS)

BINDIR=../lib
TARGET=libICDManager.so

SOURCE :=  $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))
#TARGETFILE=$(BINDIR)/$(TARGET)

all:$(TARGET)
$(TARGET):  $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
#	$(CXX) -o $(TARGETFILE) $(OBJS)  $(LIBS) $(LIBPATH) 
#	ar -r -o $(TARGETFILE) $(OBJS) 
#	$(CXX) -o $(TARGET) $(OBJS)  $(LIBS) $(LIBPATH)  
	@echo 'Finished building target: $@'
	@echo ' '
	cp $(TARGET) $(BINDIR)

*.o:$(SOURCE)
	@echo 'Building target: $@'
#	$(CXX) $(CXXFLAGS) -c  -o $@ $< $(INCLUDES) $(LIBS) $(LIBPATH) 
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
.PHONY:clean
clean:
	rm -rf $(TARGET) $(OBJS) *.o

