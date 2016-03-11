CC = g++
CFLAGS = -std=c++11
RM = /bin/rm -f
all: p4tester p4gen
p4tester: p4tester.o IntelWeb.o DiskMultiMap.o
	$(CC) $(CFLAGS) -o p4tester p4tester.o IntelWeb.o DiskMultiMap.o
p4gen: p4gen.o
	$(CC) $(CFLAGS) -o p4gen p4gen.o
p4tester.o: p4tester.cpp
	$(CC) $(CFLAGS) -c p4tester.cpp
IntelWeb.o: IntelWeb.cpp
	$(CC) $(CFLAGS) -c IntelWeb.cpp
DiskMultiMap.o: DiskMultiMap.cpp
	$(CC) $(CFLAGS) -c DiskMultiMap.cpp
p4gen.o: p4gen.cpp
	$(CC) $(CFLAGS) -c p4gen.cpp
zip: IntelWeb.h IntelWeb.cpp DiskMultiMap.h DiskMultiMap.cpp report.txt
	zip turn-in IntelWeb.h IntelWeb.cpp DiskMultiMap.h DiskMultiMap.cpp report.txt
clean:
	$(RM) *.o p4tester p4gen turn-in.zip
