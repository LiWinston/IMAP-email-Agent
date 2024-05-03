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
release: clean $(EXE)

$(EXE): main.c emailClient.o network.o ssl.o
	$(CC) $(CFLAGS) main.c -o $(EXE) emailClient.o network.o ssl.o -lssl -lcrypto $(LDFLAGS)
emailClient.o: emailClient.c emailClient.h macros.h 
	$(CC) $(CFLAGS) -c emailClient.c -o emailClient.o
network.o: network.c network.h macros.h
	$(CC) $(CFLAGS) -c network.c -o network.o
ssl.o: ssl.c ssl.h
	$(CC) $(CFLAGS) -c ssl.c -o ssl.o


OUT_DIR = ../out
test: $(EXEC)
	../fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-ed512.out
	../fetchmail -f Test -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-mst.out
	../fetchmail -f Test -u test@comp30023 -p pass1 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-loginfail.out
	../fetchmail -u test@comp30023 -p pass -n 1 -f Test1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-nofolder.out
	../fetchmail -n 42 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-nomessage.out
	../fetchmail -u test.test@comp30023 -p -p -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-mst.out
	../fetchmail -f 'With Space' -n 1 -u test@comp30023 -p pass retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-mst.out
	../fetchmail -f Test -p pass -n 2 -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-mst.out
	../fetchmail -f Test -n 3 -p pass -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-minimal.out
	../fetchmail -p pass -f headers -u test@comp30023 -n 2 parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-caps.out
	../fetchmail -f headers -u test@comp30023 -p pass -n 3 parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-nosubj.out
	../fetchmail -u test@comp30023 -n 4 -p pass -f headers parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-nested.out
	../fetchmail -f headers -u test@comp30023 -n 5 -p pass parse unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/parse-ws.out
	../fetchmail -n 1 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/mime-ed512.out
	../fetchmail -f Test -n 2 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/mime-mst.out
	../fetchmail -p pass -u test@comp30023 -f Test list unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/list-Test.out
	../fetchmail -p pass -u test@comp30023 list unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/list-INBOX.out
	../fetchmail -f Test -p pass -u test@comp30023 -n 1 -t retrieve unimelb-comp30023-2024.cloud.edu.au | diff - $(OUT_DIR)/ret-ed512.out

format:
	clang-format-14 -style=file -i *.c *.h

clean:
	rm -f *.o $(EXE)