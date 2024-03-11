
FILES = \
	dds2Interface.cpp 

OBJS = $(patsubst %.cpp, %.o, $(FILES))

DIR = $(foreach n,$(dir $(FILES)),.obj/$(n))

INCLUDES +=$(foreach n,$(dir $(FILES)),-I $(n))

INCLUDES += -I$(SIMPOLE_HOME)/dds2_pt
INCLUDES += -I$(SIMPOLE_HOME)/dds2_pt/gencpp
INCLUDES += -I$(OSPL_HOME)/include
INCLUDES += -I$(OSPL_HOME)/include/sys 
INCLUDES += -I$(OSPL_HOME)/include/dcps/C++/SACPP 

EXENAME = libdds2Interface.so
BIN = \
     $(SIMPOLE_HOME)/lib/$(EXENAME)

CPPFLAGS= -g -Wall -O0 -fPIC
CPPFLAGS  +=  "-msse" -march=core2  -fomit-frame-pointer 
CPPFLAGS  +=   -Wall -Wundef -Wpointer-arith   
#-Werror

LIBS = \
-lddsdatabase \
-ldcpsgapi \
-ldcpssacpp \
-lddsos \
-ldds2_pt 

LIBPATH = \
-L. \
-L$(OSPL_HOME)/lib/ \
-L$(HRP_HOME)/lib

.PHONY:all
all : DIRS $(BIN)
$(BIN) :  $(OBJS)
	@echo "Linking" $(BIN)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LIBPATH) $(LIBS)

.PHONY:test
test : test
	@echo "build test"
	rm -f test
	$(CXX) $(CFLAGS) -o $@ test.cpp $(LIBPATH) -ldl -g -O0

.PHONY:test1
test1 : test1
	@echo "build test"
	rm -f test1
	$(CXX) $(CFLAGS) -o $@  test1.cpp $(LIBPATH) -ldl -g -O0

test3 : test3
	@echo "build test"
	rm -f test3
	$(CXX) $(CFLAGS) -o $@  test3.cpp $(LIBPATH) -ldl -g -O0 -ldds2Interface

test4 : test4
	@echo "build test"
	rm -f test4
	$(CXX) $(CFLAGS) -o $@  test4.cpp $(LIBPATH) -ldl -g -O0 -ldds2Interface

.cpp.o :
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) -c $< -o $@ 

DIRS :
	@mkdir -p $(DIR)
.PHONY:clean
clean:
	@echo "rm "$(OBJS)
	rm -f $(OBJS)
	rm -f test
