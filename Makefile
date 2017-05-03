CC=gcc

CFLAGS=-c -Wall -O2 -std=c99
#CFLAGS=-c -Wall -O0 -std=c99 -g -fvar-tracking

LDFLAGS=-lft -lconfig
SOURCES=conf.c main.c mybinarytree.c tests.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=djing_flow

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
