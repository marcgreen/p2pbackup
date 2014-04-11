
CFLAGS = -c -std=c++11 -I.
CLIENT_LDFLAGS = -pthread -lboost_system -Lcore -lbtcore -Ltracker -ltrackerclient -L/usr/local/lib -ljsoncpp peer/peer.o -Lmetadata -lmeta btsync/BTSyncInterface.o -lcrypto -L/usr/lib/x86_64-linux-gnu -lcurlpp -lcurl -Wl,-Bsymbolic-functions -Wl,-z,relro
TRACKER_LDFLAGS = -pthread -lboost_system -Ltracker -ltrackerserver -Lcore -lbtcore -L/usr/local/lib -ljsoncpp -Lmetadata -lmeta
CC = g++
LD = $(CC)
CLIENT_TARGET = btbackup
TRACKER_TARGET = bttracker
CLEANLIST = *.o *.a *~ $(CLIENT_TARGET) $(TRACKER_TARGET)

all: $(CLIENT_TARGET) $(TRACKER_TARGET)

$(CLIENT_TARGET): $(CLIENT_TARGET).o corelib trackerclientlib peerlib metadatalib btsynclib
	$(LD) $(CLIENT_TARGET).o $(CLIENT_LDFLAGS) -o $(CLIENT_TARGET)

$(CLIENT_TARGET).o: $(CLIENT_TARGET).cpp
	$(CC) $(CFLAGS) $(CLIENT_TARGET).cpp -o $(CLIENT_TARGET).o

$(TRACKER_TARGET): $(TRACKER_TARGET).o corelib trackerserverlib metadatalib
	$(LD) $(TRACKER_TARGET).o $(TRACKER_LDFLAGS) -o $(TRACKER_TARGET)

$(TRACKER_TARGET).o: $(TRACKER_TARGET).cpp
	$(CC) $(CFLAGS) $(TRACKER_TARGET).cpp -o $(TRACKER_TARGET).o

corelib:
	$(MAKE) -C core

trackerclientlib:
	$(MAKE) -C tracker trackerclient

trackerserverlib:
	$(MAKE) -C tracker trackerserver

metadatalib:
	$(MAKE) -C metadata

peerlib:
	$(MAKE) -C peer

btsynclib:
	$(MAKE) -C btsync

clean:
	 rm -f `find . -name '$(shell echo '$(CLEANLIST)' | sed "s/ /' -or -name '/g")'`
