EXE=fetchmail
EXE_SUFFIX=.exe
RM = rm -f
ifeq ($(OS),Windows_NT)
    RM = del
    MEMCHECK_CMD = wsl valgrind --leak-check=full
else
    MEMCHECK_CMD = valgrind
endif

all: $(EXE)
$(EXE): src/main.c src/tag_manager.o src/set.o src/connection_manager.o
	gcc -Wall -o $(EXE) $+

src/tag_manager.o: src/tag_manager.c src/tag_manager.h src/set.o
	gcc -Wall -c -o $@ $<

src/set.o: src/set.c src/set.h
	gcc -Wall -c -o $@ $<

src/connection_manager.o: src/connection_manager.c src/connection_manager.h
	gcc -Wall -c -o $@ $<



run: $(EXE)
	./$(EXE) -u myusername -p mypassword -f myfolder -n 123 -t retrieve example.com

memcheck: $(EXE)
	$(MEMCHECK_CMD) ./$(EXE)
clean:
	@if exist src\*.o $(RM) src\*.o
	@if exist $(EXE)$(EXE_SUFFIX) $(RM) $(EXE)$(EXE_SUFFIX)