CC=g++
CFLAGS=-c -Wall -w -I/usr/include/SDL2 -std=c++11
LDFLAGS=-L/usr/local/lib -lSDL2 -lSDL2_net
SOURCES=main.cpp server.cpp client.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all:libpath $(SOURCES) $(EXECUTABLE) clean

libpath:
	export LD_LIBRARY_PATH=/usr/local/lib/

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
