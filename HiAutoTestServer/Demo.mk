Demo:Demo.cpp
	g++  Demo.cpp parseXmlApi.cpp -o Demo -g -O0 -lpthread -Wall -W -Wpointer-arith -pipe -D_GNU_SOURCE   -I/usr/local/ACE_wrappers -D__ACE_INLINE__ -I/usr/local/ACE_wrappers -I/usr/local/ACE_wrappers/TAO -I/usr/local/ACE_wrappers/TAO/orbsvcs/ -I$(HRP_HOME)/tinyxml -I$(HRP_HOME)/HiReport -lpthread    -L./ -L../lib -L$(HRP_HOME)/lib -L/usr/local/ACE_wrappers/lib  -ltinyxml -lHiReport -lTAO_CosNaming -lTAO -lACE -ldl -lrt -lHiAutoTest_Client