CC=gcc
CFLAGS= -Wall -Werror
LDFLAGS= -Wall -Werror
DEBUGFLAGS= -g -fsanitize=address -fsanitize=undefined
RELEASEFLAGS= -O2

EXE=../fetchmail

all: release

debug: CFLAGS += $(DEBUGFLAGS)
debug: LDFLAGS += $(DEBUGFLAGS)
debug: clean format $(EXE)

release: CFLAGS += $(RELEASEFLAGS)
release: LDFLAGS += $(RELEASEFLAGS)
release: $(EXE)

$(EXE): main.c emailClient.o network.o ssl.o
	$(CC) $(CFLAGS) main.c -o $(EXE) emailClient.o network.o ssl.o -lssl -lcrypto $(LDFLAGS)
emailClient.o: emailClient.c emailClient.h macros.h 
	$(CC) $(CFLAGS) -c emailClient.c -o emailClient.o
network.o: network.c network.h macros.h
	$(CC) $(CFLAGS) -c network.c -o network.o
ssl.o: ssl.c ssl.h
	$(CC) $(CFLAGS) -c ssl.c -o ssl.o

format:
	clang-format-14 -style=file -i *.c *.h

clean:
	rm -f *.o $(EXE)