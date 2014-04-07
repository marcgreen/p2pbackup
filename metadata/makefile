CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
LDFLAGS = -L/usr/local/lib -ljsoncpp -lboost_system -pthread

all: trackerInterface metadataRecord trackerProtocol
	$(CC) $(LDFLAGS) TrackerInterface.o MetadataRecord.o TrackerProtocol.o -o TrackerInterface

trackerInterface: TrackerInterface.cpp TrackerInterface.h MetadataInterface.h
	$(CC) $(CFLAGS) TrackerInterface.cpp

metadataRecord: MetadataRecord.cpp MetadataRecord.h
	$(CC) $(CFLAGS) MetadataRecord.cpp

trackerProtocol: ../tracker/TrackerProtocol.cpp ../tracker/TrackerProtocol.h
	$(CC) $(CFLAGS) ../tracker/TrackerProtocol.cpp -o TrackerProtocol.o

clean:
	rm TrackerInterface TrackerInterface.o
	rm MetadataRecord.o
