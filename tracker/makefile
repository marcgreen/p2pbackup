
CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
ARCHIVER = ar
ARFLAGS = rcs
OBJLIST = Tracker.o

tracker: $(OBJLIST)
	$(ARCHIVER) $(ARFLAGS) libtracker.a $(OBJLIST)

Tracker.o: Tracker.cpp Tracker.h
	$(CC) $(CFLAGS) Tracker.cpp -o Tracker.o
