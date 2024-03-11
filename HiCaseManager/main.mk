Demo:main.cpp
	g++ main.cpp -o main -g -O0 -lpthread -lrt -Wall -W -Wpointer-arith -pipe -D_GNU_SOURCE   \
	-I. -I../HiReport -I../tinyxml -I../HiICDManager -I../HiICDManager/Block -I ../HiICDManager/Field -I ../HiLog \
	-L./ -L../lib  -lCaseManager -lICDManager