
# -lcurl -lcurlpp -Wl,-Bsymbolic-functions -Wl,-z,relro

# /usr/lib/x86_64-linux-gnu/libcurl.a -Wl,-Bsymbolic-functions -Wl,-z,relro -lidn -lssl -lcrypto -llber -lldap -lrt -lgssapi_krb5 -lssl -lcrypto -lz -lrtmp -lz -lgnutls

CURLPP_LDFLAGS = -L/usr/local/lib -lcurlpp -lstdc++ -L/usr/lib/x86_64-linux-gnu -lcurl -Wl,-Bsymbolic-functions -Wl,-z,relro /usr/lib/x86_64-linux-gnu/libcurl.a -Wl,-Bsymbolic-functions -Wl,-z,relro -lidn -lssl -lcrypto -llber -lldap -lrt -lgssapi_krb5 -lssl -lcrypto -lz -lrtmp -lz -lgnutls
JSONCPP_LDFLAGS = /usr/local/lib/libjsoncpp.a
BOOST_LDFLAGS = /usr/lib/x86_64-linux-gnu/libboost_system.a
BOOSTFS_LDFLAGS = /usr/lib/x86_64-linux-gnu/libboost_filesystem.a

CFLAGS = -c -std=c++11 -I.

CLIENT_LDFLAGS = -pthread -Lcore -lbtcore -Ltracker -ltrackerclient $(JSONCPP_LDFLAGS) peer/peer.o -Lmetadata -lmeta btsync/BTSyncInterface.o $(CURLPP_LDFLAGS) $(BOOST_LDFLAGS) $(BOOSTFS_LDFLAGS)

TRACKER_LDFLAGS = -pthread -Ltracker -ltrackerserver -Lcore -lbtcore $(JSONCPP_LDFLAGS) -Lmetadata -lmeta $(BOOST_LDFLAGS)

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
