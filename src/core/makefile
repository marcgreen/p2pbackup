
CFLAGS = -c -std=c++11 -I..
CC = g++
LD = $(CC)
ARCHIVER = ar
ARFLAGS = rcs
OBJLIST = BTSyncController.o ConsoleController.o Controller.o Dispatcher.o \
	MetadataController.o NetworkController.o PeerSocketConnection.o \
	Worker.o Job.o

btcore: $(OBJLIST)
	$(ARCHIVER) $(ARFLAGS) libbtcore.a $(OBJLIST)

Dispatcher.o: Dispatcher.cpp Dispatcher.h Job.h Worker.h
	$(CC) $(CFLAGS) Dispatcher.cpp -o Dispatcher.o

Worker.o: Worker.cpp Dispatcher.h Job.h Worker.h
	$(CC) $(CFLAGS) Worker.cpp -o Worker.o

Job.o: Job.cpp Job.h
	$(CC) $(CFLAGS) Job.cpp -o Job.o

Controller.o: Controller.cpp Controller.h
	$(CC) $(CFLAGS) Controller.cpp -o Controller.o

ConsoleController.o: ConsoleController.cpp ConsoleController.h Controller.h PeerSocketConnection.h
	$(CC) $(CFLAGS) ConsoleController.cpp -o ConsoleController.o

MetadataController.o: MetadataController.cpp MetadataController.h Controller.h
	$(CC) $(CFLAGS) MetadataController.cpp -o MetadataController.o

NetworkController.o: NetworkController.cpp NetworkController.h Controller.h Job.h
	$(CC) $(CFLAGS) NetworkController.cpp -o NetworkController.o

BTSyncController.o: BTSyncController.cpp BTSyncController.h Controller.h
	$(CC) $(CFLAGS) BTSyncController.cpp -o BTSyncController.o

PeerSocketConnection.o: PeerSocketConnection.cpp PeerSocketConnection.h
	$(CC) $(CFLAGS) PeerSocketConnection.cpp -o PeerSocketConnection.o
