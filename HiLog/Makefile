FILES = \
	HiLog.cpp \
	HiPacLog.cpp 

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

#INCLUDES += -I../tinyxml
#INCLUDES += -I../HiReport
#INCLUDES += -I../CCard
INCLUDES += -I./
#INCLUDES += -I../DataManager

EXENAME = libHiLog.so
BIN = \
     ../lib/$(EXENAME) 

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith   
#-Werror

#LD_LIB = 

LIBS = 

LIBPATH = \
-L. \
-L../lib 

.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)
	cp $(BIN) ./
	#cp $(BIN) $(AUTO_HOME)/HiAutoTestServer/bin
.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@
DIRS :
	@mkdir -p $(DIR)

.PHONY:test
test:
	$(CXX) -g -O0 -o test main.cpp -lHiLog $(INCLUDES) $(LIBPATH)  

.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
	rm -f test
