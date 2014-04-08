
CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
ARCHIVER = ar
ARFLAGS = rcs

trackerclient: TrackerProtocol.o clientdir
	$(ARCHIVER) $(ARFLAGS) libtrackerclient.a TrackerProtocol.o client/*.o

trackerserver: TrackerProtocol.o serverdir
	$(ARCHIVER) $(ARFLAGS) libtrackerserver.a TrackerProtocol.o server/*.o

TrackerProtocol.o: TrackerProtocol.cpp TrackerProtocol.h
	$(CC) $(CFLAGS) TrackerProtocol.cpp -o TrackerProtocol.o

clientdir:
	$(MAKE) -C client

serverdir:
	$(MAKE) -C server
