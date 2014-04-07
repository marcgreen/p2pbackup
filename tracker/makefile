
CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
ARCHIVER = ar
ARFLAGS = rcs
OBJLIST = Tracker.o TrackerDatabase.o TrackerProtocol.o TrackerSocketConnection.o MetadataRecord.o

tracker: $(OBJLIST)
	$(ARCHIVER) $(ARFLAGS) libtracker.a $(OBJLIST)

Tracker.o: Tracker.cpp Tracker.h TrackerSocketConnection.h
	$(CC) $(CFLAGS) Tracker.cpp -o Tracker.o

TrackerDatabase.o: TrackerDatabase.cpp TrackerDatabase.h ../metadata/MetadataRecord.h
	$(CC) $(CFLAGS) TrackerDatabase.cpp -o TrackerDatabase.o

TrackerProtocol.o: TrackerProtocol.cpp TrackerProtocol.h
	$(CC) $(CFLAGS) TrackerProtocol.cpp -o TrackerProtocol.o

TrackerSocketConnection.o: TrackerSocketConnection.cpp TrackerSocketConnection.h
	$(CC) $(CFLAGS) TrackerSocketConnection.cpp -o TrackerSocketConnection.o

MetadataRecord.o: ../metadata/MetadataRecord.cpp ../metadata/MetadataRecord.h
	$(CC) $(CFLAGS) ../metadata/MetadataRecord.cpp -o MetadataRecord.o
