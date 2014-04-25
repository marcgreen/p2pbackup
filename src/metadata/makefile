
CFLAGS = -c -std=c++11 -I..
CC = g++
ARCHIVER = ar
ARFLAGS = rcs
OBJLIST = MetadataRecord.o LocalBackupInfo.o

metadatalib: $(OBJLIST)
	$(ARCHIVER) $(ARFLAGS) libmeta.a $(OBJLIST)

MetadataRecord.o: MetadataRecord.cpp MetadataRecord.h
	$(CC) $(CFLAGS) MetadataRecord.cpp -o MetadataRecord.o

LocalBackupInfo.o: LocalBackupInfo.cpp LocalBackupInfo.h
	$(CC) $(CFLAGS) LocalBackupInfo.cpp -o LocalBackupInfo.o

clean:
	rm $(OBJLIST)
