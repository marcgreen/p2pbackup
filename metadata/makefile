all: trackerInterface metadataRecord
	g++ TrackerInterface.o MetadataRecord.o -o TrackerInterface

trackerInterface: TrackerInterface.cpp TrackerInterface.h MetadataInterface.h
	g++ -std=c++11 -c TrackerInterface.cpp

metadataRecord: MetadataRecord.cpp MetadataRecord.h
	g++ -std=c++11 -c MetadataRecord.cpp

clean:
	rm TrackerInterface TrackerInterface.o
	rm MetadataRecord.o
