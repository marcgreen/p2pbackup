
CFLAGS = -c -std=c++11
LDFLAGS = -pthread
CC = g++
LD = $(CC)

main: main.o Dispatcher.o Worker.o Job.o
	$(LD) $(LDFLAGS) main.o Dispatcher.o Worker.o Job.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Dispatcher.o: Dispatcher.cpp
	$(CC) $(CFLAGS) Dispatcher.cpp

Worker.o: Worker.cpp
	$(CC) $(CFLAGS) Worker.cpp

Job.o: Job.cpp
	$(CC) $(CFLAGS) Job.cpp

clean:
	rm -rf *~ *.o main
