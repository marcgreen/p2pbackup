CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
LDFLAGS = -L/usr/local/lib -ljsoncpp -lboost_system -pthread

all: metadataRecord

metadataRecord: MetadataRecord.cpp MetadataRecord.h
	$(CC) $(CFLAGS) MetadataRecord.cpp

clean:
	rm MetadataRecord.o
