EXE=fetchmail
EXE_SUFFIX=.exe
ifeq ($(OS),Windows_NT)
    RM = del
    MEMCHECK_CMD = wsl valgrind --leak-check=full
else
	Rm = rm -f
    MEMCHECK_CMD = valgrind
endif

all: $(EXE)
$(EXE): src/main.c src/tag_manager.o src/set.o src/connection_manager.o src/PriorityQueue.o src/Email.o
	gcc -Wall -o $(EXE) $+

src/tag_manager.o: src/tag_manager.c src/tag_manager.h src/set.o
	gcc -Wall -c -o $@ $<

src/set.o: src/set.c src/set.h
	gcc -Wall -c -o $@ $<

src/connection_manager.o: src/connection_manager.c src/connection_manager.h src/PriorityQueue.o src/Email.o
	gcc -Wall -c -o $@ $<

src/PriorityQueue.o: src/PriorityQueue.c src/PriorityQueue.h
	gcc -Wall -c -o $@ $<

src/Email.o: src/Email.c src/Email.h
	gcc -Wall -c -o $@ $<


run: $(EXE)
	./$(EXE) -u test@comp30023 -p pass -f Test -n 2 -t retrieve unimelb-comp30023-2024.cloud.edu.au

memcheck: $(EXE)
	$(MEMCHECK_CMD) ./$(EXE)
clean:
	$(RM) $(EXE) $(EXE)$(EXE_SUFFIX) src/*.o