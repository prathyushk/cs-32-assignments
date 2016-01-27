CC = g++
RM = /bin/rm -f
all: testWeightMap testMap testnewMap
testWeightMap: WeightMap.o testWeightMap.o Map.o
	$(CC) -o testWeightMap WeightMap.o testWeightMap.o Map.o
testMap: Map.o testMap.o
	$(CC) -o testMap Map.o testMap.o
testnewMap: newMap.o testnewMap.o
	$(CC) -o testnewMap newMap.o testnewMap.o
WeightMap.o: WeightMap.cpp
	$(CC) -c WeightMap.cpp
testWeightMap.o: testWeightMap.cpp
	$(CC) -c testWeightMap.cpp
Map.o: Map.cpp
	$(CC) -c Map.cpp
testMap.o: testMap.cpp
	$(CC) -c testMap.cpp
newMap.o: newMap.cpp
	$(CC) -c newMap.cpp
testnewMap.o: testnewMap.cpp
	$(CC) -c testnewMap.cpp
zip: WeightMap.cpp WeightMap.h testWeightMap.cpp Map.h Map.cpp testMap.cpp newMap.h newMap.cpp testnewMap.cpp
	zip turn-in WeightMap.cpp WeightMap.h testWeightMap.cpp Map.h Map.cpp testMap.cpp newMap.h newMap.cpp testnewMap.cpp
clean:
	$(RM) *.o testWeightMap testMap testnewMap turn-in.zip
