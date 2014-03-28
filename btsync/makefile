all: btsync

btsync: BTSync.cpp BTSync.h
	g++ -L/usr/local/lib -lcurlpp -lstdc++ -L/usr/lib/x86_64-linux-gnu -lcurl -Wl,-Bsymbolic-functions -Wl,-z,relro -std=c++11 -ljsoncpp -o BTSync BTSync.cpp

clean:
	rm BTSync
