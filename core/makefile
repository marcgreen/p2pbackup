
CFLAGS = -c -std=c++11
LDFLAGS = -pthread
CC = g++
LD = $(CC)

main: main.o ConsoleController.o Controller.o CoreController.o Dispatcher.o \
	Worker.o Job.o
	$(LD) $(LDFLAGS) main.o ConsoleController.o Controller.o \
	CoreController.o Dispatcher.o Worker.o Job.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Dispatcher.o: Dispatcher.cpp Dispatcher.h Job.h Worker.h
	$(CC) $(CFLAGS) Dispatcher.cpp -o Dispatcher.o

Worker.o: Worker.cpp Dispatcher.h Job.h Worker.h
	$(CC) $(CFLAGS) Worker.cpp -o Worker.o

Job.o: Job.cpp Job.h
	$(CC) $(CFLAGS) Job.cpp -o Job.o

CoreController.o: CoreController.cpp CoreController.h
	$(CC) $(CFLAGS) CoreController.cpp -o CoreController.o

Controller.o: Controller.cpp Controller.h
	$(CC) $(CFLAGS) Controller.cpp -o Controller.o

ConsoleController.o: ConsoleController.cpp ConsoleController.h Controller.h
	$(CC) $(CFLAGS) ConsoleController.cpp -o ConsoleController.o

clean:
	rm -rf *~ *.o *.o *~ main
