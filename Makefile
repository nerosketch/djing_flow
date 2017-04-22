CC=gcc

CFLAGS=-c -Wall -O2 -std=c99
LDFLAGS=-lft -lconfig
SOURCES=conf.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=djing_flow

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
