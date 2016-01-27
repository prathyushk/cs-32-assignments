CC = g++
RM = /bin/rm -f
all: testMap
testMap: testMap.o Map.o
	$(CC) -o testMap testMap.o Map.o
testMap.o: testMap.cpp
	$(CC) -c testMap.cpp
Map.o: Map.cpp
	$(CC) -c Map.cpp
zip: Map.h Map.cpp report.txt testMap.cpp
	zip turn-in Map.h Map.cpp report.txt testMap.cpp
clean:
	$(RM) *.o testMap turn-in.zip
