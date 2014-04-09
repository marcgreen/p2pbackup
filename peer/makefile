CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
LDFLAGS = -L/usr/local/lib -ljsoncpp -lboost_system -pthread -lcrypto

all: peer.o

peer: peer.o metadataRecord.o
	$(LD) $(LDFLAGS) peer.o metadataRecord.o -o Peer 

metadataRecord.o: ../metadata/MetadataRecord.cpp ../metadata/MetadataRecord.h
	$(CC) $(CFLAGS) ../metadata/MetadataRecord.cpp -o metadataRecord.o

peer.o: Peer.cpp Peer.h
	$(CC) $(CFLAGS) Peer.cpp -o peer.o

clean:
	rm peer.o Peer
