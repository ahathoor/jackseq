CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS= -ljack -I. -std=c++11
SOURCES= JackInterface.cpp NoteHandler.cpp Note.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=jackseq

all: $(OBJECTS)
	$(CC) $(OBJECTS) main.cpp $(LDFLAGS) -o main

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	rm *.o main tests

test: $(OBJECTS) 
	$(CC) $(OBJECTS) tests.cpp $(LDFLAGS) -o tests
	./tests

