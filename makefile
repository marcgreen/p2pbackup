
CFLAGS = -c -std=c++11 -I.
CLIENT_LDFLAGS = -pthread -lboost_system -Lcore -lbtcore
TRACKER_LDFLAGS = -pthread -lboost_system -Ltracker -ltracker -Lcore -lbtcore -L/usr/local/lib -ljsoncpp
CC = g++
LD = $(CC)
CLEANLIST = *.o *.a *~ btbackup bttracker
CLIENT_TARGET = btbackup
TRACKER_TARGET = bttracker

all: $(CLIENT_TARGET) $(TRACKER_TARGET)

$(CLIENT_TARGET): $(CLIENT_TARGET).o corelib
	$(LD) $(CLIENT_TARGET).o $(CLIENT_LDFLAGS) -o $(CLIENT_TARGET)

$(CLIENT_TARGET).o: $(CLIENT_TARGET).cpp
	$(CC) $(CFLAGS) $(CLIENT_TARGET).cpp -o $(CLIENT_TARGET).o

$(TRACKER_TARGET): $(TRACKER_TARGET).o corelib trackerlib
	$(LD) $(TRACKER_TARGET).o $(TRACKER_LDFLAGS) -o $(TRACKER_TARGET)

$(TRACKER_TARGET).o: $(TRACKER_TARGET).cpp
	$(CC) $(CFLAGS) $(TRACKER_TARGET).cpp -o $(TRACKER_TARGET).o

corelib:
	$(MAKE) -C core

trackerlib:
	$(MAKE) -C tracker

clean:
	 rm -f `find . -name '$(shell echo '$(CLEANLIST)' | sed "s/ /' -or -name '/g")'`
