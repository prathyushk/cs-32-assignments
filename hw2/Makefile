CC = g++
RM = /bin/rm -f
all: mazestack mazequeue eval
mazestack: mazestack.o
	$(CC) -o mazestack mazestack.o
mazequeue: mazequeue.o
	$(CC) -o mazequeue mazequeue.o
eval: Map.o eval.o
	$(CC) -o eval eval.o Map.o
mazestack.o: mazestack.cpp
	$(CC) -c mazestack.cpp
mazequeue.o: mazequeue.cpp
	$(CC) -c mazequeue.cpp
Map.o: Map.cpp
	$(CC) -c Map.cpp
eval.o: eval.cpp
	$(CC) -c eval.cpp
zip: mazestack.cpp mazequeue.cpp eval.cpp hw.txt
	zip turn-in mazestack.cpp mazequeue.cpp eval.cpp hw.txt
clean:
	$(RM) *.o mazestack mazequeue eval turn-in.zip
