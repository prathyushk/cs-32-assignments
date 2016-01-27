CC = g++
RM = /bin/rm -f
all: main
main: main.o  Game.o Arena.o Player.o History.o Robot.o utilities.o
	$(CC) -o game main.o Game.o Arena.o Player.o History.o Robot.o utilities.o
main.o: main.cpp
	$(CC) -c main.cpp
Game.o: Game.cpp
	$(CC) -c Game.cpp
Arena.o: Arena.cpp
	$(CC) -c Arena.cpp
Player.o: Player.cpp
	$(CC) -c Player.cpp
History.o: History.cpp
	$(CC) -c History.cpp
Robot.o: Robot.cpp
	$(CC) -c Robot.cpp
utilities.o: utilities.cpp
	$(CC) -c utilities.cpp
zip: main.cpp utilities.cpp Game.h Game.cpp Arena.h Arena.cpp Player.h Player.cpp History.h History.cpp Robot.h Robot.cpp globals.h
	zip turn-in main.cpp utilities.cpp Game.h Game.cpp Arena.h Arena.cpp Player.h Player.cpp History.h History.cpp	Robot.h	Robot.cpp globals.h
clean:
	$(RM) *.o game turn-in.zip
