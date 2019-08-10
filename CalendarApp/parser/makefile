all: parser

LinkedListAPI.o: src/LinkedListAPI.c
	gcc -Iinclude -c src/LinkedListAPI.c -fPIC -o LinkedListAPI.o -Wall -std=c11

CalendarParser.o: src/CalendarParser.c
	gcc -Iinclude -c src/CalendarParser.c -fPIC -o CalendarParser.o -Wall -std=c11

parser: CalendarParser.o LinkedListAPI.o
	gcc CalendarParser.o LinkedListAPI.o -shared -o ../sharedLib.so

