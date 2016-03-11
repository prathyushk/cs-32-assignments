CC = g++
CFLAGS = -std=c++11
LDFLAGS = -framework OpenGL -L/opt/X11/lib -lglut
RM = /bin/rm -f
all: FrackMan
FrackMan: StudentWorld.o GameWorld.o main.o GameController.o Actor.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o Frackman StudentWorld.o GameWorld.o main.o GameController.o Actor.o
StudentWorld.o:
	$(CC) $(CFLAGS) -c StudentWorld.cpp
GameWorld.o:
	$(CC) $(CFLAGS) -c GameWorld.cpp
main.o:
	$(CC) $(CFLAGS) -c main.cpp
GameController.o:
	$(CC) $(CFLAGS) -c GameController.cpp
Actor.o:
	$(CC) $(CFLAGS) -c Actor.cpp
zip: StudentWorld.h StudentWorld.cpp Actor.h Actor.cpp report.txt
	zip turn-in StudentWorld.h StudentWorld.cpp Actor.h Actor.cpp report.txt
clean:
	$(RM) *.o FrackMan turn-in.zip
