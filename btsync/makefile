all: btsync

btsync: BTSyncInterface.cpp BTSyncInterface.h
	g++ -L/usr/local/lib -lcurlpp -lstdc++ -L/usr/lib/x86_64-linux-gnu -lcurl -Wl,-Bsymbolic-functions -Wl,-z,relro -std=c++11 -ljsoncpp -o BTSyncInterface BTSyncInterface.cpp

clean:
	rm BTSyncInterface
