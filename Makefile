EXE=fetchmail
EXE_SUFFIX=.exe
RM = rm -f
ifeq ($(OS),Windows_NT)
    RM = del
endif

$(EXE): src/main.c
	gcc -Wall -o $(EXE) $<
run: $(EXE)
	./$(EXE)
memcheck: $(EXE)
	valgrind ./$(EXE)
clean:
	@if exist src\*.o $(RM) src\*.o
	@if exist $(EXE)$(EXE_SUFFIX) $(RM) $(EXE)$(EXE_SUFFIX)