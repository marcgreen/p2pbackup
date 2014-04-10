all: btsync

btsync: BTSyncInterface.cpp BTSyncInterface.h
	g++  -Wl,-Bsymbolic-functions -Wl,-z,relro -std=c++11 -c BTSyncInterface.cpp -o BTSyncInterface.o

clean:
	rm BTSyncInterface
