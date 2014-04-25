
CFLAGS = -c -std=c++11 -I../..
CC = g++
LD = $(CC)

all: Tracker.o TrackerDatabase.o TrackerSocketConnection.o

Tracker.o: Tracker.cpp Tracker.h
	$(CC) $(CFLAGS) Tracker.cpp -o Tracker.o

TrackerDatabase.o: TrackerDatabase.cpp TrackerDatabase.h
	$(CC) $(CFLAGS) TrackerDatabase.cpp -o TrackerDatabase.o

TrackerSocketConnection.o: TrackerSocketConnection.cpp TrackerSocketConnection.h
	$(CC) $(CFLAGS) TrackerSocketConnection.cpp -o TrackerSocketConnection.o
