CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS= -ljack -I. -std=c++11 -lncurses -lpthread
SOURCES= JackEngine.cpp NoteHandler.cpp Note.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=jackseq

all: $(OBJECTS)
	$(CC) $(OBJECTS) main.cpp $(LDFLAGS) -o main

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rf *.o main tests ||: 2> /dev/null

test: $(OBJECTS)
	$(CC) $(OBJECTS) tests.cpp $(LDFLAGS) -o tests
	./tests
